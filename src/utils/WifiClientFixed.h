//
// Created by arne- on 20.11.2023.
//

#ifndef NDN_ARDUINO_WIFIFIX_H
#define NDN_ARDUINO_WIFIFIX_H

#include <WiFi.h>

/**
 * Normal wifiClient has a bug where it prints a lot of meaningless error messages. This fixes it
 * Source: https://github.com/espressif/arduino-esp32/issues/6129#issuecomment-1418051304
 */
class WifiClientFixed : public WiFiClient {
public:
    void flush() override;
};


#endif //NDN_ARDUINO_WIFIFIX_H
