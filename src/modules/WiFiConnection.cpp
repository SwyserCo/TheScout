#include "modules/WiFiConnection.h"
#include "config.h"

WiFiConnection::WiFiConnection() :
    systemLED(nullptr),
    buzzer(nullptr),
    currentState(ConnectionState::IDLE),
    connectionStartTime(0),
    connectionAttempts(0),
    isConnecting(false) {}

void WiFiConnection::begin(LEDController* sysLED, BuzzerController* buzz) {
    systemLED = sysLED;
    buzzer = buzz;
    Serial.println("WiFiConnection: Initializing...");
}

void WiFiConnection::update() {
    if (!isConnecting) return;
    
    unsigned long currentTime = millis();
    
    switch (currentState) {
        case ConnectionState::CONNECTING:
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("WiFi connected successfully!");
                Serial.println("IP: " + WiFi.localIP().toString());
                Serial.println("SSID: " + WiFi.SSID());
                
                currentState = ConnectionState::CONNECTED;
                isConnecting = false;
                
                // Play success feedback
                if (buzzer && millis() > 5000) {
                    buzzer->playSuccessChime();
                }
                
            } else if (currentTime - connectionStartTime > Config::WiFi::CONNECTION_TIMEOUT_MS) {
                Serial.println("Connection timeout - WiFi status: " + String(WiFi.status()));
                Serial.println("Time elapsed: " + String(currentTime - connectionStartTime) + "ms");
                
                currentState = ConnectionState::TIMEOUT;
                isConnecting = false;
                connectionAttempts++;
                
            } else {
                // Still connecting - log status periodically
                static unsigned long lastStatusLog = 0;
                if (currentTime - lastStatusLog > 2000) {
                    Serial.println("Connecting... WiFi status: " + String(WiFi.status()) + 
                                 ", elapsed: " + String(currentTime - connectionStartTime) + "ms");
                    lastStatusLog = currentTime;
                    
                    // Check for immediate failures
                    if (WiFi.status() == WL_CONNECT_FAILED) {
                        Serial.println("Authentication failure detected");
                        currentState = ConnectionState::FAILED;
                        isConnecting = false;
                        connectionAttempts++;
                        return;
                    }
                }
            }
            break;
            
        default:
            break;
    }
    
    updateConnectionFeedback();
}

bool WiFiConnection::startConnection(const String& ssid, const String& password) {
    Serial.println("=== WiFiConnection::startConnection() ===");
    Serial.println("SSID: '" + ssid + "'");
    Serial.println("Password length: " + String(password.length()));
    Serial.println("Current WiFi mode: " + String(WiFi.getMode()));
    
    if (ssid.isEmpty()) {
        Serial.println("ERROR: Empty SSID provided");
        return false;
    }
    
    targetSSID = ssid;
    targetPassword = password;
    
    Serial.println("Starting WiFi.begin()...");
    WiFi.begin(ssid.c_str(), password.c_str());
    
    connectionStartTime = millis();
    currentState = ConnectionState::CONNECTING;
    isConnecting = true;
    
    Serial.println("Connection attempt started at: " + String(connectionStartTime));
    return true;
}

void WiFiConnection::stopConnection() {
    Serial.println("Stopping WiFi connection...");
    WiFi.disconnect();
    isConnecting = false;
    currentState = ConnectionState::IDLE;
}

bool WiFiConnection::isConnected() const {
    return currentState == ConnectionState::CONNECTED && WiFi.status() == WL_CONNECTED;
}

ConnectionState WiFiConnection::getState() const {
    return currentState;
}

String WiFiConnection::getConnectedSSID() const {
    if (isConnected()) {
        return WiFi.SSID();
    }
    return "";
}

String WiFiConnection::getLocalIP() const {
    if (isConnected()) {
        return WiFi.localIP().toString();
    }
    return "";
}

uint8_t WiFiConnection::getAttemptCount() const {
    return connectionAttempts;
}

void WiFiConnection::setStationMode() {
    Serial.println("Setting WiFi to STA mode");
    WiFi.mode(WIFI_STA);
    delay(100);
}

void WiFiConnection::setAPStationMode() {
    Serial.println("Setting WiFi to AP+STA mode");
    WiFi.mode(WIFI_AP_STA);
    delay(100);
}

void WiFiConnection::updateConnectionFeedback() {
    if (!systemLED) return;
    
    static ConnectionState lastFeedbackState = ConnectionState::IDLE;
    static unsigned long lastLEDAction = 0;
    static uint8_t flashCount = 0;
    static bool flashOn = false;
    
    // Only update LED when state changes
    if (currentState != lastFeedbackState) {
        flashCount = 0;
        lastLEDAction = millis();
        
        switch (currentState) {
            case ConnectionState::CONNECTING:
                systemLED->setBlink(Config::LED_BLINK_FAST, Config::Colors::CASTLE_BLUE);
                Serial.println("LED: Fast blue blink (connecting)");
                break;
                
            case ConnectionState::CONNECTED:
                systemLED->setColor(Config::Colors::SUCCESS_GREEN);
                flashOn = true;
                flashCount = 1;
                lastLEDAction = millis();
                Serial.println("LED: Starting green flash sequence (connected)");
                break;
                
            case ConnectionState::FAILED:
            case ConnectionState::TIMEOUT:
                if (connectionAttempts >= Config::WiFi::MAX_FAST_RETRIES) {
                    systemLED->setColor(Config::Colors::DANGER_RED);
                    flashOn = true;
                    flashCount = 1;
                    lastLEDAction = millis();
                    Serial.println("LED: Starting red flash sequence (failed)");
                } else {
                    systemLED->setOff();
                    Serial.println("LED: Off (failed, will retry)");
                }
                break;
                
            default:
                systemLED->setOff();
                break;
        }
        lastFeedbackState = currentState;
    }
    
    // Handle flash sequences for connected/failed states
    unsigned long currentTime = millis();
    if ((currentState == ConnectionState::CONNECTED || 
         ((currentState == ConnectionState::FAILED || currentState == ConnectionState::TIMEOUT) && 
          connectionAttempts >= Config::WiFi::MAX_FAST_RETRIES)) 
        && flashCount > 0 && flashCount <= 4) {
        
        if (flashOn && currentTime - lastLEDAction >= 200) {
            systemLED->setOff();
            flashOn = false;
            lastLEDAction = currentTime;
        } else if (!flashOn && currentTime - lastLEDAction >= 100) {
            if (flashCount == 1) {
                CRGB color = (currentState == ConnectionState::CONNECTED) ? 
                    Config::Colors::SUCCESS_GREEN : Config::Colors::DANGER_RED;
                systemLED->setColor(color);
                flashOn = true;
                flashCount = 2;
                lastLEDAction = currentTime;
            } else if (flashCount == 2) {
                systemLED->setOff();
                flashCount = 5; // Mark as complete
                lastLEDAction = currentTime;
            }
        }
    }
}
