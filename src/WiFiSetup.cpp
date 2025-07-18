#include "WiFiSetup.h"
#include <EEPROM.h>
#include <ESPmDNS.h>
#include <esp_task_wdt.h>

// Setup AP SSID
const char* AP_SSID = "TheScout Setup";
const char* HTML_FORM = R"(
<!DOCTYPE html>
<html>
<head>
    <title>TheScout Setup</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        * { 
            box-sizing: border-box; 
            margin: 0;
            padding: 0;
        }
        
        body { 
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
            line-height: 1.6;
            padding: 20px;
            max-width: 600px;
            margin: 0 auto;
            background: #f5f5f5;
        }
        
        .container {
            background: white;
            padding: 25px;
            border-radius: 12px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        
        .branding {
            text-align: center;
            margin-bottom: 24px;
        }
        
        .company {
            color: #666;
            font-size: 16px;
            text-transform: uppercase;
            letter-spacing: 2px;
            margin-bottom: 4px;
        }
        
        .product {
            font-size: 36px;
            font-weight: bold;
            color: #222;
            margin-bottom: 16px;
        }
        
        .instructions {
            color: #666;
            margin-bottom: 24px;
            padding: 15px;
            background: #f8f9fa;
            border-radius: 8px;
            font-size: 15px;
        }
        
        .form-group { 
            margin-bottom: 20px; 
        }
        
        label {
            display: block;
            margin-bottom: 8px;
            color: #444;
            font-weight: 500;
        }
        
        input[type="text"], input[type="password"] {
            width: 100%;
            padding: 12px;
            border: 2px solid #ddd;
            border-radius: 6px;
            font-size: 16px;
            transition: border-color 0.3s;
        }
        
        input:focus {
            border-color: #007bff;
            outline: none;
        }
        
        button {
            background-color: #007bff;
            color: white;
            padding: 14px 20px;
            border: none;
            border-radius: 6px;
            width: 100%;
            font-size: 16px;
            font-weight: 500;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        
        button:hover {
            background-color: #0056b3;
        }
        
        @media (max-width: 480px) {
            body { padding: 15px; }
            .product { font-size: 30px; }
            .instructions { font-size: 14px; }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="branding">
            <div class="company">Guardian</div>
            <div class="product">The Scout</div>
        </div>
        
        <div class="instructions">
            Welcome to the setup process for your Scout device. To get started, please connect the device to your home WiFi network. Enter your network name (SSID) and password below, and the Scout will automatically connect and begin protecting your space.
        </div>

        <form action="/save" method="POST">
            <div class="form-group">
                <label>Network Name (SSID):</label>
                <input type="text" name="ssid" required placeholder="Your WiFi network name">
            </div>
            <div class="form-group">
                <label>Network Password:</label>
                <input type="password" name="password" required placeholder="Your WiFi password">
            </div>
            <button type="submit">Connect to Network</button>
        </form>
    </div>
</body>
</html>
)";

void WiFiSetup::begin() {
    // Initialize random seed from ESP32's random number generator
    randomSeed(esp_random());
    
    Serial.println("Initializing WiFi setup...");
    if (!prefs.begin("scout-config", false)) {  // false = read/write mode
        Serial.println("Failed to initialize preferences");
        startAP();  // Start AP mode if preferences failed
        return;
    }
    Serial.println("Preferences initialized successfully");
    
    // Check if WiFi credentials exist
    if (!prefs.isKey(WIFI_SSID_KEY)) {
        Serial.println("No saved WiFi credentials found");
        startAP();
        return;
    }
    
    String ssid = prefs.getString(WIFI_SSID_KEY, "");
    String password = prefs.getString(WIFI_PASS_KEY, "");
    deviceID = prefs.getString(DEVICE_ID_KEY, "");
    
    Serial.println("Retrieved stored credentials:");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Device ID exists: ");
    Serial.println(!deviceID.isEmpty());
    
    if (deviceID.isEmpty()) {
        Serial.println("Generating new device ID...");
        generateDeviceID();
        prefs.putString(DEVICE_ID_KEY, deviceID);
        Serial.print("New device ID: ");
        Serial.println(deviceID);
    }
    
    if (ssid.isEmpty() || !connectToWiFi(ssid, password)) {
        startAP();
    }
}

WiFiSetup::~WiFiSetup() {
    if (networkTaskHandle != nullptr) {
        vTaskDelete(networkTaskHandle);
        networkTaskHandle = nullptr;
    }
}

void WiFiSetup::networkTask(void* parameter) {
    WiFiSetup* setup = static_cast<WiFiSetup*>(parameter);
    unsigned long lastDnsCheck = 0;
    unsigned long lastWebCheck = 0;
    
    // Set up watchdog for this task
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    ESP_ERROR_CHECK(esp_task_wdt_reset());
    
    while (true) {
        if (setup->apMode) {
            unsigned long now = millis();
            
            // Process DNS requests at DNS_CHECK_INTERVAL
            if (now - lastDnsCheck >= Config::DNS_CHECK_INTERVAL) {
                lastDnsCheck = now;
                setup->dnsServer.processNextRequest();
            }
            
            // Handle web clients at WEBSERVER_CHECK_INTERVAL
            if (now - lastWebCheck >= Config::WEBSERVER_CHECK_INTERVAL) {
                lastWebCheck = now;
                setup->webServer.handleClient();
            }
        }
        
        // Feed watchdog and yield
        ESP_ERROR_CHECK(esp_task_wdt_reset());
        vTaskDelay(pdMS_TO_TICKS(Config::NETWORK_YIELD_INTERVAL));
    }
}

void WiFiSetup::handle() {
    // Main loop doesn't need to do anything - network task handles everything
    yield();
}

bool WiFiSetup::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

String WiFiSetup::getDeviceID() const {
    return deviceID;
}

void WiFiSetup::startAP() {
    Serial.println("Starting AP mode...");
    
    // Stop any existing WiFi activities
    WiFi.disconnect(true, true);
    WiFi.softAPdisconnect(true);
    delay(10);
    yield();
    
    // Set mode to AP
    WiFi.mode(WIFI_OFF);
    delay(10);
    yield();
    WiFi.mode(WIFI_AP);
    yield();
    
    // Configure AP with specific IP matching Android's default DNS (8.8.8.8)
    IPAddress apIP(8, 8, 8, 8);  // Use Google's DNS IP
    IPAddress subnet(255, 255, 255, 0);
    
    if (WiFi.softAPConfig(apIP, apIP, subnet)) {
        Serial.println("AP IP configured successfully");
    } else {
        Serial.println("AP IP configuration failed!");
    }
    yield();  // Feed watchdog
    
    // Create AP with higher stability settings
    if (WiFi.softAP(AP_SSID, nullptr, 1, false, 1)) {  // Single connection, channel 1
        Serial.println("AP started successfully");
    } else {
        Serial.println("AP start failed!");
    }
    yield();  // Feed watchdog
    
    // Short delay to allow AP to initialize fully
    delay(100);
    yield();  // Feed watchdog after delay
    
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("AP MAC Address: ");
    Serial.println(WiFi.softAPmacAddress());
    
    // Configure DNS server to handle all requests
    Serial.println("Setting up DNS server...");
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    if (dnsServer.start(53, "*", apIP)) {  // Using port 53 is crucial for DNS
        Serial.println("DNS server started successfully on port 53");
    } else {
        Serial.println("DNS server start failed!");
    }
    
    // Set up mDNS responder for captive portal detection
    if (MDNS.begin("thescout")) {
        MDNS.addService("http", "tcp", 80);
    }
    
    setupWebServer();
    apMode = true;
    
    // Create network handling task if it doesn't exist
    if (networkTaskHandle == nullptr) {
        BaseType_t result = xTaskCreatePinnedToCore(
            networkTask,        // Task function
            "NetworkTask",      // Task name
            STACK_SIZE,        // Stack size
            this,              // Task parameters
            TASK_PRIORITY,     // Task priority
            &networkTaskHandle, // Task handle
            CORE_ID            // Core ID
        );
        
        if (result != pdPASS || networkTaskHandle == nullptr) {
            Serial.println("Failed to create network task!");
            ESP.restart();  // Restart if task creation fails
            return;
        }
        
        Serial.println("Network task created successfully");
    }
    
#if DEBUG
    Serial.println("Access Point Started");
    Serial.print("SSID: ");
    Serial.println(AP_SSID);
    Serial.print("AP IP address: ");
    Serial.println(apIP);
#endif
}

void WiFiSetup::setupWebServer() {
    // Handle captive portal detection and connectivity checks
    webServer.on("/generate_204", HTTP_GET, [this]() { handleRoot(); });  // Android captive portal detection
    webServer.on("/gen_204", HTTP_GET, [this]() { handleRoot(); });      // Android alternate
    webServer.on("/redirect", HTTP_GET, [this]() { handleRoot(); });      // Android 9+ captive portal redirect
    webServer.on("/mobile/status.php", HTTP_GET, [this]() { handleRoot(); });  // Android 7+ captive portal
    webServer.on("/connecttest.txt", HTTP_GET, [this]() { handleRoot(); });    // Windows captive portal
    webServer.on("/connectivity-check.html", HTTP_GET, [this]() { handleRoot(); }); // Linux
    webServer.on("/fwlink", HTTP_GET, [this]() { handleRoot(); });       // Microsoft
    webServer.on("/hotspot-detect.html", HTTP_GET, [this]() { handleRoot(); }); // Apple
    webServer.on("/library/test/success.html", HTTP_GET, [this]() { handleRoot(); }); // Apple
    webServer.on("/kindle-wifi/wifistub.html", HTTP_GET, [this]() { handleRoot(); }); // Kindle // Kindle
    webServer.on("/success.txt", HTTP_GET, [this]() { 
        webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        webServer.sendHeader("Pragma", "no-cache");
        webServer.sendHeader("Expires", "-1");
        webServer.send(200, "text/plain", "fail"); // Send fail to keep captive portal active
    });
    webServer.on("/ncsi.txt", HTTP_GET, [this]() { 
        webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        webServer.sendHeader("Pragma", "no-cache");
        webServer.sendHeader("Expires", "-1");
        webServer.send(200, "text/plain", "fail"); // Send fail to keep captive portal active
    });
    
    // Handle main pages
    webServer.on("/", HTTP_GET, [this]() { handleRoot(); });
    webServer.on("/save", HTTP_POST, [this]() { handleSave(); });
    
    // Catch-all handler for captive portal
    webServer.onNotFound([this]() { handleRoot(); });
    
    webServer.begin();
}

void WiFiSetup::handleRoot() {
    webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    webServer.sendHeader("Pragma", "no-cache");
    webServer.sendHeader("Expires", "-1");
    webServer.send(200, "text/html", HTML_FORM);
}

void WiFiSetup::handleSave() {
    String ssid = webServer.arg("ssid");
    String password = webServer.arg("password");
    
    if (ssid.length() > 0) {
        webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        webServer.sendHeader("Pragma", "no-cache");
        webServer.sendHeader("Expires", "-1");
        
        // Send an immediate response to keep the connection alive
        webServer.send(200, "text/html", "Attempting to connect...");
        
        // Try to connect to the new network
        if (connectToWiFi(ssid, password)) {
            // Only save credentials if connection was successful
            prefs.putString(WIFI_SSID_KEY, ssid);
            prefs.putString(WIFI_PASS_KEY, password);
            
            String successPage = R"(
                <!DOCTYPE html>
                <html>
                <head>
                    <meta name="viewport" content="width=device-width, initial-scale=1.0">
                    <style>
                        body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; text-align: center; padding: 20px; }
                        .success { color: #4CAF50; font-size: 24px; margin: 20px 0; }
                        .message { margin: 20px 0; }
                    </style>
                </head>
                <body>
                    <div class="success">✓ Connected Successfully</div>
                    <div class="message">Your Scout device is now connected to WiFi.<br>The device will restart in a few seconds...</div>
                </body>
                </html>
            )";
            
            webServer.send(200, "text/html", successPage);
            // Schedule restart after response is sent
            static unsigned long restartTime = millis() + 1000;
            if (millis() >= restartTime) {
                ESP.restart();
            }
        } else {
            String errorPage = R"(
                <!DOCTYPE html>
                <html>
                <head>
                    <meta name="viewport" content="width=device-width, initial-scale=1.0">
                    <style>
                        body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; text-align: center; padding: 20px; }
                        .error { color: #f44336; font-size: 24px; margin: 20px 0; }
                        .message { margin: 20px 0; }
                        .button { display: inline-block; background: #007bff; color: white; padding: 10px 20px; text-decoration: none; border-radius: 5px; }
                    </style>
                </head>
                <body>
                    <div class="error">Connection Failed</div>
                    <div class="message">Could not connect to the specified network.<br>Please check your credentials and try again.</div>
                    <a href="/" class="button">Try Again</a>
                </body>
                </html>
            )";
            
            webServer.send(200, "text/html", errorPage);
        }
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
    Serial.println("Attempting to connect to WiFi...");
    Serial.print("SSID: ");
    Serial.println(ssid);
    
    // Clean up existing connections
    WiFi.disconnect(true, true);
    delay(10);
    yield();
    
    WiFi.mode(WIFI_STA);
    yield();
    
    WiFi.begin(ssid.c_str(), password.c_str());
    
    unsigned long startAttemptTime = millis();
    Serial.print("Connecting");
    
    // Try for 10 seconds
    while (WiFi.status() != WL_CONNECTED && 
           millis() - startAttemptTime < 10000) {
        Serial.print(".");
        delay(100);
        yield();  // Feed the watchdog
    }
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Successfully connected to WiFi");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    } else {
        Serial.print("Failed to connect. Status code: ");
        Serial.println(WiFi.status());
        return false;
    }
}
