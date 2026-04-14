#include "ToggleButton.hpp"
#include "Feedback.hpp"

/// Pin definitions
// Pins for Feedback
const int pinFeedbackModeButton = 19;
const int pinAudioFeedback = 18;
const int pinHapticFeedbackLeft = 17;
const int pinHapticFeedbackMiddle = 16;
const int pinHapticFeedbackRight = 15;

/// Object definitions
// Objects for Feedback
ToggleButton button(pinFeedbackModeButton);
Feedback feedback(
  pinAudioFeedback, 
  pinHapticFeedbackLeft, 
  pinHapticFeedbackMiddle, 
  pinHapticFeedbackRight
);


void setup() {
  Serial.begin(115200);

  button.setup();
  feedback.setup();
}

void loop() {
  button.update();
  feedback.update(button.getFeedbackMode());
}