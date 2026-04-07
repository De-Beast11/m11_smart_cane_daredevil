#include "FeedbackDevicesGroup.hpp"

FeedbackDevicesGroup::FeedbackDevicesGroup(int pinsHapticFeedbackDevices[], int numDevices) {
    for (int i = 0; i < numDevices; i++) {
        feedbackDevices.emplace_back(pinsHapticFeedbackDevices[i]);
    }
}

void FeedbackDevicesGroup::setup() {
    for (FeedbackDevice& device : feedbackDevices) {
        device.setup();
    }
}

void FeedbackDevicesGroup::turnOnFor(bool shouldTurnOn, unsigned long duration) {
    for (FeedbackDevice& device : feedbackDevices) {
        device.turnOnFor(shouldTurnOn, duration);
    }
}

bool FeedbackDevicesGroup::getTurnedOn() {
    for (FeedbackDevice& device : feedbackDevices) {
        if (device.getTurnedOn()) {
            return true;
        }
    }
    return false;
}