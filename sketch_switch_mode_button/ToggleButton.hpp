#ifndef TOGGLE_BUTTON_HPP
#define TOGGLE_BUTTON_HPP

#include <Arduino.h>
#include "FeedbackDevice.hpp"

enum feedbackMode {
  HAPTIC,
  AUDIO,
  BOTH,
  NUM_FEEDBACK_MODES
};

class ToggleButton {
private:
  int pin;

  feedbackMode mode = HAPTIC;

  bool lastButtonState = HIGH;
  bool stableState = HIGH;

  unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50;

public:
  ToggleButton(int buttonPin);

  void setup();

  void update(FeedbackDevice &device); 
  
  feedbackMode getFeedbackMode();
};

#endif