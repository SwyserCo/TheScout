#include "setup/MQTTClientHandler.h"

MQTTClientHandler::MQTTClientHandler(const char* server, const char* user, const char* pass, const char* clientId)
    : client(espClient), mqttServer(server), mqttUser(user), mqttPass(pass), mqttClientId(clientId) {}

void MQTTClientHandler::begin() {
    client.setServer(mqttServer, 1883);
    // Optional: set callback if you want to receive messages
    // client.setCallback(callbackFunction);
}

void MQTTClientHandler::loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}

void MQTTClientHandler::reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(mqttClientId, mqttUser, mqttPass)) {
            Serial.println("connected");
            publishDiscovery();
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void MQTTClientHandler::publishLux(float lux) {
    char payload[16];
    dtostrf(lux, 6, 2, payload);
    client.publish((String("guardian/") + mqttClientId + "/ambient_light").c_str(), payload);
}

void MQTTClientHandler::publishTemperature(float temperature) {
    char payload[16];
    dtostrf(temperature, 6, 2, payload);
    client.publish((String("guardian/") + mqttClientId + "/temperature").c_str(), payload);
}

void MQTTClientHandler::publishHumidity(float humidity) {
    char payload[16];
    dtostrf(humidity, 6, 2, payload);
    client.publish((String("guardian/") + mqttClientId + "/humidity").c_str(), payload);
}

void MQTTClientHandler::publishPressure(float pressure) {
    char payload[16];
    dtostrf(pressure, 6, 2, payload);
    client.publish((String("guardian/") + mqttClientId + "/pressure").c_str(), payload);
}

void MQTTClientHandler::publishDiscovery() {
    Serial.println("Publishing MQTT discovery messages...");
    publishSensorConfig("ambient_light", "Ambient Light", String("guardian/") + mqttClientId + "/ambient_light", "lx", "illuminance");
    publishSensorConfig("presence", "Presence", String("guardian/") + mqttClientId + "/presence", nullptr, "motion", "1", "0");
    publishSensorConfig("temperature", "Temperature", String("guardian/") + mqttClientId + "/temperature", "°C", "temperature");
    publishSensorConfig("humidity", "Humidity", String("guardian/") + mqttClientId + "/humidity", "%", "humidity");
    publishSensorConfig("co2", "CO2", String("guardian/") + mqttClientId + "/co2", "ppm", "carbon_dioxide");
    publishSensorConfig("tamper", "Tamper", String("guardian/") + mqttClientId + "/tamper", nullptr, "vibration", "1", "0");
    publishSensorConfig("spl", "Sound Level", String("guardian/") + mqttClientId + "/spl", "dB", nullptr);
}

void MQTTClientHandler::publishSensorConfig(const char* objectId, const char* name, const String& stateTopic,
                                            const char* unit, const char* deviceClass,
                                            const char* payloadOn, const char* payloadOff) {
    String topic = "homeassistant/sensor/";
    topic += mqttClientId;
    topic += "/";
    topic += objectId;
    topic += "/config";

    String payload = "{";
    payload += "\"name\": \"" + String(name) + "\",";
    payload += "\"state_topic\": \"" + stateTopic + "\",";
    if (unit) {
        payload += "\"unit_of_measurement\": \"" + String(unit) + "\",";
    }
    if (deviceClass) {
        payload += "\"device_class\": \"" + String(deviceClass) + "\",";
    }
    if (payloadOn && payloadOff) {
        payload += "\"payload_on\": \"" + String(payloadOn) + "\",";
        payload += "\"payload_off\": \"" + String(payloadOff) + "\",";
    }
    payload += "\"unique_id\": \"" + String(mqttClientId) + "_" + String(objectId) + "\",";
    payload += "\"device\": {";
    payload += "\"identifiers\": [\"" + String(mqttClientId) + "\"],";
    payload += "\"name\": \"" + String(mqttClientId) + "\",";
    payload += "\"manufacturer\": \"Francois\",";
    payload += "\"model\": \"TheScout\"";
    payload += "}";
    payload += "}";

    Serial.print("Publishing to topic: ");
    Serial.println(topic);
    Serial.println(payload);

    client.publish(topic.c_str(), payload.c_str(), true); // retained = true
}
