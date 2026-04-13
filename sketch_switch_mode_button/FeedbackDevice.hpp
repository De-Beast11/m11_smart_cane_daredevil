#ifndef FEEDBACK_DEVICE_HPP
#define FEEDBACK_DEVICE_HPP

#include <Arduino.h>

enum feedbackMode {
    HAPTIC,
    AUDIO,
    BOTH,
    OFF,
    LOW_BATTERY
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
    // Performs the logic for the directional feedback
    void directionalFeedback(float rawData);
    // Main function that should be called in loop()
    // Performs the mode switching feedback and the directional feedback by calling helper functions
    void update(feedbackMode currentFeedbackMode, float rawData, bool lowBattery);

private:
    int pin;
    bool state = false; // false = Off, true = On
    unsigned long switchOnOffTime = 0;
    

    // Variables for switching modes
    enum Mode {
        DIRECTIONAL_FEEDBACK,
        SWITCH_MODE_FEEDBACK,
        LOW_BATTERY_FEEDBACK
    };
    Mode mode = DIRECTIONAL_FEEDBACK;
    feedbackMode previousFeedbackMode = NUM_FEEDBACK_MODES;
    feedbackMode deviceFeedbackMode;
    // Variables for switch mode feedback
    unsigned long fbModeChangedTime = 0;
    const unsigned long switchModeFbDuration = 1500;

    // Methods for directional feedback
    float smooth(float current, float previous);
    // Variables for directional feedback
    const float MAX_DIST = 200.0;
    const float MIN_DIST = 10.0;
    const float SMOOTHING_ALPHA = 0.3;
    float filteredDist;

    unsigned long turnedOnTime = 0;
    unsigned long turnedOffTime = 0;
    unsigned long startIntervalTime = 0;
    const unsigned long longFeedbackPulse = 500;
    const unsigned long shortFeedbackPulse = 100;

    // Variables for low battery feedback
    enum LowBatteryState {
        IDLE,
        PULSE_ON,
        PULSE_OFF
    };
    LowBatteryState lowBatteryState = IDLE;
    int pulseCount = 0;
    int maxPulseCount = 2;
    unsigned long stateStartTime = 0;
    unsigned long timePreviousLowBatteryFeedback = millis();
    const unsigned long timeBetweenLowBatteryFeedback = 5000;
};

#endif //FEEDBACK_DEVICE_HPP