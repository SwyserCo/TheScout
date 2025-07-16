#pragma once
#include <cstdint>

class VEML7700 {
public:
    bool begin() { return true; }
    void setGain(uint8_t gain) {}
    void setIntegrationTime(uint8_t itime) {}
    float readLux() { return 100.0f; }
    uint16_t readALS() { return 1000; }
    uint16_t readWhite() { return 1000; }
    float readHighResolution() { return 100.0f; }
    float getLux() { return readLux(); }
};
