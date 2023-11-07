#include <esp8266ndn.h>
#include "utils/wifi_utils.h"
#include "servers/TempHumidServer.h"
#include "sensors/dht11.h"
#include "servers/MotionServer.h"

#define DHT_SENSOR_PIN 16
#define MOTION_SENSOR_PIN 17
#define SERVER_TYPE 2   // 1 = Temp/Humid, 2 = Motion

ndnph::StaticRegion<1024> region;

std::array<uint8_t, esp8266ndn::UdpTransport::DefaultMtu> udpBuffer;
esp8266ndn::UdpTransport transport(udpBuffer);
ndnph::Face face(transport);
#if SERVER_TYPE == 1
TempHumidServer server(face, ndnph::Name::parse(region, "/esp/2/data"), DHT_SENSOR_PIN);
#elif SERVER_TYPE == 2
MotionServer server(face, ndnph::Name::parse(region, "/esp/3/data"), MOTION_SENSOR_PIN);
#else
#error "Please configure a valid sensor"
#endif


void setup() {
    Serial.begin(115200);
    Serial.println();
    esp8266ndn::setLogOutput(Serial);

#if SERVER_TYPE == 2
    pinMode(MOTION_SENSOR_PIN, INPUT);
#endif

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