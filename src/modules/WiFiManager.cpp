#include "modules/WiFiManager.h"
#include "config.h"

WiFiManager::WiFiManager(LEDController* sysLED, BuzzerController* buzz) :
    systemLED(sysLED),
    buzzer(buzz),
    webServer(Config::WiFi::AP_PORT),
    currentState(WiFiState::CHECKING_CREDENTIALS),
    lastConnectionAttempt(0),
    lastRetryAttempt(0),
    connectionAttempts(0),
    hasStoredCredentials(false),
    isConnecting(false),
    hasEverConnected(false) {}

void WiFiManager::begin() {
    Serial.println("WiFiManager: Initializing...");
    
    // Load or generate device name
    preferences.begin("thescout", false);
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
    
    // Check for stored credentials
    hasStoredCredentials = loadCredentials();
    
    if (hasStoredCredentials) {
        Serial.println("Found stored WiFi credentials");
        currentState = WiFiState::CONNECTING;
        connectToWiFi();
    } else {
        Serial.println("No stored credentials found");
        currentState = WiFiState::AP_MODE;
        startAccessPoint();
    }
}

void WiFiManager::update() {
    unsigned long currentTime = millis();
    
    // Handle DNS requests in AP mode
    if (currentState == WiFiState::AP_MODE) {
        dnsServer.processNextRequest();
        webServer.handleClient();
    }
    
    // Update feedback LEDs/buzzer
    updateFeedback();
    
    switch (currentState) {
        case WiFiState::CONNECTING:
            if (WiFi.status() == WL_CONNECTED) {
                currentState = WiFiState::CONNECTED;
                hasEverConnected = true; // Mark that we've successfully connected
                Serial.println("WiFi connected successfully!");
                Serial.println("IP: " + WiFi.localIP().toString());
                
                // Play success chime and flash green
                buzzer->playConnectSuccess();
                systemLED->setColor(Config::Colors::SUCCESS_GREEN);
                delay(200);
                systemLED->setOff();
                delay(100);
                systemLED->setColor(Config::Colors::SUCCESS_GREEN);
                delay(200);
                systemLED->setColor(Config::Colors::TORCH_ORANGE);
                
                // Stop DNS server since we're no longer in AP mode
                dnsServer.stop();
                
            } else if (currentTime - lastConnectionAttempt > Config::WiFi::CONNECTION_TIMEOUT_MS) {
                // Connection timeout
                Serial.println("Connection timeout - WiFi status: " + String(WiFi.status()));
                Serial.println("Time elapsed: " + String(currentTime - lastConnectionAttempt) + "ms");
                currentState = WiFiState::CONNECTION_FAILED;
                connectionAttempts++;
                
                // Only play failure feedback if we've reached max fast retries
                if (connectionAttempts >= Config::WiFi::MAX_FAST_RETRIES) {
                    Serial.println("Max retries reached, playing failure feedback");
                    buzzer->playConnectFailed();
                    systemLED->setColor(Config::Colors::DANGER_RED);
                    delay(200);
                    systemLED->setOff();
                    delay(100);
                    systemLED->setColor(Config::Colors::DANGER_RED);
                    delay(200);
                    systemLED->setOff();
                } else {
                    Serial.println("Attempt " + String(connectionAttempts) + " of " + String(Config::WiFi::MAX_FAST_RETRIES) + " failed, will retry");
                }
                
                scheduleRetry();
            } else {
                // Still connecting - log status periodically
                static unsigned long lastStatusLog = 0;
                if (currentTime - lastStatusLog > 2000) { // Log every 2 seconds
                    Serial.println("Connecting... WiFi status: " + String(WiFi.status()) + ", elapsed: " + String(currentTime - lastConnectionAttempt) + "ms");
                    lastStatusLog = currentTime;
                }
            }
            break;
            
        case WiFiState::CONNECTION_FAILED:
            // If this is the first connection attempt and it failed, restart AP
            if (!hasEverConnected && connectionAttempts == 1) {
                Serial.println("First connection failed, restarting AP mode");
                currentState = WiFiState::AP_MODE;
                startAccessPoint();
            } else if (currentTime - lastRetryAttempt > getRetryInterval()) {
                Serial.println("Retrying WiFi connection...");
                currentState = WiFiState::CONNECTING;
                connectToWiFi();
            }
            break;
            
        case WiFiState::CONNECTED:
            if (WiFi.status() != WL_CONNECTED) {
                Serial.println("WiFi connection lost");
                currentState = WiFiState::CONNECTION_FAILED;
                connectionAttempts = 0; // Reset counter for fresh attempts
                scheduleRetry();
            }
            break;
            
        default:
            break;
    }
}

bool WiFiManager::connectToWiFi() {
    if (storedSSID.isEmpty()) {
        Serial.println("No SSID to connect to");
        return false;
    }
    
    Serial.println("Connecting to: " + storedSSID);
    
    // Stop AP mode and switch to STA mode for connection
    if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
        Serial.println("Stopping AP mode for connection attempt");
        WiFi.softAPdisconnect(true);
        delay(100);
    }
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(storedSSID.c_str(), storedPassword.c_str());
    
    lastConnectionAttempt = millis();
    isConnecting = true;
    
    return true;
}

void WiFiManager::startAccessPoint() {
    Serial.println("Starting Access Point: " + String(Config::WiFi::AP_SSID));
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP(Config::WiFi::AP_SSID);
    
    // Configure DNS server for captive portal
    dnsServer.start(53, "*", WiFi.softAPIP());
    
    // Setup web server routes
    webServer.on("/", [this]() { handleRoot(); });
    webServer.on("/api/scan", [this]() { handleScan(); });
    webServer.on("/connect", [this]() { handleConnect(); });
    webServer.on("/connecting", [this]() { handleConnecting(); });
    webServer.onNotFound([this]() { handleNotFound(); });
    
    webServer.begin();
    
    Serial.println("Captive portal ready at: " + WiFi.softAPIP().toString());
}

void WiFiManager::stopAccessPoint() {
    dnsServer.stop();
    webServer.stop();
    WiFi.softAPdisconnect(true);
    Serial.println("Access Point stopped");
}

bool WiFiManager::loadCredentials() {
    storedSSID = preferences.getString("wifi_ssid", "");
    storedPassword = preferences.getString("wifi_pass", "");
    
    return !storedSSID.isEmpty();
}

void WiFiManager::saveCredentials(const String& ssid, const String& password) {
    preferences.putString("wifi_ssid", ssid);
    preferences.putString("wifi_pass", password);
    
    storedSSID = ssid;
    storedPassword = password;
    hasStoredCredentials = true;
    
    Serial.println("Saved WiFi credentials for: " + ssid);
}

String WiFiManager::generateDeviceName() {
    randomSeed(ESP.getEfuseMac());
    uint32_t deviceId = random(100000, 999999);
    return "TheScout-" + String(deviceId);
}

void WiFiManager::updateFeedback() {
    switch (currentState) {
        case WiFiState::CONNECTING:
            // Flash blue while connecting
            systemLED->setBlink(Config::LED_BLINK_FAST, Config::Colors::CASTLE_BLUE);
            break;
            
        case WiFiState::AP_MODE:
            // Slow purple blink in AP mode
            systemLED->setBlink(Config::LED_BLINK_SLOW, Config::Colors::ROYAL_PURPLE);
            break;
            
        case WiFiState::CONNECTED:
            // Steady torch orange when connected
            systemLED->setOn(Config::Colors::TORCH_ORANGE);
            break;
            
        case WiFiState::CONNECTION_FAILED:
            // Brief red indication, then off
            systemLED->setOff();
            break;
            
        default:
            break;
    }
}

unsigned long WiFiManager::getRetryInterval() {
    if (connectionAttempts < Config::WiFi::MAX_FAST_RETRIES) {
        return Config::WiFi::RETRY_INTERVAL_FAST_MS;
    } else {
        return Config::WiFi::RETRY_INTERVAL_SLOW_MS;
    }
}

void WiFiManager::scheduleRetry() {
    lastRetryAttempt = millis();
    Serial.println("Next retry in " + String(getRetryInterval() / 1000) + " seconds");
}

bool WiFiManager::isConnected() {
    return currentState == WiFiState::CONNECTED && WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getDeviceName() {
    return deviceName;
}

WiFiState WiFiManager::getState() {
    return currentState;
}

// Web server handlers
void WiFiManager::handleRoot() {
    webServer.send(200, "text/html", getMainPage());
}

void WiFiManager::handleScan() {
    // Scan for networks
    int numNetworks = WiFi.scanNetworks();
    
    // Create JSON array of unique SSIDs
    String jsonResponse = "[";
    String addedSSIDs[50]; // Support up to 50 unique networks
    int uniqueCount = 0;
    
    if (numNetworks > 0) {
        for (int i = 0; i < numNetworks && uniqueCount < 50; i++) {
            String ssid = WiFi.SSID(i);
            
            // Skip empty SSIDs
            if (ssid.isEmpty()) continue;
            
            // Check if we've already added this SSID
            bool isDuplicate = false;
            for (int j = 0; j < uniqueCount; j++) {
                if (addedSSIDs[j] == ssid) {
                    isDuplicate = true;
                    break;
                }
            }
            
            // Add if it's not a duplicate
            if (!isDuplicate) {
                if (uniqueCount > 0) {
                    jsonResponse += ",";
                }
                addedSSIDs[uniqueCount] = ssid;
                uniqueCount++;
                jsonResponse += "\"" + ssid + "\"";
            }
        }
    }
    
    jsonResponse += "]";
    
    webServer.send(200, "application/json", jsonResponse);
}

void WiFiManager::handleConnect() {
    if (webServer.hasArg("ssid") && webServer.hasArg("password")) {
        String ssid = webServer.arg("ssid");
        String password = webServer.arg("password");
        
        Serial.println("Connection request for SSID: " + ssid);
        
        if (isValidCredentials(ssid, password)) {
            saveCredentials(ssid, password);
            
            // Send connecting page
            webServer.send(200, "text/html", getConnectingPage());
            
            // Give web server time to send response
            delay(100);
            
            // Start connection attempt
            currentState = WiFiState::CONNECTING;
            connectionAttempts = 0;
            connectToWiFi();
        } else {
            Serial.println("Invalid credentials provided");
            webServer.send(400, "text/plain", "Invalid credentials");
        }
    } else {
        Serial.println("Missing SSID or password in request");
        webServer.send(400, "text/plain", "Missing SSID or password");
    }
}

void WiFiManager::handleConnecting() {
    if (isConnected()) {
        webServer.send(200, "text/html", getSuccessPage());
    } else {
        webServer.send(200, "text/html", getConnectingPage());
    }
}

void WiFiManager::handleNotFound() {
    // Redirect to main page for captive portal
    webServer.sendHeader("Location", "/", true);
    webServer.send(302, "text/plain", "");
}

String WiFiManager::scanNetworks() {
    Serial.println("Scanning for WiFi networks...");
    
    WiFi.scanDelete();
    int networkCount = WiFi.scanNetworks();
    
    JsonDocument doc;
    JsonArray networks = doc["networks"].to<JsonArray>();
    
    for (int i = 0; i < networkCount; i++) {
        JsonObject network = networks.add<JsonObject>();
        network["ssid"] = WiFi.SSID(i);
        network["rssi"] = WiFi.RSSI(i);
        network["encryption"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "open" : "secured";
    }
    
    String result;
    serializeJson(doc, result);
    return result;
}

bool WiFiManager::isValidCredentials(const String& ssid, const String& password) {
    bool valid = !ssid.isEmpty() && ssid.length() <= 32 && password.length() <= 64;
    Serial.println("Credential validation - SSID: '" + ssid + "' (length: " + String(ssid.length()) + "), Password length: " + String(password.length()) + ", Valid: " + String(valid));
    return valid;
}

String WiFiManager::getMainPage() {
    return String(R"(<!DOCTYPE html>
<html>
<head>
    <title>The Scout WiFi Setup</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; background: #1a1a2e; color: #eee; margin: 0; padding: 20px; }
        .container { max-width: 500px; margin: 0 auto; background: #16213e; padding: 30px; border-radius: 10px; box-shadow: 0 0 20px rgba(255,140,0,0.3); }
        h1 { color: #ff8c00; text-align: center; margin-bottom: 30px; }
        .scout { text-align: center; font-size: 2em; margin-bottom: 20px; color: #ff8c00; font-weight: bold; }
        form { margin: 20px 0; }
        label { display: block; margin: 15px 0 5px 0; font-weight: bold; color: #ff8c00; }
        select, input[type="password"] { width: 100%; padding: 12px; margin: 5px 0 15px 0; border: 2px solid #ff8c00; border-radius: 5px; background: #1a1a2e; color: #eee; font-size: 16px; box-sizing: border-box; }
        select:disabled { opacity: 0.6; cursor: not-allowed; background: #333; }
        button { width: 100%; padding: 15px; background: #ff8c00; color: #1a1a2e; border: none; border-radius: 5px; font-size: 16px; font-weight: bold; cursor: pointer; margin: 10px 0; }
        button:hover:not(:disabled) { background: #e07b00; }
        button:disabled { background: #666; cursor: not-allowed; opacity: 0.6; }
        .intro { margin: 20px 0; padding: 15px; background: #0f3460; border-radius: 5px; color: #eee; }
        
        /* Loading overlay */
        .loading-overlay { 
            position: fixed; 
            top: 0; 
            left: 0; 
            width: 100%; 
            height: 100%; 
            background: rgba(0, 0, 0, 0.7); 
            display: flex; 
            flex-direction: column; 
            justify-content: center; 
            align-items: center; 
            z-index: 9999; 
        }
        .loading-spinner { 
            width: 50px; 
            height: 50px; 
            border: 4px solid #333; 
            border-top: 4px solid #ff8c00; 
            border-radius: 50%; 
            animation: spin 1s linear infinite; 
            margin-bottom: 20px; 
        }
        .loading-text { 
            color: #eee; 
            font-size: 1.1em; 
            text-align: center; 
        }
        @keyframes spin { 
            0% { transform: rotate(0deg); } 
            100% { transform: rotate(360deg); } 
        }
        .hidden { display: none !important; }
    </style>
</head>
<body>
    <div class="container">
        <div class="scout">The Scout</div>
        <h1>WiFi Setup</h1>
        <div class="intro">
            Welcome to your new Scout device. To get started, please select your home WiFi network from the list below and enter the password to bring it online.
        </div>
        
        <form action="/connect" method="POST">
            <label for="ssid">Select Network:</label>
            <select name="ssid" id="ssid" required>
                <option value="">Scanning for networks...</option>
            </select>
            
            <label for="password">Password:</label>
            <input type="password" name="password" id="password" placeholder="Enter WiFi password" required />
            
            <button type="submit" id="connectBtn" disabled>Connect to Network</button>
        </form>
    </div>

    <!-- Loading overlay -->
    <div id="loadingOverlay" class="loading-overlay">
        <div class="loading-spinner"></div>
        <div class="loading-text">Scanning for networks<br>Please wait...</div>
    </div>

    <script>
        // Auto-scan for networks when page loads
        window.addEventListener('load', function() {
            const loadingOverlay = document.getElementById('loadingOverlay');
            const select = document.getElementById('ssid');
            const connectBtn = document.getElementById('connectBtn');
            
            // Show loading overlay
            loadingOverlay.classList.remove('hidden');
            
            setTimeout(function() {
                fetch('/api/scan')
                    .then(response => response.json())
                    .then(data => {
                        select.innerHTML = '<option value="">Choose a network...</option>';
                        
                        if (data && data.length > 0) {
                            data.forEach(ssid => {
                                const option = document.createElement('option');
                                option.value = ssid;
                                option.textContent = ssid;
                                select.appendChild(option);
                            });
                        } else {
                            select.innerHTML = '<option value="">No networks found</option>';
                        }
                        
                        // Enable form interaction
                        select.disabled = false;
                        select.addEventListener('change', function() {
                            connectBtn.disabled = !this.value;
                        });
                        
                        // Hide loading overlay
                        loadingOverlay.classList.add('hidden');
                    })
                    .catch(error => {
                        console.error('Failed to load networks:', error);
                        select.innerHTML = '<option value="">Failed to scan - refresh page</option>';
                        
                        // Hide loading overlay even on error
                        loadingOverlay.classList.add('hidden');
                    });
            }, 500);
        });
    </script>
</body>
</html>)");
}

String WiFiManager::getConnectingPage() {
    return R"(
<!DOCTYPE html>
<html>
<head>
    <title>TheScout - Connecting</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta http-equiv="refresh" content="3;url=/connecting">
    <style>
        body { font-family: Arial, sans-serif; background: #1a1a2e; color: #eee; margin: 0; padding: 20px; text-align: center; }
        .container { max-width: 500px; margin: 50px auto; background: #16213e; padding: 40px; border-radius: 10px; box-shadow: 0 0 20px rgba(255,140,0,0.3); }
        h1 { color: #ff8c00; margin-bottom: 30px; }
        .castle { font-size: 3em; margin-bottom: 20px; animation: pulse 2s infinite; }
        .connecting-text { font-size: 1.2em; margin: 20px 0; }
        .spinner { width: 50px; height: 50px; border: 3px solid #333; border-top: 3px solid #ff8c00; border-radius: 50%; animation: spin 1s linear infinite; margin: 20px auto; }
        @keyframes spin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }
        @keyframes pulse { 0%, 100% { opacity: 1; } 50% { opacity: 0.5; } }
    </style>
</head>
<body>
    <div class="container">
        <div class="castle">Castle</div>
        <h1>Connecting to Castle Network</h1>
        <div class="spinner"></div>
        <div class="connecting-text">
            Your faithful scout is establishing communication with the castle's network...
        </div>
        <p>Please wait while the connection is established.</p>
    </div>
</body>
</html>
)";
}

String WiFiManager::getSuccessPage() {
    return R"(
<!DOCTYPE html>
<html>
<head>
    <title>TheScout - Connected!</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; background: #1a1a2e; color: #eee; margin: 0; padding: 20px; text-align: center; }
        .container { max-width: 500px; margin: 50px auto; background: #16213e; padding: 40px; border-radius: 10px; box-shadow: 0 0 20px rgba(50,205,50,0.3); }
        h1 { color: #32cd32; margin-bottom: 30px; }
        .castle { font-size: 3em; margin-bottom: 20px; }
        .success-text { font-size: 1.2em; margin: 20px 0; color: #32cd32; }
        .info { background: #0f3460; padding: 15px; border-radius: 5px; margin: 20px 0; }
    </style>
</head>
<body>
    <div class="container">
        <div class="castle">Castle Connected</div>
        <h1>Connection Successful!</h1>
        <div class="success-text">
            Huzzah! Your scout has successfully connected to the castle network!
        </div>
        <div class="info">
            <p><strong>Device Name:</strong> )" + deviceName + R"(</p>
            <p><strong>Network:</strong> )" + storedSSID + R"(</p>
            <p><strong>Status:</strong> Ready for duty!</p>
        </div>
        <p>The WiFi setup is complete. Your device will now automatically connect to this network.</p>
    </div>
</body>
</html>
)";
}

