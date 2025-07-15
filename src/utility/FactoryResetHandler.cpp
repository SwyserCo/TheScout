#include "utility/FactoryResetHandler.h"
#include "config/Colors.h"

FactoryResetHandler::FactoryResetHandler(WiFiManager& wifiManager, StatusLEDController& ledController)
  : wifiManager(wifiManager), ledController(ledController) {}

#define RESET_BUTTON_PIN 3

void FactoryResetHandler::begin() {
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
}

void FactoryResetHandler::update() {
  bool buttonState = digitalRead(RESET_BUTTON_PIN) == LOW;
  unsigned long now = millis();

  if (buttonState && !isPressed) {
    isPressed = true;
    pressStartTime = now;
    lastFlashTime = now;
    flashState = false;
    Serial.println("Factory reset button pressed.");
  }

  if (!buttonState && isPressed) {
    isPressed = false;
    pressStartTime = 0;
    Serial.println("Factory reset cancelled (button released too early).");
    ledController.setSystemLED(COLOR_OFF);
  }

  if (isPressed && !resetTriggered && (now - pressStartTime >= 5000)) {
    Serial.println("Factory reset confirmed. Clearing Wi-Fi credentials...");
    wifiManager.clearCredentials();

    for (int i = 0; i < 3; i++) {
      ledController.setSystemLED(COLOR_BLUE);
      delay(300);
      ledController.setSystemLED(COLOR_OFF);
      delay(300);
    }

    delay(1000);
    ESP.restart();
    resetTriggered = true;
  }

  // Non-blocking red flash while holding
  if (isPressed && !resetTriggered && (now - lastFlashTime >= 500)) {
    flashState = !flashState;
    ledController.setSystemLED(flashState ? COLOR_RED : COLOR_OFF);
    lastFlashTime = now;
  }
}
