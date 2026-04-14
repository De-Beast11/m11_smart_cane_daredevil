#ifndef BATTERY_READOUT_HPP
#define BATTERY_READOUT_HPP

#include <Arduino.h>

class BatteryReadout {
public:
    // Assign pin to input pin number
    BatteryReadout(const int pinBatteryReadoutVoltage);
    // Set ADC resolution to 12 bits
    void setup();
    // Read out the pin 5 times and take the average
    // Convert to voltage
    // Check if previous 5 readings were below threshold
    void update();
    // Returns True if battery is low, False otherwise
    bool getBatteryLow() const;
private:
    const int pin;

    const int analogBitResolution = 12;
    const float voltageThreshold = 3.65;
    const float alpha = 0.1;

    int currentIndex = 0;
    int maxIndex = 5;
    float filteredVoltage = 0;
    float previousReadings[5] = {0, 0, 0, 0, 0};

    bool batteryLow = false;
};

#endif //BATTERY_READOUT_HPP