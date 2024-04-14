#include "wifi_utils.h"
#include "Logger.h"


bool _tryConnectToWiFi(const char *WIFI_SSID, const char *WIFI_PASS) {
    LOG_INFO("[WIFI] Trying to connect to wifi network '%s'...", WIFI_SSID);
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        LOG_ERROR("[WIFI] WiFi connect to '%s' failed.", WIFI_SSID);
        return false;
    }

    // Wait for connection to start
    delay(1000);
    LOG_INFO("[WIFI] Connected to wifi network '%s'", WIFI_SSID);

    return true;
}


/**
 * Enabled and connects to a WiFi network
 */
void enableAndConnectToWifi() {
    if (_tryConnectToWiFi(WLAN_SSID, WLAN_PASSWORD)) {
        return;
    }

    LOG_INFO("Connection failed. Trying next network...");
    WiFi.disconnect(true, true);
    delay(250);

    if (_tryConnectToWiFi(WLAN_SSID2, WLAN_PASSWORD2)) {
        return;
    }

    // Reboot on failure
    ESP.restart();
}