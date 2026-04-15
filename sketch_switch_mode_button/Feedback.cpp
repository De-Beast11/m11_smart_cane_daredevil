#include "Feedback.hpp"

Feedback::Feedback(int pinAudioFb, int pinHapticFbLeft, int pinHapticFbMiddle, int pinHapticFbRight)
: audio(pinAudioFb),
  hapticLeft(pinHapticFbLeft),
  hapticMiddle(pinHapticFbMiddle),
  hapticRight(pinHapticFbRight)
{}

void Feedback::setup() {
    audio.setup();
    hapticLeft.setup();
    hapticMiddle.setup();
    hapticRight.setup();
}

void Feedback::update(feedbackMode currentFeedbackMode, float distanceLeft, float distanceMiddle, float distanceRight, float distanceUltrasound, bool lowBattery, bool lowBatteryAcknowledged) {
    //// State handling for the State Machine
    if (previousFeedbacMode != currentFeedbackMode) {
        previousFeedbacMode = currentFeedbackMode;
        mode = SWITCH_MODE_FEEDBACK;
        switchModeState = ENTER;
        lowBatteryState = IDLE;
    }
    else if (switchModeState == IDLE) {
        // Do low battery handling here
        if (lowBattery && !lowBatteryAcknowledged && lowBatteryState == IDLE) {
            mode = LOW_BATTERY_FEEDBACK;
            lowBatteryState = ENTER;
        }
        if (lowBattery && lowBatteryAcknowledged && lowBatteryState == UPDATE) {
            mode = LOW_BATTERY_FEEDBACK;
            lowBatteryState = EXIT;
        }
        if (!lowBattery && lowBatteryState != IDLE) {
            mode = LOW_BATTERY_FEEDBACK;
            lowBatteryState = EXIT;
        }
        if (switchModeState == IDLE && lowBatteryState == IDLE) {
            mode = DIRECTIONAL_FEEDBACK;
        }
    }
    // FOR TESTING
    if (previousMode != mode) {
        Serial.print("MODE: ");
        Serial.println(mode);
        previousMode = mode;
    }

    // State Machine for the different feedback: DIRECTIONAL, SWITCH_MODE, LOW_BATTERY
    switch(mode) {
        case DIRECTIONAL_FEEDBACK:
        {
            // Parse distances
            float minMiddle = min(distanceMiddle, distanceUltrasound);
            float minAll = min(distanceLeft, min(distanceRight, minMiddle));
            // Pass distance to devices
            if (currentFeedbackMode == AUDIO || currentFeedbackMode == BOTH) {
                audio.directionalFeedback(minAll);
            }
            if (currentFeedbackMode == HAPTIC || currentFeedbackMode == BOTH) {
                hapticLeft.directionalFeedback(distanceLeft);
                hapticMiddle.directionalFeedback(minMiddle);
                hapticRight.directionalFeedback(distanceRight);
            }
            break;
        }
        case SWITCH_MODE_FEEDBACK:
        {
            switchModeFeedback(currentFeedbackMode);
            break;
        }
        case LOW_BATTERY_FEEDBACK:
        {
            lowBatteryFeedback(currentFeedbackMode);
            break;
        }
    }
}

void Feedback::switchModeFeedback(feedbackMode currentFeedbackMode) {
    switch(switchModeState) {
        case ENTER:
            turnOffAll();
            if (currentFeedbackMode == AUDIO || currentFeedbackMode == BOTH) {
                audio.turnOn();
            }
            if (currentFeedbackMode == HAPTIC || currentFeedbackMode == BOTH) {
                hapticLeft.turnOn();
                hapticMiddle.turnOn();
                hapticRight.turnOn();
            } 

            timeEnterSwitchModeFeedback = millis();
            switchModeState = UPDATE;
            break;
        case UPDATE:
            if (millis() - timeEnterSwitchModeFeedback >= durationSwitchModeFeedback) {
                switchModeState = EXIT;
            }
            break;
        case EXIT:
            turnOffAll();
            switchModeState = IDLE;
            break;
        case IDLE:
            break;
    }
}

void Feedback::lowBatteryFeedback(feedbackMode currentFeedbackMode) {
    switch (lowBatteryState) {
    case ENTER:
    {
        turnOffAll();
        feedbackState = OFF;
        lowBatteryState = UPDATE;
        break;
    }
    case UPDATE:
    {
        unsigned long now = millis();
        if (feedbackState == ON && now - timeSwitchOnOff >= timeBetweenSwitching) {
            if (currentFeedbackMode == AUDIO || currentFeedbackMode == BOTH) {
                audio.turnOff();
            }
            if (currentFeedbackMode == HAPTIC || currentFeedbackMode == BOTH) {
                hapticLeft.turnOff();
                hapticMiddle.turnOff();
                hapticRight.turnOff();
            } 
            feedbackState = OFF;
            timeSwitchOnOff = now;
        }
        if (feedbackState == OFF && now - timeSwitchOnOff >= timeBetweenSwitching) {
            if (currentFeedbackMode == AUDIO || currentFeedbackMode == BOTH) {
                audio.turnOn();
            }
            if (currentFeedbackMode == HAPTIC || currentFeedbackMode == BOTH) {
                hapticLeft.turnOn();
                hapticMiddle.turnOn();
                hapticRight.turnOn();
            } 
            feedbackState = ON;
            timeSwitchOnOff = now;
        }
        break;
    }
    case EXIT:
    {
        turnOffAll();
        feedbackState = ON;
        lowBatteryState = IDLE;
        break;
    }
    case IDLE:
    {
        break;
    }
    }
}

void Feedback::turnOffAll() {
    audio.turnOff();
    hapticLeft.turnOff();
    hapticMiddle.turnOff();
    hapticRight.turnOff();
}