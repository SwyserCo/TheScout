#include "sensors/SensorManager.h"
#include <Arduino.h>

void SensorManager::begin() {
    bme280.begin();
    veml7700.begin();
    ld2420.begin();
    microphone.begin();
}

void SensorManager::handle() {
    unsigned long now = millis();
    if (now - lastPollTime >= pollInterval) {
        lastPollTime = now;
        bme280.read();
        veml7700.read();
        ld2420.read();
        microphone.read();
        // Publish sensor data via MQTT if needed
    }
}

float SensorManager::getTemperature() const { return bme280.getTemperature(); }
float SensorManager::getHumidity() const { return bme280.getHumidity(); }
float SensorManager::getPressure() const { return bme280.getPressure(); }
float SensorManager::getAmbientLight() const { return veml7700.getAmbientLight(); }
bool SensorManager::getPresence() const { return ld2420.isPresenceDetected(); }
float SensorManager::getSoundLevel() const { return microphone.getSoundLevel(); }
