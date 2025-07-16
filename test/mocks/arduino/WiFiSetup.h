#pragma once
#include <string>
#include "Preferences.h"

class WiFiSetup {
public:
    void begin() {}
    void handle() {}
    bool isConnected() { return connected; }
    std::string getDeviceID() { return deviceId; }
    
private:
    Preferences prefs;
    bool connected = false;
    std::string deviceId = "test-device";
};
