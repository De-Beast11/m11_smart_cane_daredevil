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

void Feedback::update(
    feedbackMode currentFeedbackMode, 
    float distanceLeft, 
    float distanceMiddle, 
    float distanceRight,
    float distanceUltrasound, 
    bool lowBattery, 
    bool lowBatteryAcknowledged
) {
    // Check if battery is low
    if (lowBattery && !lowBatteryAcknowledged) {
        mode = LOW_BATTERY_FEEDBACK;
    }

    // Check if feedback mode changed
    if (previousFeedbacMode != currentFeedbackMode) {
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
        }
        case LOW_BATTERY_FEEDBACK:
        {
            if (!lowBattery || (lowBattery && lowBatteryAcknowledged)) {
                mode = DIRECTIONAL_FEEDBACK;
            }

            if (currentFeedbackMode == AUDIO || currentFeedbackMode == BOTH) {
                audio.lowBatteryFeedback();
            }
            if (currentFeedbackMode == HAPTIC || currentFeedbackMode == BOTH) {
                hapticLeft.lowBatteryFeedback();
                hapticMiddle.lowBatteryFeedback();
                hapticRight.lowBatteryFeedback();
            }
            break;
        }
    }
}