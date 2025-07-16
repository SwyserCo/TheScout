#include "setup/FactoryResetHandler.h"
#include <Preferences.h>

void FactoryResetHandler::begin() {
    pinMode(Config::FACTORY_RESET_BTN, INPUT_PULLUP);
    buttonPressed = false;
    buttonPressStart = 0;
}

void FactoryResetHandler::update() {
    checkButton();
}

void FactoryResetHandler::checkButton() {
    bool currentState = digitalRead(Config::FACTORY_RESET_BTN) == LOW;
    
    // Button just pressed
    if (currentState && !buttonPressed) {
        buttonPressed = true;
        buttonPressStart = millis();
    }
    // Button released
    else if (!currentState && buttonPressed) {
        buttonPressed = false;
        buttonPressStart = 0;
    }
    // Check for long press
    else if (buttonPressed) {
        if (millis() - buttonPressStart >= RESET_HOLD_TIME) {
            performReset();
        }
    }
}

void FactoryResetHandler::performReset() {
    Preferences prefs;
    prefs.begin("scout-config", false);
    prefs.clear();
    prefs.end();
    
    if (resetCallback) {
        resetCallback();
    }
    
    ESP.restart();
}
