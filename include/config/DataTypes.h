#pragma once

/**
 * @file DataTypes.h
 * @brief Common data types and enums for HearthGuard: The Scout
 */

#include <Arduino.h>

// ==========================================
// System State Machine
// ==========================================

/**
 * @brief Main system states for the finite state machine
 */
enum class SystemState {
    BOOTING,                // Initial boot sequence
    INITIALIZING,          // Initializing all subsystems
    WIFI_CONNECTING,       // Connecting to WiFi
    SENSOR_CALIBRATING,    // Calibrating sensors
    NORMAL_OPERATION,      // Normal operational state
    FACTORY_RESET,         // Factory reset in progress
    ERROR_STATE            // Error state - system malfunction
};

// ==========================================
// LED Animation Types
// ==========================================

// ==========================================
// Sensor Data Structures
// ==========================================

/**
 * @brief PIR sensor data
 */
struct PirData {
    bool motionDetected;
    unsigned long lastDetectionTime;
    unsigned long detectionCount;
};

/**
 * @brief LD2410S radar sensor data
 */
struct RadarData {
    bool movingTargetDetected;
    bool stationaryTargetDetected;
    uint16_t movingTargetDistance;
    uint16_t movingTargetEnergy;
    uint16_t stationaryTargetDistance;
    uint16_t stationaryTargetEnergy;
    unsigned long lastUpdateTime;
};

/**
 * @brief Power status data
 */
struct PowerData {
    float batteryVoltage;
    uint8_t batteryPercentage;
    bool usbPowerConnected;
    bool batteryLow;
    unsigned long lastUpdateTime;
};

// ==========================================
// Network & Communication Types
// ==========================================

/**
 * @brief WiFi connection state
 */
enum class WifiState {
    DISCONNECTED,          // Not connected
    CONNECTING,            // Connection in progress
    CONNECTED,             // Successfully connected
    FAILED,                // Connection failed
    AP_MODE                // Access Point mode (setup)
};

/**
 * @brief MQTT connection state
 */
enum class MqttState {
    DISCONNECTED,          // Not connected to broker
    CONNECTING,            // Connection in progress
    CONNECTED,             // Successfully connected
    FAILED,                // Connection failed
    RECONNECTING           // Attempting to reconnect
};

// ==========================================
// Data Structures
// ==========================================

/**
 * @brief Central data contract between SensorManager and MqttHandler
 * 
 * This struct serves as the data contract for sensor readings that will
 * be published to MQTT. All sensor data passes through this structure.
 */
struct SensorData {
    // Motion Detection
    bool pirMotionDetected;
    bool ld2410sPresenceDetected;
    bool ld2410sMovementDetected;
    
    // Power Status
    bool isUsbPowered;
    bool isBatteryCharging;
    bool isChargeComplete;
    
    // Sensor Values
    float batteryVoltage;
    int ld2410sDistance;
    
    // Timestamps
    unsigned long lastPirTrigger;
    unsigned long lastLd2410sTrigger;
    unsigned long dataTimestamp;
    
    // Constructor
    SensorData() {
        pirMotionDetected = false;
        ld2410sPresenceDetected = false;
        ld2410sMovementDetected = false;
        isUsbPowered = false;
        isBatteryCharging = false;
        isChargeComplete = false;
        batteryVoltage = 0.0;
        ld2410sDistance = 0;
        lastPirTrigger = 0;
        lastLd2410sTrigger = 0;
        dataTimestamp = millis();
    }
};
