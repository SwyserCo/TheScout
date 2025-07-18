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
    #ifdef DEBUG
    static uint32_t lastDebugTime = 0;
    uint32_t currentTime = millis();
    if (currentTime - lastDebugTime > 5000) { // Debug every 5 seconds
        int availableBytes = _serial.available();
        Serial.printf("[LD2420] Available bytes: %d\n", availableBytes);
        if (availableBytes == 0) {
            Serial.println("[LD2420] No data received - check wiring and power");
        }
        lastDebugTime = currentTime;
    }
    #endif
    
    // Read incoming UART data
    while (_serial.available() > 0) {
        uint8_t byte = _serial.read();
        
        #ifdef DEBUG
        static uint32_t lastByteDebug = 0;
        if (millis() - lastByteDebug > 2000) { // Debug bytes every 2 seconds
            Serial.printf("[LD2420] Received byte: 0x%02X (total in buffer: %d)\n", byte, bufferIndex);
            lastByteDebug = millis();
        }
        #endif        // Check for frame header
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
                    Serial.println("[LD2420] Valid frame header detected, parsing...");
                    #endif
                    
                    parseFrame();
                } else {
                    #ifdef DEBUG
                    Serial.printf("[LD2420] Invalid header: %02X %02X %02X %02X\n", 
                                  buffer[0], buffer[1], buffer[2], buffer[3]);
                    #endif
                }
                bufferIndex = 0;
            } else if (bufferIndex >= 25) {
                // Buffer overflow protection
                #ifdef DEBUG
                Serial.println("[LD2420] Buffer overflow, resetting");
                #endif
                bufferIndex = 0;
            }
        }
    }
}

void LD2420Sensor::parseFrame() {
    // LD2420 frame format (23 bytes total):
    // [0-3]: Header F4 F3 F2 F1
    // [4-5]: Data length
    // [6]: Command type
    // [7-8]: Moving target distance (little endian)
    // [9]: Moving target energy
    // [10-11]: Static target distance (little endian) 
    // [12]: Static target energy
    // [13-21]: Reserved/other data
    // [22]: Checksum
    
    #ifdef DEBUG
    Serial.print("[LD2420] Raw frame data: ");
    for (int i = 0; i < 23; i++) {
        Serial.printf("%02X ", buffer[i]);
    }
    Serial.println();
    #endif
    
    // Extract moving target distance (bytes 7-8, little endian)
    uint16_t movingDistance = buffer[7] | (buffer[8] << 8);
    uint8_t movingEnergy = buffer[9];
    
    // Extract static target distance (bytes 10-11, little endian)
    uint16_t staticDistance = buffer[10] | (buffer[11] << 8);
    uint8_t staticEnergy = buffer[12];
    
    #ifdef DEBUG
    Serial.printf("[LD2420] Moving: distance=%d cm, energy=%d\n", movingDistance, movingEnergy);
    Serial.printf("[LD2420] Static: distance=%d cm, energy=%d\n", staticDistance, staticEnergy);
    #endif
    
    // Update presence detection - consider targets with reasonable energy levels
    _movingTarget = (movingDistance > 0 && movingDistance < 600 && movingEnergy > 10); // Max 6m, min energy
    _staticTarget = (staticDistance > 0 && staticDistance < 600 && staticEnergy > 10);
    _presenceDetected = _movingTarget || _staticTarget;
    
    // Store signal strength from the stronger target
    _signalStrength = max(movingEnergy, staticEnergy);
    
    // Use closer distance, convert cm to meters
    if (_movingTarget && _staticTarget) {
        _distance = min(movingDistance, staticDistance) / 100.0f;
    } else if (_movingTarget) {
        _distance = movingDistance / 100.0f;
    } else if (_staticTarget) {
        _distance = staticDistance / 100.0f;
    } else {
        _distance = 0.0f;
    }
    
    #ifdef DEBUG
    Serial.printf("[LD2420] Result - Moving: %s, Static: %s, Presence: %s, Distance: %.2fm, Signal: %d\n", 
                  _movingTarget ? "YES" : "NO", 
                  _staticTarget ? "YES" : "NO",
                  _presenceDetected ? "YES" : "NO",
                  _distance, _signalStrength);
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
