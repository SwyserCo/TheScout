#pragma once

/**
 * @file Pins.h
 * @brief Hardware pin assignments for HearthGuard: The Scout
 * 
 * Pin assignments from HARDWARE_SPEC.md - Single source of truth for all GPIO
 */

// ==========================================
// User Interface Pins
// ==========================================

// Factory Reset Button - Active low, requires external pull-up
#define FACTORY_RESET_BTN_PIN   2

// ==========================================
// LED & Visual Feedback Pins
// ==========================================

// System LED (WS2812B) - System status feedback
#define SYSTEM_LED_PIN          3

// Activity LED (WS2812B) - Activity status feedback  
#define ACTIVITY_LED_PIN        45

// Charge Status LED - Simple digital output, mirrors IO14 state  
#define CHARGE_STATUS_LED_PIN   42

// ==========================================
// Sensor Interface Pins
// ==========================================

// PIR Sensor Output - Digital input from logic level shifter  
#define PIR_SENSOR_PIN          13

// LD2410S UART Interface
#define LD2410S_RX_PIN          15  // Connects to LD2410S TX pin
#define LD2410S_TX_PIN          16  // Connects to LD2410S RX pin

// LD2410S Interrupt - Digital input for presence/motion detection
#define LD2410S_INTERRUPT_PIN   48

// ==========================================
// Power Management Pins
// ==========================================

// Charge Status Input - HIGH = Charging, LOW = Charge Complete
#define CHARGE_STATUS_PIN       14

// Power Good Input - HIGH = External Power, LOW = On Battery
#define POWER_GOOD_PIN          21

// ==========================================
// Audio Feedback Pin
// ==========================================

// Buzzer - Output for passive buzzer, requires PWM
#define BUZZER_PIN              41

// ==========================================
// Hardware Configuration Constants
// ==========================================

// Serial Device Configuration
#define LD2410S_BAUD_RATE       256000

// LED Configuration
#define NUM_LEDS                2

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

/** ADC resolution for battery monitoring */
#define ADC_RESOLUTION          12  // 12-bit ADC (0-4095)