//
// Created by arne- on 07.11.2023.
//

#ifndef NDN_ARDUINO_MOTIONSERVER_H
#define NDN_ARDUINO_MOTIONSERVER_H

#include <ndnph/face/packet-handler.hpp>
#include <ndnph/keychain/digest.hpp>

// HC-SR501 PIR Sensor
class MotionServer : public ndnph::PacketHandler {
public:
    explicit MotionServer(ndnph::Face &face, ndnph::Name prefix, int sensorPin) :
            PacketHandler(face),
            namePrefix(std::move(prefix)),
            m_signer(ndnph::DigestKey::get()),
            sensorPin(sensorPin)
    {}

private:
    bool processInterest(ndnph::Interest interest) override;

    bool processData(ndnph::Data data) override;

    ndnph::Name namePrefix;
    const ndnph::PrivateKey &m_signer;

    int sensorPin;
};

#endif //NDN_ARDUINO_MOTIONSERVER_H
