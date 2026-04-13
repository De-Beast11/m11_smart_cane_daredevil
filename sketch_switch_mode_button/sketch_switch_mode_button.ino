#include "ToggleButton.hpp"
#include "FeedbackDevice.hpp"
#include "BatteryReadout.hpp"

/// Pin definitions
// Pins for Feedback
const int pinFeedbackModeButton = 19;
const int pinAudioFeedback = 18;
const int pinHapticFeedbackLeft = 17;
const int pinHapticFeedbackMiddle = 16;
const int pinHapticFeedbackRight = 15;
// Pins for Power
const int pinBatteryVoltageAnalog = 34;

/// Object definitions
// Objects for Feedback
ToggleButton feedbackButton(pinFeedbackModeButton);
FeedbackDevice audioFeedback(pinAudioFeedback, AUDIO);
FeedbackDevice hapticFeedbackLeft(pinHapticFeedbackLeft, HAPTIC);
FeedbackDevice hapticFeedbackMiddle(pinHapticFeedbackMiddle, HAPTIC);
FeedbackDevice hapticFeedbackRight(pinHapticFeedbackRight, HAPTIC);
// Objects for Power
BatteryReadout battery(pinBatteryVoltageAnalog);

void setup() {
  Serial.begin(115200); // Usually better to use 115200 for ESP32, but 9600 works too!
  
  // Setup button and feedback devices
  feedbackButton.setup();
  audioFeedback.setup();
  hapticFeedbackLeft.setup();
  hapticFeedbackMiddle.setup();
  hapticFeedbackRight.setup();
  // Setup battery
  battery.setup();
}

// --- TEST VARIABLES ---
float rawL = 250.0;  // Left
float rawC = 250.0; // Center 
float rawR = 250.0; // Right
float rawU = 250.0; // Ultrasonic

unsigned long increaseDistanceTime = 0;

void loop() {
  // Check for button press
  feedbackButton.update();
  
  float minCenter = min(rawC, rawU);
  float minAll = min(rawL, min(rawR, minCenter));

  // Update battery
  battery.update();
  // Update feedback devices
  feedbackMode currentFeedbackMode = feedbackButton.getFeedbackMode();
  bool lowBattery = battery.getBatteryLow();
  Serial.println(lowBattery);
  audioFeedback.update(currentFeedbackMode, minAll, lowBattery);
  hapticFeedbackLeft.update(currentFeedbackMode, rawL, lowBattery);
  hapticFeedbackMiddle.update(currentFeedbackMode, rawC, lowBattery);
  hapticFeedbackRight.update(currentFeedbackMode, rawR, lowBattery);
  

  // --- TESTING DISTANCE INCREASING (SWEEP PATTERN) ---
  // I changed this to 500ms so you don't have to wait 25 seconds per motor!
  // if (millis() - increaseDistanceTime >= 500) {
  //   increaseDistanceTime = millis();

  //   // Testing LEFT
  //   rawL += 10.0;
  //   if (rawL > 250.0) {
  //     rawL = 10.0;
  //   }
    
  //   // Testing CENTER
  //   rawC += 10.0;
  //   if (rawC > 250.0) {
  //     rawC = 10.0;
  //   }
    
  //   // Testing RIGHT
  //   rawR += 10.0;
  //   if (rawR > 250.0) {
  //     rawR = 10.0;
  //   }
  // }
}