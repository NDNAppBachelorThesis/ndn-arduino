//
// Created by arne- on 06.11.2023.
//

#include "TempHumidServer.h"
#include "iostream"
#include <ndnph/tlv/value.hpp>
#include "utils/unit_conversions.h"
#include "utils/Logger.h"


bool TempHumidServer::processInterest(ndnph::Interest interest) {
    const auto &name = interest.getName();
    if (!namePrefix.isPrefixOf(name)) {
        return false;
    }

    LOG_INFO("Received interest in TempHumidServer");

    ndnph::StaticRegion<1024> region;

    auto tempPrefix = namePrefix.append(region, ndnph::Component::parse(region, "temperature"));
    auto humidityPrefix = namePrefix.append(region, ndnph::Component::parse(region, "humidity"));

    ndnph::Data data = region.create<ndnph::Data>();
    NDNPH_ASSERT(!!data);
    data.setName(name);
    data.setFreshnessPeriod(1000);  // Packet is only valid for 1000ms

    DHT11.read(dhtPort);

    byte buffer[8];
    if (tempPrefix.isPrefixOf(interest.getName())) {
        auto temp = (double) DHT11.temperature;
        std::cout << "Temperature: " <<  temp << " C" << std::endl;
        doubleToByte(buffer, temp);
        data.setContent(ndnph::tlv::Value(buffer, 8));
    } else if (humidityPrefix.isPrefixOf(interest.getName())) {
        auto humid = (double) DHT11.humidity;
        std::cout << "Humidity: " << (int) humid << "%" << std::endl;
        doubleToByte(buffer, humid);
        data.setContent(ndnph::tlv::Value(buffer, 8));
    } else {
        return false;   // Invalid arguments
    }

    reply(data.sign(m_signer));
    return true;
}

bool TempHumidServer::processData(ndnph::Data data) {
    return false;
}