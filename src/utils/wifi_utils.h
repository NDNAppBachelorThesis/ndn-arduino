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

#ifndef WLAN_SSID2
#define WLAN_SSID2 "ssid2"
#endif

#ifndef WLAN_PASSWORD2
#define WLAN_PASSWORD2 "password2"
#endif

/**
 * Enabled and connects to a WiFi network
 */
void enableAndConnectToWifi();

#endif