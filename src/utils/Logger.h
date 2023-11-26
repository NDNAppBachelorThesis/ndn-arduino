//
// Created by arne- on 21.11.2023.
//

#ifndef NDN_ARDUINO_LOGGER_H
#define NDN_ARDUINO_LOGGER_H

#include "LogBuffer.h"
#include <cstring>
#include <sstream>


#define LOG_INFO(...) Logger::instance.logInfo(__VA_ARGS__)
#define LOG_ERROR(...) Logger::instance.logError(__VA_ARGS__)

/**
 * Convert ndnph::Name object to string.
 * Can't use type ndnph::Name because including esp8266ndn.h here will break everything.
 */
std::string nameToString(const void* name);

class Logger {
public:
    Logger() = default;

    template<typename Msg_t, typename... Args>
    void _logHelper(FILE *output, const char *prefix, Msg_t rawMsg, Args... args) {
        char* msg = (char*) rawMsg;

        auto seconds = (unsigned) time(NULL);
        char strFormatBuffer[strlen(msg) + 6 + 1];
        std::strcpy(strFormatBuffer, "%08d %s ");
        std::strcat(strFormatBuffer, msg);
        std::strcat(strFormatBuffer, "\n");

        int printedCnt = fprintf(output, strFormatBuffer, seconds, prefix, args...);

        // Also Save to ring buffer
        char strBuffer[printedCnt];
        sprintf(strBuffer, strFormatBuffer, seconds, prefix, args...);
        buffer.push(strBuffer);
    }

    template<typename... Args>
    void logInfo(Args... args) {
        _logHelper(stdout, "[INFO ] ", args...);
    }

    template<typename... Args>
    void logError(Args... args) {
        _logHelper(stderr, "[ERROR] ", args...);
    }

    static Logger instance;
    LogBuffer buffer = 512;
};


#endif //NDN_ARDUINO_LOGGER_H
