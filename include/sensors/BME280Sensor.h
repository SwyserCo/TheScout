#pragma once
#include <Adafruit_BME280.h>

class BME280Sensor {
public:
    bool begin();
    void readValues(float& temperature, float& humidity, float& pressure);

private:
    Adafruit_BME280 bme;
};
