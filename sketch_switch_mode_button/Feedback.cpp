#include "Feedback.hpp"

Feedback::Feedback(int pinAudioFb, int pinHapticFbLeft, int pinHapticFbMiddle, int pinHapticFbRight)
: audio(pinAudioFb),
  hapticLeft(pinHapticFbLeft),
  hapticMiddle(pinHapticFbMiddle),
  hapticRight(pinHapticFbRight)
{}

void Feedback::setup() {
    audio.setup();
    hapticLeft.setup();
    hapticMiddle.setup();
    hapticRight.setup();
}

void Feedback::update(feedbackMode currentFeedbackMode, float distanceLeft, float distanceMiddle, float distanceRight, float distanceUltrasound) {
    // Check if feedback mode changed
    if (previousFeedbacMode != currentFeedbackMode) {
        // Check if feedback mode changed during switch mode feedback
        if (mode = SWITCH_MODE_FEEDBACK) {
            switchModeTriggered = false;
            audio.turnOff();
            hapticLeft.turnOff();
            hapticMiddle.turnOff();
            hapticRight.turnOff();
        }
        mode = SWITCH_MODE_FEEDBACK;
        previousFeedbacMode = currentFeedbackMode;
    }

    switch(mode) {
        case DIRECTIONAL_FEEDBACK:
        {
            // Parse distances
            float minMiddle = min(distanceMiddle, distanceUltrasound);
            float minAll = min(distanceLeft, min(distanceRight, minMiddle));
            // Pass distance to devices
            if (currentFeedbackMode == AUDIO || currentFeedbackMode == BOTH) {
                audio.directionalFeedback(minAll);
            }
            if (currentFeedbackMode == HAPTIC || currentFeedbackMode == BOTH) {
                hapticLeft.directionalFeedback(distanceLeft);
                hapticMiddle.directionalFeedback(minMiddle);
                hapticRight.directionalFeedback(distanceRight);
            }
            break;
        }
        case SWITCH_MODE_FEEDBACK:
        {
            // Turn on relevant feedback devices
            // Execute only when entering this mode for the first time
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
                startSwitchModeFeedback = millis();
            }
            // If the feedback devices are turned on, check if enough time has passed to turn them off again
            if (switchModeTriggered && millis() - startSwitchModeFeedback >= switchModeFbDuration) {
                if (currentFeedbackMode == AUDIO || currentFeedbackMode == BOTH) {
                    audio.turnOff();
                }
                if (currentFeedbackMode == HAPTIC || currentFeedbackMode == BOTH) {
                    hapticLeft.turnOff();
                    hapticMiddle.turnOff();
                    hapticRight.turnOff();
                }
                mode = DIRECTIONAL_FEEDBACK;
                switchModeTriggered = false;
            }
            break;
        }
        case LOW_BATTERY_FEEDBACK:
        {
            break;
        }
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