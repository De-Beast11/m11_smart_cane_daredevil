#ifndef FEEDBACK_DEVICE_HPP
#define FEEDBACK_DEVICE_HPP

#include <Arduino.h>


class FeedbackDevice {
public:
    FeedbackDevice(int pinFbDevice);  

    bool getState() const;

    void setup();

    void turnOn();

    void turnOff();

    void directionalFeedback(float rawData);
    // Returns true when the switch mode feedback is finisched, false otherwise
    bool switchModeFeedback();

    void lowBatteryFeedback();
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