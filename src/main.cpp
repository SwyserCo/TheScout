

// Main application for The Scout (ESP32-S3)
#include <Arduino.h>
#include "setup/WiFiManager.h"
#include "utility/MQTTHandler.h"

MQTTHandler mqttHandler;
#include "sensors/SensorManager.h"
#include "utility/LEDController.h"
#include "utility/Buzzer.h"
#include "sensors/Relay.h"
#include "utility/TamperingProtection.h"
#include "setup/FactoryResetHandler.h"
#include "sensors/BME280.h"
#include "sensors/VEML7700.h"
#include "sensors/LD2420.h"
#include "sensors/Microphone.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include "utility/MQTTClientWrapper.h"


WiFiManager wifiManager;
SensorManager sensorManager;
LEDController ledController;
Buzzer buzzer;
Relay relay;
TamperingProtection tamperingProtection;
FactoryResetHandler factoryResetHandler;

WiFiClient espClient;
MQTTClientWrapper mqttClient(espClient);

void setup() {
  Serial.begin(115200);
  ledController.begin();
  buzzer.begin();
  factoryResetHandler.begin();
  wifiManager.begin();
  sensorManager.begin();
  relay.begin();
  tamperingProtection.begin();
  tamperingProtection.setMQTTHandler(&mqttHandler);

  // Connect to WiFi (replace with WiFiManager logic)
  WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Setup MQTT
  mqttClient.begin("YOUR_MQTT_BROKER", 1883); // Replace with actual broker and port
  mqttClient.subscribeRelay(&relay);

  // Link SensorManager to MQTTHandler
  // If using MQTTHandler, uncomment and use:
  // mqttHandler.setSensorManagerPtr(&sensorManager);
}

void loop() {
  factoryResetHandler.handle();
  wifiManager.handle();
  sensorManager.handle();
  relay.handle();
  tamperingProtection.handle();
  ledController.handle();
  buzzer.handle();
  mqttClient.loop();
  // ...existing code...

  // Example: publish sensor data every second
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 1000) {
    lastPublish = millis();
    // If using MQTTHandler, uncomment and use:
    // mqttHandler.publishSensorData();
  }
}
