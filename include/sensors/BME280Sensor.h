#pragma once
#include "BaseSensor.h"
#include "../Config.h"
#include <Wire.h>
#include <Adafruit_BME280.h>

class BME280Sensor : public BaseSensor {
public:
    BME280Sensor() = default;
    
    bool begin() override;
    bool read() override;
    bool isReady() const override;
    const char* getLastError() const override;
    
    // Data accessors
    float getTemperature() const { return temperature; }
    float getHumidity() const { return humidity; }
    float getPressure() const { return pressure; }

private:
    static constexpr const char* ERROR_NOT_FOUND = "Could not find BME280 sensor";
    static constexpr const char* ERROR_NOT_READY = "Sensor not ready";
    static constexpr const char* ERROR_INVALID_READING = "Invalid sensor readings";

    Adafruit_BME280 bme;
    float temperature = 0.0F;
    float humidity = 0.0F;
    float pressure = 0.0F;
    const char* lastError = nullptr;
    bool initialized = false;
};
