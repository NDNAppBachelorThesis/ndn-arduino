//
// Created by arne- on 06.11.2023.
//

#ifndef NDN_ARDUINO_TEMPHUMIDSERVER_H
#define NDN_ARDUINO_TEMPHUMIDSERVER_H

#include <utility>

#include "servers/ServerBase.h"
#include "sensors/dht11.h"

class TempHumidServer : public ServerBase {
public:
    explicit TempHumidServer(ndnph::Face &face, ndnph::Name prefix, int dhtPort) :
            ServerBase(face, std::move(prefix), 1000),
            dhtPort(dhtPort)
            {}

protected:
    void sendAllAutoInterests() override;
    std::string getServiceName() override;

private:
    bool createInterestResponseData(ndnph::StaticRegion<1024> &region, const ndnph::Name &name, byte *buffer) override;

public:

    int dhtPort;
    dht11 DHT11{};
};


#endif //NDN_ARDUINO_TEMPHUMIDSERVER_H