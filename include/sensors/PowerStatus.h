#pragma once

/**
 * @file PowerStatus.h
 * @brief Power management and battery monitoring
 */

#include <Arduino.h>
#include "config/DataTypes.h"

/**
 * @class PowerStatus
 * @brief Monitors power status including battery and USB power
 * 
 * This class provides power monitoring functionality including
 * battery voltage, charging status, and power source detection.
 */
class PowerStatus {
public:
    /**
     * @brief Constructor
     */
    PowerStatus();

    /**
     * @brief Initialize power monitoring
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Update power status readings (non-blocking)
     * 
     * This method should be called regularly from the main loop
     * to update battery voltage and power status.
     */
    void update();

    /**
     * @brief Get current power status data
     * @return Power status data structure
     */
    PowerData getData();

    /**
     * @brief Check if USB power is connected
     * @return true if USB power detected, false otherwise
     */
    bool isUsbPowerConnected();

    /**
     * @brief Check if battery is low
     * @return true if battery voltage is low, false otherwise
     */
    bool isBatteryLow();

    /**
     * @brief Get battery percentage
     * @return Battery percentage (0-100)
     */
    uint8_t getBatteryPercentage();

private:
    PowerData powerData;
    unsigned long lastUpdate;
    
    // Private helper methods will be implemented in Phase 4
};
