#pragma once

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include "modules/Feedback.h"

enum class WiFiState {
    CHECKING_CREDENTIALS,
    CONNECTING,
    CONNECTED,
    CONNECTION_FAILED,
    AP_MODE,
    SCANNING
};

class WiFiManager {
private:
    LEDController* systemLED;
    BuzzerController* buzzer;
    
    Preferences preferences;
    WebServer webServer;
    DNSServer dnsServer;
    
    WiFiState currentState;
    String deviceName;
    String storedSSID;
    String storedPassword;
    
    unsigned long lastConnectionAttempt;
    unsigned long lastRetryAttempt;
    uint8_t connectionAttempts;
    bool hasStoredCredentials;
    bool isConnecting;
    bool hasEverConnected;
    bool isSetupMode; // True when connecting via captive portal, false when using stored credentials
    
    // Reboot timing for setup mode
    unsigned long setupConnectedTime;
    bool setupRebootScheduled;
    
    // Non-blocking scan variables
    bool scanInProgress;
    unsigned long lastScanTime;
    String scanResults;
    
    // Web server handlers
    void handleRoot();
    void handleScan();
    void handleConnect();
    void handleConnecting();
    void handleNotFound();
    
    // WiFi management
    bool loadCredentials();
    void saveCredentials(const String& ssid, const String& password);
    void clearCredentials();
    String generateDeviceName();
    void startAccessPoint();
    void stopAccessPoint();
    bool connectToWiFi();
    void updateFeedback();
    
    // Utility
    String scanNetworks();
    bool isValidCredentials(const String& ssid, const String& password);
    unsigned long getRetryInterval();
    void scheduleRetry();
    void updateWiFiScan();
    
public:
    WiFiManager(LEDController* sysLED, BuzzerController* buzz);
    
    void begin();
    void update();
    bool isConnected();
    String getDeviceName();
    WiFiState getState();
    void factoryReset(); // For future use
    
    // Web page templates
    String getMainPage();
    String getConnectingPage();
    String getSuccessPage();
};
