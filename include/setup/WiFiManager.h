#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

class WiFiManager {
public:
    WiFiManager();
    void begin();
    bool autoConnect();
    void startConfigPortal();
    void handleClient();
    bool isConnected() const;
    String getDeviceId() const;
    void clearCredentials();
    
private:
    WebServer _server;
    DNSServer _dnsServer;
    Preferences _prefs;
    String _deviceId;
    bool _portalRunning;
    
    void setupConfigPortal();
    void handleRoot();
    void handleSave();
    void handleNotFound();
    String getConfigPage();
    String getSuccessPage();
    void generateDeviceId();
    bool connectToWiFi(const String& ssid, const String& password);
};
