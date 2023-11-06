#ifndef WIFI_UTILS_HPP
#define WIFI_UTILS_HPP

#include <WiFi.h>
#include <iostream>

#ifndef WLAN_SSID
#define WLAN_SSID "ssid"
#endif

#ifndef WLAN_PASSWORD
#define WLAN_PASSWORD "password"
#endif

/**
 * Enabled and connects to a WiFi network
 */
void enableAndConnectToWifi() {
    const char *WIFI_SSID = WLAN_SSID;
    const char *WIFI_PASS = WLAN_PASSWORD;

    std::cout << "[WIFI] Trying to connect to wifi network " << WIFI_SSID << "..." << std::endl;
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println(F("[WIFI] WiFi connect failed"));
        ESP.restart();
    }

    // Wait for connection to start
    delay(1000);
    std::cout << "[WIFI] Connected to wifi network " << WIFI_SSID << std::endl;

}

#endif