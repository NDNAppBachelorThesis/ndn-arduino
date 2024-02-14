//
// Created by arne- on 13.02.2024.
//

#include "LinkQualityStore.h"
#include <esp_timer.h>
#include <vector>


// ----------  LinkQuality Class  ----------


float LinkQuality::calculateLinkQuality() {
    const uint32_t expectedDelay = 1000;
    auto averageDelay = timestampBuffer.calculateAverageDelay();

    if (averageDelay == 0) {
        return 0.0;
    }
    return std::min<float>((float) expectedDelay / (float) averageDelay, 1.0f);
}

LinkQuality *LinkQualityStore::ensureDeviceExists(uint64_t deviceId) {
    if (qualityMap.find(deviceId) == qualityMap.end()) {
        qualityMap[deviceId] = new LinkQuality();
    }

    return qualityMap[deviceId];
}


// ----------  LinkQualityStore Class  ----------


LinkQualityStore::~LinkQualityStore() {
    for (const auto &item: qualityMap) {
        delete item.second;
    }
}

void LinkQualityStore::receiveFromDevice(uint64_t deviceId, uint64_t timestamp) {
    auto quality = ensureDeviceExists(deviceId);

    quality->timestampBuffer.add(timestamp);
    quality->lastAddTime = esp_timer_get_time() / 1000;

    cleanQualityMap();
}

float LinkQualityStore::linkQuality(uint64_t deviceId) {
    auto quality = ensureDeviceExists(deviceId);

    return quality->calculateLinkQuality();
}

void LinkQualityStore::cleanQualityMap() {
    // Only run this every 60 seconds
    auto now = ndnph::port::Clock::now();
    if (ndnph::port::Clock::isBefore(now, nextPurgeTime)) {
        return;
    }
    nextPurgeTime = ndnph::port::Clock::add(now, 60000);

    auto currentTime = esp_timer_get_time() / 1000;
    std::vector<uint64_t> toRemove;
    for (const auto &item: qualityMap) {
        if (currentTime - item.second->lastAddTime > 1000 * 60 * 60) {  // 1 hour
            toRemove.push_back(item.first);
        }
    }

    for (const auto &item: toRemove) {
        qualityMap.erase(item);
    }

}
