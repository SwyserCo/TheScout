#include "sensors/LD2402Sensor.h"

bool LD2402Sensor::begin() {
    serial.begin(256000, SERIAL_8N1, Config::LD2420_RX, Config::LD2420_TX);
    initialized = true;
    return true;
}

bool LD2402Sensor::read() {
    if (!initialized) {
        lastError = "Sensor not initialized";
        return false;
    }
    
    if (parseData()) {
        notifyUpdate();
        return true;
    }
    return false;
}

bool LD2402Sensor::isReady() const {
    return initialized;
}

const char* LD2402Sensor::getLastError() const {
    return lastError;
}

bool LD2402Sensor::parseData() {
    static uint8_t buffer[6];
    static uint8_t index = 0;
    
    while (serial.available()) {
        uint8_t byte = serial.read();
        
        if (index == 0 && byte != 0x53) { // Start byte
            continue;
        }
        
        buffer[index++] = byte;
        
        if (index == 6) {
            index = 0;
            
            if (buffer[5] != 0x54) { // End byte
                continue;
            }
            
            // Parse data
            data.presenceState = buffer[1];
            data.movementState = buffer[2];
            data.distance = (buffer[4] << 8) | buffer[3];
            data.energy = buffer[3];
            
            return true;
        }
    }
    
    return false;
}
