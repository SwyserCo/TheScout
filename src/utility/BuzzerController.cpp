#include "utility/BuzzerController.h"

BuzzerController::BuzzerController() {}

void BuzzerController::begin() {
    ledcSetup(PWM_CHANNEL, 2000, 8); // 2kHz, 8-bit resolution
    ledcAttachPin(BUZZER_PIN, PWM_CHANNEL);
}

void BuzzerController::beep(uint16_t frequency, uint16_t duration) {
    ledcWriteTone(PWM_CHANNEL, frequency);
    delay(duration);
    ledcWriteTone(PWM_CHANNEL, 0); // Stop tone
}

void BuzzerController::stop() {
    ledcWriteTone(PWM_CHANNEL, 0);
}
