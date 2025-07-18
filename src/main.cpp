
#include "Config.h"
#include "setup/WiFiManager.h"
#include "utility/LEDController.h"
#include "utility/Buzzer.h"
#include "utility/MQTTHandler.h"
#include "utility/AlarmSystem.h"
#include "sensors/BME280Sensor.h"
#include "sensors/VEML7700Sensor.h"
#include "sensors/AccelerometerSensor.h"
#include "sensors/LD2420Sensor.h"
#include <ArduinoJson.h>
#include <Preferences.h>

// Hardware components
LEDController systemLED(Config::SYSTEM_LED_PIN);
LEDController activityLED(Config::ACTIVITY_LED_PIN);
Buzzer buzzer(Config::BUZZER_PIN);
WiFiManager wifiManager;
MQTTHandler mqtt;

// Sensors
BME280Sensor bme280;
VEML7700Sensor veml7700;
AccelerometerSensor accelerometer;
LD2420Sensor ld2420;

// Alarm system
AlarmSystem alarmSystem(activityLED, buzzer, accelerometer, ld2420);

// State variables
bool relayState = false;
uint32_t lastSensorPublish = 0;
uint32_t lastStatusPublish = 0;
uint32_t lastDebugOutput = 0;
bool factoryResetActive = false;
uint32_t factoryResetStart = 0;

// Preferences for persistent storage
Preferences preferences;

// Function declarations
void publishSensorData();
void publishStatus();
void handleAlarmCommand(const String& command);
void handleRelayCommand(bool state);
void handleThresholdCommand(const String& sensor, float threshold);
void checkFactoryReset();
void performFactoryReset();
void printSystemInfo();
void scanI2CDevices();

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    
    Serial.println("=== The Scout Starting ===");
    
    // Print system information
    printSystemInfo();
    
    // Initialize hardware
    pinMode(Config::FACTORY_RESET_BTN, INPUT_PULLUP);
    pinMode(Config::RELAY_PIN, OUTPUT);
    pinMode(Config::POWER_GOOD_PIN, INPUT);
    pinMode(Config::CHARGED_STATUS_PIN, INPUT);
    
    // Initialize LEDs and buzzer
    systemLED.begin();
    activityLED.begin();
    buzzer.begin();
    
    // Initialize preferences
    preferences.begin("scout-config", false);
    
    // Welcome pattern
    systemLED.flashPattern(3, 200);
    buzzer.playPattern(BuzzerPattern::SINGLE_BEEP);
    
    Serial.println("Hardware initialized");
    
    // Initialize sensors
    Serial.println("Initializing sensors...");
    
    Wire.begin(Config::I2C_SDA_PIN, Config::I2C_SCL_PIN);
    
    // Scan for I2C devices
    Serial.println("Scanning I2C bus...");
    scanI2CDevices();
    
    bool sensorSuccess = true;
    
    Serial.print("BME280 (Environmental): ");
    if (!bme280.begin()) {
        Serial.println("FAILED - Check I2C wiring and address 0x76");
        sensorSuccess = false;
    } else {
        Serial.println("SUCCESS");
    }
    
    Serial.print("VEML7700 (Light): ");
    if (!veml7700.begin()) {
        Serial.println("FAILED - Check I2C wiring and address 0x10");
        sensorSuccess = false;
    } else {
        Serial.println("SUCCESS");
    }
    
    Serial.print("MPU6050 (Accelerometer): ");
    if (!accelerometer.begin()) {
        Serial.println("FAILED - Check I2C wiring and address 0x68");
        sensorSuccess = false;
    } else {
        Serial.println("SUCCESS");
    }
    
    Serial.print("LD2420 (mmWave Radar): ");
    if (!ld2420.begin()) {
        Serial.println("FAILED - Check UART wiring TX/RX pins");
        sensorSuccess = false;
    } else {
        Serial.println("SUCCESS");
    }
    
    if (sensorSuccess) {
        buzzer.playPattern(BuzzerPattern::SUCCESS_CHIME);
        Serial.println("All sensors initialized successfully");
    } else {
        buzzer.playPattern(BuzzerPattern::FAILURE_CHIME);
        Serial.println("Some sensors failed to initialize");
    }
    
    // Initialize WiFi
    Serial.println("Initializing WiFi...");
    wifiManager.begin();
    
    systemLED.setState(LEDState::BLINK); // Red blinking during connection
    
    if (!wifiManager.autoConnect()) {
        Serial.println("Starting WiFi config portal...");
        wifiManager.startConfigPortal();
        
        // Wait for WiFi configuration
        while (!wifiManager.isConnected()) {
            wifiManager.handleClient();
            systemLED.update();
            delay(100);
            
            // Check for factory reset during setup
            if (digitalRead(Config::FACTORY_RESET_BTN) == LOW) {
                if (!factoryResetActive) {
                    factoryResetActive = true;
                    factoryResetStart = millis();
                    systemLED.setState(LEDState::FAST_BLINK);
                } else if (millis() - factoryResetStart >= 5000) {
                    // Factory reset triggered
                    performFactoryReset();
                    return;
                }
            } else {
                factoryResetActive = false;
                systemLED.setState(LEDState::BLINK);
            }
        }
    }
    
    if (wifiManager.isConnected()) {
        systemLED.flashPattern(3, 200); // Green flash 3 times
        buzzer.playPattern(BuzzerPattern::SUCCESS_CHIME);
        Serial.println("WiFi connected successfully");
        Serial.print("Device ID: ");
        Serial.println(wifiManager.getDeviceId());
    } else {
        buzzer.playPattern(BuzzerPattern::WIFI_TIMEOUT);
        Serial.println("WiFi connection failed");
    }
    
    // Initialize MQTT
    if (wifiManager.isConnected()) {
        Serial.println("Initializing MQTT...");
        mqtt.begin(wifiManager.getDeviceId());
        
        // Set up MQTT callbacks
        mqtt.setAlarmCallback([](const String& command) {
            handleAlarmCommand(command);
        });
        
        mqtt.setRelayCallback([](bool state) {
            handleRelayCommand(state);
        });
        
        mqtt.setThresholdCallback([](const String& sensor, float threshold) {
            handleThresholdCommand(sensor, threshold);
        });
    }
    
    // Initialize alarm system
    alarmSystem.begin();
    
    // Load saved relay state
    relayState = preferences.getBool("relay_state", false);
    digitalWrite(Config::RELAY_PIN, relayState ? HIGH : LOW);
    
    Serial.println("=== The Scout Ready ===");
    systemLED.setState(LEDState::OFF);
    
    // Initial sensor readings
    Serial.println("=== Initial Sensor Readings ===");
    delay(1000); // Allow sensors to stabilize
    publishSensorData();
}

void loop() {
    uint32_t currentTime = millis();
    
    // Update all components
    systemLED.update();
    buzzer.update();
    alarmSystem.update();
    
    // Handle WiFi client (if in setup mode)
    wifiManager.handleClient();
    
    // Handle MQTT
    if (wifiManager.isConnected()) {
        mqtt.loop();
    }
    
    // Update sensors
    accelerometer.update();
    ld2420.update();
    
    // Check factory reset button
    checkFactoryReset();
    
    // Publish sensor data
    if (currentTime - lastSensorPublish >= Config::SENSOR_READ_INTERVAL) {
        publishSensorData();
        lastSensorPublish = currentTime;
    }
    
    // Publish status
    if (currentTime - lastStatusPublish >= Config::STATUS_UPDATE_INTERVAL) {
        publishStatus();
        lastStatusPublish = currentTime;
    }
    
    // Debug output (more frequent than sensor publishing)
    #ifdef DEBUG_SERIAL
    if (currentTime - lastDebugOutput >= Config::DEBUG_UPDATE_INTERVAL) {
        publishSensorData(); // This will show current sensor readings
        lastDebugOutput = currentTime;
    }
    #endif
    
    // Small delay to prevent watchdog timeout
    delay(10);
}

void publishSensorData() {
    DynamicJsonDocument doc(1024);
    
    Serial.println("=== Sensor Status ===");
    
    // Show connection status
    Serial.printf("Connection Status: BME280=%s, VEML7700=%s, MPU6050=%s, LD2420=%s\n",
                  bme280.isConnected() ? "OK" : "FAIL",
                  veml7700.isConnected() ? "OK" : "FAIL", 
                  accelerometer.isConnected() ? "OK" : "FAIL",
                  ld2420.isConnected() ? "OK" : "FAIL");
    
    // Environmental data
    if (bme280.isConnected()) {
        bme280.readSensor(doc);
        float temp = doc["temperature"].as<float>();
        float hum = doc["humidity"].as<float>();
        float press = doc["pressure"].as<float>();
        
        if (isnan(temp) || isnan(hum) || isnan(press)) {
            Serial.println("BME280: Invalid readings (NaN)");
        } else {
            Serial.printf("BME280: T=%.1f°C, H=%.1f%%, P=%.1fhPa\n", temp, hum, press);
        }
        
        if (mqtt.isConnected()) {
            mqtt.publishSensorData("environment", doc);
        }
        doc.clear();
    } else {
        Serial.println("BME280: DISCONNECTED");
    }
    
    // Light data
    if (veml7700.isConnected()) {
        veml7700.readSensor(doc);
        float illum = doc["illuminance"].as<float>();
        float als = doc["als"].as<float>();
        
        if (isnan(illum) || isnan(als)) {
            Serial.println("VEML7700: Invalid readings (NaN)");
        } else {
            Serial.printf("VEML7700: Light=%.1flux, ALS=%.1f\n", illum, als);
        }
        
        if (mqtt.isConnected()) {
            mqtt.publishSensorData("light", doc);
        }
        doc.clear();
    } else {
        Serial.println("VEML7700: DISCONNECTED");
    }
    
    // Motion/tamper data
    if (accelerometer.isConnected()) {
        accelerometer.readSensor(doc);
        Serial.printf("MPU6050: X=%.2fg, Y=%.2fg, Z=%.2fg, Motion=%s\n", 
                      doc["accel_x"].as<float>(), 
                      doc["accel_y"].as<float>(), 
                      doc["accel_z"].as<float>(),
                      doc["motion_detected"].as<bool>() ? "YES" : "NO");
        
        if (mqtt.isConnected()) {
            mqtt.publishSensorData("motion", doc);
        }
        doc.clear();
    } else {
        Serial.println("MPU6050: DISCONNECTED");
    }
    
    // Presence data
    if (ld2420.isConnected()) {
        ld2420.readSensor(doc);
        Serial.printf("LD2420: Presence=%s, Distance=%.1fm\n", 
                      doc["presence"].as<bool>() ? "YES" : "NO",
                      doc["distance"].as<float>());
        
        if (mqtt.isConnected()) {
            mqtt.publishSensorData("presence", doc);
        }
        doc.clear();
    } else {
        Serial.println("LD2420: DISCONNECTED");
    }
    
    // System status
    Serial.printf("System: Alarm=%s, Relay=%s, WiFi=%ddBm, Heap=%d bytes\n", 
                  alarmSystem.getStateString().c_str(),
                  relayState ? "ON" : "OFF",
                  WiFi.RSSI(),
                  ESP.getFreeHeap());
    
    Serial.println("==================");
}

void publishStatus() {
    DynamicJsonDocument doc(1024);
    doc["device_id"] = wifiManager.getDeviceId();
    doc["uptime"] = millis() / 1000;
    doc["free_heap"] = ESP.getFreeHeap();
    doc["wifi_rssi"] = WiFi.RSSI();
    doc["alarm_state"] = alarmSystem.getStateString();
    doc["relay_state"] = relayState ? "ON" : "OFF";
    
    // Serial status output
    Serial.println("=== Device Status ===");
    Serial.printf("Device ID: %s\n", wifiManager.getDeviceId().c_str());
    Serial.printf("Uptime: %d seconds\n", millis() / 1000);
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("WiFi RSSI: %d dBm\n", WiFi.RSSI());
    Serial.printf("Alarm State: %s\n", alarmSystem.getStateString().c_str());
    Serial.printf("Relay State: %s\n", relayState ? "ON" : "OFF");
    Serial.printf("MQTT Connected: %s\n", mqtt.isConnected() ? "YES" : "NO");
    
    // Power status
    bool powerGood = digitalRead(Config::POWER_GOOD_PIN);
    bool charged = digitalRead(Config::CHARGED_STATUS_PIN);
    Serial.printf("Power Good: %s\n", powerGood ? "YES" : "NO");
    Serial.printf("Battery Charged: %s\n", charged ? "YES" : "NO");
    
    Serial.println("==================");
    
    if (mqtt.isConnected()) {
        mqtt.publishSensorData("status", doc);
        mqtt.publishAlarmState(alarmSystem.getStateString());
        mqtt.publishRelayState(relayState);
    }
}

void handleAlarmCommand(const String& command) {
    Serial.println("=== ALARM COMMAND ===");
    Serial.println("Command: " + command);
    
    if (command == "ARM_AWAY") {
        alarmSystem.setState(AlarmState::ARMED);
        Serial.println("Alarm system ARMED");
    } else if (command == "DISARM") {
        alarmSystem.setState(AlarmState::DISARMED);
        Serial.println("Alarm system DISARMED");
    } else if (command == "SILENCE") {
        alarmSystem.silence();
        Serial.println("Alarm system SILENCED");
    }
    Serial.println("===================");
}

void handleRelayCommand(bool state) {
    Serial.println("=== RELAY COMMAND ===");
    Serial.println("State: " + String(state ? "ON" : "OFF"));
    
    relayState = state;
    digitalWrite(Config::RELAY_PIN, relayState ? HIGH : LOW);
    
    // Save state to preferences
    preferences.putBool("relay_state", relayState);
    
    // Publish state update
    if (mqtt.isConnected()) {
        mqtt.publishRelayState(relayState);
    }
    
    Serial.println("Relay switched " + String(relayState ? "ON" : "OFF"));
    Serial.println("==================");
}

void handleThresholdCommand(const String& sensor, float threshold) {
    Serial.println("Threshold command received: " + sensor + " = " + String(threshold));
    
    if (sensor == "motion") {
        alarmSystem.setTamperThreshold(threshold);
        preferences.putFloat("tamper_threshold", threshold);
    }
}

void checkFactoryReset() {
    if (digitalRead(Config::FACTORY_RESET_BTN) == LOW) {
        if (!factoryResetActive) {
            factoryResetActive = true;
            factoryResetStart = millis();
            systemLED.setState(LEDState::FAST_BLINK);
        } else if (millis() - factoryResetStart >= 5000) {
            // Factory reset triggered
            performFactoryReset();
        }
    } else {
        if (factoryResetActive) {
            factoryResetActive = false;
            systemLED.setState(LEDState::OFF);
        }
    }
}

void performFactoryReset() {
    Serial.println("Factory reset triggered!");
    
    // Flash LED and play chime
    systemLED.flashPattern(5, 100);
    buzzer.playPattern(BuzzerPattern::TRIPLE_BEEP);
    
    // Clear all stored data
    wifiManager.clearCredentials();
    preferences.clear();
    
    // Wait for feedback to complete
    delay(2000);
    
    // Restart
    ESP.restart();
}

void printSystemInfo() {
    Serial.println("=== System Information ===");
    Serial.printf("Chip Model: %s\n", ESP.getChipModel());
    Serial.printf("Chip Revision: %d\n", ESP.getChipRevision());
    Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Flash Size: %d bytes\n", ESP.getFlashChipSize());
    Serial.printf("Flash Speed: %d Hz\n", ESP.getFlashChipSpeed());
    Serial.printf("MAC Address: %s\n", WiFi.macAddress().c_str());
    Serial.println("=== Pin Configuration ===");
    Serial.printf("Factory Reset Button: GPIO%d\n", Config::FACTORY_RESET_BTN);
    Serial.printf("System LED: GPIO%d\n", Config::SYSTEM_LED_PIN);
    Serial.printf("Activity LED: GPIO%d\n", Config::ACTIVITY_LED_PIN);
    Serial.printf("Buzzer: GPIO%d\n", Config::BUZZER_PIN);
    Serial.printf("Relay: GPIO%d\n", Config::RELAY_PIN);
    Serial.printf("I2C SDA: GPIO%d\n", Config::I2C_SDA_PIN);
    Serial.printf("I2C SCL: GPIO%d\n", Config::I2C_SCL_PIN);
    Serial.printf("LD2420 RX: GPIO%d\n", Config::LD2420_RX_PIN);
    Serial.printf("LD2420 TX: GPIO%d\n", Config::LD2420_TX_PIN);
    Serial.printf("Accelerometer INT: GPIO%d\n", Config::ACCEL_INT_PIN);
    Serial.printf("LD2420 INT: GPIO%d\n", Config::LD2420_INT_PIN);
    Serial.printf("Power Good: GPIO%d\n", Config::POWER_GOOD_PIN);
    Serial.printf("Charged Status: GPIO%d\n", Config::CHARGED_STATUS_PIN);
    Serial.println("=== MQTT Configuration ===");
    Serial.printf("Broker: %s:%d\n", Config::MQTT_BROKER, Config::MQTT_PORT);
    Serial.printf("Username: %s\n", Config::MQTT_USER);
    Serial.printf("Keep Alive: %d seconds\n", Config::MQTT_KEEP_ALIVE);
    Serial.println("=========================");
}

void scanI2CDevices() {
    Serial.println("=== I2C Device Scanner ===");
    byte error, address;
    int nDevices = 0;
    
    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.printf("I2C device found at address 0x%02X", address);
            
            // Identify known devices
            switch (address) {
                case 0x10:
                    Serial.println(" (VEML7700)");
                    break;
                case 0x68:
                    Serial.println(" (MPU6050)");
                    break;
                case 0x76:
                    Serial.println(" (BME280)");
                    break;
                case 0x77:
                    Serial.println(" (BME280 alt)");
                    break;
                default:
                    Serial.println(" (Unknown)");
                    break;
            }
            nDevices++;
        } else if (error == 4) {
            Serial.printf("Unknown error at address 0x%02X\n", address);
        }
    }
    
    if (nDevices == 0) {
        Serial.println("No I2C devices found");
        Serial.println("Check wiring:");
        Serial.printf("  SDA: GPIO%d\n", Config::I2C_SDA_PIN);
        Serial.printf("  SCL: GPIO%d\n", Config::I2C_SCL_PIN);
        Serial.println("  3.3V power to sensors");
        Serial.println("  Ground connections");
    } else {
        Serial.printf("Found %d I2C devices\n", nDevices);
    }
    Serial.println("=========================");
}
