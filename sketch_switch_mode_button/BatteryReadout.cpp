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
    // Factor *2, because of the voltage divider
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
    //// Functionality of low battery ack flag
    // If the low battery was acked, but the battery is not low anymore -> reset the low battery ack and time between notifications
    if (!batteryLow && lowBatteryAcknowledged) {
        lowBatteryAcknowledged = false;
        timeBetweenLowBatteryNotification = minTimeBetweenLowBatteryNotification;
    }
    // 
    if (lowBatteryAcknowledged && millis() - timeLowBatteryNotificationAcknowledged >= timeBetweenLowBatteryNotification) {
        lowBatteryAcknowledged = false;
    }
}

bool BatteryReadout::getBatteryLow() const {
    return batteryLow;
}

bool BatteryReadout::getLowBatteryAck() const {
    return lowBatteryAcknowledged;
}

void BatteryReadout::setLowBatteryAck(bool flag) {
    if (flag) {
        timeLowBatteryNotificationAcknowledged = millis();
        timeBetweenLowBatteryNotification *= 2;
    }
    lowBatteryAcknowledged = flag;
}