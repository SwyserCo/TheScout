#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>

class LD2420Sensor {
public:
    LD2420Sensor();
    bool begin();
    bool isConnected() const;
    void readSensor(DynamicJsonDocument& doc);
    bool checkPresence();
    void update();
    
private:
    HardwareSerial _serial;
    bool _connected;
    uint32_t _lastReading;
    
    // Simple presence data
    bool _presenceDetected;
    float _distance;
    uint8_t _signalStrength;
    
    void checkForData();
};
