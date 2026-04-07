#include "FeedbackDevice.hpp"

FeedbackDevice::FeedbackDevice(int fbDevicePin) {
    pin = fbDevicePin;
}

bool FeedbackDevice::getTurnedOn() {
    return turnedOn;
}

void FeedbackDevice::setup() {
    pinMode(pin, OUTPUT);
}

void FeedbackDevice::turnOn() {
    digitalWrite(pin, HIGH);
    turnedOn = true;
}

void FeedbackDevice::turnOff() {
    digitalWrite(pin, LOW);
    turnedOn = false;
}

void FeedbackDevice::turnOnFor(bool shouldTurnOn
    , unsigned long duration) {
    if (shouldTurnOn && !turnedOn) {
        turnOn();
        endTime = millis() + duration;
    }
    if (turnedOn && millis() >= endTime) {
        turnOff();
    }
}