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
    digitalWrite(pin, HIGH);
    state = true;
}

void FeedbackDevice::turnOff() {
    digitalWrite(pin, LOW);
    state = false;
}

void FeedbackDevice::switchOnOff() {
    switchOnOffTime = millis();
    state = !state;
    digitalWrite(pin, state ? HIGH : LOW);
}

void FeedbackDevice::directionalFeedback(float rawDist) {
    if (mode != DIRECTIONAL_FEEDBACK) {
        return;
    }

    filteredDist = smooth(rawDist, filteredDist);

    if (filteredDist > 0 && filteredDist <= MAX_DIST) {
        int interval = map(constrain(filteredDist, MIN_DIST, MAX_DIST), MIN_DIST, MAX_DIST, 60, 600);
        
        if (millis() - switchOnOffTime >= interval) {
            switchOnOff();
        }
    }
    else {
        turnOff();
    }
}

void FeedbackDevice::update(feedbackMode currentFeedbackMode, float rawDist) {
    // Check if the device should be idle
    if (currentFeedbackMode != deviceFeedbackMode && currentFeedbackMode != BOTH) {
        mode = IDLE;
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