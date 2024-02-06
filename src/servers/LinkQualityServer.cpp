//
// Created by arne- on 06.02.2024.
//

#include "LinkQualityServer.h"
#include "utils/Logger.h"
#include "utils/unit_conversions.h"

bool LinkQualityServer::processNack(ndnph::Nack nack) {
    LOG_ERROR("LinkQualityServer received NACK");
    return false;
}

void LinkQualityServer::loop() {
    auto now = ndnph::port::Clock::now();
    if (ndnph::port::Clock::isBefore(now, m_next)) {
        return;
    }
    sendLinkQualityPacket();
    m_next = ndnph::port::Clock::add(now, autoSendDelay);
}


void LinkQualityServer::sendLinkQualityPacket() {
    LOG_INFO("Sending link quality packet.");
    auto interestNameStr = "/esp/linkqualityhandler/" +  std::to_string(deviceId);
    ndnph::StaticRegion<1024> region;
    auto name = ndnph::Name::parse(region, interestNameStr.c_str());
    NDNPH_ASSERT(!!name);
    ndnph::Interest interest = region.create<ndnph::Interest>();
    NDNPH_ASSERT(!!interest);

    byte buffer[8];
    uint64ToByte(buffer, esp_timer_get_time() / 1000);

    auto dataComp = ndnph::Component::from(region, ndnph::TT::GenericNameComponent, ndnph::tlv::Value(buffer, 8));
    interest.setName(name.append(region, dataComp));
    interest.setMustBeFresh(true);
    interest.setLifetime(5000);
    interest.setNonce(generateNonce()); // Must set nonce!

    if (!send(interest.sign(m_signer))) {
        LOG_INFO("  -> Failed");
        return;
    }
    LOG_INFO("  -> OK");
}

bool LinkQualityServer::processData(ndnph::Data data) {
    LOG_INFO("LinkQualityServer received data");
    return true;
}


uint32_t LinkQualityServer::generateNonce() {
    uint32_t nonce;
    // Does not work. I somehow get linked to the null implementation
//    ndnph::port::RandomSource::generate(reinterpret_cast<uint8_t*>(&nonce), sizeof(nonce));
    esp_fill_random(reinterpret_cast<uint8_t*>(&nonce), sizeof(nonce));
    return nonce;
}