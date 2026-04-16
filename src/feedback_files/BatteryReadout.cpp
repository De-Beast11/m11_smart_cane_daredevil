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
    float voltage = (rawValue / 4095.0) * 3.3;
    Serial.println(voltage);
    // Factor *2, because of the voltage divider
    float convertedVoltage = voltage * 2;
    // Filter voltage
    filteredVoltage = alpha * convertedVoltage + (1 - alpha) * filteredVoltage;
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
    // If the low battery was acked, but it has been some time -> reset the low battery ack
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
    // If the low battery was acked ->
    // Set the time at which this happened
    // And increase the time between low battery notifications
    if (flag) {
        timeLowBatteryNotificationAcknowledged = millis();
        timeBetweenLowBatteryNotification *= 2;
    }
    lowBatteryAcknowledged = flag;
}