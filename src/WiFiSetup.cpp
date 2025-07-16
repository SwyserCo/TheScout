#include "WiFiSetup.h"
#include <EEPROM.h>
#include <ESPmDNS.h>

// Setup AP SSID
const char* AP_SSID = "TheScout Setup";
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
    // Initialize random seed from ESP32's random number generator
    randomSeed(esp_random());
    
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
    // Create open AP for easy setup
    WiFi.softAP(AP_SSID);
    
    // Configure DNS server to redirect all requests to our IP
    IPAddress apIP = WiFi.softAPIP();
    dnsServer.start(53, "*", apIP);
    
    // Set up mDNS responder for captive portal detection
    if (MDNS.begin("thescout")) {
        MDNS.addService("http", "tcp", 80);
    }
    
    setupWebServer();
    apMode = true;
    
#if DEBUG
    Serial.println("Access Point Started");
    Serial.print("SSID: ");
    Serial.println(AP_SSID);
    Serial.print("AP IP address: ");
    Serial.println(apIP);
#endif
}

void WiFiSetup::setupWebServer() {
    // Handle captive portal detection
    webServer.on("/generate_204", HTTP_GET, [this]() { handleRoot(); });  // Android captive portal detection
    webServer.on("/fwlink", HTTP_GET, [this]() { handleRoot(); });       // Microsoft captive portal detection
    webServer.on("/hotspot-detect.html", HTTP_GET, [this]() { handleRoot(); }); // Apple captive portal detection
    
    // Handle main pages
    webServer.on("/", HTTP_GET, [this]() { handleRoot(); });
    webServer.on("/save", HTTP_POST, [this]() { handleSave(); });
    
    // Catch-all handler for captive portal
    webServer.onNotFound([this]() { handleRoot(); });
    
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
    deviceID = String("TheScout-");
    // Generate 6 random digits
    for (int i = 0; i < 6; i++) {
        deviceID += String(random(0, 10));  // 0-9 for each digit
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
