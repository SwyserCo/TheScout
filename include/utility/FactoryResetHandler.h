#pragma once
#include <Arduino.h>
#include "setup/WiFiManagerGuardian.h"
#include "StatusLEDController.h"

class FactoryResetHandler {
public:
  FactoryResetHandler(uint8_t buttonPin, WiFiManagerGuardian& wifiManager, StatusLEDController& ledController);
  void update();  // Call this from loop()

private:
  uint8_t buttonPin;
  WiFiManagerGuardian& wifiManager;
  StatusLEDController& ledController;

  bool isPressed = false;
  unsigned long pressStartTime = 0;
  bool resetTriggered = false;
};
