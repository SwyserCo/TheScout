#include "sensors/PirSensor.h"
#include "config/Pins.h"

PirSensor::PirSensor() 
    : lastState(false), lastUpdate(0) {
    // Initialize sensor data
    sensorData.motionDetected = false;
    sensorData.lastDetectionTime = 0;
    sensorData.detectionCount = 0;
}

bool PirSensor::begin() {
    Serial.println("PirSensor::begin() called");
    // Implementation will be added in Phase 4
    return true;
}

void PirSensor::update() {
    // Serial.println("PirSensor::update() called");
    // Implementation will be added in Phase 4
}

PirData PirSensor::getData() {
    Serial.println("PirSensor::getData() called");
    return sensorData;
}

bool PirSensor::isMotionDetected() {
    Serial.println("PirSensor::isMotionDetected() called");
    return sensorData.motionDetected;
}
