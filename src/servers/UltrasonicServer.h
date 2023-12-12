//
// Created by arne- on 11.12.2023.
//

#ifndef NDN_ARDUINO_ULTRASONICSERVER_H
#define NDN_ARDUINO_ULTRASONICSERVER_H

#include <utility>

#include "servers/ServerBase.h"
#include "sensors/Ultrasonic.h"

class UltrasonicServer : public ServerBase {
public:
    explicit UltrasonicServer(ndnph::Face &face, ndnph::Name prefix, int sensorTriggerPin, int sensorEchoPin) :
            ServerBase(face, std::move(prefix), 60000),
            sensor(sensorTriggerPin, sensorEchoPin) {}

protected:
    void sendAllAutoInterests() override;

    std::string getServiceName() override;

private:
    bool createInterestResponseData(ndnph::StaticRegion<1024> &region, const ndnph::Name &name, byte *buffer) override;

public:
    Ultrasonic sensor;
};


#endif //NDN_ARDUINO_ULTRASONICSERVER_H
