#ifndef FEEDBACK_HPP
#define FEEDBACK_HPP

#include <Arduino.h>

#include "FeedbackModes.hpp"
#include "FeedbackDevice.hpp"


class Feedback {
public:
    Feedback(int pinAudioFb, int pinHapticFbLeft, int pinHapticFbMiddle, int pinHapticFbRight);

    void setup();

    void update(feedbackMode currentFeedbackMode, float distanceLeft, float distanceMiddle, float distanceRight, float distanceUltrasound, bool lowBattery, bool lowBatteryAcknowledged);

private:
    feedbackMode previousFeedbacMode = NUM_FEEDBACK_MODES;

    FeedbackDevice audio;
    FeedbackDevice hapticLeft;
    FeedbackDevice hapticMiddle;
    FeedbackDevice hapticRight;

    enum Mode {
        DIRECTIONAL_FEEDBACK,
        SWITCH_MODE_FEEDBACK,
        LOW_BATTERY_FEEDBACK
    };
    Mode mode = DIRECTIONAL_FEEDBACK;
};

#endif //FEEDBACK_HPP