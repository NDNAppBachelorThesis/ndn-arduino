//
// Created by arne- on 13.11.2023.
//

#include "DiscoveryClient.h"
#include <iostream>

bool DiscoveryClient::processNack(ndnph::Nack nack) {
    std::cout << "Received NACK" << std::endl;
    return false;
}

bool DiscoveryClient::processData(ndnph::Data data) {
    std::cout << "Received response packet" << std::endl;

    const ndnph::Name& dataName = data.getName();
    if (!m_prefix.isPrefixOf(dataName) || m_prefix.size() + 1 != dataName.size()) {
        return false;
    }

    ndnph::Component lastComp = dataName[-1];
    ndnph::Decoder::Tlv d;
    ndnph::Decoder::readTlv(d, lastComp.tlv(), lastComp.tlv() + lastComp.size());
    uint64_t seqNum = 0;

    return true;
}

bool DiscoveryClient::processInterest(ndnph::Interest interest) {
    std::cout << "Received interest" << std::endl;

    if (!m_prefix.isPrefixOf(interest.getName())) {
        return false;
    }

    std::cout << "Processing " << interest.getName() << std::endl;

    ndnph::StaticRegion<1024> region;

    ndnph::Data data = region.create<ndnph::Data>();
    NDNPH_ASSERT(!!data);
    data.setName(interest.getName());
    data.setFreshnessPeriod(1);

    uint8_t val = 69;
    data.setContent(ndnph::tlv::Value(&val, 1));

    reply(data.sign(m_signer));
    return true;
}

bool DiscoveryClient::sendInterest() {
    ndnph::StaticRegion<1024> region;
    ndnph::Component seqNumComp = ndnph::Component::from(region, ndnph::TT::GenericNameComponent, ndnph::tlv::NNI8(++m_seqNum));
    NDNPH_ASSERT(!!seqNumComp);
    ndnph::Name name = m_prefix.append(region, seqNumComp);
    NDNPH_ASSERT(!!name);

    ndnph::Interest interest = region.create<ndnph::Interest>();
    NDNPH_ASSERT(!!interest);
    interest.setName(name);
    interest.setMustBeFresh(true);

    std::cout << "Sending packet " << name << "..." << std::endl;

    if (!send(region, interest)) {
        std::cout << "Packet sent failed." << std::endl;
        return false;
    }

    std::cout << "Packet sent successfully." << std::endl;

    return true;
}
