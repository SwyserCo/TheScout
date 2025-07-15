#include "utility/BuzzerController.h"

BuzzerController::BuzzerController() {}

void BuzzerController::begin() {
    ledcSetup(PWM_CHANNEL, 2000, 8); // Default 2kHz, 8-bit resolution
    ledcAttachPin(BUZZER_PIN, PWM_CHANNEL);
}

void BuzzerController::beep(uint16_t frequency, uint16_t duration, uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        ledcWriteTone(PWM_CHANNEL, frequency);
        delay(duration);
        ledcWriteTone(PWM_CHANNEL, 0); // Stop tone
        if (i < count - 1) {
            delay(duration); // Pause between beeps
        }
    }
}

void BuzzerController::stop() {
    ledcWriteTone(PWM_CHANNEL, 0);
}
