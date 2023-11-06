//
// Created by arne- on 06.11.2023.
//

#ifndef NDN_ARDUINO_TEMPHUMIDSERVER_H
#define NDN_ARDUINO_TEMPHUMIDSERVER_H

#include <ndnph/face/packet-handler.hpp>
#include <ndnph/keychain/digest.hpp>
#include "sensors/dht11.h"

class TempHumidServer : public ndnph::PacketHandler {
public:
    explicit TempHumidServer(ndnph::Face &face, ndnph::Name prefix, int dhtPort) :
            PacketHandler(face),
            namePrefix(std::move(prefix)),
            m_signer(ndnph::DigestKey::get()),
            dhtPort(dhtPort)
            {}

private:
    bool processInterest(ndnph::Interest interest) override;

    bool processData(ndnph::Data data) override;;

    ndnph::Name namePrefix;
    const ndnph::PrivateKey &m_signer;

    int dhtPort;
    dht11 DHT11{};
};


#endif //NDN_ARDUINO_TEMPHUMIDSERVER_H