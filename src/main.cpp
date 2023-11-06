#include <WiFi.h>
#include <esp8266ndn.h>

#ifndef WLAN_SSID
#define WLAN_SSID "ssid"
#endif

#ifndef WLAN_PASSWORD
#define WLAN_PASSWORD "password"
#endif

const char *WIFI_SSID = WLAN_SSID;
const char *WIFI_PASS = WLAN_PASSWORD;

ndnph::StaticRegion<1024> region;

esp8266ndn::EthernetTransport transport0;
ndnph::Face face0(transport0);
const char *PREFIX0 = "/example/esp8266/ether/ping";
ndnph::PingServer server0(ndnph::Name::parse(region, PREFIX0), face0);

std::array<uint8_t, esp8266ndn::UdpTransport::DefaultMtu> udpBuffer;
esp8266ndn::UdpTransport transport1(udpBuffer);
ndnph::Face face1(transport1);
const char *PREFIX1 = "/example/esp8266/udp/ping";
ndnph::PingServer server1(ndnph::Name::parse(region, PREFIX1), face1);

esp8266ndn::UdpTransport transport2(udpBuffer);
ndnph::transport::ForceEndpointId transport2w(transport2);
ndnph::Face face2(transport2w);
const char *PREFIX2 = "/example/esp8266/udpm/ping";
ndnph::PingServer server2(ndnph::Name::parse(region, PREFIX2), face2);

void setup() {
    Serial.begin(115200);
    Serial.println();
    esp8266ndn::setLogOutput(Serial);

    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println(F("WiFi connect failed"));
        ESP.restart();
    }

    // Wait for connection to start
    delay(1000);

    esp8266ndn::EthernetTransport::listNetifs(Serial);
    bool ok = transport0.begin(); // select any STA netif
    if (!ok) {
        Serial.println(F("Ethernet transport initialization failed"));
        ESP.restart();
    }

    ok = transport1.beginListen();
    if (!ok) {
        Serial.println(F("UDP unicast transport initialization failed"));
        ESP.restart();
    }

    ok = transport2.beginMulticast(WiFi.localIP());
    if (!ok) {
        Serial.println(F("UDP multicast transport initialization failed"));
        ESP.restart();
    }

    Serial.println(F("Please register prefixes on your router:"));
    Serial.println(F("nfdc route add /example/esp8266/ether [ETHER-MCAST-FACEID]"));

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
    face0.loop();
    face1.loop();
    face2.loop();
    delay(1);
}