#pragma once

// Pin Mapping for The Scout (ESP32-S3 DevKit)
// Reference: Master PRD.md Hardware Specifications

namespace Pins {
    // Button Inputs
    constexpr uint8_t FACTORY_RESET_BUTTON = 2;   // IO02 - Factory reset button (pull-up required)
    
    // LED Outputs
    constexpr uint8_t SYSTEM_LED = 3;              // IO03 - System status LED
    constexpr uint8_t ACTIVITY_LED = 45;           // IO45 - Activity LED
    
    // Sensor Interrupts
    constexpr uint8_t ACCEL_INTERRUPT = 9;         // IO09 - Accelerometer interrupt
    constexpr uint8_t LD2420_INTERRUPT = 10;       // IO10 - LD2420 presence detection
    
    // Relay & Power Management
    constexpr uint8_t RELAY_CONTROL = 12;          // IO12 - Relay control output
    constexpr uint8_t CHARGED_STATUS = 14;         // IO14 - BQ24074 charge status
    constexpr uint8_t POWER_GOOD = 21;             // IO21 - BQ24074 power good
    
    // LD2420 mmWave Sensor UART
    constexpr uint8_t LD2420_RX = 15;              // IO15 - Connects to LD2420 TX
    constexpr uint8_t LD2420_TX = 16;              // IO16 - Connects to LD2420 RX
    
    // I2C Communication
    constexpr uint8_t I2C_SDA = 17;                // IO17 - I2C Data Line
    constexpr uint8_t I2C_SCL = 18;                // IO18 - I2C Clock Line
    
    // USB (if native USB is used)
    constexpr uint8_t USB_DP = 19;                 // IO19 - USB D+
    constexpr uint8_t USB_DM = 20;                 // IO20 - USB D-
    
    // Audio & Sound
    constexpr uint8_t BUZZER = 40;                 // IO40 - Buzzer (PWM required)
    constexpr uint8_t SPL_MICROPHONE = 41;         // IO41 - SPL Microphone (ADC)
}

// I2C Device Addresses
namespace I2C_Addresses {
    constexpr uint8_t BME280 = 0x76;               // Temperature, Humidity, Pressure
    constexpr uint8_t VEML7700 = 0x10;             // Ambient Light Sensor
    constexpr uint8_t LIS2DW12 = 0x19;             // Accelerometer
}
