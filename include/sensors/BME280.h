#pragma once
#include <Arduino.h>

class BME280 {
public:
    void begin();
    void read();
    float getTemperature() const;
    float getHumidity() const;
    float getPressure() const;
private:
    float temperature = 0.0f;
    float humidity = 0.0f;
    float pressure = 0.0f;
};
