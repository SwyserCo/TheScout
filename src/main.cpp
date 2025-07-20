#include <Arduino.h>
#include "feedback/FeedbackController.h"
#include "config/Settings.h"
#include "config/Pins.h"

// Create feedback controller instance
FeedbackController feedback;

// Demo timing variables
unsigned long lastDemoAction = 0;
int demoStep = 0;
const uint16_t DEMO_INTERVAL = 3000;  // 3 seconds between demo steps

void setup() {
    // Initialize serial communication
    Serial.begin(Settings::SERIAL_BAUD_RATE);
    delay(Settings::STARTUP_DELAY);
    
    Serial.println(Settings::Theme::STARTUP_MESSAGE);
    Serial.println("Phase 1: User Feedback Modules Demo");
    Serial.println("Testing LED patterns and medieval-themed buzzer chimes...");
    Serial.printf("LED Brightness: %d/255, Stealth Mode: %s\n", 
                  Settings::LED::DEFAULT_BRIGHTNESS, 
                  Settings::Mode::STEALTH_MODE ? "ENABLED" : "DISABLED");
    
    // Initialize feedback controller
    feedback.begin();
    
    // Demonstrate brightness control
    feedback.setBrightness(Settings::LED::DEFAULT_BRIGHTNESS);
    
    // Play success chime to indicate startup
    feedback.playSuccessChime();
    
    Serial.println("Initialization complete. Starting demo sequence...");
    Serial.println("System LED (IO03) and Activity LED (IO45) patterns:");
}

void loop() {
    // CRITICAL: Must call update() every loop for non-blocking operation
    feedback.update();
    
    // Demo sequence - cycle through all feedback functions
    if (millis() - lastDemoAction >= DEMO_INTERVAL) {
        lastDemoAction = millis();
        
        switch (demoStep) {
            case 0:
                Serial.println("Demo Step 1: System LED - Solid Red");
                feedback.on(Pins::SYSTEM_LED, CRGB::Red);
                break;
                
            case 1:
                Serial.println("Demo Step 2: Activity LED - Blinking Blue (500ms)");
                feedback.blink(Pins::ACTIVITY_LED, 500, CRGB::Blue);
                break;
                
            case 2:
                Serial.println("Demo Step 3: System LED - Pulsing Green");
                feedback.pulse(Pins::SYSTEM_LED, 30, CRGB::Green);
                break;
                
            case 3:
                Serial.println("Demo Step 4: Playing Success Chime (C5 -> G5)");
                feedback.playSuccessChime();
                break;
                
            case 4:
                Serial.println("Demo Step 5: Setup Mode Pattern (Pulsing for Phase 2)");
                feedback.setSetupMode();
                break;
                
            case 5:
                Serial.println("Demo Step 6: Connecting Mode Pattern (Blinking for Phase 2)");
                feedback.setConnecting();
                break;
                
            case 6:
                Serial.println("Demo Step 7: Playing Failure Tone (G4 -> C4)");
                feedback.playFailureTone();
                break;
                
            case 7:
                Serial.println("Demo Step 8: Success Mode Pattern (Solid for Phase 2)");
                feedback.setSuccess();
                break;
                
            case 8:
                Serial.println("Demo Step 9: Activity LED - Pulsing Orange");
                feedback.pulse(Pins::ACTIVITY_LED, 25, CRGB::Orange);
                break;
                
            case 9:
                Serial.println("Demo Step 10: Playing Reset Chime (A4)");
                feedback.playResetChime();
                break;
                
            case 10:
                Serial.println("Demo Step 11: Turning off all LEDs");
                feedback.off(Pins::SYSTEM_LED);
                feedback.off(Pins::ACTIVITY_LED);
                break;
                
            case 11:
                Serial.println("Demo Step 12: Testing Stealth Mode");
                feedback.setStealthMode(true);
                feedback.on(Pins::SYSTEM_LED, CRGB::Red);  // Should not show due to stealth
                feedback.playSuccessChime();               // Should not play due to stealth
                break;
                
            case 12:
                Serial.println("Demo Step 13: Disabling Stealth Mode");
                feedback.setStealthMode(false);
                feedback.on(Pins::ACTIVITY_LED, CRGB::Purple);  // Should now show
                break;
                
            default:
                // Reset demo sequence
                demoStep = -1;
                Serial.println("Demo sequence complete! Restarting...");
                Serial.println("=== The Scout Phase 1 - All functions tested ===");
                break;
        }
        
        demoStep++;
    }
    
    // Small delay to prevent watchdog issues
    delay(Settings::LOOP_DELAY);
}