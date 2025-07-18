#pragma once

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>

class AccelerometerSensor {
public:
    AccelerometerSensor();
    bool begin();
    bool isConnected() const;
    void readSensor(DynamicJsonDocument& doc);
    bool checkTamper();
    void update();
    void setTamperThreshold(float threshold);
    
private:
    Adafruit_MPU6050 _mpu;
    bool _connected;
    uint32_t _lastReading;
    float _accelX, _accelY, _accelZ;
    float _baselineX, _baselineY, _baselineZ;
    float _tamperThreshold;
    bool _tamperDetected;
    uint32_t _tamperStartTime;
    bool _baselineSet;
    
    static constexpr uint32_t READING_INTERVAL = 100; // 0.1 seconds
    static constexpr uint32_t TAMPER_DURATION = 3000; // 3 seconds
    static constexpr uint32_t BASELINE_SAMPLES = 50;
    static constexpr float DEFAULT_TAMPER_THRESHOLD = 2.0; // m/s²
    
    void updateBaseline();
    float calculateMagnitude(float x, float y, float z);
};
