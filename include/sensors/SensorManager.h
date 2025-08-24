#pragma once

/**
 * @file SensorManager.h
 * @brief Coordinates all sensor operations
 */

#include <Arduino.h>
#include "sensors/PirSensor.h"
#include "sensors/Ld2410sSensor.h"
#include "sensors/PowerStatus.h"
#include "config/DataTypes.h"

/**
 * @class SensorManager
 * @brief Central coordinator for all sensor operations
 * 
 * This class manages all sensors and provides a unified interface
 * for sensor data collection and processing.
 */
class SensorManager {
public:
    /**
     * @brief Constructor
     */
    SensorManager();

    /**
     * @brief Initialize all sensors
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Update all sensors (non-blocking)
     * 
     * This method should be called regularly from the main loop
     * to update sensor readings and process data.
     */
    void update();

    /**
     * @brief Get PIR sensor data
     * @return Current PIR sensor data structure
     */
    PirData getPirData();

    /**
     * @brief Get radar sensor data
     * @return Current radar sensor data structure
     */
    RadarData getRadarData();

    /**
     * @brief Get power status data
     * @return Current power status data structure
     */
    PowerData getPowerData();

    /**
     * @brief Check if any motion is detected
     * @return true if motion detected by any sensor, false otherwise
     */
    bool isMotionDetected();

private:
    PirSensor pirSensor;
    Ld2410sSensor radarSensor;
    PowerStatus powerStatus;
    unsigned long lastUpdate;
    
    // Private helper methods will be implemented in Phase 4
};
