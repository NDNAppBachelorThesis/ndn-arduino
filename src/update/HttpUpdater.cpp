//
// Created by arne- on 14.11.2023.
//

#include "HttpUpdater.hpp"
#include "utils/Logger.h"
#include <iostream>
#include <sstream>


WebServer HttpUpdater::server;  // Allocate memory for static variable


void HttpUpdater::setup() {
    // Return logs
    server.on("/log", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        std::stringstream ss;
        for (auto& line : Logger::instance.buffer) {
            ss << line;
        }
        server.send(200, "text/plain", ss.str().c_str());
    });

    // Must answer to allow CORS requests
    server.on("/update", HTTP_OPTIONS, []() {
        server.sendHeader("Connection", "close");
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "text/plain", "OK");
    });
    // handling uploading firmware file
    server.on("/update", HTTP_POST, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
    }, []() {
        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
            std::cout << "---------- Update starting ----------" << std::endl;
            Serial.printf("Filename : %s\n", upload.filename.c_str());
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            /* flashing firmware to ESP*/
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) { //true to set the size to the current progress
                std::cout << "Rebooting: true" << std::endl;
                std::cout << "---------- Update Successful ----------" << std::endl;
            } else {
                Update.printError(Serial);
            }
        }
    });

    server.begin();
    setupDone = true;
}
