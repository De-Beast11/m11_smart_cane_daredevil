#include "FeedbackDevice.hpp"
#include "FeedbackSystem.hpp"
#include "ToggleButton.hpp"

/// Pin definitions
// Pins for Feedback
const int pinFeedbackModeButton = 19;
const int pinAudioFeedback = 18;
const int pinHapticFeedbackLeft = 17;
const int pinHapticFeedbackMiddle = 16;
const int pinHapticFeedbackRight = 15;

// Object definitions
ToggleButton feedbackButton(pinFeedbackModeButton);
FeedbackDevice audioFeedback(pinAudioFeedback, AUDIO);
FeedbackDevice hapticFeedbackLeft(pinHapticFeedbackLeft, HAPTIC);
FeedbackDevice hapticFeedbackMiddle(pinHapticFeedbackMiddle, HAPTIC);
FeedbackDevice hapticFeedbackRight(pinHapticFeedbackRight, HAPTIC);

FeedbackDevice* totaldevices[] = {
    &audioFeedback,
    &hapticFeedbackLeft,
    &hapticFeedbackMiddle,
    &hapticFeedbackRight
};



void FeedbackSystem::feedback_loop(const std::array<float,4>& in, bool battery_status){
    // Input Data {Left, Center, Right, Ultrasound}
    std::array<float, 4> data; 
    float minCenter = min(in[1], in[3]);
    float minAll = min(in[0], min(in[2], minCenter));
    data[0] = minAll; data[1] = in[0]; data[2] = in[1]; data[3] = in[2];

    feedbackMode currentFeedbackMode = feedbackButton.getFeedbackMode();

    for (int i = 0; i < 4; i++){
        if(totaldevices[i].getFBMode() == currentFeedbackMode || currentFeedbackMode == BOTH){
            totaldevices[i].update(currentFeedbackMode, data[i]);
        }
    }

    if (!battery_status){
        feedbackMode prevFB = currentFeedbackMode;
        currentFeedbackMode = WARNING;
        while(currentFeedbackMode == WARNING){
            for(float i = 0.0; i<100.0; i++) audioFeedback.directionalFeedback(i);
            feedbackButton.update(true, prevFB);
        }
    }
}

