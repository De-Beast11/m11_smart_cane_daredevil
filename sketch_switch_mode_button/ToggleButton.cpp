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

    // If the reading stayed stable long enough (debouncing)
    if ((millis() - lastDebounceTime) > debounceDelay) {

        if (buttonState != stableState) {
        stableState = buttonState;

        // Detect button press (HIGH -> LOW)
        // Do not allow changing feedback mode if any feedback device is currently turned on to prevent interrupting feedback
        if (stableState == LOW && !audioFeedback.getTurnedOn() && !hapticFeedbackGroup.getTurnedOn()) {
            mode = (feedbackMode)((mode + 1) % NUM_FEEDBACK_MODES);
            //Serial.print("BUTTON PRESSED, Feedback Mode: ");
            //Serial.println(mode);
            if (mode == HAPTIC || mode == BOTH) {
                hapticFeedbackGroup.turnOnFor(true, feedbackDuration);
            }
            if (mode == AUDIO || mode == BOTH) {
                audioFeedback.turnOnFor(true, feedbackDuration);
            }
        }
        }
    }

    lastButtonState = buttonState;
}

feedbackMode ToggleButton::getFeedbackMode() {
    return mode;
}
