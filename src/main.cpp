
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
        ledController->setRGBColor(255, 0, 0);
        ledController->setRGBPattern(LEDPattern::BLINK_FAST);
        buzzerController->playPattern(BuzzerPattern::ALARM);
    } else {
        ledController->setRGBPattern(LEDPattern::OFF);
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

    // Initialize components
    ledController->begin();
    buzzerController->begin();
    resetHandler.begin();
    
    // Show startup indication
    ledController->setSystemPattern(SystemPattern::CONNECTING);
    
    // Setup factory reset handler
    resetHandler.setResetCallback([](void) {
        ledController->setSystemPattern(SystemPattern::ERROR);
        buzzerController->playChime(BuzzerChime::ERROR);
    });
    
    // Initialize WiFi
    wifiSetup.begin();
    
    // Wait for WiFi connection with timeout
    unsigned long startTime = millis();
    while (!wifiSetup.isConnected() && millis() - startTime < 30000) {
        wifiSetup.handle();
        resetHandler.update();
        ledController->update();
        delay(10);
    }
    
    if (!wifiSetup.isConnected()) {
        buzzerController->playChime(BuzzerChime::ERROR);
        ledController->setSystemPattern(SystemPattern::ERROR);
        return;
    }
    
    // Show connection success
    ledController->setSystemPattern(SystemPattern::CONNECTED);
    buzzerController->playChime(BuzzerChime::SUCCESS);
    
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
        return;
    }
    
    // Initialize alarm system
    alarmController.begin();
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
