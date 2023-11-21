#include "wifi_utils.h"
#include "Logger.h"

/**
 * Enabled and connects to a WiFi network
 */
void enableAndConnectToWifi() {
    const char *WIFI_SSID = WLAN_SSID;
    const char *WIFI_PASS = WLAN_PASSWORD;

    LOG_INFO("[WIFI] Trying to connect to wifi network %s...", WIFI_SSID);
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        LOG_ERROR("[WIFI] WiFi connect failed");
        ESP.restart();
    }

    // Wait for connection to start
    delay(1000);
    LOG_INFO("[WIFI] Connected to wifi network %s", WIFI_SSID);
}