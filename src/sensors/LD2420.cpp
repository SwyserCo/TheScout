#include "sensors/LD2420.h"

void LD2420::begin() {
    // Initialize LD2420 sensor
}

void LD2420::read() {
    // Read presence (stub)
    presence = false;
}

bool LD2420::isPresenceDetected() const { return presence; }
