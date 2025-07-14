#pragma once
#include <PubSubClient.h>
#include <WiFi.h>

class MQTTClientHandler {
public:
    MQTTClientHandler(const char* server, const char* user, const char* pass, const char* clientId);
    void begin();
    void loop();
    void publishLux(float lux);
    void publishTemperature(float temperature);
    void publishHumidity(float humidity);
    void publishPressure(float pressure);
    void publishDiscovery();

private:
    WiFiClient espClient;
    PubSubClient client;
    const char* mqttServer;
    const char* mqttUser;
    const char* mqttPass;
    const char* mqttClientId;

    void reconnect();
    void publishSensorConfig(const char* objectId, const char* name, const char* stateTopic,
                             const char* unit, const char* deviceClass,
                             const char* payloadOn = nullptr, const char* payloadOff = nullptr);
};
