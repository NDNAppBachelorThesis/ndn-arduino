//
// Created by arne- on 21.11.2023.
//

#include "LogBuffer.h"

void LogBuffer::push(const std::basic_string<char> &x) {
    if (size() >= limit) {
        pop();
    }

    queue::push(x);
}

void LogBuffer::push(std::basic_string<char> &&x) {
    if (size() > limit) {
        pop();
    }

    queue::push(x);
}
