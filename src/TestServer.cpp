//
// Created by arne- on 06.11.2023.
//

#include "TestServer.h"
#include "iostream"
#include <ndnph/tlv/value.hpp>

bool TestServer::processInterest(ndnph::Interest interest) {
    std::cout << "Request incoming" << std::endl;

    if (!namePrefix.isPrefixOf(interest.getName())) {
        return false;
    }

    std::cout << "Processing " << interest.getName().length() << std::endl;

    ndnph::StaticRegion<1024> region;
    ndnph::Data data = region.create<ndnph::Data>();
    NDNPH_ASSERT(!!data);
    data.setName(interest.getName());
    data.setFreshnessPeriod(1);

    auto content = ndnph::tlv::Value::fromString("Hallo Welt");
    data.setContent(content);

    reply(data.sign(m_signer));
    return true;
}

bool TestServer::processData(ndnph::Data data) {
    return false;
}
