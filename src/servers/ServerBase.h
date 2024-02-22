//
// Created by arne- on 10.12.2023.
//

#ifndef NDN_ARDUINO_SERVERBASE_H
#define NDN_ARDUINO_SERVERBASE_H

#include <ndnph/face/packet-handler.hpp>
#include <ndnph/keychain/digest.hpp>
#include <ndnph/port/clock/port.hpp>
#include <functional>
#include <utility>
#include <iostream>

/**
 * The base class for all NDN sensor servers
 */
class ServerBase : public ndnph::PacketHandler {
public:
    ServerBase(ndnph::Face &face, ndnph::Name namePrefix, int autoSendDelay) :
            PacketHandler(face),
            m_signer(ndnph::DigestKey::get()),
            namePrefix(std::move(namePrefix)),
            autoSendDelay(autoSendDelay) {}

    ~ServerBase() override = 0;

protected:
    /**
     * Gathers the sensor data and sends the periodic measurements to fiware-orion
     */
    virtual void sendAllAutoInterests() = 0;
    /**
     * Returns the servers name. Should be the class name.
     */
    virtual std::string getServiceName() = 0;
    bool sendAutoInterest(const std::string& nameSuffix, const std::function<void(byte*)>& getData);
    uint32_t generateNonce();

private:
    void loop() override;
    bool processInterest(ndnph::Interest interest) override;
    virtual bool createInterestResponseData(ndnph::StaticRegion<1024> &region, const ndnph::Name &name, byte *buffer) = 0;

    bool processNack(ndnph::Nack nack) override;

protected:
    const ndnph::Name namePrefix;
    const ndnph::PrivateKey &m_signer;
    ndnph::port::Clock::Time m_next;    // Timer for automatically sending messages to Fiware-Orion
    const int autoSendDelay;            // Delay in ms for the auto sends to Fiware-Orion
    const uint64_t deviceId = ESP.getEfuseMac();
};


#endif //NDN_ARDUINO_SERVERBASE_H
