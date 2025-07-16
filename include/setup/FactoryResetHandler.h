#pragma once
#include "Config.h"

class FactoryResetHandler {
public:
    void begin();
    void handle();
private:
    const uint8_t resetBtnPin = PIN_FACTORY_RESET_BTN;
};
