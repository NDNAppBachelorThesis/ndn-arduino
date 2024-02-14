#include "unit_conversions.h"
#include <cstring>

void floatToByte(byte *buf, float value) {
    long l = *(long *) &value;

    for (int i = 0; i < 4; i++) {
        buf[i] = (l >> (8 * i)) & 0xFF;
    }

}

void doubleToByte(byte *buf, double value) {
    long long l = *(long long *) &value;

    for (int i = 0; i < 8; i++) {
        buf[i] = (l >> (8 * i)) & 0xFF;
    }
}

void uint64ToByte(byte* buf, uint64_t value) {
//    long long l = *(long long *) &value;
//
//    for (int i = 0; i < 8; i++) {
//        buf[i] = (l >> (8 * i)) & 0xFF;
//    }
    auto tmp = reinterpret_cast<char*>(&value);
    std::memcpy(buf, tmp, 8);
}

uint64_t bytesToUint64(byte* buf) {
    return *reinterpret_cast<uint64_t*>(buf);
}