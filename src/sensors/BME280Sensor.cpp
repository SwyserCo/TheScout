#include "sensors/BME280Sensor.h"
#include "Config.h"

BME280Sensor::BME280Sensor() : _connected(false), _lastReading(0), 
    _temperature(0), _humidity(0), _pressure(0) {
}

bool BME280Sensor::begin() {
    Wire.begin(Config::I2C_SDA_PIN, Config::I2C_SCL_PIN);
    _connected = _bme.begin(Config::BME280_I2C_ADDR);
    
    if (_connected) {
        // Configure sensor for normal mode, 16x oversampling
        _bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                        Adafruit_BME280::SAMPLING_X16,  // temperature
                        Adafruit_BME280::SAMPLING_X16,  // pressure
                        Adafruit_BME280::SAMPLING_X16,  // humidity
                        Adafruit_BME280::FILTER_X16,
                        Adafruit_BME280::STANDBY_MS_500);
        
        Serial.println("BME280 sensor initialized");
    } else {
        Serial.println("BME280 sensor not found");
    }
    
    return _connected;
}

bool BME280Sensor::isConnected() const {
    return _connected;
}

void BME280Sensor::readSensor(DynamicJsonDocument& doc) {
    if (!_connected) return;
    
    uint32_t currentTime = millis();
    if (currentTime - _lastReading >= READING_INTERVAL) {
        _temperature = _bme.readTemperature();
        _humidity = _bme.readHumidity();
        _pressure = _bme.readPressure() / 100.0F; // Convert to hPa
        
        _lastReading = currentTime;
    }
    
    doc["temperature"] = _temperature;
    doc["humidity"] = _humidity;
    doc["pressure"] = _pressure;
}

float BME280Sensor::getTemperature() {
    if (!_connected) return 0.0;
    
    uint32_t currentTime = millis();
    if (currentTime - _lastReading >= READING_INTERVAL) {
        _temperature = _bme.readTemperature();
        _lastReading = currentTime;
    }
    
    return _temperature;
}

float BME280Sensor::getHumidity() {
    if (!_connected) return 0.0;
    
    uint32_t currentTime = millis();
    if (currentTime - _lastReading >= READING_INTERVAL) {
        _humidity = _bme.readHumidity();
        _lastReading = currentTime;
    }
    
    return _humidity;
}

float BME280Sensor::getPressure() {
    if (!_connected) return 0.0;
    
    uint32_t currentTime = millis();
    if (currentTime - _lastReading >= READING_INTERVAL) {
        _pressure = _bme.readPressure() / 100.0F; // Convert to hPa
        _lastReading = currentTime;
    }
    
    return _pressure;
}
