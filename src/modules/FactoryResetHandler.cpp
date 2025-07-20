#include "modules/FactoryResetHandler.h"
#include "Config.h"
#include <Preferences.h>

FactoryResetHandler::FactoryResetHandler(LEDController* sysLED, BuzzerController* buzz) 
    : systemLED(sysLED), buzzer(buzz), currentState(FactoryResetState::IDLE),
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
    unsigned long currentTime = millis();
    
    // Read button state
    bool buttonPressed = (digitalRead(Config::FACTORY_RESET_PIN) == LOW);
    
    // State machine for factory reset process
    switch (currentState) {
        case FactoryResetState::IDLE:
            // Check if button was just pressed
            if (buttonPressed && !lastButtonState) {
                Serial.println("Factory reset button pressed - starting 5-second timer");
                currentState = FactoryResetState::BUTTON_PRESSED;
                buttonPressStartTime = currentTime;
                lastBlinkTime = currentTime;
                blinkState = false;
            }
            break;
            
        case FactoryResetState::BUTTON_PRESSED:
            if (!buttonPressed) {
                // Button released before 5 seconds - cancel reset
                Serial.println("Factory reset cancelled - button released");
                currentState = FactoryResetState::IDLE;
                systemLED->setOff(); // Turn off any blinking
            } else if (currentTime - buttonPressStartTime >= Config::FACTORY_RESET_HOLD_TIME_MS) {
                // 5 seconds reached - initiate reset confirmation
                Serial.println("Factory reset 5-second hold complete - starting confirmation");
                currentState = FactoryResetState::CONFIRMING_RESET;
                confirmationStartTime = currentTime;
                confirmationFlashCount = 0;
                performFactoryReset();
            }
            break;
            
        case FactoryResetState::CONFIRMING_RESET:
            // Handle confirmation flash sequence
            if (confirmationFlashCount < Config::FACTORY_RESET_CONFIRM_FLASHES * 2) {
                if (currentTime - confirmationStartTime >= (confirmationFlashCount + 1) * 150) {
                    if (confirmationFlashCount % 2 == 0) {
                        // Turn on green LED
                        systemLED->setColor(Config::Colors::SUCCESS_GREEN);
                    } else {
                        // Turn off LED
                        systemLED->setOff();
                    }
                    confirmationFlashCount++;
                }
            } else {
                // Confirmation complete - trigger reboot
                Serial.println("Factory reset complete - rebooting...");
                currentState = FactoryResetState::RESET_COMPLETE;
                delay(500); // Brief delay to ensure serial output
                ESP.restart();
            }
            break;
            
        case FactoryResetState::RESET_COMPLETE:
            // Should never reach here due to reboot
            break;
    }
    
    // Update feedback based on current state
    updateFeedback();
    
    lastButtonState = buttonPressed;
}

bool FactoryResetHandler::isButtonPressed() {
    bool pressed = digitalRead(Config::FACTORY_RESET_PIN) == LOW;
    return pressed;
}

void FactoryResetHandler::updateButtonState() {
    // Simplified for debugging
}

void FactoryResetHandler::updateFeedback() {
    unsigned long currentTime = millis();
    
    switch (currentState) {
        case FactoryResetState::BUTTON_PRESSED:
            // Rapid blinking red LED during hold
            if (currentTime - lastBlinkTime >= Config::FACTORY_RESET_BLINK_INTERVAL) {
                blinkState = !blinkState;
                if (blinkState) {
                    systemLED->setColor(Config::Colors::DANGER_RED);
                } else {
                    systemLED->setOff();
                }
                lastBlinkTime = currentTime;
                
                // Log progress every second
                unsigned long elapsedTime = currentTime - buttonPressStartTime;
                if (elapsedTime % 1000 < Config::FACTORY_RESET_BLINK_INTERVAL) {
                    Serial.println("Factory reset progress: " + String(elapsedTime / 1000) + "/5 seconds");
                }
            }
            break;
            
        case FactoryResetState::IDLE:
        case FactoryResetState::CONFIRMING_RESET:
        case FactoryResetState::RESET_COMPLETE:
            // Feedback handled in main state machine
            break;
    }
}

void FactoryResetHandler::performFactoryReset() {
    Serial.println("Performing factory reset...");
    
    // Play reset confirmation chime
    if (buzzer) {
        buzzer->playResetChime();
    }
    
    // Clear all stored preferences - must match WiFiManager namespace
    Preferences prefs;
    prefs.begin("thescout", false);  // Match WiFiManager's namespace
    prefs.clear();
    prefs.end();
    
    Serial.println("All preferences cleared from 'thescout' namespace");
    Serial.println("Device will reboot to captive portal mode");
}
