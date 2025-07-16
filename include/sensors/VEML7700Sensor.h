#pragma once
#include "BaseSensor.h"
#include "../Config.h"
#include <Wire.h>
#include <Adafruit_VEML7700.h>

class VEML7700Sensor : public BaseSensor {
public:
    bool begin() override;
    bool read() override;
    bool isReady() const override;
    const char* getLastError() const override;
    
    float getLux() const { return lux; }
    float getWhite() const { return white; }
    uint16_t getRaw() const { return raw; }
    
private:
    Adafruit_VEML7700 veml;
    float lux = 0.0F;
    float white = 0.0F;
    uint16_t raw = 0;
    bool initialized = false;
    const char* lastError = nullptr;
};
