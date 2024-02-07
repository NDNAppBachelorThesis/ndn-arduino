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
    auto interestNameStr = "/esp/linkqualityhandler/" + std::to_string(deviceId);
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

    if (!send(interest)) {
        LOG_INFO("  -> Failed");
        return;
    }

    // Make sure the counter doesn't overflow
    if (sentPackagesCnt++ > 2000) {
        sentPackagesCnt /= 2;
        receivedResponsesCnt /= 2;
    }

}

bool LinkQualityServer::processData(ndnph::Data data) {
    auto currentTime = esp_timer_get_time() / 1000;
    auto delta = currentTime - this->lastSendTime;
    durationBuffer.add(delta);

    LOG_INFO("LinkQualityServer delay: %d", delta);
    receivedResponsesCnt++;
    return true;
}


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
    data.setFreshnessPeriod(1000);

    byte buffer[4];
    floatToByte(buffer, calculateLinkQuality());
    data.setContent(ndnph::tlv::Value(buffer, 4));

    reply(data.sign(m_signer));
    return true;
}


float LinkQualityServer::calculateLinkQuality() {
    uint8_t base;
    auto averagePing = durationBuffer.calculateAverage();

    if (averagePing <= 25) {
        base = 100;
    } else if (averagePing <= 30) {
        base = 95;
    } else if (averagePing <= 40) {
        base = 92;
    } else if (averagePing <= 50) {
        base = 90;
    } else if (averagePing <= 80) {
        base = 80;
    } else if (averagePing <= 100) {
        base = 75;
    } else if (averagePing <= 150) {
        base = 70;
    } else if (averagePing <= 200) {
        base = 60;
    } else if (averagePing <= 350) {
        base = 50;
    } else if (averagePing <= 300) {
        base = 40;
    } else if (averagePing <= 400) {
        base = 30;
    } else if (averagePing <= 500) {
        base = 20;
    } else {
        base = 10;
    }

    float responseRatio;
    if (sentPackagesCnt == 0) {
        responseRatio = 1.0;
    } else {
        responseRatio = (receivedResponsesCnt / (float) sentPackagesCnt);
    }

    return base * responseRatio;
}


uint32_t LinkQualityServer::generateNonce() {
    uint32_t nonce;
    // Does not work. I somehow get linked to the null implementation
//    ndnph::port::RandomSource::generate(reinterpret_cast<uint8_t*>(&nonce), sizeof(nonce));
    esp_fill_random(reinterpret_cast<uint8_t *>(&nonce), sizeof(nonce));
    return nonce;
}
