//
// Created by arne- on 07.02.2024.
//

#ifndef NDN_ARDUINO_RINGBUFFER_H
#define NDN_ARDUINO_RINGBUFFER_H

#include <cstdint>
#include <cstring>
#include <iostream>


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

    uint32_t usedSize() {
        uint32_t size = 0;

        for (uint32_t i = 0; i < SIZE; i++) {
            size += data[i] != 0;
        }

        return size;
    }

    uint64_t first() {
        if (usedSize() != SIZE) {
            return data[0];
        }
        return data[position];
    }

    uint64_t last() {
        if (position == 0) {
            return data[SIZE - 1];
        }
        return data[position - 1];
    }

    uint64_t calculateAverage() {
        uint64_t res = 0;
        uint32_t count = 0;

        for (size_t i = 0; i < SIZE; i++) {
            if (data[i] != 0) {
                res += data[i];
                count++;
            }
        }

        return res / count;
    }

    uint64_t calculateAverageDelay() {
        auto l = last();
        auto f = first();
        auto s = usedSize();

        if (s == 1) {
            return l - f;
        }

        // Must subtract 1 because I want the average of the differences
        return (l - f) / (s - 1);
    }

private:
    uint32_t position = 0;
    uint64_t data[SIZE]{};
};


#endif //NDN_ARDUINO_RINGBUFFER_H