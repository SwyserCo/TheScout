#include <Arduino.h>
#include "setup/WiFiManager.h"
#include "utility/FactoryResetHandler.h"
#include "utility/StatusLEDController.h"
#include "setup/MQTTClientHandler.h"
#include "sensors/VEML7700Sensor.h"
#include "sensors/BME280Sensor.h"

#define RESET_BUTTON_PIN 2
#define SYSTEM_LED_PIN 3
#define ACTIVITY_LED_PIN 45

WiFiManager wifiManager;
StatusLEDController ledController(SYSTEM_LED_PIN, ACTIVITY_LED_PIN);
FactoryResetHandler resetHandler(RESET_BUTTON_PIN, wifiManager, ledController);
VEML7700Sensor lightSensor;
BME280Sensor bmeSensor;

MQTTClientHandler* mqttHandler;

unsigned long lastPublish = 0;

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("Start!");

    ledController.begin();
    wifiManager.begin();

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

        char tempPayload[16], humPayload[16], presPayload[16];
        dtostrf(temperature, 6, 2, tempPayload);
        dtostrf(humidity, 6, 2, humPayload);
        dtostrf(pressure, 6, 2, presPayload);

        
        mqttHandler->publishTemperature(temperature);
        mqttHandler->publishHumidity(humidity);
        mqttHandler->publishPressure(pressure);


        Serial.print("Published temperature: ");
        Serial.println(tempPayload);
        Serial.print("Published humidity: ");
        Serial.println(humPayload);
        Serial.print("Published pressure: ");
        Serial.println(presPayload);

        lastPublish = now;
    }
}
