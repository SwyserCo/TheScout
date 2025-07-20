#pragma once

#include <WiFi.h>
#include <Preferences.h>
#include "modules/Feedback.h"

enum class ConnectionState {
    IDLE,
    CONNECTING,
    CONNECTED,
    FAILED,
    TIMEOUT
};

class WiFiConnection {
private:
    LEDController* systemLED;
    BuzzerController* buzzer;
    
    ConnectionState currentState;
    String targetSSID;
    String targetPassword;
    
    unsigned long connectionStartTime;
    uint8_t connectionAttempts;
    bool isConnecting;
    
    void updateConnectionFeedback();
    
public:
    WiFiConnection();
    
    void begin(LEDController* sysLED, BuzzerController* buzz);
    void update();
    
    // Connection management
    bool startConnection(const String& ssid, const String& password);
    void stopConnection();
    
    // Status checking
    bool isConnected() const;
    ConnectionState getState() const;
    ConnectionState getConnectionState() const { return getState(); }
    String getConnectedSSID() const;
    String getLocalIP() const;
    uint8_t getAttemptCount() const;
    
    // WiFi mode management
    void setStationMode();
    void setAPStationMode();
};
