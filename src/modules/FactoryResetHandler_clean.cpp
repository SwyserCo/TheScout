#include "modules/FactoryResetHandler.h"
#include "Config.h"
#include <Preferences.h>

FactoryResetHandler::FactoryResetHandler(LEDController* sysLED, LEDController* actLED, Feedback* fb) 
    : systemLED(sysLED), activityLED(actLED), feedback(fb), currentState(FactoryResetState::IDLE),
      buttonPressStartTime(0), lastButtonState(false), lastBlinkTime(0), blinkState(false),
      confirmationStartTime(0), confirmationFlashCount(0) {
}

void FactoryResetHandler::begin() {
    Serial.println("FactoryResetHandler: Initializing...");
    Serial.flush();
    
    // Configure factory reset button as input (external pull-up resistor present)
    pinMode(Config::FACTORY_RESET_PIN, INPUT);
    
    Serial.println("Factory reset button configured on pin " + String(Config::FACTORY_RESET_PIN) + " with external pull-up");
    Serial.flush();
    
    // Test initial button state
    bool initialButtonState = digitalRead(Config::FACTORY_RESET_PIN);
    Serial.println("Initial button state: " + String(initialButtonState ? "HIGH (released)" : "LOW (pressed)"));
    Serial.flush();
    
    Serial.println("=== FACTORY RESET HANDLER INITIALIZED ===");
    Serial.flush();
}

void FactoryResetHandler::update() {
    static unsigned long lastHeartbeat = 0;
    static unsigned long buttonHoldStart = 0;
    unsigned long currentTime = millis();
    
    // Heartbeat every 1 second - alternating blue/off
    if (currentTime - lastHeartbeat >= 1000) {
        static bool heartbeatState = false;
        heartbeatState = !heartbeatState;
        
        if (heartbeatState) {
            systemLED->setColor(Config::Colors::CASTLE_BLUE);
            Serial.println("=== BLUE HEARTBEAT ===");
        } else {
            systemLED->setOff();
            Serial.println("=== HEARTBEAT OFF ===");
        }
        lastHeartbeat = currentTime;
        Serial.flush();
    }
    
    // Direct button reading with immediate response
    bool buttonPressed = (digitalRead(Config::FACTORY_RESET_PIN) == LOW);
    
    if (buttonPressed) {
        if (buttonHoldStart == 0) {
            buttonHoldStart = currentTime;
            Serial.println("*** BUTTON PRESS DETECTED - TURNING RED ***");
            Serial.flush();
        }
        
        // Turn red immediately and keep it red while pressed
        systemLED->setColor(Config::Colors::DANGER_RED);
        Serial.println("Button held for: " + String(currentTime - buttonHoldStart) + "ms");
        Serial.flush();
        
    } else {
        if (buttonHoldStart > 0) {
            Serial.println("*** BUTTON RELEASED - TURNING OFF ***");
            buttonHoldStart = 0;
            Serial.flush();
        }
        // Button not pressed - don't interfere with heartbeat
    }
}

bool FactoryResetHandler::isButtonPressed() {
    bool pressed = digitalRead(Config::FACTORY_RESET_PIN) == LOW;
    return pressed;
}

void FactoryResetHandler::updateButtonState() {
    // Simplified for debugging
}

void FactoryResetHandler::updateFeedback() {
    // Simplified for debugging  
}

void FactoryResetHandler::performFactoryReset() {
    Serial.println("Performing factory reset...");
    
    // Clear all stored preferences
    Preferences prefs;
    prefs.begin("scout", false);
    prefs.clear();
    prefs.end();
    
    Serial.println("All preferences cleared");
    
    // Play reset chime through feedback system
    if (feedback) {
        feedback->playResetChime();
    }
    
    Serial.println("Factory reset complete - device will reboot");
}
