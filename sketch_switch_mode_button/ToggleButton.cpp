#include "ToggleButton.hpp"

ToggleButton::ToggleButton(int buttonPin) {
    pin = buttonPin;
}

void ToggleButton::setup() {
    pinMode(pin, INPUT_PULLUP);
}

void ToggleButton::update(BatteryReadout& battery) {
    bool buttonState = digitalRead(pin);

    if (buttonState != lastButtonState) {
        lastDebounceTime = millis();
    }

    // If the reading stayed stable long enough (debouncing)
    if ((millis() - lastDebounceTime) > debounceDelay) {

        if (buttonState != stableState) {
            stableState = buttonState;

            // Detect button press (HIGH -> LOW)
            if (stableState == LOW) {
                if (battery.getBatteryLow() && !battery.getLowBatteryAck()) {
                    battery.setLowBatteryAck(true);
                }
                else {
                    mode = (feedbackMode)((mode + 1) % NUM_FEEDBACK_MODES);
                }
            }
        }
    }

    lastButtonState = buttonState;
}

feedbackMode ToggleButton::getFeedbackMode() {
    return mode;
}
