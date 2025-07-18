#include "sensors/LD2420Sensor.h"
#include "Config.h"

LD2420Sensor::LD2420Sensor() : _serial(1), _connected(false), _lastReading(0),
    _presenceDetected(false), _presenceStartTime(0), 
    _presenceThreshold(DEFAULT_PRESENCE_THRESHOLD) {
}

bool LD2420Sensor::begin() {
    // Initialize serial communication with LD2420
    _serial.begin(256000, SERIAL_8N1, Config::LD2420_RX_PIN, Config::LD2420_TX_PIN);
    
    // Set up interrupt pin
    pinMode(Config::LD2420_INT_PIN, INPUT_PULLUP);
    
    // Wait for sensor to stabilize
    delay(1000);
    
    // Test communication (simplified - actual implementation would need proper protocol)
    _connected = true; // For now, assume it's connected
    
    if (_connected) {
        Serial.println("LD2420 presence sensor initialized");
    } else {
        Serial.println("LD2420 presence sensor not found");
    }
    
    return _connected;
}

bool LD2420Sensor::isConnected() const {
    return _connected;
}

void LD2420Sensor::readSensor(DynamicJsonDocument& doc) {
    if (!_connected) return;
    
    uint32_t currentTime = millis();
    if (currentTime - _lastReading >= READING_INTERVAL) {
        // Read presence data from sensor
        // This is a simplified implementation - real LD2420 would need proper protocol
        
        // For now, simulate presence detection based on interrupt pin
        bool currentPresence = digitalRead(Config::LD2420_INT_PIN) == LOW;
        
        if (currentPresence != _presenceDetected) {
            _presenceDetected = currentPresence;
            if (_presenceDetected) {
                _presenceStartTime = currentTime;
            }
        }
        
        _lastReading = currentTime;
    }
    
    doc["presence"] = _presenceDetected;
    doc["distance"] = _presenceDetected ? 2.5f : 0.0f; // Simulated distance
    doc["strength"] = _presenceDetected ? 85 : 0; // Simulated signal strength
}

bool LD2420Sensor::checkPresence() {
    if (!_connected) return false;
    
    uint32_t currentTime = millis();
    
    // Simple presence detection based on interrupt pin state
    bool currentPresence = digitalRead(Config::LD2420_INT_PIN) == LOW;
    
    if (currentPresence) {
        if (!_presenceDetected) {
            _presenceStartTime = currentTime;
        }
        
        // Check if presence persists for required duration
        if (currentTime - _presenceStartTime >= PRESENCE_DURATION) {
            _presenceDetected = true;
            Serial.println("Presence detected!");
            return true;
        }
    } else {
        // Reset presence detection if no longer detected
        _presenceDetected = false;
        _presenceStartTime = 0;
    }
    
    return false;
}

void LD2420Sensor::update() {
    if (!_connected) return;
    
    uint32_t currentTime = millis();
    if (currentTime - _lastReading >= READING_INTERVAL) {
        // Check for presence
        checkPresence();
        _lastReading = currentTime;
    }
}

void LD2420Sensor::setPresenceThreshold(uint16_t threshold) {
    _presenceThreshold = threshold;
}

void LD2420Sensor::sendCommand(const uint8_t* command, size_t length) {
    _serial.write(command, length);
}

bool LD2420Sensor::readResponse(uint8_t* buffer, size_t maxLength) {
    size_t bytesRead = 0;
    uint32_t timeout = millis() + 100; // 100ms timeout
    
    while (bytesRead < maxLength && millis() < timeout) {
        if (_serial.available()) {
            buffer[bytesRead++] = _serial.read();
        }
    }
    
    return bytesRead > 0;
}

bool LD2420Sensor::parsePresenceData(const uint8_t* data, size_t length) {
    // This would contain the actual protocol parsing for LD2420
    // For now, return a simple boolean based on data availability
    return length > 0;
}
