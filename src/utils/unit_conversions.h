#ifndef UNIT_CONVERSIONS_HPP
#define UNIT_CONVERSIONS_HPP

#include <Arduino.h>

/**
 * Converts a float into a byte array
 */
void floatToByte(byte* buf, float value);
/**
 * Converts a double into a byte array
 */
void doubleToByte(byte* buf, double value);
/**
 * Converts a uint64_t into a byte array
 */
void uint64ToByte(byte* buf, uint64_t value);
/**
 * Converts bytes back to a uint64_t instance
 */
uint64_t bytesToUint64(byte* buf);

#endif  // UNIT_CONVERSIONS_HPP
