#pragma once
#include "../Config.h"
#include <Arduino.h>
#include <functional>

enum class FactoryResetStage {
    START,
    IN_PROGRESS,
    COMPLETE,
    FAILED,
    CANCELLED
};

class FactoryResetHandler {
public:
    using ResetCallback = std::function<void(FactoryResetStage)>;

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
