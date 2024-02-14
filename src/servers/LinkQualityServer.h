//
// Created by arne- on 14.02.2024.
//

#ifndef NDN_ARDUINO_LINKQUALITYSERVER_H
#define NDN_ARDUINO_LINKQUALITYSERVER_H

#include <ndnph/face/packet-handler.hpp>
#include <ndnph/keychain/digest.hpp>
#include <ndnph/port/clock/port.hpp>
#include <utility>
#include <vector>
#include "datastructures/LinkQualityStore.h"

class LinkQualityServer : public ndnph::PacketHandler {
public:
    LinkQualityServer(ndnph::Face &face, LinkQualityStore *linkQualityStore, ndnph::Name prefix) :
            PacketHandler(face),
            linkQualityStore(linkQualityStore),
            m_prefix(std::move(prefix)),
            m_signer(ndnph::DigestKey::get()) {}


private:
    bool processInterest(ndnph::Interest interest) override;

private:
    ndnph::Name m_prefix;
    const ndnph::PrivateKey &m_signer;
    const uint64_t deviceId = ESP.getEfuseMac();
    LinkQualityStore *linkQualityStore;
};


#endif //NDN_ARDUINO_LINKQUALITYSERVER_H
