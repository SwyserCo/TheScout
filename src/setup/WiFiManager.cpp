#include "setup/WiFiManager.h"
#include "Config.h"

WiFiManager::WiFiManager() : _server(80), _portalRunning(false) {
}

void WiFiManager::begin() {
    _prefs.begin("wifi-config", false);
    generateDeviceId();
}

bool WiFiManager::autoConnect() {
    String ssid = _prefs.getString("ssid", "");
    String password = _prefs.getString("password", "");
    
    if (ssid.length() > 0) {
        return connectToWiFi(ssid, password);
    }
    
    return false;
}

void WiFiManager::startConfigPortal() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("TheScout-Setup", "");
    
    _dnsServer.start(53, "*", WiFi.softAPIP());
    setupConfigPortal();
    _portalRunning = true;
    
    Serial.println("Started config portal");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
}

void WiFiManager::handleClient() {
    if (_portalRunning) {
        _dnsServer.processNextRequest();
        _server.handleClient();
    }
}

bool WiFiManager::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getDeviceId() const {
    return _deviceId;
}

void WiFiManager::clearCredentials() {
    _prefs.clear();
}

void WiFiManager::setupConfigPortal() {
    _server.on("/", [this]() { handleRoot(); });
    _server.on("/save", HTTP_POST, [this]() { handleSave(); });
    _server.onNotFound([this]() { handleNotFound(); });
    _server.begin();
}

void WiFiManager::handleRoot() {
    _server.send(200, "text/html", getConfigPage());
}

void WiFiManager::handleSave() {
    String ssid = _server.arg("ssid");
    String password = _server.arg("password");
    
    if (ssid.length() > 0) {
        _prefs.putString("ssid", ssid);
        _prefs.putString("password", password);
        
        _server.send(200, "text/html", getSuccessPage());
        
        delay(2000);
        if (connectToWiFi(ssid, password)) {
            _portalRunning = false;
            _server.stop();
            _dnsServer.stop();
            WiFi.softAPdisconnect(true);
        }
    } else {
        _server.send(400, "text/html", "SSID cannot be empty");
    }
}

void WiFiManager::handleNotFound() {
    // Captive portal redirect
    String Location = "http://";
    Location += WiFi.softAPIP().toString();
    Location += "/";
    _server.sendHeader("Location", Location, true);
    _server.send(302, "text/plain", "");
}

String WiFiManager::getConfigPage() {
    String html = "<!DOCTYPE html>";
    html += "<html><head><meta charset=\"UTF-8\">";
    html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    html += "<title>Guardian Security System - The Scout Setup</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; max-width: 400px; margin: 50px auto; padding: 20px; background-color: #f5f5f5; }";
    html += ".container { background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
    html += "h1 { color: #333; text-align: center; margin-bottom: 10px; }";
    html += "h2 { color: #666; text-align: center; margin-bottom: 20px; }";
    html += ".intro { color: #555; line-height: 1.6; margin-bottom: 30px; text-align: center; }";
    html += "input { width: 100%; padding: 12px; margin: 10px 0; border: 1px solid #ddd; border-radius: 4px; box-sizing: border-box; }";
    html += "button { width: 100%; padding: 12px; background-color: #4CAF50; color: white; border: none; border-radius: 4px; cursor: pointer; font-size: 16px; }";
    html += "button:hover { background-color: #45a049; }";
    html += "</style></head><body>";
    html += "<div class=\"container\">";
    html += "<h1>Guardian Security System</h1>";
    html += "<h2>The Scout</h2>";
    html += "<div class=\"intro\">";
    html += "Welcome to The Scout setup. This page allows you to configure your device's ";
    html += "Wi-Fi connection. Enter your network credentials below to connect The Scout ";
    html += "to your home network and enable integration with Home Assistant. Once connected, ";
    html += "your device will be ready to monitor your space.";
    html += "</div>";
    html += "<form action=\"/save\" method=\"post\">";
    html += "<input type=\"text\" name=\"ssid\" placeholder=\"Wi-Fi Network Name (SSID)\" required>";
    html += "<input type=\"password\" name=\"password\" placeholder=\"Wi-Fi Password\">";
    html += "<button type=\"submit\">Connect to Wi-Fi</button>";
    html += "</form></div></body></html>";
    return html;
}

String WiFiManager::getSuccessPage() {
    String html = "<!DOCTYPE html>";
    html += "<html><head><meta charset=\"UTF-8\">";
    html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    html += "<title>Connection Successful</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; max-width: 400px; margin: 50px auto; padding: 20px; background-color: #f5f5f5; }";
    html += ".container { background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); text-align: center; }";
    html += "h1 { color: #4CAF50; margin-bottom: 20px; }";
    html += "p { color: #555; line-height: 1.6; }";
    html += "</style></head><body>";
    html += "<div class=\"container\">";
    html += "<h1>✓ Success!</h1>";
    html += "<p>The Scout has been successfully connected to your Wi-Fi network. The device will now restart and be available in Home Assistant.</p>";
    html += "<p><strong>Device ID:</strong> " + _deviceId + "</p>";
    html += "</div></body></html>";
    return html;
}

void WiFiManager::generateDeviceId() {
    _deviceId = _prefs.getString("device_id", "");
    if (_deviceId.length() == 0) {
        // Generate unique 6-digit number based on MAC address
        uint8_t mac[6];
        WiFi.macAddress(mac);
        uint32_t chipId = (mac[3] << 16) | (mac[4] << 8) | mac[5];
        uint16_t deviceNumber = (chipId % 900000) + 100000; // Ensure 6 digits
        
        _deviceId = "TheScout-" + String(deviceNumber);
        _prefs.putString("device_id", _deviceId);
    }
}

bool WiFiManager::connectToWiFi(const String& ssid, const String& password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    
    uint32_t startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < Config::WIFI_SETUP_TIMEOUT) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    }
    
    Serial.println("");
    Serial.println("Failed to connect to Wi-Fi");
    return false;
}
