#pragma once

/**
 * @file Ld2410sSensor.h
 * @brief LD2410S radar sensor interface
 */

#include <Arduino.h>
#include "config/DataTypes.h"

/**
 * @class Ld2410sSensor
 * @brief Interface for LD2410S 24GHz mmWave radar sensor
 * 
 * This class wraps the LD2410 library and provides a clean interface
 * for radar-based motion and presence detection.
 */
class Ld2410sSensor {
public:
    /**
     * @brief Constructor
     */
    Ld2410sSensor();

    /**
     * @brief Initialize LD2410S sensor
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Update radar sensor reading (non-blocking)
     * 
     * This method should be called regularly from the main loop
     * to read sensor data and update detection status.
     */
    void update();

    /**
     * @brief Get current radar sensor data
     * @return Radar sensor data structure
     */
    RadarData getData();

    /**
     * @brief Check if moving target is detected
     * @return true if moving target detected, false otherwise
     */
    bool isMovingTargetDetected();

    /**
     * @brief Check if stationary target is detected
     * @return true if stationary target detected, false otherwise
     */
    bool isStationaryTargetDetected();

private:
    RadarData sensorData;
    unsigned long lastUpdate;
    
    // Private helper methods will be implemented in Phase 4
};
