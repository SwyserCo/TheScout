#pragma once

/**
 * @file DeviceManager.h
 * @brief Device-level operations including factory reset
 */

#include <Arduino.h>
#include "feedback/FeedbackManager.h"

/**
 * @class DeviceManager
 * @brief Manages device-level operations like factory reset
 * 
 * This class handles device-level functionality including
 * factory reset button monitoring and device configuration management.
 */
class DeviceManager {
public:
    /**
     * @brief Constructor
     */
    DeviceManager();

    /**
     * @brief Initialize device manager
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Update device manager (non-blocking)
     * 
     * This method should be called regularly from the main loop
     * to monitor factory reset button and handle device operations.
     */
    void update();

    /**
     * @brief Check if factory reset is in progress
     * @return true if factory reset is active, false otherwise
     */
    bool isFactoryResetActive();

    /**
     * @brief Trigger factory reset programmatically
     */
    void triggerFactoryReset();

private:
    bool factoryResetActive;
    unsigned long buttonPressStart;
    unsigned long lastButtonState;
    
    // Private helper methods will be implemented in Phase 3
};
