//
// Created by arne- on 06.11.2023.
//

#include "TempHumidServer.h"
#include "iostream"
#include "utils/unit_conversions.h"


void TempHumidServer::sendAllAutoInterests() {
    auto dht = this->DHT11;
    dht.read(dhtPort);

    sendAutoInterest("value/temp", [dht](byte *buffer) {
        auto temp = (double) dht.temperature;
//        std::cout << "Temperature: " << temp << " C" << std::endl;
        doubleToByte(buffer, temp);
    });
    sendAutoInterest("value/humid", [dht](byte *buffer) {
        auto humid = (double) dht.humidity;
//        std::cout << "Humidity: " << (int) humid << "%" << std::endl;
        doubleToByte(buffer, humid);
    });
}

std::string TempHumidServer::getServiceName() {
    return "TempHumidServer";
}

bool TempHumidServer::createInterestResponseData(ndnph::StaticRegion<1024> &region, const ndnph::Name &name, byte *buffer) {
    auto tempPrefix = namePrefix.append(region, ndnph::Component::parse(region, "temperature"));
    auto humidityPrefix = namePrefix.append(region, ndnph::Component::parse(region, "humidity"));

    DHT11.read(dhtPort);

    if (tempPrefix.isPrefixOf(name)) {
        auto temp = (double) DHT11.temperature;
        std::cout << "Temperature: " <<  temp << " C" << std::endl;
        doubleToByte(buffer, temp);

    } else if (humidityPrefix.isPrefixOf(name)) {
        auto humid = (double) DHT11.humidity;
        std::cout << "Humidity: " << (int) humid << "%" << std::endl;
        doubleToByte(buffer, humid);

    } else {
        return false;   // Invalid arguments
    }

    return true;
}
