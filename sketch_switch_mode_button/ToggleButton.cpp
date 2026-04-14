#include "ToggleButton.hpp"

ToggleButton::ToggleButton(int buttonPin) {
    pin = buttonPin;
}

void ToggleButton::setup() {
    pinMode(pin, INPUT_PULLUP);
}

void ToggleButton::update(bool forced, feedbackMode prev_forced) {
    bool buttonState = digitalRead(pin);

    if (buttonState != lastButtonState) {
        lastDebounceTime = millis();
    }

    // If the reading stayed stable long enough (debouncing)
    if ((millis() - lastDebounceTime) > debounceDelay) {

        if (buttonState != stableState) {
            stableState = buttonState;

            // Detect button press (HIGH -> LOW)
            if (stableState == LOW && !forced) {
                mode = (feedbackMode)((mode + 1) % NUM_FEEDBACK_MODES);
                Serial.print("BUTTON PRESSED, Feedback Mode: ");
                Serial.println(mode);
            }
            else mode = prev_forced;
        }
    }

    lastButtonState = buttonState;
}

feedbackMode ToggleButton::getFeedbackMode() const {
    return mode;
}

bool ToggleButton::getLowBatteryAcknowledged() const {
    return lowBatteryAcknowledged;
}