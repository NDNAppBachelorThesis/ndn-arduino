#ifndef UNIT_CONVERSIONS_HPP
#define UNIT_CONVERSIONS_HPP

#include <Arduino.h>

void floatToByte(byte* buf, float value);
void doubleToByte(byte* buf, double value);

#endif