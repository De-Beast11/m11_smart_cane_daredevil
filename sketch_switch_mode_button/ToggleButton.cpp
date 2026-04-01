#include "ToggleButton.hpp"

ToggleButton::ToggleButton(int buttonPin) {
    pin = buttonPin;
}

void ToggleButton::begin() {
    pinMode(pin, INPUT_PULLUP);
}

void ToggleButton::update() {
    bool buttonState = digitalRead(pin);

    if (buttonState != lastButtonState) {
        lastDebounceTime = millis();
    }

    // If the reading stayed stable long enough
    if ((millis() - lastDebounceTime) > debounceDelay) {

        if (buttonState != stableState) {
        stableState = buttonState;

        // Detect button press (HIGH -> LOW)
        if (stableState == LOW) {
            mode = (feedbackMode)((mode + 1) % NUM_FEEDBACK_MODES);
            Serial.print("BUTTON PRESSED, Feedback Mode: ");
            Serial.println(mode);
        }
        }
    }

    lastButtonState = buttonState;
}

feedbackMode ToggleButton::getFeedbackMode() {
    return mode;
}
