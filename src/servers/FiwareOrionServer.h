//
// Created by arne- on 05.12.2023.
//

#ifndef NDN_ARDUINO_FIWAREORIONSERVER_H
#define NDN_ARDUINO_FIWAREORIONSERVER_H


#include <ndnph/face/packet-handler.hpp>
#include <ndnph/keychain/digest.hpp>
#include <ndnph/port/clock/port.hpp>
#include "sensors/dht11.h"

class FiwareOrionServer : public ndnph::PacketHandler {
public:
    explicit FiwareOrionServer(ndnph::Face &face, int dhtPort) :
            PacketHandler(face),
            m_signer(ndnph::DigestKey::get()),
            dhtPort(dhtPort)
    {}

private:
    void loop() override;
    bool sendInterest();

private:
    const ndnph::PrivateKey &m_signer;
    ndnph::port::Clock::Time m_next;
    uint64_t deviceId = ESP.getEfuseMac();

    int dhtPort;
    dht11 DHT11{};
};


#endif //NDN_ARDUINO_FIWAREORIONSERVER_H
