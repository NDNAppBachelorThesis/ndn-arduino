//
// Created by arne- on 14.11.2023.
//

#ifndef NDN_ARDUINO_HTTPUPDATER_HPP
#define NDN_ARDUINO_HTTPUPDATER_HPP

#include <WebServer.h>
#include <Update.h>
#include "Preferences.h"

class HttpUpdater {
public:
    HttpUpdater() = default;

    void setup();
    void run() {
        assert(setupDone);
        server.handleClient();
    };


private:
    bool getMustConfirmUpdate() {
        return preferences.getBool("confirmUpdate");
    }
    void setMustConfirmUpdate(bool mustConfirm) {
        preferences.putBool("confirmUpdate", mustConfirm);
    }

    bool setupDone = false;
    static WebServer server;    // Must be static to be usable in lambda
    Preferences preferences;
};

#endif //NDN_ARDUINO_HTTPUPDATER_HPP
