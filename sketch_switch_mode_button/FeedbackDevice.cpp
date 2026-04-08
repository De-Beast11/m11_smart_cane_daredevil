#include "FeedbackDevice.hpp"

FeedbackDevice::FeedbackDevice(int fbDevicePin, feedbackMode fbMode) {
    pin = fbDevicePin;
    deviceFeedbackMode = fbMode;
}

bool FeedbackDevice::getState() {
    return state;
}

void FeedbackDevice::setup() {
    pinMode(pin, OUTPUT);
}

void FeedbackDevice::turnOn() {
    turnedOnTime = millis();
    digitalWrite(pin, HIGH);
    state = true;
}

void FeedbackDevice::turnOff() {
    digitalWrite(pin, LOW);
    state = false;
}

void FeedbackDevice::directionalFeedback(float rawDist) {
    filteredDist = smooth(rawDist, filteredDist);
    if (filteredDist > 0 && filteredDist <= MAX_DIST) {
        int interval = map(constrain(filteredDist, MIN_DIST, MAX_DIST), MIN_DIST, MAX_DIST, shortFeedbackPulse, shortFeedbackPulse*10);
        if (!state && millis() - startIntervalTime >= interval) {
            startIntervalTime = millis();
            turnOn();
        }
    }
    else {
        turnOff();
    }

    if (state && millis() - turnedOnTime >= shortFeedbackPulse) {
        turnOff();
    }
}

void FeedbackDevice::update(feedbackMode currentFeedbackMode, float rawDist) {
    // Check if the device should be idle
    if (currentFeedbackMode != deviceFeedbackMode && currentFeedbackMode != BOTH) {
        turnOff();
        return;
    }
    
    // Check if the feedback mode changed
    if (previousFeedbackMode != currentFeedbackMode) {
        turnOff();
        previousFeedbackMode = currentFeedbackMode;
        mode = SWITCH_MODE_FEEDBACK;
    }

    // Notify the user about the feedback change by applying that feedback for an amout of time
    if (mode == SWITCH_MODE_FEEDBACK) {
        if (!state) {
            turnOn();
            fbModeChangedTime = millis();
        }
        if (state && millis() - fbModeChangedTime >= switchModeFbDuration) {
            turnOff();
            mode = DIRECTIONAL_FEEDBACK;
        }
    }

    // Perform directional feedback on the feedback device
    if (mode == DIRECTIONAL_FEEDBACK) {
        directionalFeedback(rawDist);
    }
}

float FeedbackDevice::smooth(float current, float previous) {
    return (SMOOTHING_ALPHA * current) + (1.0 - SMOOTHING_ALPHA) * previous;
}