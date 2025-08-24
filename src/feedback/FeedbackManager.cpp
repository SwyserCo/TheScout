#include "feedback/FeedbackManager.h"

void FeedbackManager::begin() {
    #ifdef DEBUG
    Serial.println("[FEEDBACK] Initializing FeedbackManager...");
    #endif
    
    // Load saved settings first
    loadSettings();
    
    // Initialize sub-controllers
    ledController.begin();
    buzzerController.begin();
    
    // Apply loaded settings
    ledController.setBrightness(currentBrightness);
    ledController.setStealthMode(stealthMode);
    buzzerController.setStealthMode(stealthMode);
    
    #ifdef DEBUG
    Serial.printf("[FEEDBACK] Loaded settings - Brightness: %d, Stealth: %s\n", 
                  currentBrightness, stealthMode ? "ON" : "OFF");
    Serial.println("[FEEDBACK] FeedbackManager initialization complete");
    #endif
}

void FeedbackManager::update() {
    ledController.update();
    buzzerController.update();
}

void FeedbackManager::setBrightness(uint8_t brightness) {
    currentBrightness = brightness;
    ledController.setBrightness(brightness);
    
    // Automatically save settings
    saveSettings();
    
    #ifdef DEBUG
    Serial.printf("[FEEDBACK] Brightness set to %d and saved\n", brightness);
    #endif
}

void FeedbackManager::startAnimation(uint8_t pixelIndex, HearthGuardColors::RGBColor color, LedAnimation animation, uint16_t interval) {
    ledController.startAnimation(pixelIndex, color, animation, interval);
}

void FeedbackManager::turnOffLeds() {
    ledController.turnOff();
}

void FeedbackManager::playSuccess() {
    buzzerController.playSuccess();
}

void FeedbackManager::playFailure() {
    buzzerController.playFailure();
}

void FeedbackManager::playConfirm() {
    buzzerController.playConfirm();
}

void FeedbackManager::playInteraction() {
    buzzerController.playInteraction();
}

void FeedbackManager::setStealthMode(bool enabled) {
    stealthMode = enabled;
    
    // Apply to both controllers
    ledController.setStealthMode(enabled);
    buzzerController.setStealthMode(enabled);
    
    // Automatically save settings
    saveSettings();
    
    #ifdef DEBUG
    Serial.printf("[FEEDBACK] Stealth mode %s and saved\n", enabled ? "enabled" : "disabled");
    #endif
}

void FeedbackManager::loadSettings() {
    bool success = preferences.begin(SETTINGS_NAMESPACE, true); // Read-only mode
    
    if (!success) {
        #ifdef DEBUG
        Serial.println("[FEEDBACK] NVS namespace not found, using defaults (normal on first boot)");
        #endif
        // Use default values
        currentBrightness = 255;
        stealthMode = false;
        return;
    }
    
    // Load brightness (default 255 if not found)
    currentBrightness = preferences.getUChar(BRIGHTNESS_KEY, 255);
    
    // Load stealth mode (default false if not found)
    // TEMPORARILY FORCE STEALTH MODE OFF FOR TESTING
    stealthMode = false; // Force stealth mode off for debugging
    // stealthMode = preferences.getBool(STEALTH_KEY, false);
    
    preferences.end();
    
    #ifdef DEBUG
    Serial.printf("[FEEDBACK] Settings loaded - Brightness: %d, Stealth: %s (FORCED OFF)\n", 
                  currentBrightness, stealthMode ? "ON" : "OFF");
    #endif
}

void FeedbackManager::saveSettings() {
    bool success = preferences.begin(SETTINGS_NAMESPACE, false); // Read-write mode
    
    if (!success) {
        #ifdef DEBUG
        Serial.println("[FEEDBACK] Failed to open NVS for writing");
        #endif
        return;
    }
    
    preferences.putUChar(BRIGHTNESS_KEY, currentBrightness);
    preferences.putBool(STEALTH_KEY, stealthMode);
    
    preferences.end();
    
    #ifdef DEBUG
    Serial.println("[FEEDBACK] Settings saved to NVS");
    #endif
}
