#pragma once

/**
 * @file Settings.h
 * @brief Global configuration settings for HearthGuard: The Scout
 */

// Debug Configuration
#define DEBUG 1

// Firmware Information
#define FIRMWARE_VERSION "0.1.0"
#define DEVICE_NAME "HearthGuard Scout"

// Serial Communication
#define SERIAL_BAUD_RATE 115200

// Network Configuration
#define WIFI_CONNECTION_TIMEOUT 30000  // 30 seconds
#define PORTAL_TIMEOUT 300  // 5 minutes

// MQTT Configuration
#define MQTT_BROKER "192.168.40.6"
#define MQTT_PORT 1883
#define MQTT_USER "mqtt-user"
#define MQTT_PASSWORD "##DikTrill45"

// Timing Constants
#define UPDATE_INTERVAL 100  // Main loop update interval (ms)
#define STATUS_UPDATE_INTERVAL 30000  // Status updates every 30 seconds

// LED Configuration
#define DEFAULT_LED_BRIGHTNESS 100  // 0-255
#define LED_UPDATE_INTERVAL 50  // LED animation update interval (ms)

// Buzzer Configuration
#define DEFAULT_VOLUME 75  // 0-100
#define BUZZER_FREQUENCY 2000  // Default frequency in Hz