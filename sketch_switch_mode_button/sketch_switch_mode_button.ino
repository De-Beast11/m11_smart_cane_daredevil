#include "ToggleButton.hpp"
#include "FeedbackDevice.hpp"
#include "FeedbackDevicesGroup.hpp"

const int pinFeedbackModeButton = 19;
const int pinAudioFeedback = 18;
const int pinHaptic1Feedback = 17;
const int pinHaptic2Feedback = 16;
const int pinHaptic3Feedback = 15;

ToggleButton feedbackButton(pinFeedbackModeButton);
FeedbackDevice audioFeedback(pinAudioFeedback);
FeedbackDevicesGroup hapticFeedbackGroup((int[]){pinHaptic1Feedback, pinHaptic2Feedback, pinHaptic3Feedback}, 3);

void setup() {
  Serial.begin(9600);
  feedbackButton.setup();
  audioFeedback.setup();
  hapticFeedbackGroup.setup();
}

void loop() {
  feedbackButton.update(audioFeedback, hapticFeedbackGroup);
  audioFeedback.turnOnFor();
  hapticFeedbackGroup.turnOnFor();
}