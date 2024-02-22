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

/**
 * NDN Server for handling auto discovery packets
 */
class DiscoveryServer : public ndnph::PacketHandler {
public:
    DiscoveryServer(ndnph::Face &face, ndnph::Name prefix) :
            PacketHandler(face),
            m_prefix(std::move(prefix)),
            m_signer(ndnph::DigestKey::get()) {}

    /**
     * Adds a new name like '/esp/<deviceId>/data' to the provided resources. This ensures that it is found by the
     * auto discovery by users
     */
    void addProvidedResource(const std::string& resource) {
        providedResources.push_back(resource);
    }

private:
    bool processInterest(ndnph::Interest interest) override;
    bool processNack(ndnph::Nack nack) override;
    /**
     * Determines if the ESP already responded to this interest
     */
    bool shouldRespondToDiscovery(const ndnph::Name& name);

private:
    ndnph::Name m_prefix;
    const ndnph::PrivateKey &m_signer;
    std::vector<std::string> providedResources;
};


#endif //NDN_ARDUINO_DISCOVERYSERVER_H
