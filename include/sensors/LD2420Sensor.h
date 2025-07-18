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
    void setPresenceThreshold(uint16_t threshold);
    
private:
    HardwareSerial _serial;
    bool _connected;
    uint32_t _lastReading;
    bool _presenceDetected;
    uint32_t _presenceStartTime;
    uint16_t _presenceThreshold;
    
    static constexpr uint32_t READING_INTERVAL = 200; // 0.2 seconds
    static constexpr uint32_t PRESENCE_DURATION = 5000; // 5 seconds
    static constexpr uint16_t DEFAULT_PRESENCE_THRESHOLD = 500;
    
    void sendCommand(const uint8_t* command, size_t length);
    bool readResponse(uint8_t* buffer, size_t maxLength);
    bool parsePresenceData(const uint8_t* data, size_t length);
};
