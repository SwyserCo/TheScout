#pragma once

#include <Arduino.h>
#include <Adafruit_VEML7700.h>
#include <ArduinoJson.h>

class VEML7700Sensor {
public:
    VEML7700Sensor();
    bool begin();
    bool isConnected() const;
    void readSensor(DynamicJsonDocument& doc);
    float getLux();
    
private:
    Adafruit_VEML7700 _veml;
    bool _connected;
    uint32_t _lastReading;
    float _lux;
    
    static constexpr uint32_t READING_INTERVAL = 500; // 0.5 seconds
};
