#pragma once
#include "../Config.h"
#include <Arduino.h>

class TamperDetection {
public:
    void begin();
    void update();
    bool isTampered() const;
    
    using TamperCallback = std::function<void(bool)>;
    void setTamperCallback(TamperCallback callback);
    
private:
    int lastState = HIGH;
    unsigned long lastDebounceTime = 0;
    static constexpr unsigned long debounceDelay = 50;
    TamperCallback tamperCallback;
};
