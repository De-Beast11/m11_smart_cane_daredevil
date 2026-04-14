#include "Feedback.hpp"

Feedback::Feedback(int pinAudioFb, int pinHapticFbLeft, int pinHapticFbMiddle, int pinHapticFbRight)
: audio(pinAudioFb),
  hapticLeft(pinHapticFbLeft),
  hapticMiddle(pinHapticFbMiddle),
  hapticRight(pinHapticFbRight),
  switchModeTimer(switchModeFbDuration)
{}

void Feedback::setup() {
    audio.setup();
    hapticLeft.setup();
    hapticMiddle.setup();
    hapticRight.setup();
}

void Feedback::update(feedbackMode currentFeedbackMode) {
    Serial.println(currentFeedbackMode);
    if (currentFeedbackMode != previousFeedbacMode) {
        mode = SWITCH_MODE_FEEDBACK;
        previousFeedbacMode = currentFeedbackMode;
    }

    switch(mode) {
        case DIRECTIONAL_FEEDBACK:
            break;
        case SWITCH_MODE_FEEDBACK:
            if (!switchModeTriggered) {
                if (currentFeedbackMode == AUDIO || currentFeedbackMode == BOTH) {
                    audio.turnOn();
                }
                if (currentFeedbackMode == HAPTIC || currentFeedbackMode == BOTH) {
                    hapticLeft.turnOn();
                    hapticMiddle.turnOn();
                    hapticRight.turnOn();
                }
                switchModeTriggered = true;
            }

            if (millis() - audio.getTimeTurnedOn() >= switchModeFbDuration) {
                audio.turnOff();
                mode = DIRECTIONAL_FEEDBACK;
                switchModeTriggered = false;
            }
            if (millis() - audio.getTimeTurnedOn() >= switchModeFbDuration) {
                
            }
            break;
        case LOW_BATTERY_FEEDBACK:
            break;
    }
}

// void Feedback::switchModeFeedback(FeedbackDevice &device) {
//         if (!device.getState()) {
//             device.turnOn();
//         }
//         if (device.getState() && millis() - device.getTimeTurnedOn() >= switchModeFbDuration) {
//             device.turnOff();
//             mode = DIRECTIONAL_FEEDBACK;
//         }
// }