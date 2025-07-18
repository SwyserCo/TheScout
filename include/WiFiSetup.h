#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include "Config.h"

class WiFiSetup {
public:
    WiFiSetup() = default;
    ~WiFiSetup();
    
    void begin();
    void handle();
    bool isConnected() const;
    String getDeviceID() const;

private:
    static void networkTask(void* parameter);
    void startAP();
    void setupWebServer();
    void handleRoot();
    void handleSave();
    void generateDeviceID();
    bool connectToWiFi(const String& ssid, const String& password);
    
    // Task management
    TaskHandle_t networkTaskHandle = nullptr;
    static constexpr uint32_t STACK_SIZE = 8192;        // Increased stack size
    static constexpr UBaseType_t TASK_PRIORITY = 3;     // Higher priority for network task
    static constexpr BaseType_t CORE_ID = 0;            // Run on core 0
    
    Preferences prefs;
    WebServer webServer;
    DNSServer dnsServer;
    String deviceID;
    bool apMode = false;
    static constexpr const char* WIFI_SSID_KEY = "wifi_ssid";
    static constexpr const char* WIFI_PASS_KEY = "wifi_pass";
    static constexpr const char* DEVICE_ID_KEY = "device_id";
};
