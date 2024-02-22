//
// Created by arne- on 10.12.2023.
//

#ifndef NDN_ARDUINO_ULTRASONIC_H
#define NDN_ARDUINO_ULTRASONIC_H

/**
 * Class for interacting with the ultrasonic sensor
 */
class Ultrasonic {
public:
    explicit Ultrasonic(int triggerPin, int echoPin): triggerPin(triggerPin), echoPin(echoPin) {}

    int getDistance();

private:
    int triggerPin;
    int echoPin;
};


#endif //NDN_ARDUINO_ULTRASONIC_H
