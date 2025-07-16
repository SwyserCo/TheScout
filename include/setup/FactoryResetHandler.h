#pragma once
#include "../Config.h"
#include <Arduino.h>
#include <functional>

class FactoryResetHandler {
public:
    using ResetCallback = std::function<void(void)>;

    void begin();
    void update();
    
    void setResetCallback(ResetCallback callback) {
        resetCallback = callback;
    }

private:
    static constexpr unsigned long RESET_HOLD_TIME = 5000;  // 5 seconds
    unsigned long buttonPressStart = 0;
    bool buttonPressed = false;
    ResetCallback resetCallback;

    void checkButton();
    void performReset();
};
