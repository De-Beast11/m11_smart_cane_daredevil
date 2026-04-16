#include "src/sensor_files/Sensors_main.hpp"
#include "src/feedback_files/ToggleButton.hpp"
#include "src/feedback_files/Feedback.hpp"
#include "src/feedback_files/BatteryReadout.hpp"

Sensors sensors;

/// Pin definitions
// Pins for Feedback
const int pinFeedbackModeButton = 5;
const int pinAudioFeedback = 17;
const int pinHapticFeedbackLeft = 16;
const int pinHapticFeedbackMiddle = 19;
const int pinHapticFeedbackRight = 21;
// Pin for Power
const int pinBatteryVoltageReadout = 4;

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
    sensors.init();
    battery.setup();
    button.setup();
    feedback.setup();
}

void loop() {
    
    float pitch = sensors.getPitch();
    int ultrasound_distance = sensors.getUltrasound();
    auto tof_distance = sensors.getToF(pitch, ultrasound_distance);

    battery.update();
    button.update(battery);

    feedback.update(button.getFeedbackMode(), tof_distance[0]/10, tof_distance[1]/10, tof_distance[2]/10, ultrasound_distance/10, battery.getBatteryLow(), battery.getLowBatteryAck());

    Serial.print(tof_distance[0]); Serial.print(", "); Serial.print(tof_distance[1]); Serial.print(", "); Serial.println(tof_distance[2]);
    delay(20);
}