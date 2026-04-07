#ifndef FEEDBACK_DEVICE_HPP
#define FEEDBACK_DEVICE_HPP

#include <Arduino.h>

class FeedbackDevice {
public:
    FeedbackDevice(int fbDevicePin);

    bool getTurnedOn();

    void setup();

    void turnOn();

    void turnOff();

    // Turns the feedback device on for a duration
    // Needs the be called in loop() to function properly
    void turnOnFor(bool shouldTurnOn=false, unsigned long duration=1000);

private:
    int pin;

    bool turnedOn = false;
    unsigned long endTime = 0;
};

#endif //FEEDBACK_DEVICE_HPP