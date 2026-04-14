#ifndef FEEDBACK_DEVICE_HPP
#define FEEDBACK_DEVICE_HPP

#include <Arduino.h>


class FeedbackDevice {
public:
    FeedbackDevice(int pinFbDevice);  

    bool getState() const;

    unsigned long getTimeTurnedOn() const;

    void setup();

    void turnOn();

    void turnOff();
private:
    const int pin;

    bool state = false;

    unsigned long turnedOnTime = 0;
};

#endif //FEEDBACK_DEVICE_HPP