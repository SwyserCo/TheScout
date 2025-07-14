#include "utility/FactoryResetHandler.h"

FactoryResetHandler::FactoryResetHandler(uint8_t buttonPin, WiFiManager& wifiManager, StatusLEDController& ledController)
  : buttonPin(buttonPin), wifiManager(wifiManager), ledController(ledController) {}

void FactoryResetHandler::update() {
  pinMode(buttonPin, INPUT_PULLUP);
  bool buttonState = digitalRead(buttonPin) == LOW;

  if (buttonState && !isPressed) {
    // Button just pressed
    isPressed = true;
    pressStartTime = millis();
    Serial.println("Factory reset button pressed.");
  }

  if (!buttonState && isPressed) {
    // Button released before timeout
    isPressed = false;
    pressStartTime = 0;
    Serial.println("Factory reset cancelled (button released too early).");
  }

  if (isPressed && !resetTriggered && (millis() - pressStartTime >= 5000)) {
    Serial.println("Factory reset confirmed. Clearing Wi-Fi credentials...");
    wifiManager.clearCredentials();

    // Flash system LED blue 3 times
    for (int i = 0; i < 3; i++) {
      ledController.setSystemColor(0, 0, 255);  // Blue
      delay(300);
      ledController.clearSystem();
      delay(300);
    }

    delay(1000);
    ESP.restart();
    resetTriggered = true;
  }

  // Optional: flash red every second while holding
  if (isPressed && !resetTriggered && (millis() - pressStartTime) % 1000 < 50) {
    ledController.flashSystemRed(1);
  }
}
