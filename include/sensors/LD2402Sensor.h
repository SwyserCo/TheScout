#pragma once
#include "BaseSensor.h"
#include "../Config.h"
#include <HardwareSerial.h>

class LD2402Sensor : public BaseSensor {
public:
    struct MotionData {
        uint8_t presenceState;
        uint8_t movementState;
        uint16_t distance;
        uint8_t energy;
    };
    
    LD2402Sensor() : serial(1) {} // Using UART1
    
    bool begin() override;
    bool read() override;
    bool isReady() const override;
    const char* getLastError() const override;
    
    const MotionData& getData() const { return data; }
    
private:
    bool parseData();
    
    HardwareSerial serial;
    MotionData data = {};
    bool initialized = false;
    const char* lastError = nullptr;
};
