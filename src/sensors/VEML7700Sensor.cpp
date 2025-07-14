#include "sensors/VEML7700Sensor.h"

bool VEML7700Sensor::begin() {
    return veml.begin();
}

float VEML7700Sensor::readLux() {
    return veml.readLux();
}
