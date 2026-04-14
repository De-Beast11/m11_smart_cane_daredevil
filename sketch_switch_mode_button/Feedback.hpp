#ifndef FEEDBACK_HPP
#define FEEDBACK_HPP

#include <Arduino.h>

#include "FeedbackDevice.hpp"

enum feedbackMode {
    HAPTIC,
    AUDIO,
    BOTH,
    NUM_FEEDBACK_MODES
};

class Feedback {
public:
    Feedback(int pinAudioFb, int pinHapticFbLeft, int pinHapticFbMiddle, int pinHapticFbRight);

    void setup();

    void update(feedbackMode currentFeedbackMode);

private:
    bool switchModeTriggered = false;

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

    const unsigned long switchModeFbDuration = 1500;
};

#endif //FEEDBACK_HPP