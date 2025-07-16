#include "utility/MQTTClientWrapper.h"

MQTTClientWrapper* MQTTClientWrapper::instance = nullptr;

MQTTClientWrapper::MQTTClientWrapper(Client& netClient) : mqttClient(netClient) {
    instance = this;
    mqttClient.setCallback(MQTTClientWrapper::messageCallback);
}

void MQTTClientWrapper::begin(const char* broker, uint16_t port) {
    mqttClient.setServer(broker, port);
    // Connect logic here
}

void MQTTClientWrapper::loop() {
    mqttClient.loop();
}

void MQTTClientWrapper::subscribeRelay(Relay* relay) {
    relayPtr = relay;
    mqttClient.subscribe("thescout/relay/set");
}

void MQTTClientWrapper::publishRelayState(Relay* relay) {
    const char* stateStr = (relay->getState() == RelayState::ON) ? "ON" : "OFF";
    mqttClient.publish("thescout/relay/state", stateStr);
}

void MQTTClientWrapper::onMessage(char* topic, byte* payload, unsigned int length) {
    if (relayPtr && strcmp(topic, "thescout/relay/set") == 0) {
        char cmd[8] = {0};
        strncpy(cmd, (char*)payload, length);
        if (strcmp(cmd, "ON") == 0) relayPtr->setState(RelayState::ON);
        else if (strcmp(cmd, "OFF") == 0) relayPtr->setState(RelayState::OFF);
        else if (strcmp(cmd, "TOGGLE") == 0) relayPtr->toggle();
        publishRelayState(relayPtr);
    }
}

void MQTTClientWrapper::messageCallback(char* topic, byte* payload, unsigned int length) {
    if (instance) instance->onMessage(topic, payload, length);
}
