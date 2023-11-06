//
// Created by arne- on 06.11.2023.
//

#ifndef NDN_ARDUINO_TESTSERVER_H
#define NDN_ARDUINO_TESTSERVER_H

#include <ndnph/face/packet-handler.hpp>
#include <ndnph/keychain/digest.hpp>

class TestServer : public ndnph::PacketHandler {
public:
    explicit TestServer(ndnph::Name prefix, ndnph::Face &face) :
            PacketHandler(face),
            namePrefix(std::move(prefix)),
            m_signer(ndnph::DigestKey::get()) {}

private:
    bool processInterest(ndnph::Interest interest) override;

    bool processData(ndnph::Data data) override;;


private:
    ndnph::Name namePrefix;
    const ndnph::PrivateKey &m_signer;

};


#endif //NDN_ARDUINO_TESTSERVER_H
