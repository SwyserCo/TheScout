#ifndef SETTINGS_H
#define SETTINGS_H

// The Scout - Device Configuration
// Medieval themed Guardian IoT device

// Device Identity
#define DEVICE_NAME "TheScout"
#define DEVICE_VERSION "1.0.0"
#define DEVICE_TYPE "Guardian"

// WiFi Configuration (Phase 2)
#define WIFI_SSID "YourWiFiNetwork"
#define WIFI_PASSWORD "YourWiFiPassword"

// MQTT Configuration (Phase 5)
#define MQTT_BROKER "192.168.40.6"
#define MQTT_PORT 1883
#define MQTT_USER "mqtt-user"
#define MQTT_PASSWORD "##DikTrill45"

// MQTT Topics
#define MQTT_TOPIC_BASE "guardian/scout"
#define MQTT_TOPIC_STATUS "guardian/scout/status"
#define MQTT_TOPIC_ALERTS "guardian/scout/alerts"
#define MQTT_TOPIC_SENSORS "guardian/scout/sensors"

// Timing Configuration
#define SENSOR_READ_INTERVAL 5000    // 5 seconds
#define STATUS_UPDATE_INTERVAL 30000 // 30 seconds
#define CONNECTION_TIMEOUT 10000     // 10 seconds

// LED Configuration
#define DEFAULT_LED_BRIGHTNESS 128   // 0-255 default brightness
#define BLINK_INTERVAL 500          // Default blink interval (ms)
#define PULSE_SPEED 50              // Default pulse speed

// Buzzer Configuration
#define BUZZER_LEDC_CHANNEL 0       // LEDC channel for buzzer
#define BUZZER_LEDC_RESOLUTION 8    // 8-bit resolution
#define BUZZER_LEDC_FREQUENCY 1000  // Base frequency
#define BUZZER_DUTY_CYCLE 32        // Reduced to 12.5% (32/255) to prevent overheating
#define BUZZER_MAX_CONTINUOUS_TIME 5000  // Max continuous play time (ms)
#define BUZZER_COOLDOWN_TIME 2000   // Required cooldown time (ms)

// Preferences Keys (for persistent storage)
#define PREF_NAMESPACE "scout"
#define PREF_BRIGHTNESS "brightness"
#define PREF_STEALTH_MODE "stealth"

#endif // SETTINGS_H
