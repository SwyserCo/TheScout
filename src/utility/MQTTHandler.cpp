
#include "utility/MQTTHandler.h"

void MQTTHandler::publishAlarmState(bool triggered) {
    // Example topic, replace with your actual topic
    // mqttClient.publish("thescout/alarm/state", triggered ? "TRIGGERED" : "ARMED");
}
#include "utility/MQTTHandler.h"
#include <Arduino.h>

// Example MQTT topic for relay control
const char* RELAY_TOPIC = "thescout/relay/set";
const char* RELAY_STATE_TOPIC = "thescout/relay/state";

void MQTTHandler::begin() {
    // Initialize MQTT connection
    // Subscribe to relay control topic
    // mqttClient.subscribe(RELAY_TOPIC); // Pseudocode
}

void MQTTHandler::handle() {
    // Non-blocking MQTT logic
    // Check for incoming relay commands
    // Pseudocode: if (mqttClient.hasMessage(RELAY_TOPIC)) handleRelayCommand(payload);
}


void MQTTHandler::setRelayPtr(Relay* relay) {
    relayPtr = relay;
}

void MQTTHandler::setSensorManagerPtr(SensorManager* sensorManager) {
    sensorManagerPtr = sensorManager;
}


void MQTTHandler::handleRelayCommand(const char* payload) {
    if (!relayPtr) return;
    if (strcmp(payload, "ON") == 0) {
        relayPtr->setState(RelayState::ON);
    } else if (strcmp(payload, "OFF") == 0) {
        relayPtr->setState(RelayState::OFF);
    } else if (strcmp(payload, "TOGGLE") == 0) {
        relayPtr->toggle();
    }
    publishRelayState();
}

void MQTTHandler::publishSensorData() {
    if (!sensorManagerPtr) return;
    // Example topics, replace with your actual topics
    // mqttClient.publish("thescout/sensor/temperature", String(sensorManagerPtr->getTemperature()).c_str());
    // mqttClient.publish("thescout/sensor/humidity", String(sensorManagerPtr->getHumidity()).c_str());
    // mqttClient.publish("thescout/sensor/pressure", String(sensorManagerPtr->getPressure()).c_str());
    // mqttClient.publish("thescout/sensor/ambient_light", String(sensorManagerPtr->getAmbientLight()).c_str());
    // mqttClient.publish("thescout/sensor/presence", sensorManagerPtr->getPresence() ? "1" : "0");
    // mqttClient.publish("thescout/sensor/sound_level", String(sensorManagerPtr->getSoundLevel()).c_str());
}

void MQTTHandler::publishRelayState() {
    if (!relayPtr) return;
    const char* stateStr = (relayPtr->getState() == RelayState::ON) ? "ON" : "OFF";
    // mqttClient.publish(RELAY_STATE_TOPIC, stateStr); // Pseudocode
}
