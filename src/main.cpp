
#include "Config.h"
#include "setup/WiFiManager.h"
#include "utility/LEDController.h"
#include "utility/Buzzer.h"
#include "utility/MQTTHandler.h"
#include "utility/AlarmSystem.h"
#include "sensors/BME280Sensor.h"
#include "sensors/VEML7700Sensor.h"
#include "sensors/LIS2DW12Sensor.h"
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
LIS2DW12Sensor accelerometer;
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
void printSimpleDebug();
void runHardwareTests();
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
    
    Serial.printf("I2C Pins: SDA=%d, SCL=%d, Freq=%dHz\n", 
                  Config::I2C_SDA_PIN, Config::I2C_SCL_PIN, Config::I2C_FREQ);
    
    Wire.begin(Config::I2C_SDA_PIN, Config::I2C_SCL_PIN);
    Wire.setClock(Config::I2C_FREQ);
    
    Serial.println("I2C bus initialized");
    delay(500); // Give I2C time to stabilize
    
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
    
    Serial.print("LIS2DW12TR (Accelerometer): ");
    if (!accelerometer.begin()) {
        Serial.println("FAILED - Check I2C wiring and address 0x19");
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
        buzzer.playPattern(BuzzerPattern::STARTUP_CHIME);
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
    
    // Initial status check
    Serial.println("Performing initial sensor check...");
    delay(1000); // Allow sensors to stabilize
    printSimpleDebug();
    
    // Run comprehensive hardware tests (disabled - sensors working)
    // runHardwareTests();
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
    
    // Debug output (less frequent and simplified)
    #ifdef DEBUG_SERIAL
    if (currentTime - lastDebugOutput >= Config::DEBUG_UPDATE_INTERVAL) {
        printSimpleDebug();
        lastDebugOutput = currentTime;
    }
    #endif
    
    // Small delay to prevent watchdog timeout
    delay(10);
}

void publishSensorData() {
    DynamicJsonDocument doc(1024);
    
    // Environmental data
    if (bme280.isConnected()) {
        bme280.readSensor(doc);
        if (mqtt.isConnected()) {
            mqtt.publishSensorData("environment", doc);
        }
        doc.clear();
    }
    
    // Light data
    if (veml7700.isConnected()) {
        veml7700.readSensor(doc);
        if (mqtt.isConnected()) {
            mqtt.publishSensorData("light", doc);
        }
        doc.clear();
    }
    
    // Motion/tamper data
    if (accelerometer.isConnected()) {
        accelerometer.readSensor(doc);
        if (mqtt.isConnected()) {
            mqtt.publishSensorData("motion", doc);
        }
        doc.clear();
    }
    
    // Presence data
    if (ld2420.isConnected()) {
        ld2420.readSensor(doc);
        if (mqtt.isConnected()) {
            mqtt.publishSensorData("presence", doc);
        }
        doc.clear();
    }
}

void publishStatus() {
    DynamicJsonDocument doc(1024);
    doc["device_id"] = wifiManager.getDeviceId();
    doc["uptime"] = millis() / 1000;
    doc["free_heap"] = ESP.getFreeHeap();
    doc["wifi_rssi"] = WiFi.RSSI();
    doc["alarm_state"] = alarmSystem.getStateString();
    doc["relay_state"] = relayState ? "ON" : "OFF";
    
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
                case 0x19:
                    Serial.println(" (LIS2DW12TR)");
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

void printSimpleDebug() {
    Serial.println("=== SCOUT STATUS ===");
    
    // Quick sensor status
    Serial.printf("Sensors: BME280=%s VEML7700=%s LIS2DW12=%s LD2420=%s\n",
                  bme280.isConnected() ? "OK" : "X",
                  veml7700.isConnected() ? "OK" : "X", 
                  accelerometer.isConnected() ? "OK" : "X",
                  ld2420.isConnected() ? "OK" : "X");
    
    // Key sensor readings
    DynamicJsonDocument doc(512);
    
    if (bme280.isConnected()) {
        bme280.readSensor(doc);
        Serial.printf("Temperature: %.1f°C\n", doc["temperature"].as<float>());
        doc.clear();
    }
    
    if (accelerometer.isConnected()) {
        accelerometer.readSensor(doc);
        Serial.printf("Motion: %s\n", doc["motion_detected"].as<bool>() ? "YES" : "NO");
        doc.clear();
    }
    
    if (ld2420.isConnected()) {
        ld2420.readSensor(doc);
        Serial.printf("Presence: %s\n", doc["presence"].as<bool>() ? "YES" : "NO");
        doc.clear();
    }
    
    // System status
    Serial.printf("Alarm: %s | WiFi: %ddBm | MQTT: %s | Heap: %dk\n", 
                  alarmSystem.getStateString().c_str(),
                  WiFi.RSSI(),
                  mqtt.isConnected() ? "OK" : "X",
                  ESP.getFreeHeap() / 1024);
    
    Serial.println("===================");
}

void runHardwareTests() {
    Serial.println("==== HARDWARE DIAGNOSTIC TESTS ====");
    
    // Test GPIO functionality
    Serial.printf("Testing GPIO pins...\n");
    
    // Test I2C pins
    pinMode(Config::I2C_SDA_PIN, INPUT_PULLUP);
    pinMode(Config::I2C_SCL_PIN, INPUT_PULLUP);
    delay(100);
    
    bool sda_high = digitalRead(Config::I2C_SDA_PIN);
    bool scl_high = digitalRead(Config::I2C_SCL_PIN);
    
    Serial.printf("I2C Pin States: SDA(GPIO%d)=%s, SCL(GPIO%d)=%s\n", 
                  Config::I2C_SDA_PIN, sda_high ? "HIGH" : "LOW",
                  Config::I2C_SCL_PIN, scl_high ? "HIGH" : "LOW");
    
    if (!sda_high || !scl_high) {
        Serial.println("WARNING: I2C pins not pulled high - check pull-up resistors or connections");
    }
    
    // Test UART pins
    pinMode(Config::LD2420_RX_PIN, INPUT);
    pinMode(Config::LD2420_TX_PIN, OUTPUT);
    delay(100);
    
    Serial.printf("UART Pin Setup: RX(GPIO%d), TX(GPIO%d)\n", 
                  Config::LD2420_RX_PIN, Config::LD2420_TX_PIN);
    
    // Test voltage levels (if possible)
    Serial.printf("Power Pins: POWER_GOOD(GPIO%d)=%s, CHARGED(GPIO%d)=%s\n",
                  Config::POWER_GOOD_PIN, digitalRead(Config::POWER_GOOD_PIN) ? "HIGH" : "LOW",
                  Config::CHARGED_STATUS_PIN, digitalRead(Config::CHARGED_STATUS_PIN) ? "HIGH" : "LOW");
    
    // Extended I2C scan with detailed error reporting
    Serial.println("Extended I2C scan with error analysis...");
    
    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        Wire.beginTransmission(addr);
        uint8_t error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.printf("I2C device found at 0x%02X", addr);
            switch (addr) {
                case 0x76: case 0x77: Serial.print(" (BME280)"); break;
                case 0x10: Serial.print(" (VEML7700)"); break;
                case 0x18: case 0x19: Serial.print(" (LIS2DW12?)"); break;
                default: Serial.print(" (Unknown)"); break;
            }
            Serial.println();
        } else if (error == 2) {
            // Address was NACKed - no device
            // Don't print anything for cleaner output
        } else {
            Serial.printf("I2C error %d at address 0x%02X\n", error, addr);
        }
        delay(10); // Small delay between attempts
    }
    
    Serial.println("==================================");
}
