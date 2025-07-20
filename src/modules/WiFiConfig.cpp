#include "modules/WiFiConfig.h"

WiFiConfig::WiFiConfig() : credentialsStored(false) {}

void WiFiConfig::begin() {
    Serial.println("WiFiConfig: Initializing...");
    
    preferences.begin("thescout", false);
    
    // Load or generate device name
    deviceName = preferences.getString("deviceName", "");
    if (deviceName.isEmpty()) {
        deviceName = generateDeviceName();
        preferences.putString("deviceName", deviceName);
        Serial.println("Generated device name: " + deviceName);
    } else {
        Serial.println("Using stored device name: " + deviceName);
    }
    
    // Set WiFi hostname
    WiFi.setHostname(deviceName.c_str());
    
    // Load stored credentials
    storedSSID = preferences.getString("wifi_ssid", "");
    storedPassword = preferences.getString("wifi_pass", "");
    credentialsStored = !storedSSID.isEmpty();
    
    if (credentialsStored) {
        Serial.println("Found stored WiFi credentials for SSID: " + storedSSID);
        Serial.println("Password length: " + String(storedPassword.length()));
    } else {
        Serial.println("No stored credentials found");
    }
}

String WiFiConfig::getDeviceName() const {
    return deviceName;
}

void WiFiConfig::setDeviceName(const String& name) {
    deviceName = name;
    preferences.putString("deviceName", name);
    WiFi.setHostname(name.c_str());
    Serial.println("Device name updated to: " + name);
}

bool WiFiConfig::hasCredentials() const {
    return credentialsStored;
}

String WiFiConfig::getStoredSSID() const {
    return storedSSID;
}

String WiFiConfig::getStoredPassword() const {
    return storedPassword;
}

void WiFiConfig::saveCredentials(const String& ssid, const String& password) {
    preferences.putString("wifi_ssid", ssid);
    preferences.putString("wifi_pass", password);
    
    storedSSID = ssid;
    storedPassword = password;
    credentialsStored = true;
    
    Serial.println("Saved WiFi credentials for: " + ssid);
}

void WiFiConfig::clearCredentials() {
    preferences.remove("wifi_ssid");
    preferences.remove("wifi_pass");
    storedSSID = "";
    storedPassword = "";
    credentialsStored = false;
    Serial.println("Cleared stored WiFi credentials");
}

std::pair<String, String> WiFiConfig::getStoredCredentials() const {
    return std::make_pair(storedSSID, storedPassword);
}

void WiFiConfig::clearAllPreferences() {
    preferences.clear();
    Serial.println("Cleared all preferences");
}

bool WiFiConfig::isValidCredentials(const String& ssid, const String& password) const {
    bool valid = !ssid.isEmpty() && ssid.length() <= 32 && password.length() <= 64;
    Serial.println("Credential validation - SSID: '" + ssid + "' (length: " + 
                  String(ssid.length()) + "), Password length: " + 
                  String(password.length()) + ", Valid: " + String(valid));
    return valid;
}

String WiFiConfig::generateDeviceName() {
    randomSeed(ESP.getEfuseMac());
    uint32_t deviceId = random(100000, 999999);
    return "TheScout-" + String(deviceId);
}
