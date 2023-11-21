//
// Created by arne- on 13.11.2023.
//

#ifndef NDN_ARDUINO_DISCOVERYCLIENT_H
#define NDN_ARDUINO_DISCOVERYCLIENT_H

#include <ndnph/face/packet-handler.hpp>
#include <ndnph/keychain/digest.hpp>
#include <ndnph/port/clock/port.hpp>
#include <utility>
#include <vector>

class DiscoveryClient : public ndnph::PacketHandler {
public:
    DiscoveryClient(ndnph::Face &face, ndnph::Name prefix, std::vector<std::string> providedResources) :
            PacketHandler(face),
            m_prefix(std::move(prefix)),
            m_signer(ndnph::DigestKey::get()),
            providedResources(std::move(providedResources)) {
        ndnph::port::RandomSource::generate(reinterpret_cast<uint8_t *>(&m_seqNum), sizeof(m_seqNum));
    }

    bool sendInterest();

    bool processData(ndnph::Data data) override;

    bool shouldRespondToDiscovery(const ndnph::Name& name);

private:
    bool processInterest(ndnph::Interest interest) override;

    bool processNack(ndnph::Nack nack) override;


private:
    ndnph::Name m_prefix;
    const ndnph::PrivateKey &m_signer;
    uint64_t m_seqNum = 0;
    std::vector<std::string> providedResources;
};


#endif //NDN_ARDUINO_DISCOVERYCLIENT_H
