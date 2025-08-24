#include <Arduino.h>
#include "driver/gpio.h"

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

// Include color constants and pixel definitions for demo
#include "feedback/LedController.h"

// System State Machine
SystemState currentState = SystemState::BOOTING;
unsigned long lastStateChange = 0;
bool managersInitialized = false;
unsigned long bootStartTime = 0;

// Global Manager Instances
FeedbackManager feedbackManager;
WifiHandler wifiHandler;
DeviceManager deviceManager;
SensorManager sensorManager;
MqttHandler mqttHandler;

void setup() {
    // Initialize LED pins as outputs and ensure they're OFF
    pinMode(3, OUTPUT);   
    pinMode(45, OUTPUT);  
    digitalWrite(3, LOW);
    digitalWrite(45, LOW);
    
    bootStartTime = millis();
    
    // Initialize Serial for ESP32-S3 with USB CDC
    Serial.begin(115200);
    
    // Serial Monitor Wait (Critical for ESP32-S3)
    delay(1000); 
    while (!Serial); 
    
    #ifdef DEBUG
    Serial.println("=====================================");
    Serial.println("    HearthGuard: The Scout v1.0     ");
    Serial.println("   Medieval Themed IoT HearthGuard   ");
    Serial.println("=====================================");
    Serial.printf("Device: %s v%s\n", DEVICE_NAME, FIRMWARE_VERSION);
    Serial.printf("Build: %s %s\n", __DATE__, __TIME__);
    Serial.println("=====================================");
    Serial.println();
    #endif
    
    currentState = SystemState::BOOTING;
    lastStateChange = millis();
}

void loop() {
    unsigned long currentTime = millis();
    
    switch (currentState) {
        case SystemState::BOOTING:
            // Initialize after brief delay to ensure stable boot
            if (currentTime - bootStartTime > 1000 && !managersInitialized) {
                #ifdef DEBUG
                Serial.println("[BOOT] Initializing all subsystems...");
                Serial.flush();
                #endif
                
                // Initialize all manager classes
                feedbackManager.begin();
                wifiHandler.begin();  
                deviceManager.begin();
                sensorManager.begin();
                mqttHandler.begin();
                
                managersInitialized = true;
                
                #ifdef DEBUG
                Serial.println("[BOOT] All subsystems initialized!");
                Serial.println("[BOOT] The Scout is ready for Phase 1 development");
                Serial.flush();
                #endif
                
                currentState = SystemState::NORMAL_OPERATION;
                lastStateChange = millis();
            }
            break;
            
        case SystemState::NORMAL_OPERATION:
            // Update all managers in normal operation
            feedbackManager.update();
            wifiHandler.update();
            deviceManager.update();
            sensorManager.update();
            mqttHandler.update();
            
            // Phase 1 Demo: LED and Buzzer Test Sequence
            static unsigned long lastDemoUpdate = 0;
            static int demoStep = 0;
            static bool demoStarted = false;
            
            // Wait 3 seconds after initialization before starting demo
            if (!demoStarted && millis() - lastStateChange > 3000) {
                #ifdef DEBUG
                Serial.println("[DEMO] Starting LED demo sequence...");
                #endif
                demoStarted = true;
                lastDemoUpdate = millis();
            }
            
            if (demoStarted && millis() - lastDemoUpdate > 5000) { // Every 5 seconds
                switch (demoStep) {
                    case 0:
                        #ifdef DEBUG
                        Serial.println("[DEMO] Starting LED system status (green pulse)");
                        #endif
                        feedbackManager.startAnimation(PIXEL_SYSTEM, HearthGuardColors::HEARTHGUARD_GREEN, 
                                                     LedAnimation::PULSE, 2000);
                        feedbackManager.playConfirm();
                        break;
                    case 1:
                        #ifdef DEBUG
                        Serial.println("[DEMO] Starting LED activity status (blue blink)");
                        #endif
                        feedbackManager.startAnimation(PIXEL_ACTIVITY, HearthGuardColors::HEARTHGUARD_BLUE, 
                                                     LedAnimation::BLINK, 500);
                        feedbackManager.playInteraction();
                        break;
                    case 2:
                        #ifdef DEBUG
                        Serial.println("[DEMO] Playing success sound and solid orange LED");
                        #endif
                        feedbackManager.startAnimation(PIXEL_SYSTEM, HearthGuardColors::HEARTHGUARD_ORANGE, 
                                                     LedAnimation::SOLID, 0);
                        feedbackManager.playSuccess();
                        break;
                    case 3:
                        #ifdef DEBUG
                        Serial.println("[DEMO] Playing failure sound and red LED");
                        #endif
                        feedbackManager.startAnimation(PIXEL_ACTIVITY, HearthGuardColors::HEARTHGUARD_RED, 
                                                     LedAnimation::SOLID, 0);
                        feedbackManager.playFailure();
                        break;
                    case 4:
                        #ifdef DEBUG
                        Serial.println("[DEMO] Turning off all LEDs");
                        #endif
                        feedbackManager.turnOffLeds();
                        demoStep = -1; // Will become 0 after increment
                        break;
                }
                demoStep++;
                lastDemoUpdate = millis();
            }
            break;
            
        default:
            // Shouldn't reach here in this version
            currentState = SystemState::BOOTING;
            break;
    }
    
    // Small delay for stability  
    delay(1);
}
