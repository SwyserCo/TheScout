#pragma once
#include <Adafruit_NeoPixel.h>
#include "config/Colors.h"

class StatusLEDController {
public:
  StatusLEDController();
  void begin();
  
  void flashSystemLED(uint32_t color, int times);
  void setSystemLED(uint32_t color);

  void flashActivityLED(uint32_t color, int times);
  void setActivityLED(uint32_t color);

private:
  static constexpr uint8_t SYSTEM_LED_PIN = 3;
  static constexpr uint8_t ACTIVITY_LED_PIN = 45;

  static constexpr int FLASH_DURATION = 300;
  static constexpr int FLASH_INTERVAL = 300;

  Adafruit_NeoPixel systemLED;
  Adafruit_NeoPixel activityLED;
};
