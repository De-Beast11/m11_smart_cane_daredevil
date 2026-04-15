#include "ToggleButton.hpp"
#include "Feedback.hpp"
#include "BatteryReadout.hpp"

/// Pin definitions
// Pins for Feedback
const int pinFeedbackModeButton = 19;
const int pinAudioFeedback = 18;
const int pinHapticFeedbackLeft = 17;
const int pinHapticFeedbackMiddle = 16;
const int pinHapticFeedbackRight = 15;
// Pin for Power
const int pinBatteryVoltageReadout = 34;


/// Object definitions
// Objects for Feedback
ToggleButton button(pinFeedbackModeButton);
Feedback feedback(
  pinAudioFeedback, 
  pinHapticFeedbackLeft, 
  pinHapticFeedbackMiddle, 
  pinHapticFeedbackRight
);
// Object for Power
BatteryReadout battery(pinBatteryVoltageReadout);

void setup() {
  Serial.begin(115200);

  battery.setup();
  button.setup();
  feedback.setup();
}

// --- TEST VARIABLES ---
float rawL = 200.0;  // Left
float rawC = 250.0; // Center 
float rawR = 250.0; // Right
float rawU = 250.0; // Ultrasonic

void loop() {
  battery.update();
  button.update(battery);
  feedback.update(button.getFeedbackMode(), rawL, rawC, rawR, rawU, battery.getBatteryLow(), battery.getLowBatteryAck());
}