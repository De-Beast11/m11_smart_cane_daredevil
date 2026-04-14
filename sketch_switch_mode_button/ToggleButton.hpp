#ifndef TOGGLE_BUTTON_HPP
#define TOGGLE_BUTTON_HPP

#include <Arduino.h>

#include "FeedbackModes.hpp"

class ToggleButton {
public:
    // Constructor takes the pin number of the button
    ToggleButton(int buttonPin);
    // Sets up the button pin
    void setup();
    // Checks for button press and updates feedback mode and feedback devices accordingly
    void update(bool forced = false, feedbackMode prev_forced = NONE); 
    // Returns the current feedback mode
    feedbackMode getFeedbackMode() const;
    // Returns the low battery acknowledged flag
    bool getLowBatteryAcknowledged() const;

private:
    int pin;

    feedbackMode mode = HAPTIC;

    bool lastButtonState = HIGH;
    bool stableState = HIGH;

    unsigned long lastDebounceTime = 0;
    const unsigned long debounceDelay = 50;
    const unsigned long feedbackDuration = 1500;

    bool lowBatteryAcknowledged = false;
};

#endif