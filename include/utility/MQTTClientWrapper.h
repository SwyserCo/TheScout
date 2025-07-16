#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include "sensors/Relay.h"

class MQTTClientWrapper {
public:
    MQTTClientWrapper(Client& netClient);
    void begin(const char* broker, uint16_t port);
    void loop();
    void subscribeRelay(Relay* relay);
    void publishRelayState(Relay* relay);

private:
    PubSubClient mqttClient;
    Relay* relayPtr = nullptr;
    void onMessage(char* topic, byte* payload, unsigned int length);
    static void messageCallback(char* topic, byte* payload, unsigned int length);
    static MQTTClientWrapper* instance;
};
