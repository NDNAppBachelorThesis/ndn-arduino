//
// Created by arne- on 10.12.2023.
//

#include "Ultrasonic.h"
#include "Arduino.h"

int Ultrasonic::getDistance() {
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    unsigned long duration = pulseIn(echoPin, HIGH);
    return static_cast<int>(duration * 0.034 / 2);
}
