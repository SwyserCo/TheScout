#include "sensors/BME280Sensor.h"

bool BME280Sensor::begin() {
    if (!bme.begin(Config::BME280_I2C_ADDR)) {
        lastError = "Could not find BME280 sensor";
        return false;
    }
    
    // Set recommended settings for indoor monitoring
    bme.setSampling(
        Adafruit_BME280::MODE_NORMAL,
        Adafruit_BME280::SAMPLING_X2,  // temperature
        Adafruit_BME280::SAMPLING_X16, // pressure
        Adafruit_BME280::SAMPLING_X1,  // humidity
        Adafruit_BME280::FILTER_X16,
        Adafruit_BME280::STANDBY_MS_500
    );
    
    initialized = true;
    return true;
}

bool BME280Sensor::read() {
    if (!initialized) {
        lastError = "Sensor not initialized";
        return false;
    }
    
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure() / 100.0F; // Convert to hPa
    
    notifyUpdate();
    return true;
}

bool BME280Sensor::isReady() const {
    return initialized;  // We check sensor ID during begin()
}

const char* BME280Sensor::getLastError() const {
    return lastError;
}
