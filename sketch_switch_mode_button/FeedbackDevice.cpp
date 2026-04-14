#include "FeedbackDevice.hpp"

FeedbackDevice::FeedbackDevice(int pinFbDevice)
: pin(pinFbDevice)
{}

bool FeedbackDevice::getState() const {
    return state;
}

void FeedbackDevice::setup() {
    pinMode(pin, OUTPUT);
}

void FeedbackDevice::turnOn() {
    digitalWrite(pin, HIGH);
    state = ON;
    stateStartTime = millis();
}

void FeedbackDevice::turnOff() {
    digitalWrite(pin, LOW);
    state = OFF;
    stateStartTime = millis();
}

void FeedbackDevice::directionalFeedback(float rawDist) {
    // Update filtered distance
    filteredDist = smooth(rawDist, filteredDist);
    // Check if the distance is inside the range
    if (filteredDist < MIN_DIST || filteredDist > MAX_DIST) {
        turnOff();
        return;
    }
    // Compute blinking interval
    int interval = map(constrain(filteredDist, MIN_DIST, MAX_DIST), MIN_DIST, MAX_DIST, shortFeedbackPulse, shortFeedbackPulse*10);
    // Save the time
    unsigned long now = millis();
    // Perform switching logic
    switch (state) {
        case ON:
            if (now - stateStartTime >= shortFeedbackPulse) {
                turnOff();   
            }
            break;
        case OFF:
            if (now - stateStartTime >= interval) {
                turnOn();
            }
            break;
        default:    
            turnOff();
            break;
    }
}


bool FeedbackDevice::switchModeFeedback() {
    unsigned long now = millis();
    switch (state) {
        case ON:
            if (now - stateStartTime >= switchModeFbDuration) {
                turnOff();
                return true;
            }
            break;
        case OFF:
            turnOn();
            break;
    }
    return false;
}

void FeedbackDevice::lowBatteryFeedback() {
    unsigned long now = millis();
    switch (state) {
        case ON:
            if (firstTimeLowBatteryFeedback) {
                if (now - stateStartTime >= firstLowBatteryFeedbackDuration) {
                    turnOff();
                    firstTimeLowBatteryFeedback = false;
                }
            }
            else {
                if (now - stateStartTime >= longFeedbackPulse) {
                    turnOff();
                }
            }
            break;
        case OFF:
            if (now - stateStartTime >= longFeedbackPulse) {
                turnOn();
            }
            break;
    }
}

float FeedbackDevice::smooth(float current, float previous) {
    return (SMOOTHING_ALPHA * current) + (1.0 - SMOOTHING_ALPHA) * previous;
}