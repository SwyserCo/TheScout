#include "sensors/VEML7700.h"

void VEML7700::begin() {
    // Initialize VEML7700 sensor
}

void VEML7700::read() {
    // Read sensor value (stub)
    ambientLight = 123.4f;
}

float VEML7700::getAmbientLight() const { return ambientLight; }
