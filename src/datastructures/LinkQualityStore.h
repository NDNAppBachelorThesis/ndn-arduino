//
// Created by arne- on 13.02.2024.
//

#ifndef NDN_ARDUINO_LINKQUALITYSTORE_H
#define NDN_ARDUINO_LINKQUALITYSTORE_H

#include <cstdint>
#include <unordered_map>
#include <ndnph/port/clock/port.hpp>
#include "datastructures/RingBuffer.h"

class LinkQuality {
public:
    LinkQuality() = default;

    float calculateLinkQuality();

    uint64_t lastAddTime = 0;
    RingBuffer<8> timestampBuffer;
};


class LinkQualityStore {
public:
    LinkQualityStore() = default;
    virtual ~LinkQualityStore();

    void receiveFromDevice(uint64_t deviceId, uint64_t timestamp);
    float linkQuality(uint64_t deviceId);

private:
    void cleanQualityMap();

    ndnph::port::Clock::Time nextPurgeTime;
    LinkQuality* ensureDeviceExists(uint64_t deviceId);
    std::unordered_map<uint64_t, LinkQuality*> qualityMap;
};


#endif //NDN_ARDUINO_LINKQUALITYSTORE_H
