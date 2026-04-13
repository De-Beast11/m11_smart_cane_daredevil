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
    turnedOffTime = millis();
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

void FeedbackDevice::update(feedbackMode currentFeedbackMode, float rawDist, bool lowBattery) {
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

    // Check if the battery is low
    // Only allow if there is no switch mode feedback currently
    if (lowBattery && mode != SWITCH_MODE_FEEDBACK && millis() - timePreviousLowBatteryFeedback >= timeBetweenLowBatteryFeedback) {
        turnOff();
        mode = LOW_BATTERY_FEEDBACK;
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

    // Notify the user about a low battery by applying two long burst back to back
    if (mode == LOW_BATTERY_FEEDBACK) {
        switch (lowBatteryState) {
            case IDLE:
                pulseCount = 0;
                lowBatteryState = PULSE_ON;
                stateStartTime = millis();
                turnOn();
                break;
            case PULSE_ON:
                if (millis() - stateStartTime >= longFeedbackPulse) {
                    lowBatteryState = PULSE_OFF;
                    turnOff();
                    stateStartTime = millis();
                    break;
                }
            case PULSE_OFF:
                if (millis() - stateStartTime >= longFeedbackPulse) {
                    pulseCount++;

                    if (pulseCount >= maxPulseCount) {
                        lowBatteryState = IDLE;
                        timePreviousLowBatteryFeedback = millis();
                        mode = DIRECTIONAL_FEEDBACK;
                    }
                    else {
                        turnOn();
                        stateStartTime = millis();
                        lowBatteryState = PULSE_ON;
                    }
                }
                break;
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