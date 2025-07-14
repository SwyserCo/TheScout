#pragma once
#include <Adafruit_VEML7700.h>

class VEML7700Sensor {
public:
    bool begin();
    float readLux();

private:
    Adafruit_VEML7700 veml;
};
