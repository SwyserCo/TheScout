#pragma once
#include <cstdint>

class Adafruit_VEML7700 {
public:
    bool begin() { return true; }
    float getLux() { return 100.0f; }
    uint16_t readALS() { return 1000; }
    uint16_t readWhite() { return 1000; }
    float normalize(uint16_t raw) { return static_cast<float>(raw); }
};
