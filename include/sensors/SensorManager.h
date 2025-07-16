#pragma once
#include "BME280Sensor.h"
#include "VEML7700Sensor.h"
#include "LD2402Sensor.h"
#include "MicrophoneSensor.h"
#include "../Config.h"
#include <vector>
#include <memory>

class SensorManager {
public:
    bool begin() {
        // Initialize I2C
        Wire.begin(Config::I2C_SDA_PIN, Config::I2C_SCL_PIN);
        Wire.setClock(Config::I2C_FREQ);
        
        // Initialize all sensors
        bool success = true;
        
        if (!bme280.begin()) {
            lastError = bme280.getLastError();
            success = false;
        }
        
        if (!veml7700.begin()) {
            lastError = veml7700.getLastError();
            success = false;
        }
        
        if (!ld2402.begin()) {
            lastError = ld2402.getLastError();
            success = false;
        }
        
        if (!microphone.begin()) {
            lastError = microphone.getLastError();
            success = false;
        }
        
        // Set up periodic reading
        lastReadTime = 0;
        return success;
    }
    
    void update() {
        unsigned long currentTime = millis();
        if (currentTime - lastReadTime >= Config::SENSOR_READ_INTERVAL) {
            readAllSensors();
            lastReadTime = currentTime;
        }
    }
    
    // Sensor accessors
    BME280Sensor& getBME280() { return bme280; }
    VEML7700Sensor& getVEML7700() { return veml7700; }
    LD2402Sensor& getLD2402() { return ld2402; }
    MicrophoneSensor& getMicrophone() { return microphone; }
    
    const char* getLastError() const { return lastError; }
    
    using UpdateCallback = std::function<void(void)>;
    void setUpdateCallback(UpdateCallback callback) {
        updateCallback = callback;
    }

private:
    void readAllSensors() {
        bme280.read();
        veml7700.read();
        ld2402.read();
        microphone.read();
        
        if (updateCallback) {
            updateCallback();
        }
    }
    
    BME280Sensor bme280;
    VEML7700Sensor veml7700;
    LD2402Sensor ld2402;
    MicrophoneSensor microphone;
    UpdateCallback updateCallback;
    unsigned long lastReadTime;
    const char* lastError = nullptr;
};
