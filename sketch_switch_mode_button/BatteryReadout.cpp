#include "BatteryReadout.hpp"

BatteryReadout::BatteryReadout(const int pinBatteryVoltageReadout)
: pin(pinBatteryVoltageReadout)
{}

void BatteryReadout::setup() {
    analogReadResolution(analogBitResolution);
    analogSetPinAttenuation(pin, ADC_11db);
}

void BatteryReadout::update() {
    // Readout ADC
    int rawValue = analogRead(pin);
    // Convert to voltage
    // The *2 is introduced due to the voltage divider
    float voltage = (rawValue / 4095.0) * 3.3 * 2;
    // Filter voltage
    filteredVoltage = alpha * voltage + (1 - alpha) * filteredVoltage;
    // Add voltage to previous readings
    previousReadings[currentIndex] = filteredVoltage;
    // Update index
    currentIndex = (currentIndex + 1) % maxIndex;
    // Check previous readings
    int countBelowThreshold = 0;
    for (int i=0; i < maxIndex; i++) {
        float storedVoltage = previousReadings[i];
        if (storedVoltage != 0 && storedVoltage < voltageThreshold) {
            countBelowThreshold += 1;
        }
    }
    // Update flag if all previous readings were below 
    if (countBelowThreshold == maxIndex) {
        batteryLow = true;
    }
    else {
        batteryLow = false;
    }
}

bool BatteryReadout::getBatteryLow() const {
    return batteryLow;
}