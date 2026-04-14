#include "FeedbackDevice.hpp"

FeedbackDevice::FeedbackDevice(int pinFbDevice)
: pin(pinFbDevice)
{}

bool FeedbackDevice::getState() const {
    return state;
}

unsigned long FeedbackDevice::getTimeTurnedOn() const {
    return turnedOnTime;
}

void FeedbackDevice::setup() {
    pinMode(pin, OUTPUT);
}

void FeedbackDevice::turnOn() {
    digitalWrite(pin, HIGH);
    turnedOnTime = millis();
}

void FeedbackDevice::turnOff() {
    digitalWrite(pin, LOW);
}