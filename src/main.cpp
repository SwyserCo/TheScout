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
    // === EMERGENCY HARDWARE OVERRIDE ===
    // Try to prevent white LED with aggressive hardware configuration
    
    // Configure pins as outputs with pulldown before anything else
    pinMode(3, OUTPUT);   
    pinMode(45, OUTPUT);  
    digitalWrite(3, LOW);
    digitalWrite(45, LOW);
    
    // Try setting pin modes with pulldown
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << 3) | (1ULL << 45);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    
    // Force LOW using both methods
    gpio_set_level(GPIO_NUM_3, 0);
    gpio_set_level(GPIO_NUM_45, 0);
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
    Serial.println("    HearthGuard: The Scout (HYBRID)  ");
    Serial.println("   Medieval Themed IoT HearthGuard   ");
    Serial.println("=====================================");
    Serial.printf("Device: %s v%s\n", DEVICE_NAME, FIRMWARE_VERSION);
    Serial.printf("Build: %s %s\n", __DATE__, __TIME__);
    Serial.println("=====================================");
    Serial.println("[BOOT] HYBRID MODE: GPIO control first, then NeoPixel");
    Serial.println("[BOOT] LEDs will be GPIO-controlled for 5 seconds");
    Serial.println("[BOOT] This prevents white LED at startup");
    Serial.println();
    #endif
    
    currentState = SystemState::BOOTING;
    lastStateChange = millis();
}

void loop() {
    unsigned long currentTime = millis();
    
    switch (currentState) {
        case SystemState::BOOTING:
            // DIAGNOSTIC GPIO MODE for first 5 seconds
            if (currentTime - bootStartTime < 5000) {
                // Keep forcing LEDs OFF with multiple methods
                gpio_set_level(GPIO_NUM_3, 0);
                gpio_set_level(GPIO_NUM_45, 0);
                digitalWrite(3, LOW);
                digitalWrite(45, LOW);
                
                // Status updates
                static unsigned long lastBootMsg = 0;
                if (currentTime - lastBootMsg > 1000) {
                    #ifdef DEBUG
                    Serial.printf("[BOOT] GPIO mode active... %d/5 seconds\n", 
                                 (int)((currentTime - bootStartTime) / 1000) + 1);
                    Serial.println("[BOOT] Activity LED should be OFF - using gpio_set_level + digitalWrite");
                    Serial.flush();
                    #endif
                    lastBootMsg = currentTime;
                }
                return; // Stay in this mode
            }
            
            // After 5 seconds, initialize NeoPixel system
            if (!managersInitialized) {
                #ifdef DEBUG
                Serial.println("[BOOT] === SWITCHING TO NEOPIXEL MODE ===");
                Serial.println("[BOOT] GPIO control worked - now initializing NeoPixel");
                Serial.println("[BOOT] Initializing all subsystems...");
                Serial.flush();
                #endif
                
                // Initialize all manager classes
                feedbackManager.begin();
                
                // Immediately force GPIO override after NeoPixel init
                digitalWrite(3, LOW);
                digitalWrite(45, LOW);
                delay(100);
                
                wifiHandler.begin();  
                deviceManager.begin();
                sensorManager.begin();
                mqttHandler.begin();
                
                // Final GPIO override
                digitalWrite(3, LOW);
                digitalWrite(45, LOW);
                delay(100);
                
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
