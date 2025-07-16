#include "sensors/VEML7700Sensor.h"

bool VEML7700Sensor::begin() {
    if (!veml.begin()) {
        lastError = "Could not find VEML7700 sensor";
        return false;
    }
    
    // Configure for indoor monitoring
    veml.setGain(VEML7700_GAIN_1_8);
    veml.setIntegrationTime(VEML7700_IT_100MS);
    
    initialized = true;
    return true;
}

bool VEML7700Sensor::read() {
    if (!initialized) {
        lastError = "Sensor not initialized";
        return false;
    }
    
    lux = veml.readLux();
    white = veml.readWhite();
    raw = veml.readALS();
    
    notifyUpdate();
    return true;
}

bool VEML7700Sensor::isReady() const {
    return initialized;
}

const char* VEML7700Sensor::getLastError() const {
    return lastError;
}
