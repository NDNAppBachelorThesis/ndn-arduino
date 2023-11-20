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


#define DHT_SENSOR_PIN 16
#define MOTION_SENSOR_PIN 17
#define SERVER_TYPE 0   // 0 = no sensor, 1 = Temp/Humid, 2 = Motion
#define MGMT_URL "http://192.168.178.119:8000"


WifiClientFixed* wifiClient = new WifiClientFixed();


ndnph::StaticRegion<1024> region;

std::array<uint8_t, esp8266ndn::UdpTransport::DefaultMtu> udpBuffer;
esp8266ndn::UdpTransport transport(udpBuffer);
ndnph::Face face(transport);



#if SERVER_TYPE == 0
// Nothing to do
#elif SERVER_TYPE == 1
TempHumidServer server(face, ndnph::Name::parse(region, "/esp/2/data"), DHT_SENSOR_PIN);
#elif SERVER_TYPE == 2
MotionServer server(face, ndnph::Name::parse(region, "/esp/3/data"), MOTION_SENSOR_PIN);
#else
#error "Please configure a valid sensor"
#endif


DiscoveryClient discoveryClient(face, ndnph::Name::parse(region, "/esp/discovery"));
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
        std::cerr << "Ping returned status code " << responseCode << std::endl;
    }

    http.end();
}

void registerBoard() {
    auto deviceId = ESP.getEfuseMac();
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
        std::cerr << "Register returned status code " << responseCode << ". Restarting..." << std::endl;
        http.end();
        ESP.restart();
    }

    std::cout << "Successfully registered this board with ID " << deviceId << " and IP " << ipToString() << std::endl;
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

// Only required when discovery not working
//    bool ok = transport.beginListen();
//    if (!ok) {
//        Serial.println(F("UDP unicast transport initialization failed"));
//        ESP.restart();
//    }

    Serial.print(F("nfdc face create udp4://"));
    Serial.print(WiFi.localIP());
    Serial.println(F(":6363"));
    Serial.println();

    WiFiClientSecure fchSocketClient;
    fchSocketClient.setInsecure();

    auto nfdIp = getNFDIP();
    auto ip = IPAddress();

    if (!ip.fromString(nfdIp.c_str())) {
        std::cerr << "Failed to parse NFD ip" << std::endl;
        ESP.restart();
    } else {
        if (!transport.beginTunnel(ip)) {
            std::cerr << "UDP tunnel connection failed" << std::endl;
        } else {
            std::cout << "Tunnel successful" << std::endl;
        }
    }

    httpUpdater.setup();
    registerBoard();
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