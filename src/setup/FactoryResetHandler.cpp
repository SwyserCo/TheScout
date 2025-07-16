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
    unsigned long currentTime = millis();
    
    // Button just pressed
    if (currentState && !buttonPressed) {
        buttonPressed = true;
        buttonPressStart = currentTime;
        if (resetCallback) {
            resetCallback(FactoryResetStage::START);
        }
    }
    // Button released
    else if (!currentState && buttonPressed) {
        buttonPressed = false;
        buttonPressStart = 0;
        if (resetCallback) {
            resetCallback(FactoryResetStage::CANCELLED);
        }
    }
    // Check for long press and provide feedback
    else if (buttonPressed) {
        unsigned long pressDuration = currentTime - buttonPressStart;
        if (pressDuration >= RESET_HOLD_TIME) {
            performReset();
        } else {
            resetCallback(FactoryResetStage::IN_PROGRESS);
        }
    }
}

void FactoryResetHandler::performReset() {
    if (resetCallback) {
        resetCallback(FactoryResetStage::IN_PROGRESS);
    }

    Preferences prefs;
    prefs.begin("scout-config", false);
    prefs.clear();
    prefs.end();
    
    if (resetCallback) {
        resetCallback(FactoryResetStage::COMPLETE);
    }
    
    delay(1000);  // Give time for the completion feedback
    ESP.restart();
}
