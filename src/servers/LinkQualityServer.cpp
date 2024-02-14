//
// Created by arne- on 14.02.2024.
//

#include "LinkQualityServer.h"
#include "utils/Logger.h"
#include "utils/unit_conversions.h"

bool LinkQualityServer::processInterest(ndnph::Interest interest) {
    const auto &name = interest.getName();
    if (!m_prefix.isPrefixOf(name)) {
        return false;
    }
    LOG_INFO("Received interest in LinkQualityServer");

    ndnph::StaticRegion<1024> region;
    ndnph::Data data = region.create<ndnph::Data>();
    NDNPH_ASSERT(!!data);
    data.setName(name);
    data.setFreshnessPeriod(1000);  // Packet is only valid for 1000ms

    auto mapSize = linkQualityStore->qualityMap.size();
    byte buffer[12 * mapSize];  // uint64_t and float for each device
    size_t i = 0;

    for (const auto &item: linkQualityStore->qualityMap) {
        auto devId = item.first;
        auto quality = item.second->calculateLinkQuality();

        uint64ToByte(&buffer[12 * i], devId);
        floatToByte(&buffer[12 * i + 8], quality);

        i++;
    }

    data.setContent(ndnph::tlv::Value(buffer, 12 * mapSize));

    reply(data.sign(m_signer));
    return true;
}

