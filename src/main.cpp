#include <Arduino.h>

// Configuration
#include "config/Settings.h"
#include "config/Pins.h"
#include "config/DataTypes.h"

// Manager Classes
#include "feedback/FeedbackManager.h"
#include "network/WifiHandler.h"
#include "setup/DeviceManager.h"
#include "sensors/SensorManager.h"
#include "utilities/MqttHandler.h"

// System State Machine
SystemState currentState = SystemState::BOOTING;
unsigned long lastStateChange = 0;

// Global Manager Instances
FeedbackManager feedbackManager;
WifiHandler wifiHandler;
DeviceManager deviceManager;
SensorManager sensorManager;
MqttHandler mqttHandler;

void setup() {
    // Initialize Serial for ESP32-S3 with USB CDC
    Serial.begin(115200);
    
    // Serial Monitor Wait (Critical for ESP32-S3)
    delay(1000); // Wait 1 second
    while (!Serial); // Wait for serial connection
    
    #ifdef DEBUG
    Serial.println("=====================================");
    Serial.println("    HearthGuard: The Scout (Basic)   ");
    Serial.println("   Medieval Themed IoT HearthGuard   ");
    Serial.println("=====================================");
    Serial.printf("Device: %s v%s\n", DEVICE_NAME, FIRMWARE_VERSION);
    Serial.printf("Build: %s %s\n", __DATE__, __TIME__);
    Serial.println("=====================================");
    Serial.println("[BOOT] Starting Phase 0 - Project Scaffolding");
    Serial.println("[BOOT] All manager classes will be initialized...");
    Serial.println();
    Serial.println("[INIT] Initializing all subsystems...");
    #endif
    
    // Transition to initialization state
    currentState = SystemState::INITIALIZING;
    lastStateChange = millis();
    
    // Initialize all manager classes following PRD template exactly
    #ifdef DEBUG
    Serial.println("[INIT] Calling begin() on all manager classes...");
    #endif
    
    feedbackManager.begin();
    wifiHandler.begin();  
    deviceManager.begin();
    sensorManager.begin();
    mqttHandler.begin();
    
    #ifdef DEBUG
    Serial.println("[INIT] All subsystems initialized successfully!");
    Serial.println("[BOOT] The Scout is ready for Phase 1 development");
    #endif
    
    currentState = SystemState::NORMAL_OPERATION;
    lastStateChange = millis();
}

void loop() {
    // Main State Machine
    switch (currentState) {
        case SystemState::BOOTING:
            // This state is handled in setup()
            break;
            
        case SystemState::INITIALIZING:
            // This state is handled in setup()
            break;
            
        case SystemState::WIFI_CONNECTING:
            // Handle WiFi connection process
            wifiHandler.update();
            feedbackManager.update();
            break;
            
        case SystemState::SENSOR_CALIBRATING:
            // Handle sensor calibration
            sensorManager.update();
            feedbackManager.update();
            break;
            
        case SystemState::NORMAL_OPERATION:
            // Update all managers in normal operation
            feedbackManager.update();
            wifiHandler.update();
            deviceManager.update();
            sensorManager.update();
            mqttHandler.update();
            
            // Periodic status update
            static unsigned long lastStatusUpdate = 0;
            if (millis() - lastStatusUpdate > STATUS_UPDATE_INTERVAL) {
                #ifdef DEBUG
                Serial.println("[STATUS] The Scout is operating normally - Phase 0 Complete");
                #endif
                lastStatusUpdate = millis();
            }
            break;
            
        case SystemState::FACTORY_RESET:
            // Handle factory reset
            deviceManager.update();
            feedbackManager.update();
            break;
            
        case SystemState::ERROR_STATE:
            // Handle error state - minimal operations
            feedbackManager.update();
            
            static unsigned long lastErrorReport = 0;
            if (millis() - lastErrorReport > 10000) { // Report every 10 seconds
                #ifdef DEBUG
                Serial.println("[ERROR] System in error state - check initialization");
                #endif
                lastErrorReport = millis();
            }
            break;
    }
    
    // Small delay to prevent excessive CPU usage
    delay(10);
}
