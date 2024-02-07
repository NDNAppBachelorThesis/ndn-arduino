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

    void add(uint16_t value) {
        data[position] = value;
        position = (position + 1) % SIZE;
    }

    uint16_t calculateAverage() {
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

private:
    uint32_t position = 0;
    uint16_t data[SIZE]{};
};


#endif //NDN_ARDUINO_RINGBUFFER_H
