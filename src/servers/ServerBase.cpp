//
// Created by arne- on 10.12.2023.
//

#include "ServerBase.h"
#include "utils/Logger.h"

ServerBase::~ServerBase() = default;

void ServerBase::loop() {
    auto now = ndnph::port::Clock::now();
    if (ndnph::port::Clock::isBefore(now, m_next)) {
        return;
    }
    sendAllAutoInterests();
    m_next = ndnph::port::Clock::add(now, autoSendDelay);
}

bool ServerBase::sendAutoInterest(const std::string& nameSuffix, const std::function<void(byte*)>& getData) {
    LOG_INFO("Sending data to Fiware-Orion for %s", getServiceName().c_str());

    auto interestNameStr = "/esp/fiware/" + std::to_string(deviceId) + "/" + nameSuffix;
    ndnph::StaticRegion<1024> region;
    auto name = ndnph::Name::parse(region, interestNameStr.c_str());
    NDNPH_ASSERT(!!name);
    ndnph::Interest interest = region.create<ndnph::Interest>();
    NDNPH_ASSERT(!!interest);

    byte buffer[8];
    getData(buffer);

    auto dataComp = ndnph::Component::from(region, ndnph::TT::GenericNameComponent, ndnph::tlv::Value(buffer, 8));
    interest.setName(name.append(region, dataComp));
    interest.setMustBeFresh(true);

    if (!send(interest)) {
        return false;
    }

    return true;
}

bool ServerBase::processInterest(ndnph::Interest interest) {
    const auto &name = interest.getName();
    if (!namePrefix.isPrefixOf(name)) {
        return false;
    }

    LOG_INFO("Received interest in %s", getServiceName().c_str());

    ndnph::StaticRegion<1024> region;
    ndnph::Data data = region.create<ndnph::Data>();
    NDNPH_ASSERT(!!data);
    data.setName(name);
    data.setFreshnessPeriod(1000);  // Packet is only valid for 1000ms

    byte buffer[8];
    createInterestResponseData(region, name, buffer);
    data.setContent(ndnph::tlv::Value(buffer, 8));

    reply(data.sign(m_signer));
    return true;
}

