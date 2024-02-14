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
    auto interestNameStr = "/esp/linkqualitycheck/" + std::to_string(deviceId);
    ndnph::StaticRegion<1024> region;
    auto name = ndnph::Name::parse(region, interestNameStr.c_str());
    NDNPH_ASSERT(!!name);
    ndnph::Interest interest = region.create<ndnph::Interest>();
    NDNPH_ASSERT(!!interest);

    this->lastSendTime = esp_timer_get_time() / 1000;
    byte buffer[8];
    uint64ToByte(buffer, this->lastSendTime);

    auto dataComp = ndnph::Component::from(region, ndnph::TT::GenericNameComponent, ndnph::tlv::Value(buffer, 8));
    interest.setName(name.append(region, dataComp));
    interest.setMustBeFresh(true);
    interest.setLifetime(1000);
    interest.setNonce(generateNonce()); // Must set nonce!
    interest.setHopLimit(10);

    if (!send(interest)) {
        LOG_INFO("  -> Failed");
        return;
    }
}

bool LinkQualityServer::processData(ndnph::Data data) {
    LOG_INFO("LinkQualityServer response.");
    return true;
}


bool LinkQualityServer::processInterest(ndnph::Interest interest) {
    const auto &name = interest.getName();
    ndnph::StaticRegion<1024> region;
    auto prefix = ndnph::Name::parse(region, "/esp/linkqualitycheck");
    if (!prefix.isPrefixOf(name)) {
        return false;
    }

    LOG_INFO("Received interest for LinkQualityServer");
    auto idComponent = name[2];

    char buffer[idComponent.length() + 1];
    std::memset(buffer, 0, idComponent.length() + 1);
    std::memcpy(buffer, idComponent.value(), idComponent.length());
    auto sourceDeviceId = std::stoull(std::string(buffer));


    auto timeComponent = name[3];
    auto timestamp = bytesToUint64((byte*) timeComponent.value());
    std::cout << "TS: " << timestamp << std::endl;

    linkQualityStore->receiveFromDevice(sourceDeviceId, timestamp);
    std::cout << "  quality: " << linkQualityStore->linkQuality(sourceDeviceId) << std::endl;

    return true;
}



uint32_t LinkQualityServer::generateNonce() {
    uint32_t nonce;
    // Does not work. I somehow get linked to the null implementation
//    ndnph::port::RandomSource::generate(reinterpret_cast<uint8_t*>(&nonce), sizeof(nonce));
    esp_fill_random(reinterpret_cast<uint8_t *>(&nonce), sizeof(nonce));
    return nonce;
}
