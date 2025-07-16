#include "WiFiSetup.h"
#include <EEPROM.h>

const char* AP_SSID = "Scout-Setup";
const char* AP_PASSWORD = "12345678";
const char* HTML_FORM = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Scout WiFi Setup</title>
    <style>
        body { font-family: Arial; margin: 20px; }
        .form-group { margin-bottom: 15px; }
        label { display: block; margin-bottom: 5px; }
        input[type="text"], input[type="password"] { 
            width: 100%; 
            padding: 8px;
            margin-bottom: 10px;
        }
        button { 
            background-color: #4CAF50;
            color: white;
            padding: 10px 15px;
            border: none;
            cursor: pointer;
        }
    </style>
</head>
<body>
    <h1>Scout WiFi Setup</h1>
    <form action="/save" method="POST">
        <div class="form-group">
            <label>WiFi SSID:</label>
            <input type="text" name="ssid" required>
        </div>
        <div class="form-group">
            <label>WiFi Password:</label>
            <input type="password" name="password" required>
        </div>
        <button type="submit">Save and Connect</button>
    </form>
</body>
</html>
)";

void WiFiSetup::begin() {
    prefs.begin("scout-config");
    
    String ssid = prefs.getString(WIFI_SSID_KEY, "");
    String password = prefs.getString(WIFI_PASS_KEY, "");
    deviceID = prefs.getString(DEVICE_ID_KEY, "");
    
    if (deviceID.isEmpty()) {
        generateDeviceID();
        prefs.putString(DEVICE_ID_KEY, deviceID);
    }
    
    if (ssid.isEmpty() || !connectToWiFi(ssid, password)) {
        startAP();
    }
}

void WiFiSetup::handle() {
    if (apMode) {
        dnsServer.processNextRequest();
        webServer.handleClient();
    }
}

bool WiFiSetup::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

String WiFiSetup::getDeviceID() const {
    return deviceID;
}

void WiFiSetup::startAP() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    
    dnsServer.start(53, "*", WiFi.softAPIP());
    setupWebServer();
    apMode = true;
}

void WiFiSetup::setupWebServer() {
    webServer.on("/", HTTP_GET, [this]() { handleRoot(); });
    webServer.on("/save", HTTP_POST, [this]() { handleSave(); });
    webServer.begin();
}

void WiFiSetup::handleRoot() {
    webServer.send(200, "text/html", HTML_FORM);
}

void WiFiSetup::handleSave() {
    String ssid = webServer.arg("ssid");
    String password = webServer.arg("password");
    
    if (ssid.length() > 0) {
        prefs.putString(WIFI_SSID_KEY, ssid);
        prefs.putString(WIFI_PASS_KEY, password);
        
        webServer.send(200, "text/html", "Settings saved. The device will now restart...");
        delay(2000);
        ESP.restart();
    } else {
        webServer.send(400, "text/plain", "Invalid SSID");
    }
}

void WiFiSetup::generateDeviceID() {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    deviceID = String("Scout-");
    for (int i = 0; i < 6; i++) {
        if (mac[i] < 0x10) deviceID += "0";
        deviceID += String(mac[i], HEX);
    }
}

bool WiFiSetup::connectToWiFi(const String& ssid, const String& password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        attempts++;
    }
    
    return WiFi.status() == WL_CONNECTED;
}
