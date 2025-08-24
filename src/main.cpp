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
WifiHandler wifiHandler(feedbackManager); // Pass feedback manager reference
DeviceManager deviceManager;
SensorManager sensorManager;
MqttHandler mqttHandler;

void setup() {
    // Initialize Serial Monitor
    Serial.begin(SERIAL_BAUD_RATE);
    delay(1000); // Allow serial to stabilize
    
    Serial.println("=====================================");
    Serial.println("    HearthGuard: The Scout (Basic)   ");
    Serial.println("    Medieval Themed IoT Guardian     ");
    Serial.println("=====================================");
    Serial.printf("Device: %s v%s\n", DEVICE_NAME, FIRMWARE_VERSION);
    Serial.printf("Build: %s %s\n", __DATE__, __TIME__);
    Serial.println("=====================================");
    
    // Transition to initialization state
    currentState = SystemState::INITIALIZING;
    lastStateChange = millis();
    
    Serial.println("[BOOT] Starting Phase 0 - Project Scaffolding");
    Serial.println("[BOOT] All manager classes will be initialized...");
    
    // Initialize all manager classes
    bool initSuccess = true;
    
    Serial.println("\n[INIT] Initializing all subsystems...");
    
    if (!feedbackManager.begin()) {
        Serial.println("[ERROR] Failed to initialize Feedback Manager");
        initSuccess = false;
    }
    
    if (!wifiHandler.begin()) {
        Serial.println("[ERROR] Failed to initialize WiFi Handler");
        initSuccess = false;
    }
    
    if (!deviceManager.begin()) {
        Serial.println("[ERROR] Failed to initialize Device Manager");
        initSuccess = false;
    }
    
    if (!sensorManager.begin()) {
        Serial.println("[ERROR] Failed to initialize Sensor Manager");
        initSuccess = false;
    }
    
    if (!mqttHandler.begin()) {
        Serial.println("[ERROR] Failed to initialize MQTT Handler");
        initSuccess = false;
    }
    
    // Check initialization results
    if (initSuccess) {
        Serial.println("\n[INIT] All subsystems initialized successfully!");
        Serial.println("[BOOT] The Scout is ready for Phase 1 development");
        currentState = SystemState::NORMAL_OPERATION;
    } else {
        Serial.println("\n[ERROR] System initialization failed!");
        currentState = SystemState::ERROR_STATE;
    }
    
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
                Serial.println("[STATUS] The Scout is operating normally - Phase 0 Complete");
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
                Serial.println("[ERROR] System in error state - check initialization");
                lastErrorReport = millis();
            }
            break;
    }
    
    // Small delay to prevent excessive CPU usage
    delay(10);
}
