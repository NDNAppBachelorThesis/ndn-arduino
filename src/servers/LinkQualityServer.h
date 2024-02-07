//
// Created by arne- on 06.02.2024.
//

#ifndef NDN_ARDUINO_LINKQUALITYSERVER_H
#define NDN_ARDUINO_LINKQUALITYSERVER_H


#include <ndnph/face/packet-handler.hpp>
#include <ndnph/keychain/digest.hpp>
#include <ndnph/port/clock/port.hpp>
#include <utility>
#include <vector>
#include "datastructures/RingBuffer.h"


class LinkQualityServer : public ndnph::PacketHandler {
public:
    LinkQualityServer(ndnph::Face &face, ndnph::Name prefix) :
            PacketHandler(face),
            m_prefix(std::move(prefix)),
            m_signer(ndnph::DigestKey::get()),
            autoSendDelay(30000) {}


private:
    bool processNack(ndnph::Nack nack) override;
    void loop() override;
    bool processData(ndnph::Data data) override;
    bool processInterest(ndnph::Interest interest) override;

    void sendLinkQualityPacket();
    uint32_t generateNonce();
    float calculateLinkQuality();

private:
    ndnph::Name m_prefix;
    const ndnph::PrivateKey &m_signer;
    ndnph::port::Clock::Time m_next;    // Timer for automatically sending messages to Fiware-Orion
    const int autoSendDelay;            // Delay in ms for the auto sends
    const uint64_t deviceId = ESP.getEfuseMac();
    uint64_t lastSendTime = 0u;
    RingBuffer<128> durationBuffer;
    uint16_t sentPackagesCnt = 0;
    uint16_t receivedResponsesCnt = 0;
};


#endif //NDN_ARDUINO_LINKQUALITYSERVER_H
