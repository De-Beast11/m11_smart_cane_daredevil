#include "ToggleButton.hpp"
#include "FeedbackDevice.hpp"

const int pinFeedbackModeButton = 19;
const int pinFeedbackDeviceTest = 18;

ToggleButton feedbackButton(pinFeedbackModeButton);
FeedbackDevice device(pinFeedbackDeviceTest);

void setup() {
  Serial.begin(9600);
  feedbackButton.setup();
  device.setup();
}

void loop() {
  feedbackButton.update(device);
  device.turnOnFor();
}