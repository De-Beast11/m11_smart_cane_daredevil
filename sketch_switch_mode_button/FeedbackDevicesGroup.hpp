#ifndef HAPTICFEEDBACK_HPP
#define HAPTICFEEDBACK_HPP

#include <Arduino.h>
#include <vector>

#include "FeedbackDevice.hpp"

class FeedbackDevicesGroup {
public:
    // Constructor takes an array of pin numbers for the feedback devices and the number of devices
    // This is basically a facade of the FeedbackDevice class to control multiple feedback devices at once
    FeedbackDevicesGroup(int pinsHapticFeedbackDevices[], int numDevices);
    // Sets up all feedback devices in the group
    void setup();
    // Turns all feedback devices in the group on for a duration
    void turnOnFor(bool shouldTurnOn=false, unsigned long duration=1000);
    // Returns whether any of the feedback devices in the group is currently turned on
    bool getTurnedOn();
private:
    std::vector<FeedbackDevice> feedbackDevices;

};
#endif //HAPTICFEEDBACK_HPP