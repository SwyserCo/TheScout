#include "sensors/Ld2410sSensor.h"
#include "config/Pins.h"

Ld2410sSensor::Ld2410sSensor() 
    : lastUpdate(0) {
    // Initialize sensor data
    sensorData.movingTargetDetected = false;
    sensorData.stationaryTargetDetected = false;
    sensorData.movingTargetDistance = 0;
    sensorData.movingTargetEnergy = 0;
    sensorData.stationaryTargetDistance = 0;
    sensorData.stationaryTargetEnergy = 0;
    sensorData.lastUpdateTime = 0;
}

bool Ld2410sSensor::begin() {
    Serial.println("Ld2410sSensor::begin() called");
    // Implementation will be added in Phase 4
    return true;
}

void Ld2410sSensor::update() {
    // Serial.println("Ld2410sSensor::update() called");
    // Implementation will be added in Phase 4
}

RadarData Ld2410sSensor::getData() {
    Serial.println("Ld2410sSensor::getData() called");
    return sensorData;
}

bool Ld2410sSensor::isMovingTargetDetected() {
    Serial.println("Ld2410sSensor::isMovingTargetDetected() called");
    return sensorData.movingTargetDetected;
}

bool Ld2410sSensor::isStationaryTargetDetected() {
    Serial.println("Ld2410sSensor::isStationaryTargetDetected() called");
    return sensorData.stationaryTargetDetected;
}
