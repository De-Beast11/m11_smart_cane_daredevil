#include "ToggleButton.hpp"
#include "FeedbackDevice.hpp"
#include "FeedbackDevicesGroup.hpp"

// Pin definitions
const int pinFeedbackModeButton = 19;
const int pinAudioFeedback = 18;
const int pinHaptic1Feedback = 17;
const int pinHaptic2Feedback = 16;
const int pinHaptic3Feedback = 15;

// Object definitions
ToggleButton feedbackButton(pinFeedbackModeButton);
FeedbackDevice audioFeedback(pinAudioFeedback);
FeedbackDevicesGroup hapticFeedbackGroup((int[]){pinHaptic1Feedback, pinHaptic2Feedback, pinHaptic3Feedback}, 3);

void setup() {
  Serial.begin(9600);
  // Setup button and feedback devices
  feedbackButton.setup();
  audioFeedback.setup();
  hapticFeedbackGroup.setup();
}

void loop() {
  // Check for button press
  feedbackButton.update(audioFeedback, hapticFeedbackGroup);
  // Update timer for feedback devices
  audioFeedback.turnOnFor();
  hapticFeedbackGroup.turnOnFor();
}