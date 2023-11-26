//
// Created by arne- on 21.11.2023.
//

#include "Logger.h"

Logger Logger::instance;    // Allocate memory for static variable

std::string nameToString(const void *name) {
    std::stringstream ss;
    ss << name;

    return ss.str();
}