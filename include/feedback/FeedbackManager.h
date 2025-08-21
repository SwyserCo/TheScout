#ifndef FEEDBACK_MANAGER_H
#define FEEDBACK_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include "feedback/LedController.h"
#include "feedback/BuzzerController.h"

// The Scout - Feedback Manager
// Orchestrates LED and Buzzer controllers with persistent settings

class FeedbackManager {
private:
    LedController ledController;
    BuzzerController buzzerController;
    Preferences preferences;
    
    // Current settings
    uint8_t currentBrightness;
    bool currentStealthMode;
    
    // Helper methods
    void loadSettings();
    void saveSettings();
    
public:
    FeedbackManager();
    
    // Core lifecycle methods
    void begin();
    void update();  // Non-blocking - call in main loop
    
    // Global settings with persistence
    void setBrightness(uint8_t brightness);
    void setStealthMode(bool enabled);
    uint8_t getBrightness() const;
    bool getStealthMode() const;
    
    // LED Controller delegation methods
    void setPixelColor(uint8_t pixelIndex, uint32_t color);
    void setSystemLed(uint32_t color);
    void setActivityLed(uint32_t color);
    void blinkPixel(uint8_t pixelIndex, uint32_t color, uint16_t interval = 500);
    void pulsePixel(uint8_t pixelIndex, uint32_t color, uint16_t speed = 50);
    void blinkSystem(uint32_t color, uint16_t interval = 500);
    void blinkActivity(uint32_t color, uint16_t interval = 500);
    void pulseSystem(uint32_t color, uint16_t speed = 50);
    void pulseActivity(uint32_t color, uint16_t speed = 50);
    void turnOffLeds();
    void turnOffPixel(uint8_t pixelIndex);
    void turnOffSystem();
    void turnOffActivity();
    bool isSystemOn() const;
    bool isActivityOn() const;
    
    // Buzzer Controller delegation methods
    void playSuccessChime();
    void playFailureTone();
    void playResetChime();
    void playPendingChime();
    void playQuestionChime();
    void playWarningChime();
    void playAlertChime();
    void playWelcomeChime();
    void playFarewellChime();
    void playTone(uint16_t frequency, uint16_t duration);
    void playCustomSequence(const ToneNote* sequence, uint8_t length);
    void stopBuzzer();
    bool isBuzzerPlaying() const;
    
    // Combined feedback methods
    void showSuccess();       // Green LED + success chime
    void showFailure();       // Red LED + failure tone
    void showWarning();       // Orange LED + warning chime
    void showAlert();         // Red blinking + alert chime
    void showPending();       // Blue pulsing + pending chime
    void showQuestion();      // Orange + question chime
    void showActivity();      // Blue LED activity
    void showReset();         // Reset sequence with chime
    void showWelcome();       // Welcome sequence
    void showFarewell();      // Farewell sequence
    void turnOffAll();        // Turn off all feedback
    
    // Status methods
    void printStatus();
};

#endif // FEEDBACK_MANAGER_H
