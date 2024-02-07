#include <esp8266ndn.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <sstream>
#include "libs/ArduinoJson.h"
#include "utils/wifi_utils.h"
#include "utils/WifiClientFixed.h"
#include "utils/Logger.h"
#include "update/HttpUpdater.hpp"
#include "sensors/dht11.h"
#include "servers/TempHumidServer.h"
#include "servers/DiscoveryServer.h"
#include "servers/LinkQualityServer.h"
#include "servers/UltrasonicServer.h"


#define DHT_SENSOR_PIN                  16
#define ULTRASONIC_SENSOR_TRIGGER_PIN   38
#define ULTRASONIC_SENSOR_ECHO_PIN      39

#define MGMT_URL "http://192.168.178.179:3000"


WifiClientFixed *wifiClient = new WifiClientFixed();
const uint64_t deviceId = ESP.getEfuseMac();
HttpUpdater httpUpdater;

ndnph::StaticRegion<1024> region;
std::array<uint8_t, esp8266ndn::UdpTransport::DefaultMtu> udpBuffer;
esp8266ndn::UdpTransport transport(udpBuffer);
ndnph::Face face(transport);

TempHumidServer *tempServer = nullptr;
UltrasonicServer *ultrasonicServer = nullptr;

DiscoveryServer discoveryServer(face, ndnph::Name::parse(region, ("/esp/discovery")));
LinkQualityServer linkQualityServer(face, ndnph::Name::parse(region, ("/esp/" + std::to_string(deviceId) +
                                                                      "/linkquality").c_str()));


/**
 * The Function for the IpAddress class just doesn't work
 * @return
 */
std::string ipToString() {
    auto ip = WiFi.localIP();
    std::stringstream ss;
    ss << (int) ip[0] << "." << (int) ip[1] << "." << (int) ip[2] << "." << (int) ip[3];

    return ss.str();
}

/**
 * Sends a ping to the management server to tell it this board is still alive
 */
void sendPing() {
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

/**
 * Registers this board to the management server
 */
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
    LOG_INFO("Successfully registered this board with ID %s and IP %s", std::to_string(deviceId).c_str(),
             ipToString().c_str());
    http.end();
}

/**
 * Requests the NDF ip from the management server
 */
std::string getNFDIP() {
    HTTPClient http;
    http.begin(*wifiClient, (std::string(MGMT_URL) + std::string("/settingsApi")).c_str());

    int responseCode = http.GET();

    if (responseCode != 200) {
        std::cerr << "Getting NFD IP from management server returned status " << responseCode << ". Restarting..."
                  << std::endl;
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


bool isTempSensorPlugged() {
    return digitalRead(DHT_SENSOR_PIN) != 0;
}

bool isUltrasonicSensorPlugged() {
    digitalWrite(ULTRASONIC_SENSOR_TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(ULTRASONIC_SENSOR_TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_SENSOR_TRIGGER_PIN, LOW);

    return pulseIn(ULTRASONIC_SENSOR_ECHO_PIN, HIGH) != 0;
}


ndnph::Name getNdnNameForData() {
    return ndnph::Name::parse(region, ("/esp/" + std::to_string(deviceId) + "/data").c_str());
}


void setup() {
    Serial.begin(115200);
    Serial.println();
    esp8266ndn::setLogOutput(Serial);

    // --- Configure possible pins ---
    pinMode(DHT_SENSOR_PIN, INPUT);
    pinMode(ULTRASONIC_SENSOR_TRIGGER_PIN, OUTPUT);
    pinMode(ULTRASONIC_SENSOR_ECHO_PIN, INPUT);

    if (isTempSensorPlugged()) {
        LOG_INFO("[HW CONFIG] Using Temp/Humid Sensor.");
        tempServer = new TempHumidServer(
                face,
                getNdnNameForData(),
                DHT_SENSOR_PIN
        );
        discoveryServer.addProvidedResource("/esp/" + std::to_string(deviceId) + "/data/temperature");
        discoveryServer.addProvidedResource("/esp/" + std::to_string(deviceId) + "/data/humidity");
    } else if (isUltrasonicSensorPlugged()) {
        LOG_INFO("[HW CONFIG] Using Ultrasonic Sensor.");
        ultrasonicServer = new UltrasonicServer(
                face,
                getNdnNameForData(),
                ULTRASONIC_SENSOR_TRIGGER_PIN,
                ULTRASONIC_SENSOR_ECHO_PIN
        );
        discoveryServer.addProvidedResource("/esp/" + std::to_string(deviceId) + "/data");
    } else {
        LOG_INFO("[HW CONFIG] No sensor configured.");
    }

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

    pinMode(ULTRASONIC_SENSOR_TRIGGER_PIN, OUTPUT);
    pinMode(ULTRASONIC_SENSOR_ECHO_PIN, INPUT);

    LOG_INFO("Setup done.");
}


uint16_t pingCounter = 0;
long long cycleSum = 0;
long cycleCnt = 0;

void loop() {
    auto cnt1 = ESP.getCycleCount();

    httpUpdater.run();
    face.loop();

    auto cnt2 = ESP.getCycleCount();
    cycleSum += (cnt2 - cnt1);
    cycleCnt++;

    if (++pingCounter % 2500 == 0) {
        sendPing();
//        std::cout << "Average cycle count: " << (cycleSum / cycleCnt) << std::endl;
        cycleSum = 0;
        cycleCnt = 0;
    }

    delay(1);
}
