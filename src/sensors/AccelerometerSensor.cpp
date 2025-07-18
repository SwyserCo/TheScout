#include "sensors/AccelerometerSensor.h"
#include "Config.h"

AccelerometerSensor::AccelerometerSensor() : _connected(false), _lastReading(0),
    _accelX(0), _accelY(0), _accelZ(0), _baselineX(0), _baselineY(0), _baselineZ(0),
    _tamperThreshold(DEFAULT_TAMPER_THRESHOLD), _tamperDetected(false), 
    _tamperStartTime(0), _baselineSet(false) {
}

bool AccelerometerSensor::begin() {
    _connected = _mpu.begin();
    
    if (_connected) {
        // Configure sensor
        _mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
        _mpu.setGyroRange(MPU6050_RANGE_250_DEG);
        _mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
        
        // Set up interrupt pin
        pinMode(Config::ACCEL_INT_PIN, INPUT_PULLUP);
        
        Serial.println("MPU6050 accelerometer initialized");
        
        // Wait a bit and then establish baseline
        delay(1000);
        updateBaseline();
    } else {
        Serial.println("MPU6050 accelerometer not found");
    }
    
    return _connected;
}

bool AccelerometerSensor::isConnected() const {
    return _connected;
}

void AccelerometerSensor::readSensor(DynamicJsonDocument& doc) {
    if (!_connected) return;
    
    uint32_t currentTime = millis();
    if (currentTime - _lastReading >= READING_INTERVAL) {
        sensors_event_t accel, gyro, temp;
        _mpu.getEvent(&accel, &gyro, &temp);
        
        _accelX = accel.acceleration.x;
        _accelY = accel.acceleration.y;
        _accelZ = accel.acceleration.z;
        
        _lastReading = currentTime;
    }
    
    doc["accel_x"] = _accelX;
    doc["accel_y"] = _accelY;
    doc["accel_z"] = _accelZ;
    doc["tamper"] = _tamperDetected;
    doc["motion_detected"] = _tamperDetected;
}

bool AccelerometerSensor::checkTamper() {
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
        }
        
        // Check if tamper condition persists for required duration
        if (currentTime - _tamperStartTime >= TAMPER_DURATION) {
            _tamperDetected = true;
            Serial.println("Tamper detected!");
            return true;
        }
    } else {
        // Reset tamper detection if movement stops
        _tamperDetected = false;
        _tamperStartTime = 0;
    }
    
    return false;
}

void AccelerometerSensor::update() {
    if (!_connected) return;
    
    uint32_t currentTime = millis();
    if (currentTime - _lastReading >= READING_INTERVAL) {
        sensors_event_t accel, gyro, temp;
        _mpu.getEvent(&accel, &gyro, &temp);
        
        _accelX = accel.acceleration.x;
        _accelY = accel.acceleration.y;
        _accelZ = accel.acceleration.z;
        
        _lastReading = currentTime;
        
        // Check for tamper
        checkTamper();
    }
}

void AccelerometerSensor::setTamperThreshold(float threshold) {
    _tamperThreshold = threshold;
}

void AccelerometerSensor::updateBaseline() {
    if (!_connected) return;
    
    float sumX = 0, sumY = 0, sumZ = 0;
    
    for (int i = 0; i < BASELINE_SAMPLES; i++) {
        sensors_event_t accel, gyro, temp;
        _mpu.getEvent(&accel, &gyro, &temp);
        
        sumX += accel.acceleration.x;
        sumY += accel.acceleration.y;
        sumZ += accel.acceleration.z;
        
        delay(20); // Small delay between samples
    }
    
    _baselineX = sumX / BASELINE_SAMPLES;
    _baselineY = sumY / BASELINE_SAMPLES;
    _baselineZ = sumZ / BASELINE_SAMPLES;
    
    _baselineSet = true;
    
    Serial.print("Accelerometer baseline set: ");
    Serial.print(_baselineX); Serial.print(", ");
    Serial.print(_baselineY); Serial.print(", ");
    Serial.println(_baselineZ);
}

float AccelerometerSensor::calculateMagnitude(float x, float y, float z) {
    return sqrt(x*x + y*y + z*z);
}
