#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>

class WiFiSetup {
public:
    void begin();
    void handle();
    bool isConnected() const;
    String getDeviceID() const;

private:
    void startAP();
    void setupWebServer();
    void handleRoot();
    void handleSave();
    void generateDeviceID();
    bool connectToWiFi(const String& ssid, const String& password);
    
    Preferences prefs;
    WebServer webServer;
    DNSServer dnsServer;
    String deviceID;
    bool apMode = false;
    static constexpr const char* WIFI_SSID_KEY = "wifi_ssid";
    static constexpr const char* WIFI_PASS_KEY = "wifi_pass";
    static constexpr const char* DEVICE_ID_KEY = "device_id";
};
