//
// Created by arne- on 13.11.2023.
//

#include "DiscoveryClient.h"
#include "utils/unit_conversions.h"
#include "utils/Logger.h"
#include <iostream>
#include <numeric>
#include <queue>

bool DiscoveryClient::processNack(ndnph::Nack nack) {
    std::cout << "Received NACK" << std::endl;
    return false;
}

bool DiscoveryClient::processData(ndnph::Data data) {
    LOG_INFO("Received response packet");

    const ndnph::Name &dataName = data.getName();
    if (!m_prefix.isPrefixOf(dataName) || m_prefix.size() + 1 != dataName.size()) {
        return false;
    }

    ndnph::Component lastComp = dataName[-1];
    ndnph::Decoder::Tlv d;
    ndnph::Decoder::readTlv(d, lastComp.tlv(), lastComp.tlv() + lastComp.size());
    uint64_t seqNum = 0;

    return true;
}

bool DiscoveryClient::shouldRespondToDiscovery(const ndnph::Name& name) {
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

bool DiscoveryClient::processInterest(ndnph::Interest interest) {
    LOG_INFO("Received interest in DiscoveryClient");
    const auto &name = interest.getName();

    if (!m_prefix.isPrefixOf(name)) {
        return false;
    }

    if (!shouldRespondToDiscovery(name)) {
        LOG_INFO("Skipping interest");
        return false;
    }

    std::cout << "Processing " << name << std::endl;

    ndnph::StaticRegion<1024> region;
    ndnph::Data data = region.create<ndnph::Data>();
    NDNPH_ASSERT(!!data);
    data.setName(interest.getName().append(region,ndnph::Component::parse(region, std::to_string(ESP.getEfuseMac()).c_str())));
    data.setFreshnessPeriod(1);

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

bool DiscoveryClient::sendInterest() {
    ndnph::StaticRegion<1024> region;
    ndnph::Component seqNumComp = ndnph::Component::from(region, ndnph::TT::GenericNameComponent,
                                                         ndnph::tlv::NNI8(++m_seqNum));
    NDNPH_ASSERT(!!seqNumComp);
    ndnph::Name name = m_prefix.append(region, seqNumComp);
    NDNPH_ASSERT(!!name);

    ndnph::Interest interest = region.create<ndnph::Interest>();
    NDNPH_ASSERT(!!interest);
    interest.setName(name);
    interest.setMustBeFresh(true);

    std::cout << "Sending packet " << name << "..." << std::endl;

    if (!send(region, interest)) {
        std::cout << "Packet sent failed." << std::endl;
        return false;
    }

    std::cout << "Packet sent successfully." << std::endl;

    return true;
}


