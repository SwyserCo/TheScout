#pragma once
#include <Arduino.h>
#include "setup/WiFiManager.h"
#include "StatusLEDController.h"

class FactoryResetHandler {
public:
  FactoryResetHandler(uint8_t buttonPin, WiFiManager& wifiManager, StatusLEDController& ledController);
  void update();  // Call this from loop()

private:
  uint8_t buttonPin;
  WiFiManager& wifiManager;
  StatusLEDController& ledController;

  bool isPressed = false;
  unsigned long pressStartTime = 0;
  bool resetTriggered = false;
};
