#ifndef HAPTICFEEDBACK_HPP
#define HAPTICFEEDBACK_HPP

#include <Arduino.h>
#include <vector>

#include "FeedbackDevice.hpp"

class FeedbackDevicesGroup {
public:
    FeedbackDevicesGroup(int pinsHapticFeedbackDevices[], int numDevices);

    void setup();

    void turnOnFor(bool shouldTurnOn=false, unsigned long duration=1000);

    bool getTurnedOn();
private:
    std::vector<FeedbackDevice> feedbackDevices;

};
#endif //HAPTICFEEDBACK_HPP