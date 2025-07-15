#include "utility/StatusLEDController.h"

StatusLEDController::StatusLEDController()
  : systemLED(1, SYSTEM_LED_PIN, NEO_GRB + NEO_KHZ800),
    activityLED(1, ACTIVITY_LED_PIN, NEO_GRB + NEO_KHZ800) {}

void StatusLEDController::begin() {
  systemLED.begin();
  activityLED.begin();
  systemLED.show();
  activityLED.show();
}

void StatusLEDController::flashSystemLED(uint32_t color, int times) {
  for (int i = 0; i < times; i++) {
    systemLED.setPixelColor(0, color);
    systemLED.show();
    delay(FLASH_DURATION);
    systemLED.clear();
    systemLED.show();
    delay(FLASH_INTERVAL);
  }
}

void StatusLEDController::setSystemLED(uint32_t color) {
  systemLED.setPixelColor(0, color);
  systemLED.show();
}

void StatusLEDController::flashActivityLED(uint32_t color, int times) {
  for (int i = 0; i < times; i++) {
    activityLED.setPixelColor(0, color);
    activityLED.show();
    delay(FLASH_DURATION);
    activityLED.clear();
    activityLED.show();
    delay(FLASH_INTERVAL);
  }
}

void StatusLEDController::setActivityLED(uint32_t color) {
  activityLED.setPixelColor(0, color);
  activityLED.show();
}
