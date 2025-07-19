#include "sensors/LIS2DW12Sensor.h"
#include "Config.h"
#include <Wire.h>

// LIS2DW12 Register Addresses
#define LIS2DW12_WHO_AM_I       0x0F
#define LIS2DW12_CTRL1          0x20
#define LIS2DW12_CTRL6          0x25
#define LIS2DW12_OUT_X_L        0x28
#define LIS2DW12_OUT_X_H        0x29
#define LIS2DW12_OUT_Y_L        0x2A
#define LIS2DW12_OUT_Y_H        0x2B
#define LIS2DW12_OUT_Z_L        0x2C
#define LIS2DW12_OUT_Z_H        0x2D

// Expected WHO_AM_I value
#define LIS2DW12_ID             0x44

LIS2DW12Sensor::LIS2DW12Sensor() : _connected(false), _lastReading(0),
    _accelX(0), _accelY(0), _accelZ(0), _motionDetected(false),
    _baselineX(0), _baselineY(0), _baselineZ(0) {
}

bool LIS2DW12Sensor::begin() {
    Serial.printf("[LIS2DW12] Starting initialization - trying multiple addresses\n");
    
    // Try common LIS2DW12 addresses: 0x19 (default) and 0x18 (alternate)
    uint8_t addresses[] = {0x19, 0x18};
    uint8_t foundAddress = 0;
    
    for (uint8_t addr : addresses) {
        Serial.printf("[LIS2DW12] Testing I2C address 0x%02X\n", addr);
        
        Wire.beginTransmission(addr);
        uint8_t error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.printf("[LIS2DW12] Found device at address 0x%02X\n", addr);
            foundAddress = addr;
            break;
        } else {
            Serial.printf("[LIS2DW12] No device at address 0x%02X (error=%d)\n", addr, error);
        }
    }
    
    if (foundAddress == 0) {
        Serial.println("[LIS2DW12] No device found at any address");
        _connected = false;
        return false;
    }
    
    // Update the address temporarily for testing
    // Note: This is a hack - we should properly configure this
    Serial.printf("[LIS2DW12] Using address 0x%02X for WHO_AM_I test\n", foundAddress);
    
    // Manual WHO_AM_I read using found address
    Wire.beginTransmission(foundAddress);
    Wire.write(LIS2DW12_WHO_AM_I);
    Wire.endTransmission();
    
    Wire.requestFrom((uint8_t)foundAddress, (uint8_t)1);
    uint8_t whoami = 0xFF;
    if (Wire.available()) {
        whoami = Wire.read();
    }
    
    Serial.printf("[LIS2DW12] WHO_AM_I register: 0x%02X (expected 0x44)\n", whoami);
    
    if (whoami != LIS2DW12_ID) {
        Serial.printf("[LIS2DW12] WHO_AM_I failed: 0x%02X (expected 0x%02X)\n", whoami, LIS2DW12_ID);
        _connected = false;
        return false;
    }
    
    Serial.println("[LIS2DW12] WHO_AM_I OK - sensor identified");
    
    // Simple configuration: 50Hz, ±2g scale, normal mode
    writeRegister(LIS2DW12_CTRL1, 0x50);  // 50Hz, normal mode
    writeRegister(LIS2DW12_CTRL6, 0x00);  // ±2g scale
    
    delay(100); // Let sensor stabilize
    
    // Read initial baseline (average of a few readings)
    Serial.println("[LIS2DW12] Setting baseline...");
    float sumX = 0, sumY = 0, sumZ = 0;
    for (int i = 0; i < 10; i++) {
        readAcceleration();
        sumX += _accelX;
        sumY += _accelY; 
        sumZ += _accelZ;
        delay(50);
    }
    
    _baselineX = sumX / 10.0f;
    _baselineY = sumY / 10.0f;
    _baselineZ = sumZ / 10.0f;
    
    Serial.printf("[LIS2DW12] Baseline set: X=%.3f, Y=%.3f, Z=%.3f\n", 
                  _baselineX, _baselineY, _baselineZ);
    
    _connected = true;
    return true;
}

bool LIS2DW12Sensor::isConnected() const {
    return _connected;
}

void LIS2DW12Sensor::readSensor(DynamicJsonDocument& doc) {
    if (!_connected) return;
    
    uint32_t currentTime = millis();
    if (currentTime - _lastReading >= 100) { // Read every 100ms
        readAcceleration();
        checkMotion();
        _lastReading = currentTime;
    }
    
    doc["accel_x"] = _accelX;
    doc["accel_y"] = _accelY;
    doc["accel_z"] = _accelZ;
    doc["motion"] = _motionDetected;
}

void LIS2DW12Sensor::readAcceleration() {
    if (!_connected) return;
    
    // Read raw acceleration data (6 bytes starting from OUT_X_L)
    Wire.beginTransmission(Config::LIS2DW12_I2C_ADDR);
    Wire.write(LIS2DW12_OUT_X_L | 0x80); // Auto-increment
    Wire.endTransmission();
    
    Wire.requestFrom((uint8_t)Config::LIS2DW12_I2C_ADDR, (uint8_t)6);
    
    if (Wire.available() >= 6) {
        int16_t rawX = Wire.read() | (Wire.read() << 8);
        int16_t rawY = Wire.read() | (Wire.read() << 8);
        int16_t rawZ = Wire.read() | (Wire.read() << 8);
        
        // Convert to g (±2g scale, 16-bit resolution)
        _accelX = rawX * 2.0f / 32768.0f;
        _accelY = rawY * 2.0f / 32768.0f;
        _accelZ = rawZ * 2.0f / 32768.0f;
    }
}

bool LIS2DW12Sensor::checkMotion() {
    if (!_connected) return false;
    
    // Simple motion detection: compare current reading to baseline
    float deltaX = abs(_accelX - _baselineX);
    float deltaY = abs(_accelY - _baselineY);
    float deltaZ = abs(_accelZ - _baselineZ);
    
    float totalChange = deltaX + deltaY + deltaZ;
    
    // Simple threshold - if total change > 0.1g, consider it motion
    _motionDetected = (totalChange > 0.1f);
    
    // Serial.printf("[LIS2DW12] X=%.3f Y=%.3f Z=%.3f | Change=%.3f | Motion=%s\n",
    //               _accelX, _accelY, _accelZ, totalChange, _motionDetected ? "YES" : "NO");
    
    return _motionDetected;
}

void LIS2DW12Sensor::update() {
    // Simple update - just read acceleration
    if (_connected) {
        readAcceleration();
        checkMotion();
    }
}

uint8_t LIS2DW12Sensor::readRegister(uint8_t reg) {
    Wire.beginTransmission(Config::LIS2DW12_I2C_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    
    Wire.requestFrom((uint8_t)Config::LIS2DW12_I2C_ADDR, (uint8_t)1);
    return Wire.available() ? Wire.read() : 0;
}

void LIS2DW12Sensor::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(Config::LIS2DW12_I2C_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}
