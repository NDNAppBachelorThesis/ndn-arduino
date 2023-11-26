//
// Created by arne- on 07.11.2023.
//

#include "MotionServer.h"
#include <iostream>
#include <ndnph/tlv/value.hpp>
#include "utils/unit_conversions.h"
#include "utils/Logger.h"


bool MotionServer::processInterest(ndnph::Interest interest) {
    const auto &name = interest.getName();
    if (!namePrefix.isPrefixOf(name)) {
        return false;
    }

    LOG_INFO("Received interest in MotionServer");

    ndnph::StaticRegion<1024> region;

    ndnph::Data data = region.create<ndnph::Data>();
    NDNPH_ASSERT(!!data);
    data.setName(interest.getName());
    data.setFreshnessPeriod(1000);  // Packet is only valid for 1000ms

    uint8_t value = digitalRead(sensorPin);
    std::cout << "Motion value " << (int) value << std::endl;
    byte buffer[8];
    doubleToByte(buffer, (double) value);
    data.setContent(ndnph::tlv::Value(buffer, 8));

    reply(data.sign(m_signer));
    return true;
}

bool MotionServer::processData(ndnph::Data data) {
    return false;
}