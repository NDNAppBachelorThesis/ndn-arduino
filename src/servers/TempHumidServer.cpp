//
// Created by arne- on 06.11.2023.
//

#include "TempHumidServer.h"
#include "iostream"
#include <ndnph/tlv/value.hpp>
#include <sstream>

bool TempHumidServer::processInterest(ndnph::Interest interest) {
    if (!namePrefix.isPrefixOf(interest.getName())) {
        return false;
    }

    std::cout << "Processing " << interest.getName() << std::endl;

    ndnph::StaticRegion<1024> region;
    ndnph::Data data = region.create<ndnph::Data>();
    NDNPH_ASSERT(!!data);
    data.setName(interest.getName());
    data.setFreshnessPeriod(1);

    DHT11.read(dhtPort);
    uint8_t temp = DHT11.humidity;
    std::stringstream ss;
    ss << (float) temp;
    std::cout << "Temp " << ss.str() << std::endl;

    auto content = ndnph::tlv::Value::fromString(ss.str().c_str());
    data.setContent(content);

    reply(data.sign(m_signer));
    return true;
}

bool TempHumidServer::processData(ndnph::Data data) {
    return false;
}