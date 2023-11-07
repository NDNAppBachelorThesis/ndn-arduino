//
// Created by arne- on 06.11.2023.
//

#include "TempHumidServer.h"
#include "iostream"
#include <ndnph/tlv/value.hpp>
#include <sstream>


void floatToByte(byte* buf, float value) {
    long l = *(long*) &value;

    for (int i = 0; i < 4; i++) {
        buf[i] = (l >> (8 * i)) & 0xFF;
    }

}

void doubleToByte(byte* buf, double value) {
    long long l = *(long long*) &value;

    for (int i = 0; i < 8; i++) {
        buf[i] = (l >> (8 * i)) & 0xFF;
    }
}


bool TempHumidServer::processInterest(ndnph::Interest interest) {
    if (!namePrefix.isPrefixOf(interest.getName())) {
        return false;
    }

    std::cout << "Processing " << interest.getName() << std::endl;

    ndnph::StaticRegion<1024> region;

    auto tempPrefix = namePrefix.append(region, ndnph::Component::parse(region, "temperature"));
    auto humidityPrefix = namePrefix.append(region, ndnph::Component::parse(region, "humidity"));

    ndnph::Data data = region.create<ndnph::Data>();
    NDNPH_ASSERT(!!data);
    data.setName(interest.getName());
    data.setFreshnessPeriod(1);

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