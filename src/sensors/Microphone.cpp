#include "sensors/Microphone.h"

void Microphone::begin() {
    // Initialize microphone
}

void Microphone::read() {
    // Read sound level (stub)
    soundLevel = 60.0f;
}

float Microphone::getSoundLevel() const { return soundLevel; }
