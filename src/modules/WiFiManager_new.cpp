#include "modules/WiFiManager.h"
#include "Config.h"

WiFiManager::WiFiManager() 
    : connection(), config(), portal(&config),
      systemLED(nullptr), buzzer(nullptr),
      isSetupMode(false), setupRebootScheduled(false), setupConnectedTime(0) {
    
    // Set up connection callback
    portal.setConnectionCallback([this](String ssid, String password) {
        onConnectionRequest(ssid, password);
    });
}

void WiFiManager::begin(LEDController* led, BuzzerController* buzzerCtrl) {
    Serial.println("[WiFiManager] Initializing WiFi Manager...");
    
    systemLED = led;
    buzzer = buzzerCtrl;
    
    // Initialize components
    config.begin();
    connection.begin(systemLED, buzzer);
    
    Serial.printf("[WiFiManager] Device name: %s\n", config.getDeviceName().c_str());
    
    // Check for stored credentials and attempt connection
    if (connectToStoredNetwork()) {
        Serial.println("[WiFiManager] Attempting to connect with stored credentials");
    } else {
        Serial.println("[WiFiManager] No stored credentials, starting setup mode");
        startSetupMode();
    }
}

void WiFiManager::update() {
    connection.update();
    
    if (isSetupMode) {
        portal.update();
        
        // Handle reboot after successful setup
        if (setupRebootScheduled && millis() - setupConnectedTime > 3000) {
            Serial.println("[WiFiManager] Rebooting after successful setup...");
            ESP.restart();
        }
    }
    
    updateConnectionFeedback();
}

bool WiFiManager::isConnected() {
    return connection.isConnected();
}

void WiFiManager::startSetupMode() {
    Serial.println("[WiFiManager] Starting setup mode...");
    
    isSetupMode = true;
    setupRebootScheduled = false;
    
    // Start AP mode
    WiFi.mode(WIFI_AP_STA);
    String apName = config.getDeviceName();
    WiFi.softAP(apName.c_str());
    
    Serial.printf("[WiFiManager] AP started: %s\n", apName.c_str());
    Serial.printf("[WiFiManager] AP IP: %s\n", WiFi.softAPIP().toString().c_str());
    
    // Start web portal
    portal.begin();
    
    // Update LED for setup mode
    if (systemLED) {
        systemLED->setColor(255, 100, 0); // Orange for setup mode
        systemLED->setBrightness(128);
        systemLED->update();
    }
}

void WiFiManager::stopSetupMode() {
    Serial.println("[WiFiManager] Stopping setup mode...");
    
    isSetupMode = false;
    setupRebootScheduled = false;
    
    portal.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
}

bool WiFiManager::connectToStoredNetwork() {
    if (!config.hasStoredCredentials()) {
        return false;
    }
    
    auto credentials = config.getStoredCredentials();
    connection.startConnection(credentials.first, credentials.second);
    return true;
}

void WiFiManager::connectToNetwork(const String& ssid, const String& password) {
    connection.startConnection(ssid, password);
}

void WiFiManager::factoryReset() {
    Serial.println("[WiFiManager] Factory reset requested");
    config.clearAllPreferences();
    
    // Restart to setup mode
    ESP.restart();
}

void WiFiManager::onConnectionRequest(String ssid, String password) {
    Serial.printf("[WiFiManager] Connection request for SSID: %s\n", ssid.c_str());
    
    // Save credentials
    config.saveCredentials(ssid, password);
    
    // Stop setup mode WiFi scanning
    WiFi.scanDelete();
    
    // Start connection attempt
    connection.startConnection(ssid, password);
    
    // Mark as setup mode connection
    isSetupMode = true;
}

void WiFiManager::updateConnectionFeedback() {
    if (!systemLED || !buzzer) return;
    
    ConnectionState state = connection.getConnectionState();
    
    switch (state) {
        case ConnectionState::IDLE:
            if (isSetupMode) {
                // Orange for setup mode
                systemLED->setColor(255, 100, 0);
                systemLED->setBrightness(128);
            } else {
                // Red for no connection
                systemLED->setColor(255, 0, 0);
                systemLED->setBrightness(64);
            }
            break;
            
        case ConnectionState::CONNECTING:
            // Blue blinking for connecting
            systemLED->setColor(0, 100, 255);
            systemLED->setBrightness(128);
            systemLED->setBlinking(500);
            break;
            
        case ConnectionState::CONNECTED:
            // Green for connected
            systemLED->setColor(0, 255, 0);
            systemLED->setBrightness(128);
            systemLED->setBlinking(0); // Solid
            
            // Handle setup mode completion
            if (isSetupMode && !setupRebootScheduled) {
                scheduleReboot();
            }
            break;
            
        case ConnectionState::FAILED:
            // Red blinking for failed
            systemLED->setColor(255, 0, 0);
            systemLED->setBrightness(128);
            systemLED->setBlinking(250);
            
            // If this was a setup mode connection, clear bad credentials
            if (isSetupMode) {
                Serial.println("[WiFiManager] Setup connection failed, clearing credentials");
                config.clearStoredCredentials();
                // Stay in setup mode for retry
            }
            break;
            
        case ConnectionState::TIMEOUT:
            // Orange blinking for timeout
            systemLED->setColor(255, 165, 0);
            systemLED->setBrightness(128);
            systemLED->setBlinking(1000);
            
            // Handle timeout in setup mode
            if (isSetupMode) {
                Serial.println("[WiFiManager] Setup connection timeout, clearing credentials");
                config.clearStoredCredentials();
            }
            break;
    }
    
    systemLED->update();
}

void WiFiManager::scheduleReboot() {
    Serial.println("[WiFiManager] Scheduling reboot after successful setup");
    
    setupConnectedTime = millis();
    setupRebootScheduled = true;
    
    // Play success chime
    if (buzzer) {
        buzzer->playScoutConnectedChime();
    }
    
    // Brief success feedback
    if (systemLED) {
        systemLED->setColor(0, 255, 0);
        systemLED->setBrightness(255);
        systemLED->setBlinking(0);
        systemLED->update();
    }
}
