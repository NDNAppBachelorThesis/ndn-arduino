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
    LOG_ERROR("Received NACK");
    return false;
}

bool DiscoveryServer::shouldRespondToDiscovery(const ndnph::Name& name) {
    uint64_t deviceId = ESP.getEfuseMac();
    byte deviceIdBuffer[8];
    uint64ToByte(deviceIdBuffer, deviceId);

    for (auto comp : name.slice(2)) {
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
        LOG_INFO("Skipping interest. Already answered it.");
        return false;
    }

    LOG_INFO("Processing %s", nameToString(&name).c_str());

    ndnph::StaticRegion<1024> region;
    ndnph::Data data = region.create<ndnph::Data>();
    NDNPH_ASSERT(!!data);
    data.setName(interest.getName().append(region,ndnph::Component::parse(region, std::to_string(ESP.getEfuseMac()).c_str())));
    data.setFreshnessPeriod(0);     // Make sure we always use the newest requests

    std::queue<std::string> qu(std::deque<std::string>(250));

    // Build response message
    std::vector<int> elementLengths;
    std::transform(providedResources.begin(), providedResources.end(), std::back_inserter(elementLengths), [](const std::string& e) {
        return e.length() + 1;   // +1 for additional 0-byte
    });
    auto sum = std::accumulate(elementLengths.begin(), elementLengths.end(), 0);

    char buffer[sum];
    size_t arrayOffset = 0;

    for (const auto& res : providedResources) {
        std::strcpy(buffer + arrayOffset, res.c_str());
        arrayOffset += res.length() + 1;
    }

    data.setContent(ndnph::tlv::Value((uint8_t*) buffer, sum));

    reply(data.sign(m_signer));
    return true;
}

