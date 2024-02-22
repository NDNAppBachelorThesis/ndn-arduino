//
// Created by arne- on 13.11.2023.
//

#include "DiscoveryServer.h"
#include "utils/unit_conversions.h"
#include "utils/Logger.h"
#include <iostream>
#include <numeric>
#include <queue>

bool DiscoveryServer::processNack(ndnph::Nack nack) {
    LOG_ERROR("DiscoveryServer received NACK");
    return false;
}

bool DiscoveryServer::shouldRespondToDiscovery(const ndnph::Name &name) {
    uint64_t deviceId = ESP.getEfuseMac();
    byte deviceIdBuffer[8];
    uint64ToByte(deviceIdBuffer, deviceId);

    // Check for each component in the name if it includes the devices id
    for (auto comp: name.slice(2)) {
        if (comp.length() != 8) {
            continue;
        }

        auto value = comp.value();
        if (std::memcmp(value, deviceIdBuffer, 8) == 0) {
            return false;
        }
    }

    return true;
}

bool DiscoveryServer::processInterest(ndnph::Interest interest) {
    const auto &name = interest.getName();
    if (!m_prefix.isPrefixOf(name)) {
        return false;
    }
    LOG_INFO("Received interest in DiscoveryServer");

    if (!shouldRespondToDiscovery(name)) {
        LOG_INFO("  -> Already answered. Skipping!");
        return false;
    }

    LOG_INFO("  -> Processing it.");

    ndnph::StaticRegion<1024> region;
    ndnph::Data data = region.create<ndnph::Data>();
    NDNPH_ASSERT(!!data);
    data.setName(interest.getName()
                         .append(region, ndnph::Component::parse(region, std::to_string(ESP.getEfuseMac()).c_str()))
                                 // Identify as ESP32 board
                         .append(region, ndnph::Component::parse(region, std::to_string(0).c_str()))
    );
    // DO NOT REDUCE TO 0!!! This will, for whatever reason, cause the device to not receive any responses
    data.setFreshnessPeriod(1000);     // Make sure we always use the newest requests

    // Build response message
    std::vector<int> elementLengths;
    std::transform(providedResources.begin(), providedResources.end(), std::back_inserter(elementLengths),
                   [](const std::string &e) {
                       return e.length() + 1;   // +1 for additional 0-byte
                   });
    auto sum = std::accumulate(elementLengths.begin(), elementLengths.end(), 0);

    char buffer[sum];
    size_t arrayOffset = 0;

    for (const auto &res: providedResources) {
        std::strcpy(buffer + arrayOffset, res.c_str());
        arrayOffset += res.length() + 1;
    }

    data.setContent(ndnph::tlv::Value((uint8_t *) buffer, sum));

    reply(data.sign(m_signer));
    return true;
}

