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


class LinkQualityServer : public ndnph::PacketHandler {
public:
    LinkQualityServer(ndnph::Face &face, ndnph::Name prefix) :
            PacketHandler(face),
            m_prefix(std::move(prefix)),
            m_signer(ndnph::DigestKey::get()),
            autoSendDelay(1000) {}


private:
    bool processNack(ndnph::Nack nack) override;
    void loop() override;
    bool processData(ndnph::Data data) override;

    void sendLinkQualityPacket();
    uint32_t generateNonce();

private:
    ndnph::Name m_prefix;
    const ndnph::PrivateKey &m_signer;
    ndnph::port::Clock::Time m_next;    // Timer for automatically sending messages to Fiware-Orion
    const int autoSendDelay;            // Delay in ms for the auto sends
    const uint64_t deviceId = ESP.getEfuseMac();
};


#endif //NDN_ARDUINO_LINKQUALITYSERVER_H
