#include "feedback/FeedbackManager.h"
#include "config/Pins.h"
#include "config/Settings.h"

FeedbackManager::FeedbackManager() 
    : ledController(),
      buzzerController(BUZZER_PIN, BUZZER_LEDC_CHANNEL),
      currentBrightness(DEFAULT_LED_BRIGHTNESS),
      currentStealthMode(false) {
}

void FeedbackManager::begin() {
    Serial.println("[FEEDBACK] Initializing Feedback Manager...");
    
    // Initialize Preferences
    preferences.begin(PREF_NAMESPACE, false);
    
    // Load persistent settings
    loadSettings();
    
    // Initialize controllers
    ledController.begin();
    buzzerController.begin();
    
    // Apply loaded settings
    ledController.setBrightness(currentBrightness);
    ledController.setStealthMode(currentStealthMode);
    buzzerController.setStealthMode(currentStealthMode);
    
    Serial.println("[FEEDBACK] Feedback Manager initialized successfully");
    printStatus();
}

void FeedbackManager::update() {
    ledController.update();
    buzzerController.update();
}

void FeedbackManager::loadSettings() {
    currentBrightness = preferences.getUChar(PREF_BRIGHTNESS, DEFAULT_LED_BRIGHTNESS);
    currentStealthMode = preferences.getBool(PREF_STEALTH_MODE, false);
    
    Serial.printf("[FEEDBACK] Settings loaded - Brightness: %d, Stealth: %s\n",
                  currentBrightness, currentStealthMode ? "ON" : "OFF");
}

void FeedbackManager::saveSettings() {
    preferences.putUChar(PREF_BRIGHTNESS, currentBrightness);
    preferences.putBool(PREF_STEALTH_MODE, currentStealthMode);
    
    Serial.printf("[FEEDBACK] Settings saved - Brightness: %d, Stealth: %s\n",
                  currentBrightness, currentStealthMode ? "ON" : "OFF");
}

void FeedbackManager::setBrightness(uint8_t brightness) {
    currentBrightness = brightness;
    ledController.setBrightness(brightness);
    saveSettings();
}

void FeedbackManager::setStealthMode(bool enabled) {
    currentStealthMode = enabled;
    ledController.setStealthMode(enabled);
    buzzerController.setStealthMode(enabled);
    saveSettings();
}

uint8_t FeedbackManager::getBrightness() const {
    return currentBrightness;
}

bool FeedbackManager::getStealthMode() const {
    return currentStealthMode;
}

// LED Controller delegation methods
void FeedbackManager::setPixelColor(uint8_t pixelIndex, uint32_t color) {
    ledController.setPixelColor(pixelIndex, color);
}

void FeedbackManager::setSystemLed(uint32_t color) {
    ledController.setSystemLed(color);
}

void FeedbackManager::setActivityLed(uint32_t color) {
    ledController.setActivityLed(color);
}

void FeedbackManager::blinkPixel(uint8_t pixelIndex, uint32_t color, uint16_t interval) {
    ledController.blinkPixel(pixelIndex, color, interval);
}

void FeedbackManager::pulsePixel(uint8_t pixelIndex, uint32_t color, uint16_t speed) {
    ledController.pulsePixel(pixelIndex, color, speed);
}

void FeedbackManager::blinkSystem(uint32_t color, uint16_t interval) {
    ledController.blinkSystem(color, interval);
}

void FeedbackManager::blinkActivity(uint32_t color, uint16_t interval) {
    ledController.blinkActivity(color, interval);
}

void FeedbackManager::pulseSystem(uint32_t color, uint16_t speed) {
    ledController.pulseSystem(color, speed);
}

void FeedbackManager::pulseActivity(uint32_t color, uint16_t speed) {
    ledController.pulseActivity(color, speed);
}

void FeedbackManager::turnOffLeds() {
    ledController.turnOff();
}

void FeedbackManager::turnOffPixel(uint8_t pixelIndex) {
    ledController.turnOffPixel(pixelIndex);
}

void FeedbackManager::turnOffSystem() {
    ledController.turnOffSystem();
}

void FeedbackManager::turnOffActivity() {
    ledController.turnOffActivity();
}

bool FeedbackManager::isSystemOn() const {
    return ledController.isSystemOn();
}

bool FeedbackManager::isActivityOn() const {
    return ledController.isActivityOn();
}

// Buzzer Controller delegation methods
void FeedbackManager::playSuccessChime() {
    buzzerController.playSuccessChime();
}

void FeedbackManager::playFailureTone() {
    buzzerController.playFailureTone();
}

void FeedbackManager::playResetChime() {
    buzzerController.playResetChime();
}

void FeedbackManager::playPendingChime() {
    buzzerController.playPendingChime();
}

void FeedbackManager::playQuestionChime() {
    buzzerController.playQuestionChime();
}

void FeedbackManager::playWarningChime() {
    buzzerController.playWarningChime();
}

void FeedbackManager::playAlertChime() {
    buzzerController.playAlertChime();
}

void FeedbackManager::playWelcomeChime() {
    buzzerController.playWelcomeChime();
}

void FeedbackManager::playFarewellChime() {
    buzzerController.playFarewellChime();
}

void FeedbackManager::playTone(uint16_t frequency, uint16_t duration) {
    buzzerController.playTone(frequency, duration);
}

void FeedbackManager::playCustomSequence(const ToneNote* sequence, uint8_t length) {
    buzzerController.playCustomSequence(sequence, length);
}

void FeedbackManager::stopBuzzer() {
    buzzerController.stop();
}

bool FeedbackManager::isBuzzerPlaying() const {
    return buzzerController.isPlaying();
}

// Combined feedback methods
void FeedbackManager::showSuccess() {
    Serial.println("[FEEDBACK] Showing success feedback - Victory achieved!");
    setSystemLed(GUARDIAN_GREEN);
    playSuccessChime();
}

void FeedbackManager::showFailure() {
    Serial.println("[FEEDBACK] Showing failure feedback - Quest unsuccessful");
    blinkSystem(GUARDIAN_RED, 200);
    playFailureTone();
}

void FeedbackManager::showWarning() {
    Serial.println("[FEEDBACK] Showing warning feedback - Watchman's alert");
    blinkSystem(GUARDIAN_ORANGE, 300);
    playWarningChime();
}

void FeedbackManager::showAlert() {
    Serial.println("[FEEDBACK] Showing alert feedback - Battle horn sounds!");
    blinkSystem(GUARDIAN_RED, 100);
    blinkActivity(GUARDIAN_RED, 150);
    playAlertChime();
}

void FeedbackManager::showPending() {
    Serial.println("[FEEDBACK] Showing pending feedback - Mystery unfolds");
    pulseSystem(GUARDIAN_BLUE, 40);
    playPendingChime();
}

void FeedbackManager::showQuestion() {
    Serial.println("[FEEDBACK] Showing question feedback - Curious inquiry");
    pulseActivity(GUARDIAN_ORANGE, 30);
    playQuestionChime();
}

void FeedbackManager::showActivity() {
    Serial.println("[FEEDBACK] Showing activity feedback");
    pulseActivity(GUARDIAN_BLUE, 30);
}

void FeedbackManager::showReset() {
    Serial.println("[FEEDBACK] Showing reset feedback - Sacred bells toll");
    blinkSystem(GUARDIAN_ORANGE, 100);
    blinkActivity(GUARDIAN_ORANGE, 100);
    playResetChime();
}

void FeedbackManager::showWelcome() {
    Serial.println("[FEEDBACK] Showing welcome feedback - Tavern keeper's greeting");
    setSystemLed(GUARDIAN_GREEN);
    pulseActivity(GUARDIAN_BLUE, 20);
    playWelcomeChime();
}

void FeedbackManager::showFarewell() {
    Serial.println("[FEEDBACK] Showing farewell feedback - Peaceful blessing");
    pulseSystem(GUARDIAN_GREEN, 60);
    pulseActivity(GUARDIAN_BLUE, 80);
    playFarewellChime();
}

void FeedbackManager::turnOffAll() {
    Serial.println("[FEEDBACK] Turning off all feedback");
    turnOffLeds();
    stopBuzzer();
}

void FeedbackManager::printStatus() {
    Serial.println("=== FEEDBACK MANAGER STATUS ===");
    Serial.printf("Brightness: %d/255\n", currentBrightness);
    Serial.printf("Stealth Mode: %s\n", currentStealthMode ? "ENABLED" : "DISABLED");
    Serial.printf("System LED: %s\n", isSystemOn() ? "ON" : "OFF");
    Serial.printf("Activity LED: %s\n", isActivityOn() ? "ON" : "OFF");
    Serial.printf("Buzzer: %s\n", isBuzzerPlaying() ? "PLAYING" : "IDLE");
    Serial.println("==============================");
}
