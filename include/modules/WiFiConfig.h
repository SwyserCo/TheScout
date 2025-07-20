#pragma once

#include <Preferences.h>
#include <WiFi.h>

class WiFiConfig {
private:
    Preferences preferences;
    String deviceName;
    String storedSSID;
    String storedPassword;
    bool credentialsStored;
    
    String generateDeviceName();
    
public:
    WiFiConfig();
    
    void begin();
    
    // Device name management
    String getDeviceName() const;
    void setDeviceName(const String& name);
    
    // Credential management
    bool hasCredentials() const;
    bool hasStoredCredentials() const { return hasCredentials(); }
    std::pair<String, String> getStoredCredentials() const;
    String getStoredSSID() const;
    String getStoredPassword() const;
    void saveCredentials(const String& ssid, const String& password);
    void clearCredentials();
    void clearStoredCredentials() { clearCredentials(); }
    void clearAllPreferences();
    
    // Validation
    bool isValidCredentials(const String& ssid, const String& password) const;
};
