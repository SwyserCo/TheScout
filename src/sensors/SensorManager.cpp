#include "sensors/SensorManager.h"
#include "config/Settings.h"

SensorManager::SensorManager() 
    : lastUpdate(0) {
}

bool SensorManager::begin() {
    Serial.println("SensorManager::begin() called");
    
    // Initialize PIR sensor
    if (!pirSensor.begin()) {
        Serial.println("SensorManager: Failed to initialize PIR sensor");
        return false;
    }
    
    // Initialize radar sensor
    if (!radarSensor.begin()) {
        Serial.println("SensorManager: Failed to initialize radar sensor");
        return false;
    }
    
    // Initialize power status
    if (!powerStatus.begin()) {
        Serial.println("SensorManager: Failed to initialize power status");
        return false;
    }
    
    Serial.println("SensorManager initialized successfully");
    return true;
}

void SensorManager::update() {
    // Serial.println("SensorManager::update() called");
    pirSensor.update();
    radarSensor.update();
    powerStatus.update();
    // Implementation will be expanded in Phase 4
}

PirData SensorManager::getPirData() {
    Serial.println("SensorManager::getPirData() called");
    return pirSensor.getData();
}

RadarData SensorManager::getRadarData() {
    Serial.println("SensorManager::getRadarData() called");
    return radarSensor.getData();
}

PowerData SensorManager::getPowerData() {
    Serial.println("SensorManager::getPowerData() called");
    return powerStatus.getData();
}

bool SensorManager::isMotionDetected() {
    Serial.println("SensorManager::isMotionDetected() called");
    // Implementation will be added in Phase 4
    return false;
}
