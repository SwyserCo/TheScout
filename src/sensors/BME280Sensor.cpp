#include "sensors/BME280Sensor.h"
#include <Wire.h>

bool BME280Sensor::begin() {
    return bme.begin(0x76, &Wire);  // Use the correct I2C bus
}

void BME280Sensor::readValues(float& temperature, float& humidity, float& pressure) {
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure() / 100.0F;  // Convert Pa to hPa
}
