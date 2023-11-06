#include <esp8266ndn.h>
#include <iostream>

#include "TestServer.h"
#include "wifi_utils.hpp"

ndnph::StaticRegion<1024> region;

std::array<uint8_t, esp8266ndn::UdpTransport::DefaultMtu> udpBuffer;
esp8266ndn::UdpTransport transport(udpBuffer);
ndnph::Face face(transport);
const char *PREFIX = "/esp/test";
TestServer server(ndnph::Name::parse(region, PREFIX), face);


void setup() {
    Serial.begin(115200);
    Serial.println();
    esp8266ndn::setLogOutput(Serial);

    enableAndConnectToWifi();

    bool ok = transport.beginListen(WiFi.localIP());
    if (!ok) {
        Serial.println(F("UDP multicast initialization failed"));
        ESP.restart();
    }

    Serial.print(F("nfdc face create udp4://"));
    Serial.print(WiFi.localIP());
    Serial.println(F(":6363"));

    Serial.println(F("nfdc route add /example/esp8266/udp [UDP-UNICAST-FACEID]"));
    Serial.println(F("nfdc route add /example/esp8266/udpm [UDP-MCAST-FACEID]"));
    Serial.println();
    Serial.println(F("Then you can ping:"));
    Serial.println(F("ndnping /example/esp8266/ether"));
    Serial.println(F("ndnping /example/esp8266/udp"));
    Serial.println(F("ndnping /example/esp8266/udpm"));
    Serial.println();

}

void loop() {
    face.loop();
    delay(1);
}