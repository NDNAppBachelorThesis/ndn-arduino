//
// Created by arne- on 06.02.2024.
//

#ifndef NDN_ARDUINO_LINKQUALITYCHECKSERVER_H
#define NDN_ARDUINO_LINKQUALITYCHECKSERVER_H


#include <ndnph/face/packet-handler.hpp>
#include <ndnph/keychain/digest.hpp>
#include <ndnph/port/clock/port.hpp>
#include <utility>
#include <vector>
#include "datastructures/LinkQualityStore.h"


/**
 * The NFD server, which periodically sends interest packets to the adjacent devices so they can determine their link
 * quality
 */
class LinkQualityCheckServer : public ndnph::PacketHandler {
public:
    LinkQualityCheckServer(ndnph::Face &face, LinkQualityStore *linkQualityStore) :
            PacketHandler(face),
            linkQualityStore(linkQualityStore),
            autoSendDelay(1000) {}


private:
    bool processNack(ndnph::Nack nack) override;

    void loop() override;

    bool processData(ndnph::Data data) override;

    /**
     * Process received interest packets from adjacent devices
     */
    bool processInterest(ndnph::Interest interest) override;

    /**
     * Send interest packets to adjacent devices
     */
    void sendLinkQualityPacket();

    uint32_t generateNonce();

private:
    ndnph::port::Clock::Time m_next;    // Timer for automatically sending messages to Fiware-Orion
    const int autoSendDelay;            // Delay in ms for the auto sends
    const uint64_t deviceId = ESP.getEfuseMac();
    uint64_t lastSendTime = 0u;
    LinkQualityStore *linkQualityStore;
};


#endif //NDN_ARDUINO_LINKQUALITYCHECKSERVER_H
