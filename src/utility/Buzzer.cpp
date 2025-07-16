#include "utility/Buzzer.h"
#include <Arduino.h>

void Buzzer::begin() {
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);
}

void Buzzer::handle() {
    unsigned long now = millis();
    if (currentChime != BuzzerChime::NONE) {
        // Example: play chime for 500ms
        if (now - chimeStartTime < 500) {
            digitalWrite(buzzerPin, HIGH);
        } else {
            digitalWrite(buzzerPin, LOW);
            currentChime = BuzzerChime::NONE;
        }
    }
}

void Buzzer::playChime(BuzzerChime chime) {
    currentChime = chime;
    chimeStartTime = millis();
    // Add pattern logic for each chime type if needed
}
