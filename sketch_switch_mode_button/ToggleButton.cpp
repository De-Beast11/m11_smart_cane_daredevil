#include "ToggleButton.hpp"

ToggleButton::ToggleButton(int buttonPin) {
    pin = buttonPin;
}

void ToggleButton::setup() {
    pinMode(pin, INPUT_PULLUP);
}

void ToggleButton::update(FeedbackDevice &audioFeedback, FeedbackDevicesGroup &hapticFeedbackGroup) {
    bool buttonState = digitalRead(pin);

    if (buttonState != lastButtonState) {
        lastDebounceTime = millis();
    }

    // If the reading stayed stable long enough
    if ((millis() - lastDebounceTime) > debounceDelay) {

        if (buttonState != stableState) {
        stableState = buttonState;

        // Detect button press (HIGH -> LOW)
        if (stableState == LOW && !audioFeedback.getTurnedOn() && !hapticFeedbackGroup.getTurnedOn()) {
            mode = (feedbackMode)((mode + 1) % NUM_FEEDBACK_MODES);
            Serial.print("BUTTON PRESSED, Feedback Mode: ");
            Serial.println(mode);
            if (mode == HAPTIC || mode == BOTH) {
                hapticFeedbackGroup.turnOnFor(true, 1500);
            }
            if (mode == AUDIO || mode == BOTH) {
                audioFeedback.turnOnFor(true, 1500);
            }
        }
        }
    }

    lastButtonState = buttonState;
}

feedbackMode ToggleButton::getFeedbackMode() {
    return mode;
}
