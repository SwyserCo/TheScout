
#include "utility/TamperingProtection.h"
#include <Arduino.h>
#include "sensors/SensorManager.h"
#include "utility/LEDController.h"
#include "utility/Buzzer.h"
#include "utility/MQTTHandler.h"

void TamperingProtection::begin() {
    state = AlarmState::DISARMED;
}

void TamperingProtection::handle() {
    // Non-blocking alarm duration logic
    if (state == AlarmState::TRIGGERED) {
        if (millis() - alarmTriggeredTime > alarmDuration) {
            state = AlarmState::ARMED; // Auto-reset to ARMED after alarm duration
            // Publish alarm state reset to MQTT
            if (mqttHandler) mqttHandler->publishAlarmState(false);
        }
    }

    // Example: get sensor readings (replace with actual SensorManager reference)
    extern SensorManager sensorManager;

    // Only trigger alarm if armed
    if (state == AlarmState::ARMED) {
        // Accelerometer: movement > 3s (stubbed as presence for demo)
        static unsigned long movementStart = 0;
        if (sensorManager.getPresence()) {
            if (movementStart == 0) movementStart = millis();
            if (millis() - movementStart > 3000) {
                triggerAlarm();
                movementStart = 0;
            }
        } else {
            movementStart = 0;
        }

        // SPL mic: loud or sustained noise > 5s
        static unsigned long soundStart = 0;
        if (sensorManager.getSoundLevel() > 70.0f) { // Example threshold
            if (soundStart == 0) soundStart = millis();
            if (millis() - soundStart > 5000) {
                triggerAlarm();
                soundStart = 0;
            }
        } else {
            soundStart = 0;
        }

        // LD2420: presence > 5s
        static unsigned long presenceStart = 0;
        if (sensorManager.getPresence()) {
            if (presenceStart == 0) presenceStart = millis();
            if (millis() - presenceStart > 5000) {
                triggerAlarm();
                presenceStart = 0;
            }
        } else {
            presenceStart = 0;
        }
    }
}

void TamperingProtection::arm() {
    state = AlarmState::ARMED;
}

void TamperingProtection::disarm() {
    state = AlarmState::DISARMED;
}

AlarmState TamperingProtection::getState() const {
    return state;
}

void TamperingProtection::setMQTTHandler(MQTTHandler* handler) {
    mqttHandler = handler;
}

void TamperingProtection::triggerAlarm() {
    state = AlarmState::TRIGGERED;
    alarmTriggeredTime = millis();

    // Feedback: LED and buzzer
    extern LEDController ledController;
    extern Buzzer buzzer;
    ledController.setActivityLED(ActivityLEDState::FLASHING);
    buzzer.playChime(BuzzerChime::FAILURE);

    // Publish alarm state to MQTT
    if (mqttHandler) mqttHandler->publishAlarmState(true);
}

// Add missing closing brace for file
// ...existing code...