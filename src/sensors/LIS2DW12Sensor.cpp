#include "sensors/LIS2DW12Sensor.h"
#include "Config.h"

LIS2DW12Sensor::LIS2DW12Sensor() : _connected(false), _lastReading(0),
    _accelX(0), _accelY(0), _accelZ(0), _baselineX(0), _baselineY(0), _baselineZ(0),
    _tamperThreshold(DEFAULT_TAMPER_THRESHOLD), _tamperDetected(false), 
    _tamperStartTime(0), _baselineSet(false), _deviceAddress(Config::LIS2DW12_I2C_ADDR) {
}

bool LIS2DW12Sensor::begin() {
    // Check WHO_AM_I register
    uint8_t whoAmI = readRegister(WHO_AM_I);
    if (whoAmI != WHO_AM_I_VALUE) {
        Serial.printf("LIS2DW12TR WHO_AM_I mismatch: expected 0x%02X, got 0x%02X\n", WHO_AM_I_VALUE, whoAmI);
        return false;
    }
    
    // Configure the sensor
    // CTRL1: 100Hz ODR, ±2g, normal mode, enable X,Y,Z axes
    if (!writeRegister(CTRL1, CTRL1_ODR_100HZ | CTRL1_FS_2G | CTRL1_MODE_NORMAL | 0x07)) {
        Serial.println("Failed to configure LIS2DW12TR CTRL1");
        return false;
    }
    
    // CTRL2: Default settings (no high-pass filter)
    if (!writeRegister(CTRL2, 0x00)) {
        Serial.println("Failed to configure LIS2DW12TR CTRL2");
        return false;
    }
    
    // CTRL3: Default settings
    if (!writeRegister(CTRL3, 0x00)) {
        Serial.println("Failed to configure LIS2DW12TR CTRL3");
        return false;
    }
    
    _connected = true;
    Serial.println("LIS2DW12TR accelerometer initialized");
    
    // Wait a bit and then establish baseline
    delay(100);
    updateBaseline();
    
    return true;
}

bool LIS2DW12Sensor::isConnected() const {
    return _connected;
}

void LIS2DW12Sensor::readSensor(DynamicJsonDocument& doc) {
    if (!_connected) return;
    
    uint32_t currentTime = millis();
    if (currentTime - _lastReading >= READING_INTERVAL) {
        // Read raw acceleration data
        int16_t rawX = readRegister16(OUT_X_L);
        int16_t rawY = readRegister16(OUT_Y_L);
        int16_t rawZ = readRegister16(OUT_Z_L);
        
        // Convert to g values
        _accelX = rawX * SCALE_FACTOR;
        _accelY = rawY * SCALE_FACTOR;
        _accelZ = rawZ * SCALE_FACTOR;
        
        // Check for motion
        checkTamper();
        
        _lastReading = currentTime;
    }
    
    doc["accel_x"] = _accelX;
    doc["accel_y"] = _accelY;
    doc["accel_z"] = _accelZ;
    doc["tamper"] = _tamperDetected;
    doc["motion_detected"] = _tamperDetected;
    
    // Add baseline and deviation info for debugging
    if (_baselineSet) {
        float deviation = sqrt(pow(_accelX - _baselineX, 2) + pow(_accelY - _baselineY, 2) + pow(_accelZ - _baselineZ, 2));
        
        #ifdef DEBUG
        Serial.printf("[LIS2DW12] Current acceleration: X=%.3f, Y=%.3f, Z=%.3f (g)\n", _accelX, _accelY, _accelZ);
        Serial.printf("[LIS2DW12] Baseline: X=%.3f, Y=%.3f, Z=%.3f (g)\n", _baselineX, _baselineY, _baselineZ);
        Serial.printf("[LIS2DW12] Deviation: %.3f (threshold: %.3f)\n", deviation, _tamperThreshold);
        #endif
    }
}

bool LIS2DW12Sensor::checkTamper() {
    if (!_connected || !_baselineSet) return false;
    
    // Calculate deviation from baseline
    float deltaX = abs(_accelX - _baselineX);
    float deltaY = abs(_accelY - _baselineY);
    float deltaZ = abs(_accelZ - _baselineZ);
    
    float deviation = calculateMagnitude(deltaX, deltaY, deltaZ);
    
    uint32_t currentTime = millis();
    
    if (deviation > _tamperThreshold) {
        if (!_tamperDetected) {
            _tamperStartTime = currentTime;
            Serial.printf("Motion started: deviation=%.3fg (threshold=%.3fg)\n", deviation, _tamperThreshold);
        }
        
        // Check if tamper condition persists for required duration
        if (currentTime - _tamperStartTime >= TAMPER_DURATION) {
            _tamperDetected = true;
            return true;
        }
    } else {
        // Reset tamper detection if below threshold for some time
        if (_tamperDetected && currentTime - _tamperStartTime >= (TAMPER_DURATION * 5)) {
            _tamperDetected = false;
            _tamperStartTime = 0;
            Serial.println("Motion ended");
        }
    }
    
    return _tamperDetected;
}

void LIS2DW12Sensor::update() {
    if (!_connected) return;
    
    uint32_t currentTime = millis();
    if (currentTime - _lastReading >= READING_INTERVAL) {
        // Read current acceleration values
        int16_t rawX = readRegister16(OUT_X_L);
        int16_t rawY = readRegister16(OUT_Y_L);
        int16_t rawZ = readRegister16(OUT_Z_L);
        
        _accelX = rawX * SCALE_FACTOR;
        _accelY = rawY * SCALE_FACTOR;
        _accelZ = rawZ * SCALE_FACTOR;
        
        // Check for tamper
        checkTamper();
        
        _lastReading = currentTime;
    }
}

float LIS2DW12Sensor::getAccelX() {
    return _accelX;
}

float LIS2DW12Sensor::getAccelY() {
    return _accelY;
}

float LIS2DW12Sensor::getAccelZ() {
    return _accelZ;
}

void LIS2DW12Sensor::setTamperThreshold(float threshold) {
    _tamperThreshold = threshold;
}

void LIS2DW12Sensor::updateBaseline() {
    if (!_connected) return;
    
    // Take several readings and average them
    float sumX = 0, sumY = 0, sumZ = 0;
    const int numReadings = 10;
    
    for (int i = 0; i < numReadings; i++) {
        int16_t rawX = readRegister16(OUT_X_L);
        int16_t rawY = readRegister16(OUT_Y_L);
        int16_t rawZ = readRegister16(OUT_Z_L);
        
        sumX += rawX * SCALE_FACTOR;
        sumY += rawY * SCALE_FACTOR;
        sumZ += rawZ * SCALE_FACTOR;
        
        delay(10);
    }
    
    _baselineX = sumX / numReadings;
    _baselineY = sumY / numReadings;
    _baselineZ = sumZ / numReadings;
    
    _baselineSet = true;
    
    Serial.printf("LIS2DW12TR baseline set: X=%.3f, Y=%.3f, Z=%.3f\n", 
                  _baselineX, _baselineY, _baselineZ);
}

bool LIS2DW12Sensor::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(_deviceAddress);
    Wire.write(reg);
    Wire.write(value);
    return Wire.endTransmission() == 0;
}

uint8_t LIS2DW12Sensor::readRegister(uint8_t reg) {
    Wire.beginTransmission(_deviceAddress);
    Wire.write(reg);
    if (Wire.endTransmission() != 0) {
        return 0;
    }
    
    Wire.requestFrom(_deviceAddress, (uint8_t)1);
    if (Wire.available()) {
        return Wire.read();
    }
    return 0;
}

int16_t LIS2DW12Sensor::readRegister16(uint8_t reg) {
    Wire.beginTransmission(_deviceAddress);
    Wire.write(reg | 0x80);  // Set MSB for auto-increment
    if (Wire.endTransmission() != 0) {
        return 0;
    }
    
    Wire.requestFrom(_deviceAddress, (uint8_t)2);
    if (Wire.available() >= 2) {
        uint8_t lowByte = Wire.read();
        uint8_t highByte = Wire.read();
        return (int16_t)((highByte << 8) | lowByte);
    }
    return 0;
}

float LIS2DW12Sensor::calculateMagnitude(float x, float y, float z) {
    return sqrt(x * x + y * y + z * z);
}
