#pragma once
#include <cstdint>

#define VEML7700_GAIN_1_8 0x2
#define VEML7700_GAIN_1_4 0x3
#define VEML7700_GAIN_1 0x0
#define VEML7700_GAIN_2 0x1

#define VEML7700_IT_25MS 0xC
#define VEML7700_IT_50MS 0x8
#define VEML7700_IT_100MS 0x0
#define VEML7700_IT_200MS 0x1
#define VEML7700_IT_400MS 0x2
#define VEML7700_IT_800MS 0x3

class Adafruit_VEML7700 {
public:
    bool begin() { return true; }
    void setGain(uint8_t gain) {}
    void setIntegrationTime(uint8_t it) {}
    float readLux() { return 100.0f; }
    uint16_t readALS() { return 1000; }
    uint16_t readWhite() { return 1000; }
};
