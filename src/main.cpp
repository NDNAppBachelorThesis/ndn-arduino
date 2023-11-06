#include <esp8266ndn.h>
#include "wifi_utils.hpp"
#include "servers/TempHumidServer.h"
#include "sensors/dht11.h"

#define DHTPIN 16

ndnph::StaticRegion<1024> region;

std::array<uint8_t, esp8266ndn::UdpTransport::DefaultMtu> udpBuffer;
esp8266ndn::UdpTransport transport(udpBuffer);
ndnph::Face face(transport);
TempHumidServer server(face, ndnph::Name::parse(region, "/esp/2/data"), DHTPIN);


void setup() {
    Serial.begin(115200);
    Serial.println();
    esp8266ndn::setLogOutput(Serial);

    enableAndConnectToWifi();

    bool ok = transport.beginListen();
    if (!ok) {
        Serial.println(F("UDP unicast transport initialization failed"));
        ESP.restart();
    }

    Serial.print(F("nfdc face create udp4://"));
    Serial.print(WiFi.localIP());
    Serial.println(F(":6363"));
    Serial.println();
}

void loop() {
    face.loop();
    delay(1);
}