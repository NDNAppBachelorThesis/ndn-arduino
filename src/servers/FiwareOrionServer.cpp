//
// Created by arne- on 05.12.2023.
//

#include "FiwareOrionServer.h"
#include "utils/unit_conversions.h"
#include "utils/Logger.h"

void FiwareOrionServer::loop() {
    auto now = ndnph::port::Clock::now();
    if (ndnph::port::Clock::isBefore(now, m_next)) {
        return;
    }
    sendInterest();
    m_next = ndnph::port::Clock::add(now, 1000);
}


bool FiwareOrionServer::sendInterest()
{
    LOG_INFO("Sending data from FiwareOrionServer");

    ndnph::StaticRegion<1024> region;
    auto interestNameStr = "/esp/fiware/" + std::to_string(deviceId) + "/value";
    auto name = ndnph::Name::parse(region, interestNameStr.c_str());
    NDNPH_ASSERT(!!name);
    ndnph::Interest interest = region.create<ndnph::Interest>();
    NDNPH_ASSERT(!!interest);

    DHT11.read(dhtPort);

    byte buffer[8];
    auto temp = (double) DHT11.temperature;
    doubleToByte(buffer, temp);

    auto dataComp = ndnph::Component::from(region, ndnph::TT::GenericNameComponent, ndnph::tlv::Value(buffer, 8));
    interest.setName(name.append(region, dataComp));
    interest.setMustBeFresh(true);

    if (!send(interest)) {
        return false;
    }

    return true;
}