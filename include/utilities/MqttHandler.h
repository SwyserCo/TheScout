#pragma once

/**
 * @file MqttHandler.h
 * @brief MQTT communication and Home Assistant integration
 */

#include <Arduino.h>
#include "config/DataTypes.h"

/**
 * @class MqttHandler
 * @brief Handles MQTT communication and Home Assistant discovery
 * 
 * This class wraps PubSubClient library and provides MQTT functionality
 * including Home Assistant auto-discovery and sensor data publishing.
 */
class MqttHandler {
public:
    /**
     * @brief Constructor
     */
    MqttHandler();

    /**
     * @brief Initialize MQTT handler
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Update MQTT connection and message handling (non-blocking)
     * 
     * This method should be called regularly from the main loop
     * to maintain MQTT connection and process incoming messages.
     */
    void update();

    /**
     * @brief Check if MQTT is connected
     * @return true if connected to MQTT broker, false otherwise
     */
    bool isConnected();

    /**
     * @brief Publish sensor data to MQTT
     * @param pirData PIR sensor data
     * @param radarData Radar sensor data
     * @param powerData Power status data
     */
    void publishSensorData(const PirData& pirData, const RadarData& radarData, const PowerData& powerData);

    /**
     * @brief Send Home Assistant discovery messages
     */
    void sendDiscoveryMessages();

    /**
     * @brief Get current MQTT state
     * @return Current MQTT connection state
     */
    MqttState getState();

private:
    MqttState currentState;
    unsigned long lastReconnectAttempt;
    unsigned long lastHeartbeat;
    String deviceTopic;
    
    // Private helper methods will be implemented in Phase 5
};
