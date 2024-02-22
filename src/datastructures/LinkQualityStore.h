//
// Created by arne- on 13.02.2024.
//

#ifndef NDN_ARDUINO_LINKQUALITYSTORE_H
#define NDN_ARDUINO_LINKQUALITYSTORE_H

#include <cstdint>
#include <unordered_map>
#include <ndnph/port/clock/port.hpp>
#include "datastructures/RingBuffer.h"

/**
 * Stores the link quality for a single adjacent ESP32 board
 */
class LinkQuality {
public:
    LinkQuality() = default;

    /**
     * Calculate the link quality based off the stored timestamps
     * @return Link quality in percent in range of [0.0, 1.0]
     */
    float calculateLinkQuality();

    // To be able to determined when the device no longer exists and can be removed from the link quality
    uint64_t lastAddTime = 0;
    RingBuffer<32> timestampBuffer;
};


/**
 * Stores the link qualities for all adjacent ESP32 boards
 */
class LinkQualityStore {
public:
    LinkQualityStore() = default;
    virtual ~LinkQualityStore();

    /**
     * Receive a new timestamp from a device
     */
    void receiveFromDevice(uint64_t deviceId, uint64_t timestamp);
    /**
     * Returns the link quality for a device
     */
    float linkQuality(uint64_t deviceId);

    std::unordered_map<uint64_t, LinkQuality*> qualityMap;

private:
    /**
     * Cleans outdated devices from the qualityMap
     */
    void cleanQualityMap();

    /**
     * Ensures an entry for a device exists in the qualityMap
     */
    LinkQuality* ensureDeviceExists(uint64_t deviceId);

    ndnph::port::Clock::Time nextPurgeTime;
};


#endif //NDN_ARDUINO_LINKQUALITYSTORE_H
