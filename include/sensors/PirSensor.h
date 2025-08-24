#pragma once

/**
 * @file PirSensor.h
 * @brief PIR motion sensor interface
 */

#include <Arduino.h>
#include "config/DataTypes.h"

/**
 * @class PirSensor
 * @brief Interface for PIR (Passive Infrared) motion sensor
 * 
 * This class provides a clean interface for reading PIR sensor
 * and managing motion detection logic.
 */
class PirSensor {
public:
    /**
     * @brief Constructor
     */
    PirSensor();

    /**
     * @brief Initialize PIR sensor
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Update PIR sensor reading (non-blocking)
     * 
     * This method should be called regularly from the main loop
     * to read sensor state and update motion detection.
     */
    void update();

    /**
     * @brief Get current PIR sensor data
     * @return PIR sensor data structure
     */
    PirData getData();

    /**
     * @brief Check if motion is currently detected
     * @return true if motion detected, false otherwise
     */
    bool isMotionDetected();

private:
    PirData sensorData;
    bool lastState;
    unsigned long lastUpdate;
    
    // Private helper methods will be implemented in Phase 4
};
