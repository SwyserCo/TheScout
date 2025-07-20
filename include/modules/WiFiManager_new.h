#pragma once

#include <WiFi.h>
#include "modules/WiFiConnection.h"
#include "modules/WiFiConfig.h"
#include "modules/WebPortal.h"
#include "modules/Feedback.h"

class WiFiManager {
private:
    // Modular components
    WiFiConnection connection;
    WiFiConfig config;
    WebPortal portal;
    
    // Feedback components
    LEDController* systemLED;
    BuzzerController* buzzer;
    
    // State management
    bool isSetupMode;
    bool setupRebootScheduled;
    unsigned long setupConnectedTime;
    
    // Helper methods
    void updateConnectionFeedback();
    void scheduleReboot();
    void onConnectionRequest(String ssid, String password);
    
public:
    WiFiManager();
    
    // Core functionality
    void begin(LEDController* led, BuzzerController* buzzerCtrl);
    void update();
    bool isConnected();
    
    // Setup mode
    void startSetupMode();
    void stopSetupMode();
    bool inSetupMode() const { return isSetupMode; }
    
    // Connection management
    bool connectToStoredNetwork();
    void connectToNetwork(const String& ssid, const String& password);
    
    // Configuration
    String getDeviceName() const { return config.getDeviceName(); }
    void factoryReset();
};
