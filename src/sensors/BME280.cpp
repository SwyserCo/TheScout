#include "sensors/BME280.h"

void BME280::begin() {
    // Initialize BME280 sensor
}

void BME280::read() {
    // Read sensor values (stub)
    temperature = 22.5f;
    humidity = 45.0f;
    pressure = 1013.25f;
}

float BME280::getTemperature() const { return temperature; }
float BME280::getHumidity() const { return humidity; }
float BME280::getPressure() const { return pressure; }
