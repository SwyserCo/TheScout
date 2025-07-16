
#include "Config.h"
#include "sensors/SensorManager.h"
#include "utility/LEDController.h"
#include "utility/BuzzerController.h"
#include "utility/MQTTHandler.h"
#include "utility/TamperDetection.h"
#include "utility/AlarmController.h"
#include "setup/FactoryResetHandler.h"
#include "WiFiSetup.h"
#include "esp_task_wdt.h"
#include <Preferences.h>

// Watchdog timeout (8 seconds)
constexpr uint32_t WDT_TIMEOUT = 8;

// Preferences instance for persistent storage
Preferences preferences;

// System components
SensorManager sensorManager;
std::unique_ptr<LEDController> ledController(new LEDController());
std::unique_ptr<BuzzerController> buzzerController(new BuzzerController());
MQTTHandler mqttHandler;
TamperDetection tamperDetection;
WiFiSetup wifiSetup;
AlarmController alarmController;
FactoryResetHandler resetHandler;

// State tracking
bool isConnected = false;
bool isTampered = false;

// Circular buffer for storing sensor data when offline
struct SensorData {
    float temperature;
    float humidity;
    float pressure;
    float lux;
    uint8_t presence;
    uint16_t distance;
    float soundRMS;
    float soundPeak;
    unsigned long timestamp;
};

static constexpr size_t BUFFER_SIZE = 60; // Store up to 60 readings
static SensorData sensorBuffer[BUFFER_SIZE];
static size_t bufferHead = 0;
static size_t bufferCount = 0;

void handleSensorUpdate() {
    // Get all sensor data with error checking
    auto& bme = sensorManager.getBME280();
    auto& veml = sensorManager.getVEML7700();
    auto& radar = sensorManager.getLD2402();
    auto& mic = sensorManager.getMicrophone();

    // Basic validation of sensor readings
    if (bme.getTemperature() < -40.0f || bme.getTemperature() > 85.0f ||
        bme.getHumidity() < 0.0f || bme.getHumidity() > 100.0f ||
        veml.getLux() < 0.0f || 
        mic.getRMSLevel() < 0.0f) {
#if DEBUG
        Serial.println("Invalid sensor readings detected");
#endif
        return;
    }
    
    // Update alarm controller with sensor data
    const auto& motionData = radar.getData();
    alarmController.onMotionDetected(motionData.presenceState);
    alarmController.onSoundDetected(mic.getRMSLevel());
    
    // Store sensor data in buffer
    SensorData& data = sensorBuffer[bufferHead];
    data.temperature = bme.getTemperature();
    data.humidity = bme.getHumidity();
    data.pressure = bme.getPressure();
    data.lux = veml.getLux();
    data.presence = motionData.presenceState;
    data.distance = motionData.distance;
    data.soundRMS = mic.getRMSLevel();
    data.soundPeak = mic.getPeakLevel();
    data.timestamp = millis();

    bufferHead = (bufferHead + 1) % BUFFER_SIZE;
    if (bufferCount < BUFFER_SIZE) bufferCount++;

    // If connected, publish current and any buffered data
    if (isConnected) {
        // Publish current data
        mqttHandler.publishEnvironment(data.temperature, data.humidity, data.pressure);
        mqttHandler.publishLight(data.lux);
        mqttHandler.publishMotion(data.presence, data.distance);
        mqttHandler.publishSound(data.soundRMS, data.soundPeak);

        // Try to publish buffered data if any
        while (bufferCount > 1) { // Keep the latest reading
            size_t tail = (bufferHead - bufferCount + BUFFER_SIZE) % BUFFER_SIZE;
            SensorData& buffered = sensorBuffer[tail];
            
            // Only publish data less than 1 hour old
            if (millis() - buffered.timestamp > 3600000) {
                bufferCount--;
                continue;
            }

            if (!mqttHandler.publishEnvironment(buffered.temperature, buffered.humidity, buffered.pressure) ||
                !mqttHandler.publishLight(buffered.lux) ||
                !mqttHandler.publishMotion(buffered.presence, buffered.distance) ||
                !mqttHandler.publishSound(buffered.soundRMS, buffered.soundPeak)) {
                break; // Stop if any publish fails
            }
            bufferCount--;
        }
    }
}

void handleTamper(bool tampered) {
    isTampered = tampered;
    mqttHandler.publishTamper(tampered);
    
    if (tampered) {
        ledController->setSystemPattern(SystemPattern::ALARM);
        ledController->setActivityPattern(ActivityPattern::ALARM);
        buzzerController->playPattern(BuzzerPattern::ALARM);
    } else {
        ledController->setSystemPattern(SystemPattern::NORMAL);
        ledController->setActivityPattern(ActivityPattern::OFF);
        buzzerController->stop();
    }
}

void handleMQTTCommand(const JsonDocument& doc) {
    // Validate message structure
    if (!doc.is<JsonObject>()) {
#if DEBUG
        Serial.println("Invalid command format");
#endif
        return;
    }

    // Rate limiting - prevent rapid commands
    static unsigned long lastCommandTime = 0;
    const unsigned long commandDelay = 1000; // Minimum 1 second between commands
    
    if (millis() - lastCommandTime < commandDelay) {
#if DEBUG
        Serial.println("Command rate limit exceeded");
#endif
        return;
    }

    // Command validation and handling
    if (doc.containsKey("alarm")) {
        if (!doc["alarm"].is<bool>()) {
#if DEBUG
            Serial.println("Invalid alarm command type");
#endif
            return;
        }

        bool alarm = doc["alarm"].as<bool>();
        lastCommandTime = millis();

        if (alarm) {
            buzzerController->playPattern(BuzzerPattern::ALARM);
            mqttHandler.publishStatus("alarm_active");
        } else {
            buzzerController->stop();
            mqttHandler.publishStatus("alarm_cleared");
        }
    }
}

void handleAlarmState(AlarmController::State state, AlarmController::Trigger trigger) {
    // Save state to persistent storage
    preferences.begin("alarm", false);  // false = R/W mode
    preferences.putUChar("state", static_cast<uint8_t>(state));
    if (state == AlarmController::State::TRIGGERED) {
        preferences.putUChar("trigger", static_cast<uint8_t>(trigger));
        preferences.putULong("trigger_time", millis());
    }
    preferences.end();

    switch (state) {
        case AlarmController::State::ARMED:
            ledController->setActivityPattern(ActivityPattern::ARMED);
            mqttHandler.publishStatus("armed");
            break;
            
        case AlarmController::State::DISARMED:
            ledController->setActivityPattern(ActivityPattern::OFF);
            mqttHandler.publishStatus("disarmed");
            break;
            
        case AlarmController::State::TRIGGERED:
            ledController->setActivityPattern(ActivityPattern::ALARM);
            buzzerController->playPattern(BuzzerPattern::ALARM);
            mqttHandler.publishAlarm(true, static_cast<int>(trigger));
            break;
    }
}

void setup() {
#if DEBUG
    Serial.begin(115200);
#endif

    // Initialize watchdog
    esp_task_wdt_init(WDT_TIMEOUT, true); // true = panic on timeout
    esp_task_wdt_add(NULL); // Add current task to WDT watch

    // Initialize components with startup indication
    ledController->begin();
    ledController->setSystemPattern(SystemPattern::STARTUP);
    
    buzzerController->begin();
    buzzerController->playChime(BuzzerChime::STARTUP);
    
    resetHandler.begin();
    
    // Show startup indication
    ledController->setSystemPattern(SystemPattern::CONNECTING);
    
    // Setup factory reset handler with proper feedback
    resetHandler.setResetCallback([](FactoryResetStage stage) {
        switch (stage) {
            case FactoryResetStage::START:
                ledController->setSystemPattern(SystemPattern::FACTORY_RESET);
                buzzerController->playChime(BuzzerChime::RESET_START);
                break;
            case FactoryResetStage::IN_PROGRESS:
                ledController->setSystemPattern(SystemPattern::FACTORY_RESET_PROGRESS);
                break;
            case FactoryResetStage::COMPLETE:
                ledController->setSystemPattern(SystemPattern::FACTORY_RESET_COMPLETE);
                buzzerController->playChime(BuzzerChime::RESET_COMPLETE);
                break;
            case FactoryResetStage::FAILED:
                ledController->setSystemPattern(SystemPattern::ERROR);
                buzzerController->playChime(BuzzerChime::ERROR);
                break;
        }
    });
    
    // Initialize WiFi
    wifiSetup.begin();
    
    // Wait for WiFi connection with timeout (2 minutes as per PRD)
    unsigned long startTime = millis();
    while (!wifiSetup.isConnected() && millis() - startTime < 120000) {  // 2 minutes
        wifiSetup.handle();
        resetHandler.update();
        ledController->update();
        buzzerController->update();
        yield();  // Allow system tasks to run
        
        // Check for factory reset during setup
        if (digitalRead(Config::FACTORY_RESET_BTN) == LOW) {
            resetHandler.update();
        }
    }
    
    if (!wifiSetup.isConnected()) {
        buzzerController->playChime(BuzzerChime::ERROR);
        ledController->setSystemPattern(SystemPattern::ERROR);
        delay(2000);  // Show error for 2 seconds
        ESP.restart();  // Restart if setup fails
        return;
    }
    
    // Show connection success
    ledController->setSystemPattern(SystemPattern::CONNECTED);
    buzzerController->playChime(BuzzerChime::WIFI_CONNECTED);  // Specific chime for WiFi connection
    
    // Initialize MQTT
    mqttHandler.begin(wifiSetup.getDeviceID());
    mqttHandler.setCommandCallback(handleMQTTCommand);
    
    // Initialize sensors
    if (!sensorManager.begin()) {
#if DEBUG
        Serial.println("Failed to initialize sensors: ");
        Serial.println(sensorManager.getLastError());
#endif
        ledController->setSystemPattern(SystemPattern::ERROR);
        buzzerController->playChime(BuzzerChime::ERROR);
        delay(1000);  // Give time for the error indication
        ESP.restart();
        return;
    }
    
    // Initialize alarm system with LED and Buzzer controllers
    alarmController.begin(ledController.get(), buzzerController.get());
    alarmController.setStateCallback(handleAlarmState);

    // Restore last known alarm state
    preferences.begin("alarm", true);  // true = read-only mode
    if (preferences.isKey("state")) {
        auto savedState = static_cast<AlarmController::State>(preferences.getUChar("state"));
        if (savedState == AlarmController::State::ARMED) {
            alarmController.arm();
        } else if (savedState == AlarmController::State::TRIGGERED) {
            // Only restore triggered state if it was recent (within last hour)
            unsigned long triggerTime = preferences.getULong("trigger_time", 0);
            if (millis() - triggerTime < 3600000) {
                auto savedTrigger = static_cast<AlarmController::Trigger>(preferences.getUChar("trigger"));
                // Simulate the trigger through sensor events
                if (savedTrigger == AlarmController::Trigger::MOTION) {
                    alarmController.onMotionDetected(true);
                } else if (savedTrigger == AlarmController::Trigger::SOUND) {
                    alarmController.onSoundDetected(100.0f); // Threshold value
                } else if (savedTrigger == AlarmController::Trigger::TAMPER) {
                    alarmController.onTamperDetected(true);
                }
            }
        }
    }
    preferences.end();
    
    // Initialize tamper detection
    tamperDetection.begin();
    tamperDetection.setTamperCallback([](bool tampered) {
        alarmController.onTamperDetected(tampered);
    });
    
    // Set up sensor update callback
    sensorManager.setUpdateCallback(handleSensorUpdate);
}

void loop() {
    // Feed the watchdog
    esp_task_wdt_reset();

    // Update WiFi and check connection
    wifiSetup.handle();
    bool currentlyConnected = wifiSetup.isConnected();
    
    if (currentlyConnected != isConnected) {
        isConnected = currentlyConnected;
        
        if (isConnected) {
            ledController->setSystemPattern(SystemPattern::CONNECTED);
            buzzerController->playChime(BuzzerChime::SUCCESS);
        } else {
            ledController->setSystemPattern(SystemPattern::CONNECTING);
        }
    }
    
    // Only process MQTT if connected
    if (isConnected) {
        mqttHandler.update();
    }
    
    // Update all components with timing control
    static unsigned long lastHealthCheck = 0;
    static unsigned long lastUpdate = 0;
    unsigned long now = millis();
    
    // Component updates
    resetHandler.update();
    sensorManager.update();
    alarmController.update();
    ledController->update();
    buzzerController->update();
    tamperDetection.update();

    // System health check every minute
    if (now - lastHealthCheck >= 60000) {
        lastHealthCheck = now;
#if DEBUG
        // Report heap status
        Serial.printf("Free heap: %lu bytes\n", ESP.getFreeHeap());
        Serial.printf("Minimum free heap: %lu bytes\n", ESP.getMinFreeHeap());
        Serial.printf("Uptime: %lu minutes\n", now / 60000);
#endif
        // Monitor battery status
        bool onBattery = (digitalRead(Config::POWER_GOOD) == LOW);
        bool batteryLow = (digitalRead(Config::CHARGED_STATUS) == LOW);
        
        if (onBattery && batteryLow) {
            ledController->setSystemPattern(SystemPattern::ERROR);
            mqttHandler.publishStatus("battery_low");
        }
    }

    // Maintain consistent fast update rate for responsiveness
    unsigned long updateDuration = millis() - now;
    if (updateDuration < 10) { // Target 100Hz update rate
        delay(1);  // Minimal delay to prevent CPU hogging
    }
}
