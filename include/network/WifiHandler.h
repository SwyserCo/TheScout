#pragma once

/**
 * @file WifiHandler.h
 * @brief WiFi connectivity and setup portal management
 */

#include <Arduino.h>
#include "config/DataTypes.h"

// Forward declaration to avoid circular dependency
class FeedbackManager;

/**
 * @class WifiHandler
 * @brief Manages WiFi connectivity and captive portal setup
 * 
 * This class wraps WiFiManager library and provides a clean interface
 * for WiFi connection management, including initial setup via captive portal
 * and automatic reconnection logic.
 */
class WifiHandler {
public:
    /**
     * @brief Constructor
     * @param feedbackManager Reference to feedback manager for status indication
     */
    WifiHandler(FeedbackManager& feedbackManager);

    /**
     * @brief Initialize WiFi handler and start connection process
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Update WiFi connection state (non-blocking)
     * 
     * This method should be called regularly from the main loop
     * to handle connection state management and reconnection logic.
     */
    void update();

    /**
     * @brief Check if WiFi is currently connected
     * @return true if connected to WiFi, false otherwise
     */
    bool isConnected();

    /**
     * @brief Get unique device ID
     * @return String containing unique device identifier
     */
    String getDeviceID();

    /**
     * @brief Get current WiFi state
     * @return Current WiFi connection state
     */
    WifiState getState();

private:
    FeedbackManager& feedbackManager;
    WifiState currentState;
    unsigned long lastRetryAttempt;
    uint8_t retryCount;
    String deviceID;
    
    // Private helper methods will be implemented in Phase 2
};
