#include <Arduino.h>
#include "config/Settings.h"
#include "config/Pins.h"
#include "feedback/FeedbackManager.h"

// The Scout - Phase 1 Implementation
// Medieval themed Guardian IoT device
// Modular feedback system with shorter, better chimes

FeedbackManager feedbackManager;

void setup() {
    Serial.begin(115200);
    delay(1000);  // Only keep this initial delay for serial stability
    
    Serial.println("=====================================");
    Serial.println("      The Scout - Guardian Device    ");
    Serial.println("   Phase 1: Modular Feedback System  ");
    Serial.println("=====================================");
    Serial.printf("Device: %s v%s\n", DEVICE_NAME, DEVICE_VERSION);
    Serial.println("Theme: Medieval Guardian");
    Serial.println("=====================================");
    
    // Initialize Feedback Manager
    feedbackManager.begin();
    
    Serial.println("\n[BOOT] The Scout is ready for duty!");
    feedbackManager.printStatus();
    Serial.println("\n[TEST] Medieval chimes will play during normal operation...");
}

void loop() {
    static unsigned long lastHeartbeat = 0;
    static unsigned long lastChimeTest = 0;
    static uint8_t heartbeatPhase = 0;
    static uint8_t chimeTestPhase = 0;
    
    // Update feedback controllers (non-blocking)
    feedbackManager.update();
    
    // Non-blocking chime testing every 5 seconds
    if (millis() - lastChimeTest > 5000) {
        lastChimeTest = millis();
        chimeTestPhase = (chimeTestPhase + 1) % 6;
        
        switch (chimeTestPhase) {
            case 0:
                Serial.println("[TEST] → Welcome chime");
                feedbackManager.playWelcomeChime();
                break;
            case 1:
                Serial.println("[TEST] → Success chime");
                feedbackManager.playSuccessChime();
                break;
            case 2:
                Serial.println("[TEST] → Question chime");
                feedbackManager.playQuestionChime();
                break;
            case 3:
                Serial.println("[TEST] → Warning chime");
                feedbackManager.playWarningChime();
                break;
            case 4:
                Serial.println("[TEST] → Alert chime");
                feedbackManager.playAlertChime();
                break;
            case 5:
                Serial.println("[TEST] → Farewell chime");
                feedbackManager.playFarewellChime();
                break;
        }
    }
    
    // Shorter heartbeat sequence every 8 seconds
    if (millis() - lastHeartbeat > 8000) {
        lastHeartbeat = millis();
        heartbeatPhase = (heartbeatPhase + 1) % 4;
        
        switch (heartbeatPhase) {
            case 0:
                Serial.println("[HEARTBEAT] Scout patrol - sector clear");
                feedbackManager.setSystemLed(GUARDIAN_GREEN);
                feedbackManager.playTone(NOTE_C4, TONE_VERY_SHORT);
                break;
                
            case 1:
                Serial.println("[HEARTBEAT] Scout patrol - investigating");
                feedbackManager.blinkActivity(GUARDIAN_BLUE, 150);
                break;
                
            case 2:
                Serial.println("[HEARTBEAT] Scout patrol - all quiet");
                feedbackManager.pulseActivity(GUARDIAN_GREEN, 30);
                break;
                
            case 3:
                Serial.println("[HEARTBEAT] Scout patrol - returning");
                feedbackManager.turnOffActivity();
                feedbackManager.playTone(NOTE_G4, TONE_VERY_SHORT);
                break;
        }
    }
    
    // Small non-blocking yield for stability
    yield();
}
