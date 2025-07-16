
#include "Config.h"
#include "sensors/SensorManager.h"
#include "utility/LEDController.h"
#include "utility/BuzzerController.h"
#include "utility/MQTTHandler.h"
#include "utility/TamperDetection.h"
#include "utility/AlarmController.h"
#include "setup/FactoryResetHandler.h"
#include "WiFiSetup.h"

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

void handleSensorUpdate() {
    // Get all sensor data
    auto& bme = sensorManager.getBME280();
    auto& veml = sensorManager.getVEML7700();
    auto& radar = sensorManager.getLD2402();
    auto& mic = sensorManager.getMicrophone();
    
    // Update alarm controller with sensor data
    const auto& motionData = radar.getData();
    alarmController.onMotionDetected(motionData.presenceState);
    alarmController.onSoundDetected(mic.getRMSLevel());
    
    // Publish environmental data
    mqttHandler.publishEnvironment(
        bme.getTemperature(),
        bme.getHumidity(),
        bme.getPressure()
    );
    
    // Publish light data
    mqttHandler.publishLight(veml.getLux());
    
    // Publish motion data
    mqttHandler.publishMotion(motionData.presenceState, motionData.distance);
    
    // Publish sound data
    mqttHandler.publishSound(mic.getRMSLevel(), mic.getPeakLevel());
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
    if (doc.containsKey("alarm")) {
        bool alarm = doc["alarm"].as<bool>();
        if (alarm) {
            buzzerController->playPattern(BuzzerPattern::ALARM);
        } else {
            buzzerController->stop();
        }
    }
}

void handleAlarmState(AlarmController::State state, AlarmController::Trigger trigger) {
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
    
    // Initialize tamper detection
    tamperDetection.begin();
    tamperDetection.setTamperCallback([](bool tampered) {
        alarmController.onTamperDetected(tampered);
    });
    
    // Set up sensor update callback
    sensorManager.setUpdateCallback(handleSensorUpdate);
}

void loop() {
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
    
    // Update all components
    resetHandler.update();
    sensorManager.update();
    alarmController.update();
    ledController->update();
    buzzerController->update();
    tamperDetection.update();
    
    // Small delay to prevent tight loop
    delay(10);
}
