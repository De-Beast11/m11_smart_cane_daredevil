#ifndef FEEDBACK_DEVICE_HPP
#define FEEDBACK_DEVICE_HPP

#include <Arduino.h>

enum feedbackMode {
    NONE,
    HAPTIC,
    AUDIO,
    BOTH,
    WARNING,
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
    int getFBMode();
    // Sets up the feedback device pin
    void setup();

    void turnOn();

    void turnOff();

    void directionalFeedback(float rawData);
    // Main function that should be called in loop()
    // Performs the mode switching feedback and the directional feedback by calling helper functions
    void update(feedbackMode currentFeedbackMode, float rawData, bool lowBattery);

private:
    float smooth(float current, float previous);

    const int pin;

    float filteredDist = 0;
    const float MAX_DIST = 200.0;
    const float MIN_DIST = 10.0;
    const float SMOOTHING_ALPHA = 0.3;

    const unsigned long switchModeFbDuration = 1500;
    const unsigned long longFeedbackPulse = 500;
    const unsigned long shortFeedbackPulse = 100;

    enum State {ON, OFF};
    State state = OFF;
    unsigned long stateStartTime = 0;

    bool firstTimeLowBatteryFeedback = true;
    const unsigned long firstLowBatteryFeedbackDuration = 3000;
};

#endif //FEEDBACK_DEVICE_HPP