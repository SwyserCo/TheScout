#include "sensors/LD2420Sensor.h"
#include "Config.h"

LD2420Sensor::LD2420Sensor() : _serial(1), _connected(false), _lastReading(0),
    _presenceDetected(false), _distance(0.0f), _signalStrength(0) {
}

bool LD2420Sensor::begin() {
    Serial.printf("[LD2420] Initializing UART - RX:GPIO%d, TX:GPIO%d\n", 
                  Config::LD2420_RX_PIN, Config::LD2420_TX_PIN);
    
    // Try different common baud rates for LD2420/LD2410 sensors
    uint32_t baudRates[] = {256000, 115200, 9600, 38400, 57600};
    bool connected = false;
    
    for (uint32_t baud : baudRates) {
        Serial.printf("[LD2420] Testing baud rate: %d\n", baud);
        
        _serial.begin(baud, SERIAL_8N1, Config::LD2420_RX_PIN, Config::LD2420_TX_PIN);
        delay(500); // Give time to initialize
        
        // Clear buffer
        while (_serial.available()) {
            _serial.read();
        }
        
        // Wait for some data
        delay(1000);
        
        if (_serial.available() > 0) {
            Serial.printf("[LD2420] Data detected at baud rate %d (%d bytes available)\n", 
                          baud, _serial.available());
            connected = true;
            break;
        } else {
            Serial.printf("[LD2420] No data at baud rate %d\n", baud);
        }
    }
    
    if (!connected) {
        Serial.println("[LD2420] No valid baud rate found - check wiring and power");
        Serial.println("[LD2420] Expected connections: VCC=3.3V/5V, GND=GND, TX->RX(GPIO16), RX->TX(GPIO15)");
    } else {
        Serial.println("[LD2420] UART communication established");
    }
    
    Serial.println("[LD2420] Note: This is a basic implementation for testing UART communication");
    
    _connected = connected;
    return connected;
}

bool LD2420Sensor::isConnected() const {
    return _connected;
}

void LD2420Sensor::readSensor(DynamicJsonDocument& doc) {
    if (!_connected) return;
    
    uint32_t currentTime = millis();
    if (currentTime - _lastReading >= 200) { // Read every 200ms
        checkForData();
        _lastReading = currentTime;
    }
    
    doc["presence"] = _presenceDetected;
    doc["distance"] = _distance;
    doc["strength"] = _signalStrength;
}

void LD2420Sensor::checkForData() {
    static uint32_t lastStatusTime = 0;
    uint32_t currentTime = millis();
    
    // Show status every 10 seconds (less frequent)
    if (currentTime - lastStatusTime > 10000) {
        int availableBytes = _serial.available();
        Serial.printf("[LD2420] Status check - Available bytes: %d\n", availableBytes);
        
        if (availableBytes == 0) {
            Serial.println("[LD2420] No data received - Check: Power, GND, RX=IO16, TX=IO15");
        }
        lastStatusTime = currentTime;
    }
    
    // Read any available data and look for basic patterns (but don't spam the output)
    int bytesRead = 0;
    while (_serial.available() > 0 && bytesRead < 20) {
        uint8_t byte = _serial.read();
        bytesRead++;
        
        // Don't print every byte - just count them
        // Serial.printf("[LD2420] RX: 0x%02X ", byte);
        
        // Very simple presence detection - if we get any data, assume presence
        // This is just to see if UART communication is working
        if (bytesRead > 0) {
            _presenceDetected = true;
            _distance = 1.0f; // Just a dummy value
            _signalStrength = 50; // Just a dummy value
        }
    }
    
    if (bytesRead > 0) {
        Serial.printf("[LD2420] Received %d bytes of data\n", bytesRead);
    }
    
    // If no data for a while, assume no presence
    static uint32_t lastDataTime = 0;
    if (bytesRead > 0) {
        lastDataTime = currentTime;
    } else if (currentTime - lastDataTime > 5000) { // No data for 5 seconds
        _presenceDetected = false;
        _distance = 0.0f;
        _signalStrength = 0;
    }
}

bool LD2420Sensor::checkPresence() {
    return _presenceDetected;
}

void LD2420Sensor::update() {
    if (_connected) {
        checkForData();
    }
}
