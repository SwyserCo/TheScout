#pragma once

#include <Arduino.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>

class BME280Sensor {
public:
    BME280Sensor();
    bool begin();
    bool isConnected() const;
    void readSensor(DynamicJsonDocument& doc);
    float getTemperature();
    float getHumidity();
    float getPressure();
    
private:
    Adafruit_BME280 _bme;
    bool _connected;
    uint32_t _lastReading;
    float _temperature;
    float _humidity;
    float _pressure;
    
    static constexpr uint32_t READING_INTERVAL = 1000; // 1 second
};
