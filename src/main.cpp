#include <esp8266ndn.h>
#include "utils/wifi_utils.h"
#include <WiFiClientSecure.h>
#include "servers/TempHumidServer.h"
#include "clients/DiscoveryClient.h"
#include "sensors/dht11.h"
#include "servers/MotionServer.h"
#include "update/HttpUpdater.hpp"
#include <HTTPClient.h>
#include <sstream>
#include "libs/ArduinoJson.h"
#include "utils/WifiClientFixed.h"
#include "utils/Logger.h"


#define DHT_SENSOR_PIN 16
#define MOTION_SENSOR_PIN 17
#define SERVER_TYPE 0   // 0 = no sensor, 1 = Temp/Humid, 2 = Motion
#define MGMT_URL "http://192.168.178.119:8000"


WifiClientFixed* wifiClient = new WifiClientFixed();


ndnph::StaticRegion<1024> region;

std::array<uint8_t, esp8266ndn::UdpTransport::DefaultMtu> udpBuffer;
esp8266ndn::UdpTransport transport(udpBuffer);
ndnph::Face face(transport);

const uint64_t deviceId = ESP.getEfuseMac();

#if SERVER_TYPE == 0
// Nothing to do
#elif SERVER_TYPE == 1
TempHumidServer server(face, ndnph::Name::parse(region, "/esp/2/data"), DHT_SENSOR_PIN);
#elif SERVER_TYPE == 2
MotionServer server(face, ndnph::Name::parse(region, "/esp/3/data"), MOTION_SENSOR_PIN);
#else
#error "Please configure a valid sensor"
#endif

DiscoveryClient discoveryClient(face, ndnph::Name::parse(region, ("/esp/discovery")), {
        "/esp/" + std::to_string(deviceId),
});

HttpUpdater httpUpdater;


/**
 * The Function for the IpAddress class just doesn't work
 * @return
 */
std::string ipToString() {
    auto ip = WiFi.localIP();
    std::stringstream ss;
    ss << (int)ip[0] << "." << (int) ip[1] << "." << (int) ip[2] << "." << (int) ip[3];

    return ss.str();
}


void sendPing() {
    auto deviceId = ESP.getEfuseMac();
    HTTPClient http;
    std::stringstream ss;
    ss << MGMT_URL;
    ss << "/ping/";
    ss << deviceId;

    http.begin(*wifiClient, ss.str().c_str());
    int responseCode = http.POST("PING");

    if (responseCode != 200) {
        LOG_ERROR("Ping returned status code %d", &responseCode);
    }

    http.end();
}

void registerBoard() {
    HTTPClient http;
    std::stringstream ss;
    ss << MGMT_URL;
    ss << "/register";

    StaticJsonDocument<200> doc;
    doc["deviceId"] = deviceId;
    doc["ip"] = ipToString();
    uint8_t jsonBuffer[200];
    auto serializedSize = serializeJson(doc, jsonBuffer, 200);

    http.begin(*wifiClient, ss.str().c_str());
    int responseCode = http.POST(jsonBuffer, serializedSize);

    if (responseCode != 200) {
        LOG_ERROR("Register returned status code %d. Restarting...", &responseCode);

        http.end();
        ESP.restart();
    }

    // Printing uint64_t is not supported. That's why this workaround is required
    LOG_INFO("Successfully registered this board with ID %s and IP %s", std::to_string(deviceId).c_str(), ipToString().c_str());
    http.end();
}

std::string getNFDIP() {
    HTTPClient http;
    http.begin(*wifiClient, (std::string(MGMT_URL) + std::string("/settingsApi")).c_str());

    int responseCode = http.GET();

    if (responseCode != 200) {
        std::cerr << "Getting NFD IP from management server returned status " << responseCode << ". Restarting..." << std::endl;
        http.end();
        ESP.restart();
    }

    StaticJsonDocument<200> doc;
    auto error = deserializeJson(doc, http.getString());

    if (error) {
        std::cerr << "Couldn't fetch NFD IP from server. Restarting..." << std::endl;
        http.end();
        ESP.restart();
    }

    http.end();
    return std::string(doc["ndfIp"].as<std::string>());
}


void setup() {
    Serial.begin(115200);
    Serial.println();
    esp8266ndn::setLogOutput(Serial);

#if SERVER_TYPE == 2
    pinMode(MOTION_SENSOR_PIN, INPUT);
#endif

    enableAndConnectToWifi();

    LOG_INFO("nfdc face create udp4://%s:6363", ipToString().c_str());

    WiFiClientSecure fchSocketClient;
    fchSocketClient.setInsecure();

    auto nfdIp = getNFDIP();
    auto ip = IPAddress();

    if (!ip.fromString(nfdIp.c_str())) {
        LOG_ERROR("Failed to parse NFD ip");
        ESP.restart();
    } else {
        if (!transport.beginTunnel(ip)) {
            LOG_ERROR("UDP tunnel connection failed");
        } else {
            LOG_INFO("Tunnel successful");
        }
    }

    registerBoard();
    httpUpdater.setup();

    LOG_INFO("Setup done.");
}

int counter = 0;
uint16_t pingCounter = 0;

void loop() {
    httpUpdater.run();
    face.loop();

//    if (++counter >= 2500) {
//        counter = 0;
//        discoveryClient.sendInterest();
//    }

    if (++pingCounter % 1000 == 0) {
        sendPing();
    }

    delay(1);
}