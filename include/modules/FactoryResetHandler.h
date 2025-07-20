#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "modules/Feedback.h"

enum class FactoryResetState {
    IDLE,               // Button not pressed
    BUTTON_PRESSED,     // Button pressed, timing started
    CONFIRMING_RESET,   // 10 seconds reached, showing confirmation
    RESET_COMPLETE      // Reset performed, ready to reboot
};

class FactoryResetHandler {
private:
    LEDController* systemLED;
    BuzzerController* buzzer;
    Preferences preferences;
    
    FactoryResetState currentState;
    unsigned long buttonPressStartTime;
    unsigned long lastBlinkTime;
    unsigned long confirmationStartTime;
    bool blinkState;
    uint8_t confirmationFlashCount;
    bool lastButtonState;
    
    // Private methods
    void updateButtonState();
    void updateFeedback();
    void performFactoryReset();
    bool isButtonPressed();
    
public:
    FactoryResetHandler(LEDController* sysLED, BuzzerController* buzz);
    void begin();
    void update();  // Non-blocking update - call in main loop()
    
    // Getters for debugging/status
    FactoryResetState getState() const { return currentState; }
    bool isResetInProgress() const { return currentState != FactoryResetState::IDLE; }
};
