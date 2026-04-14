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

void Feedback::update(feedbackMode currentFeedbackMode, float distanceLeft, float distanceMiddle, float distanceRight, float distanceUltrasound, bool lowBattery, bool lowBatteryAcknowledged) {
    // Priority of feedback: LOW_BATTERY, SWITCH_MODE, DIRECTIONAL
    if (lowBattery && !lowBatteryAcknowledged) {
        if (previousLowBatteryFlag != lowBattery) {
            audio.turnOff();
            hapticLeft.turnOff();
            hapticMiddle.turnOff();
            hapticRight.turnOff();

            previousLowBatteryFlag = lowBattery;
        }
        mode = LOW_BATTERY_FEEDBACK;
    }
    // Check if feedback mode changed
    else if (previousFeedbacMode != currentFeedbackMode) {
        audio.turnOff();
        hapticLeft.turnOff();
        hapticMiddle.turnOff();
        hapticRight.turnOff();

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
            bool finished = false;
            if (currentFeedbackMode == AUDIO || currentFeedbackMode == BOTH) {
                finished = audio.switchModeFeedback();
            }
            if (currentFeedbackMode == HAPTIC || currentFeedbackMode == BOTH) {
                finished = hapticLeft.switchModeFeedback();
                hapticMiddle.switchModeFeedback();
                hapticRight.switchModeFeedback();
            }

            if (finished) {
                mode = DIRECTIONAL_FEEDBACK;
            }
            break;
        }
        case LOW_BATTERY_FEEDBACK:
        {
            if (!lowBattery || (lowBattery && lowBatteryAcknowledged)) {
                audio.turnOff();
                hapticLeft.turnOff();
                hapticMiddle.turnOff();
                hapticRight.turnOff();
                mode = DIRECTIONAL_FEEDBACK;
            }
            audio.lowBatteryFeedback();
            hapticLeft.lowBatteryFeedback();
            hapticMiddle.lowBatteryFeedback();
            hapticRight.lowBatteryFeedback();
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