#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

class LIS2DW12Sensor {
public:
    LIS2DW12Sensor();
    bool begin();
    bool isConnected() const;
    void readSensor(DynamicJsonDocument& doc);
    bool checkMotion();
    void update();
    
private:
    bool _connected;
    uint32_t _lastReading;
    
    // Acceleration values
    float _accelX, _accelY, _accelZ;
    bool _motionDetected;
    
    // Baseline for motion detection
    float _baselineX, _baselineY, _baselineZ;
    
    void readAcceleration();
    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t value);
};
