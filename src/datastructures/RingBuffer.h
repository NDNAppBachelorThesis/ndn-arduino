//
// Created by arne- on 07.02.2024.
//

#ifndef NDN_ARDUINO_RINGBUFFER_H
#define NDN_ARDUINO_RINGBUFFER_H

#include <cstdint>
#include <cstring>
#include <iostream>


/**
 * A ringbuffer, which acts like a queue with a fixed size where the oldest element gets evicted when the capacity is
 * full. Focuses on performance.
 */
template<uint32_t SIZE>
class RingBuffer {
public:
    explicit RingBuffer() {
        std::memset(data, 0, SIZE);
    };

    void add(uint64_t value) {
        data[position] = value;
        position = (position + 1) % SIZE;
    }

    /**
     * Returns how many entries of the capacity are actually used.
     */
    uint32_t usedSize() {
        uint32_t size = 0;

        for (uint32_t i = 0; i < SIZE; i++) {
            size += data[i] != 0;
        }

        return size;
    }

    /**
     * Returns the oldest element
     */
    uint64_t first() {
        if (usedSize() != SIZE) {
            return data[0];
        }
        return data[position];
    }

    /**
     * Returns the newest element
     */
    uint64_t last() {
        if (position == 0) {
            return data[SIZE - 1];
        }
        return data[position - 1];
    }

    /**
     * Calculates the average delay between two received timestamps
     */
    uint64_t calculateAverageDelay() {
        auto tsLast = last();
        auto tsFirst = first();
        auto size = usedSize();

        if (size == 1) {
            return tsLast - tsFirst;
        }

        // Must subtract 1 because I want the average of the differences, not the overall average
        return (tsLast - tsFirst) / (size - 1);
    }

private:
    uint32_t position = 0;
    uint64_t data[SIZE]{};
};


#endif //NDN_ARDUINO_RINGBUFFER_H
