#pragma once

#include <WebServer.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <ArduinoJson.h>

class WiFiConfig;

class WebPortal {
private:
    WebServer webServer;
    DNSServer dnsServer;
    WiFiConfig* config;
    
    // Scanning
    bool scanInProgress;
    unsigned long lastScanTime;
    String scanResults;
    
    // Connection callback
    std::function<void(String, String)> onConnectRequest;
    
    // Web server handlers
    void handleRoot();
    void handleScan();
    void handleConnect();
    void handleConnecting();
    void handleNotFound();
    
    // Page generators
    String getMainPage();
    String getConnectingPage();
    String getSuccessPage();
    
    void updateWiFiScan();
    
public:
    WebPortal(WiFiConfig* wifiConfig);
    
    void begin();
    void update();
    void stop();
    
    // Event handlers
    void setConnectionCallback(std::function<void(String, String)> callback);
    
    // Status
    bool isRunning() const;
};
