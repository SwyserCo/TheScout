
#pragma once
#include <Arduino.h>
#include "sensors/BME280.h"
#include "sensors/VEML7700.h"
#include "sensors/LD2420.h"
#include "sensors/Microphone.h"

class SensorManager {
public:
    void begin();
    void handle();
    float getTemperature() const;
    float getHumidity() const;
    float getPressure() const;
    float getAmbientLight() const;
    bool getPresence() const;
    float getSoundLevel() const;

private:
    BME280 bme280;
    VEML7700 veml7700;
    LD2420 ld2420;
    Microphone microphone;
    unsigned long lastPollTime = 0;
    const unsigned long pollInterval = 500; // ms
};
