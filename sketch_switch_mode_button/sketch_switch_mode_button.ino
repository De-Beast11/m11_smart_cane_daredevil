#include "ToggleButton.hpp"
#include "FeedbackDevice.hpp"
//#include "FeedbackDevicesGroup.hpp"

// Pin definitions
const int pinFeedbackModeButton = 19;
const int pinAudioFeedback = 18;
const int pinHapticFeedbackLeft = 17;
const int pinHapticFeedbackMiddle = 16;
const int pinHapticFeedbackRight = 15;

// Object definitions
ToggleButton feedbackButton(pinFeedbackModeButton);
FeedbackDevice audioFeedback(pinAudioFeedback, AUDIO);
FeedbackDevice hapticFeedbackLeft(pinHapticFeedbackLeft, HAPTIC);
FeedbackDevice hapticFeedbackMiddle(pinHapticFeedbackMiddle, HAPTIC);
FeedbackDevice hapticFeedbackRight(pinHapticFeedbackRight, HAPTIC);

void setup() {
  Serial.begin(9600);
  // Setup button and feedback devices
  feedbackButton.setup();
  audioFeedback.setup();
  hapticFeedbackLeft.setup();
  hapticFeedbackMiddle.setup();
  hapticFeedbackRight.setup();
}

// Placeholder Sensor Inputs (In real life, replace these with sensor.read())
float rawL = 50.0;  // Danger on left!
float rawC = 180.0; // Distant ahead
float rawR = 250.0; // Clear
float rawU = 150.0; // Ultrasonic backup sees closer than Center ToF
float minCenter = min(rawC, rawU);
float minAll = min(rawL, min(rawR, minCenter));

void loop() {
  // Check for button press
  feedbackButton.update();
  // Update feedback devices
  feedbackMode currentFeedbackMode = feedbackButton.getFeedbackMode();
  audioFeedback.update(currentFeedbackMode, minAll);
  hapticFeedbackLeft.update(currentFeedbackMode, rawL);
  hapticFeedbackMiddle.update(currentFeedbackMode, rawC);
  hapticFeedbackRight.update(currentFeedbackMode, rawR);
}