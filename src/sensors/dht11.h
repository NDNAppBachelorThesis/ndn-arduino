/*
 *     FILE: dht11.h
 *  VERSION: 0.4.1
 *  PURPOSE: DHT11 Temperature & Humidity Sensor library for Arduino
 *  LICENSE: GPL v3 (http://www.gnu.org/licenses/gpl.html)
 *
 *  DATASHEET: http://www.micro4you.com/files/sensor/DHT11.pdf
 *
 *      URL: http://playground.arduino.cc/Main/DHT11Lib
 *
 *  HISTORY:
 *  George Hadjikyriacou - Original version
 *  see dht.cpp file
 */


#ifndef NDN_ARDUINO_DHT11_H
#define NDN_ARDUINO_DHT11_H


#include <Arduino.h>

#define DHT11LIB_VERSION "0.4.1"

#define DHTLIB_OK                0
#define DHTLIB_ERROR_CHECKSUM    -1
#define DHTLIB_ERROR_TIMEOUT    -2

class dht11 {
public:
    int read(int pin);

    int humidity;
    int temperature;
};

#endif //NDN_ARDUINO_DHT11_H
