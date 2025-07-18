#include "sensors/VEML7700Sensor.h"
#include "Config.h"

VEML7700Sensor::VEML7700Sensor() : _connected(false), _lastReading(0), _lux(0) {
}

bool VEML7700Sensor::begin() {
    _connected = _veml.begin();
    
    if (_connected) {
        // Configure sensor for optimal performance
        _veml.setGain(VEML7700_GAIN_1);
        _veml.setIntegrationTime(VEML7700_IT_100MS);
        
        Serial.println("VEML7700 sensor initialized");
    } else {
        Serial.println("VEML7700 sensor not found");
    }
    
    return _connected;
}

bool VEML7700Sensor::isConnected() const {
    return _connected;
}

void VEML7700Sensor::readSensor(DynamicJsonDocument& doc) {
    if (!_connected) return;
    
    uint32_t currentTime = millis();
    if (currentTime - _lastReading >= READING_INTERVAL) {
        _lux = _veml.readLux();
        _lastReading = currentTime;
    }
    
    doc["illuminance"] = _lux;
    doc["lux"] = _lux;
    doc["als"] = _veml.readALS();
}

float VEML7700Sensor::getLux() {
    if (!_connected) return 0.0;
    
    uint32_t currentTime = millis();
    if (currentTime - _lastReading >= READING_INTERVAL) {
        _lux = _veml.readLux();
        _lastReading = currentTime;
    }
    
    return _lux;
}
