#include "sensors/LD2420Sensor.h"
#include "Config.h"

LD2420Sensor::LD2420Sensor() : _serial(1), _connected(false), _lastReading(0),
    _presenceDetected(false), _presenceStartTime(0), 
    _presenceThreshold(DEFAULT_PRESENCE_THRESHOLD), _distance(0.0f), 
    _signalStrength(0), _movingTarget(false), _staticTarget(false), bufferIndex(0) {
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
        
        // Read data from LD2420 via UART
        readLD2420Data();
        
        _lastReading = currentTime;
    }
    
    doc["presence"] = _presenceDetected;
    doc["distance"] = _distance;
    doc["strength"] = _signalStrength;
    doc["moving_target"] = _movingTarget;
    doc["static_target"] = _staticTarget;
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

void LD2420Sensor::readLD2420Data() {
        // Read incoming UART data
    while (_serial.available() > 0) {
        uint8_t byte = _serial.read();
        
        #ifdef DEBUG
        Serial.printf("[LD2420] Received byte: 0x%02X\n", byte);
        #endif
        
        // Check for frame header
        if (byte == 0xF4) {
            bufferIndex = 0;
            buffer[bufferIndex++] = byte;
        } else if (bufferIndex > 0) {
            buffer[bufferIndex++] = byte;
            
            // Check if we have a complete frame
            if (bufferIndex >= 23) {
                // Verify frame header
                if (buffer[0] == 0xF4 && buffer[1] == 0xF3 && 
                    buffer[2] == 0xF2 && buffer[3] == 0xF1) {
                    
                    #ifdef DEBUG
                    Serial.print("[LD2420] Frame received: ");
                    for (int i = 0; i < 23; i++) {
                        Serial.printf("0x%02X ", buffer[i]);
                    }
                    Serial.println();
                    #endif
                    
                    parseFrame();
                }
                bufferIndex = 0;
            }
        }
    }
}

void LD2420Sensor::parseFrame() {
    // Parse 23-byte frame according to LD2420 protocol
    // Frame format: [F4 F3 F2 F1] [length] [command] [data...] [checksum]
    
    // Extract moving target data (bytes 5-6)
    uint16_t movingDistance = (buffer[6] << 8) | buffer[5];
    
    // Extract static target data (bytes 7-8) 
    uint16_t staticDistance = (buffer[8] << 8) | buffer[7];
    
    // Extract signal strengths (bytes 9-10)
    _signalStrength = buffer[9];
    
    // Update presence detection based on targets
    _movingTarget = (movingDistance > 0 && movingDistance < 6000); // Max 6m range
    _staticTarget = (staticDistance > 0 && staticDistance < 6000);
    _presenceDetected = _movingTarget || _staticTarget;
    
    // Use closer distance
    if (_movingTarget && _staticTarget) {
        _distance = min(movingDistance, staticDistance) / 100.0f; // Convert to meters
    } else if (_movingTarget) {
        _distance = movingDistance / 100.0f;
    } else if (_staticTarget) {
        _distance = staticDistance / 100.0f;
    } else {
        _distance = 0.0f;
    }
    
    #ifdef DEBUG
    Serial.printf("[LD2420] Moving: %s (%.2fm), Static: %s (%.2fm), Signal: %d\n", 
                  _movingTarget ? "YES" : "NO", movingDistance/100.0f,
                  _staticTarget ? "YES" : "NO", staticDistance/100.0f,
                  _signalStrength);
    #endif
}

void LD2420Sensor::parseLD2420Frame(const uint8_t* frame, size_t length) {
    if (length < 23) return;
    
    // LD2420 frame format (after header):
    // [4-7]: Data length
    // [8-9]: Target state (0x00=no target, 0x01=moving, 0x02=static, 0x03=both)
    // [10-11]: Moving target distance (cm)
    // [12]: Moving target energy
    // [13-14]: Static target distance (cm) 
    // [15]: Static target energy
    // [16-17]: Detection distance (cm)
    // [18-22]: Footer (0xF8 0xF7 0xF6 0xF5)
    
    if (length >= 18) {
        uint8_t targetState = frame[8];
        uint16_t movingDistance = (frame[11] << 8) | frame[10];
        uint8_t movingEnergy = frame[12];
        uint16_t staticDistance = (frame[14] << 8) | frame[13];
        uint8_t staticEnergy = frame[15];
        uint16_t detectionDistance = (frame[17] << 8) | frame[16];
        
        // Update sensor state
        _movingTarget = (targetState & 0x01) != 0;
        _staticTarget = (targetState & 0x02) != 0;
        _presenceDetected = _movingTarget || _staticTarget;
        
        if (_presenceDetected) {
            if (_movingTarget && movingDistance > 0) {
                _distance = movingDistance / 100.0f; // Convert cm to meters
                _signalStrength = movingEnergy;
            } else if (_staticTarget && staticDistance > 0) {
                _distance = staticDistance / 100.0f;
                _signalStrength = staticEnergy;
            } else {
                _distance = detectionDistance / 100.0f;
                _signalStrength = max(movingEnergy, staticEnergy);
            }
        } else {
            _distance = 0.0f;
            _signalStrength = 0;
        }
        
        Serial.printf("LD2420: State=0x%02X, Moving=%dcm(%d), Static=%dcm(%d), Detection=%dcm\n",
                      targetState, movingDistance, movingEnergy, staticDistance, staticEnergy, detectionDistance);
    }
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
