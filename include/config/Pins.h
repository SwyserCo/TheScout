#pragma once

/**
 * @file Pins.h
 * @brief Hardware pin assignments for HearthGuard: The Scout
 * 
 * This file defines all GPIO pin assignments for ESP32-S3 DevKit.
 * Pin assignments follow the Master PRD specifications.
 */

// ==========================================
// LED & Visual Feedback Pins
// ==========================================

/** System status LED (WS2812B) - Individual pixel */
#define SYSTEM_LED_PIN          3

/** Activity status LED (WS2812B) - Individual pixel */  
#define ACTIVITY_LED_PIN        45

// ==========================================
// Audio Feedback Pin
// ==========================================

/** Buzzer/Speaker pin for audio feedback */
#define BUZZER_PIN              41

// ==========================================
// User Interface Pins
// ==========================================

/** Factory reset button (active low with internal pullup) */
#define FACTORY_RESET_BTN_PIN   0

// ==========================================
// Sensor Interface Pins
// ==========================================

/** PIR motion sensor digital input */
#define PIR_SENSOR_PIN          4

/** LD2410S radar sensor UART pins */
#define LD2410S_RX_PIN          5
#define LD2410S_TX_PIN          6

// ==========================================
// Power Management Pins
// ==========================================

/** Battery voltage monitoring (ADC) */
#define BATTERY_VOLTAGE_PIN     1

/** USB power detection */
#define USB_POWER_DETECT_PIN    2

// ==========================================
// Debug & Communication Pins
// ==========================================

/** Hardware UART0 for Serial Monitor (built-in USB) */
// RX: GPIO44 (built-in)
// TX: GPIO43 (built-in)

/** Hardware UART1 for LD2410S sensor communication */
#define UART1_RX_PIN            LD2410S_RX_PIN
#define UART1_TX_PIN            LD2410S_TX_PIN

// ==========================================
// Pin Configuration Helpers
// ==========================================

/** Number of WS2812B LEDs in the system */
#define NUM_LEDS                2

/** ADC resolution for battery monitoring */
#define ADC_RESOLUTION          12  // 12-bit ADC (0-4095)