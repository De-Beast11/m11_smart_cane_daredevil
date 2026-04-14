#ifndef TIMER_HPP
#define TIMER_HPP

#include <Arduino.h>

class Timer {
public:
    Timer(unsigned long inDuration)
        : duration(inDuration), startTime(0), active(false) {}

    // Start the timer (only if not already running)
    void start() {
        if (!active) {
            startTime = millis();
            active = true;
        }
    }

    // Call this every loop to update state
    void update() {
        if (active && (millis() - startTime >= duration)) {
            active = false;
        }
    }

    // Check if timer is currently running
    bool isActive() const {
        return active;
    }

private:
    unsigned long duration;
    unsigned long startTime;
    bool active;
};

#endif // TIMER_HPP