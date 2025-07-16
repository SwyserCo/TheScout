#pragma once
#include <Arduino.h>
#include <Preferences.h>

class WiFiCredentials {
public:
    bool load();
    void save(const String& ssid, const String& password);
    void clear();
    String getSSID() const { return ssid; }
    String getPassword() const { return password; }
private:
    String ssid;
    String password;
    Preferences prefs;
    const char* ns = "wifi";
};
