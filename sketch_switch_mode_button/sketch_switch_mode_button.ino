# include "ToggleButton.hpp"

const int pinFeedbackModeButton = 19;

ToggleButton feedbackButton(pinFeedbackModeButton);

void setup() {
  Serial.begin(9600);
  feedbackButton.begin();
}

void loop() {
  feedbackButton.update();
}