//
// Created by arne- on 11.12.2023.
//

#include "UltrasonicServer.h"
#include "utils/unit_conversions.h"

void UltrasonicServer::sendAllAutoInterests() {
    auto distance = (double) sensor.getDistance();

    sendAutoInterest("value", [distance](byte *buffer) {
        doubleToByte(buffer, distance);
    });
}

std::string UltrasonicServer::getServiceName() {
    return "UltrasonicServer";
}

bool UltrasonicServer::createInterestResponseData(ndnph::StaticRegion<1024> &region, const ndnph::Name &name, byte *buffer) {
    auto distance = (double) sensor.getDistance();
    std::cout << "Distance: " <<  distance << " cm" << std::endl;
    doubleToByte(buffer, distance);

    return true;
}
