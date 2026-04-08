#ifndef FEEDBACK_DEVICE_HPP
#define FEEDBACK_DEVICE_HPP

#include <Arduino.h>

enum feedbackMode {
    HAPTIC,
    AUDIO,
    BOTH,
    NUM_FEEDBACK_MODES
};

class FeedbackDevice {
public:
    // Constructor assigns the pin number and feedback mode of the device
    FeedbackDevice(int fbDevicePin, feedbackMode fbMode);
    // Returns the state of the device: false = Off, true = On
    bool getState();
    // Sets up the feedback device pin
    void setup();
    // Turns the feedback device on
    void turnOn();
    // Turns the feedback device off
    void turnOff();
    // Turns the feedback device off if it is on and the other way around
    // Off -> On
    // On -> Off
    // The time at which this happens is stored in switchOnOffTime
    void switchOnOff();
    // Performs the logic for the directional feedback
    void directionalFeedback(float rawData);
    // Main function that should be called in loop()
    // Performs the mode switching feedback and the directional feedback by calling helper functions
    void update(feedbackMode currentFeedbackMode, float rawData);

private:
    int pin;
    bool state = false; // false = Off, true = On
    unsigned long switchOnOffTime = 0;

    // Variables for switching modes
    enum Mode {
        DIRECTIONAL_FEEDBACK,
        SWITCH_MODE_FEEDBACK,
        IDLE
    };
    Mode mode = DIRECTIONAL_FEEDBACK;
    feedbackMode previousFeedbackMode = NUM_FEEDBACK_MODES;
    feedbackMode deviceFeedbackMode;
    // Variables for switch mode feedback
    unsigned long fbModeChangedTime = 0;
    unsigned long switchModeFbDuration = 1500;

    // Methods for directional feedback
    float smooth(float current, float previous);
    // Variables for directional feedback
    float MAX_DIST = 200.0;
    float MIN_DIST = 10.0;
    float SMOOTHING_ALPHA = 0.3;
    float filteredDist;
};

#endif //FEEDBACK_DEVICE_HPP