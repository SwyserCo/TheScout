#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>

class LIS2DW12Sensor {
public:
    LIS2DW12Sensor();
    bool begin();
    bool isConnected() const;
    void readSensor(DynamicJsonDocument& doc);
    void update();
    bool checkTamper();
    float getAccelX();
    float getAccelY();
    float getAccelZ();
    void setTamperThreshold(float threshold);
    void updateBaseline();

private:
    // LIS2DW12TR Register Addresses
    static constexpr uint8_t WHO_AM_I = 0x0F;
    static constexpr uint8_t CTRL1 = 0x20;
    static constexpr uint8_t CTRL2 = 0x21;
    static constexpr uint8_t CTRL3 = 0x22;
    static constexpr uint8_t STATUS = 0x27;
    static constexpr uint8_t OUT_X_L = 0x28;
    static constexpr uint8_t OUT_X_H = 0x29;
    static constexpr uint8_t OUT_Y_L = 0x2A;
    static constexpr uint8_t OUT_Y_H = 0x2B;
    static constexpr uint8_t OUT_Z_L = 0x2C;
    static constexpr uint8_t OUT_Z_H = 0x2D;
    
    // Expected WHO_AM_I value for LIS2DW12TR
    static constexpr uint8_t WHO_AM_I_VALUE = 0x44;
    
    // Configuration values
    static constexpr uint8_t CTRL1_ODR_100HZ = 0x50;  // 100Hz output data rate
    static constexpr uint8_t CTRL1_FS_2G = 0x00;      // ±2g full scale
    static constexpr uint8_t CTRL1_MODE_NORMAL = 0x04; // Normal mode
    
    static constexpr uint32_t READING_INTERVAL = 100;  // 100ms between readings
    static constexpr uint32_t TAMPER_DURATION = 100;   // 100ms tamper detection duration
    static constexpr float DEFAULT_TAMPER_THRESHOLD = 0.2f; // 0.2g threshold
    static constexpr float SCALE_FACTOR = 0.000061f;   // LSB to g conversion (±2g, 16-bit)

    bool _connected;
    uint32_t _lastReading;
    float _accelX, _accelY, _accelZ;
    float _baselineX, _baselineY, _baselineZ;
    float _tamperThreshold;
    bool _tamperDetected;
    uint32_t _tamperStartTime;
    bool _baselineSet;
    uint8_t _deviceAddress;

    // Helper functions
    bool writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    int16_t readRegister16(uint8_t reg);
    float calculateMagnitude(float x, float y, float z);
};
