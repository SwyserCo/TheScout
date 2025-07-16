#pragma once
#include <cstdint>

class Adafruit_BME280 {
public:
    enum {
        MODE_NORMAL = 0x03,
        SAMPLING_X1 = 0x01,
        SAMPLING_X2 = 0x02,
        SAMPLING_X16 = 0x05,
        FILTER_X16 = 0x04,
        STANDBY_MS_500 = 0x05
    };

    bool begin(uint8_t addr = 0x77) { return true; }
    void setSampling(uint8_t mode = MODE_NORMAL,
                    uint8_t tempSampling = SAMPLING_X16,
                    uint8_t pressSampling = SAMPLING_X16,
                    uint8_t humSampling = SAMPLING_X16,
                    uint8_t filter = FILTER_X16,
                    uint8_t duration = STANDBY_MS_500) {}
                    
    float readTemperature() { return 20.0f; }
    float readHumidity() { return 50.0f; }
    float readPressure() { return 101325.0f; }
};
