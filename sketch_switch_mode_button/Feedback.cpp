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

void Feedback::update(feedbackMode currentFeedbackMode, float distanceLeft, float distanceMiddle, float distanceRight, float distanceUltrasound, bool lowBattery) {
    // Check if feedback mode changed
    if (previousFeedbacMode != currentFeedbackMode) {
        audio.turnOff();
        hapticLeft.turnOff();
        hapticMiddle.turnOff();
        hapticRight.turnOff();

        mode = SWITCH_MODE_FEEDBACK;
        previousFeedbacMode = currentFeedbackMode;
    }
    // Check if battery is low
    if (lowBattery) {
        mode = LOW_BATTERY_FEEDBACK;
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
            updateArtificialDistance();
            // Pass distance to devices
            if (currentFeedbackMode == AUDIO || currentFeedbackMode == BOTH) {
                audio.directionalFeedback(artificialDistance);
            }
            if (currentFeedbackMode == HAPTIC || currentFeedbackMode == BOTH) {
                hapticLeft.directionalFeedback(artificialDistance);
                hapticMiddle.directionalFeedback(artificialDistance);
                hapticRight.directionalFeedback(artificialDistance);
            }
            break;
        }
    }
}

void Feedback::updateArtificialDistance() {
    unsigned long now = millis();
    
    if (now - previousUpdateArtificialDistance >= timeBetweenArtificialDistanceUpdates) {
        artificialDistance += artificialDistanceIncreasing ? 10.0 : -10.0;
        if (artificialDistance >= 200.0) {
            artificialDistance = 200.0;
            artificialDistanceIncreasing = false;
        }
        else if (artificialDistance <= 10.0) {
            artificialDistance = 10.0;
            artificialDistanceIncreasing = true;
        }
        previousUpdateArtificialDistance = now;
    }

}