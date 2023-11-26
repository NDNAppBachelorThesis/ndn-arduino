//
// Created by arne- on 13.11.2023.
//

#ifndef NDN_ARDUINO_DISCOVERYSERVER_H
#define NDN_ARDUINO_DISCOVERYSERVER_H

#include <ndnph/face/packet-handler.hpp>
#include <ndnph/keychain/digest.hpp>
#include <ndnph/port/clock/port.hpp>
#include <utility>
#include <vector>

class DiscoveryServer : public ndnph::PacketHandler {
public:
    DiscoveryServer(ndnph::Face &face, ndnph::Name prefix) :
            PacketHandler(face),
            m_prefix(std::move(prefix)),
            m_signer(ndnph::DigestKey::get()) {
        ndnph::port::RandomSource::generate(reinterpret_cast<uint8_t *>(&m_seqNum), sizeof(m_seqNum));
    }

    void addProvidedResource(const std::string& resource) {
        providedResources.push_back(resource);
    }

private:
    bool processInterest(ndnph::Interest interest) override;
    bool processNack(ndnph::Nack nack) override;
    bool shouldRespondToDiscovery(const ndnph::Name& name);

private:
    ndnph::Name m_prefix;
    const ndnph::PrivateKey &m_signer;
    uint64_t m_seqNum = 0;
    std::vector<std::string> providedResources;
};


#endif //NDN_ARDUINO_DISCOVERYSERVER_H
