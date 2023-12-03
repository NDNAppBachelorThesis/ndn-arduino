//
// Created by arne- on 14.11.2023.
//

#ifndef NDN_ARDUINO_HTTPUPDATER_HPP
#define NDN_ARDUINO_HTTPUPDATER_HPP

#include <WebServer.h>
#include <Update.h>

class HttpUpdater {
public:
    HttpUpdater() = default;

    void setup();
    void run() const {
        assert(setupDone);
        server.handleClient();
    };

private:
    bool setupDone = false;
    static WebServer server;    // Must be static to be usable in lambda
};

#endif //NDN_ARDUINO_HTTPUPDATER_HPP
