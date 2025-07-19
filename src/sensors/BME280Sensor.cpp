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
        // Add retry logic for BME280 readings to handle I2C errors
        for (int retry = 0; retry < 3; retry++) {
            _temperature = _bme.readTemperature();
            _humidity = _bme.readHumidity();
            _pressure = _bme.readPressure() / 100.0F; // Convert to hPa
            
            // Validate readings - BME280 should give reasonable values
            if (!isnan(_temperature) && !isnan(_humidity) && !isnan(_pressure) &&
                _temperature > -40 && _temperature < 85 &&  // BME280 range
                _humidity >= 0 && _humidity <= 100 &&       // Humidity range
                _pressure > 300 && _pressure < 1100) {      // Pressure range
                break; // Good readings, exit retry loop
            }
            
            if (retry < 2) {
                delay(50); // Small delay before retry
            }
        }
        
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
