#pragma once
#include <cstdint>

class Adafruit_BME280 {
public:
    bool begin(uint8_t addr = 0x77) { return true; }
    float readTemperature() { return 20.0f; }
    float readPressure() { return 1013.25f; }
    float readHumidity() { return 50.0f; }
};
