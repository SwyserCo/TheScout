#include <Arduino.h>

// setup includes
#include "setup/MQTTClientHandler.h"
#include "setup/WiFiManager.h"

// utility includes
#include "utility/FactoryResetHandler.h"
#include "utility/StatusLEDController.h"
#include "utility/BuzzerController.h"

// sensor includes
#include "sensors/VEML7700Sensor.h"
#include "sensors/BME280Sensor.h"

WiFiManager wifiManager;
StatusLEDController ledController;
FactoryResetHandler resetHandler(wifiManager, ledController);
VEML7700Sensor lightSensor;
BME280Sensor bmeSensor;
BuzzerController buzzer;

MQTTClientHandler* mqttHandler;

unsigned long lastPublish = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Start!");

  buzzer.begin();
  ledController.begin();
  wifiManager.begin();
  resetHandler.begin();  // ✅ Important: sets pinMode for the button

  Wire.begin(17, 18);  // SDA = IO17, SCL = IO18

  const char* hostname = wifiManager.getHostname();
  mqttHandler = new MQTTClientHandler("192.168.40.6", "mqtt-user", "##DikTrill45", hostname);
  mqttHandler->begin();

  if (!lightSensor.begin()) {
    Serial.println("VEML7700 not found");
  }

  if (!bmeSensor.begin()) {
    Serial.println("BME280 not found");
  }
}

void loop() {
  resetHandler.update();
  mqttHandler->loop();

  unsigned long now = millis();
  if (now - lastPublish > 10000) {
    if (lightSensor.begin()) {
      float lux = lightSensor.readLux();
      mqttHandler->publishLux(lux);
      Serial.print("Published lux: ");
      Serial.println(lux);
    }

    float temperature, humidity, pressure;
    bmeSensor.readValues(temperature, humidity, pressure);

    mqttHandler->publishTemperature(temperature);
    mqttHandler->publishHumidity(humidity);
    mqttHandler->publishPressure(pressure);

    Serial.print("Published temperature: ");
    Serial.println(temperature);
    Serial.print("Published humidity: ");
    Serial.println(humidity);
    Serial.print("Published pressure: ");
    Serial.println(pressure);

    lastPublish = now;
  }
}
