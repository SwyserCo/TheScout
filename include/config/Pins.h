#ifndef PINS_H
#define PINS_H

// The Scout - Hardware Pin Configuration
// ESP32-S3 DevKit GPIO Pin Mapping

// Factory Reset & User Input
#define FACTORY_RESET_BUTTON    2   // IO02 - Input, requires pull-up

// LED Control (Separate LEDs, not WS2812B chain)
#define SYSTEM_LED_PIN          3   // IO03 - System Status LED
#define ACTIVITY_LED_PIN       45   // IO45 - Activity Status LED

// Sensors & Detection
#define ACCEL_INTERRUPT_PIN     9   // IO09 - Accelerometer interrupt
#define LD2420_INTERRUPT_PIN   10   // IO10 - mmWave presence interrupt

// Relay & Power Control
#define RELAY_CONTROL_PIN      12   // IO12 - Relay control output
#define CHARGED_STATUS_PIN     14   // IO14 - Battery charge status
#define POWER_GOOD_PIN         21   // IO21 - Power good status

// LD2420 mmWave Sensor Communication
#define LD2420_RX_PIN          15   // IO15 - Connect to LD2420 TX
#define LD2420_TX_PIN          16   // IO16 - Connect to LD2420 RX

// I2C Communication
#define I2C_SDA_PIN            17   // IO17 - I2C Data
#define I2C_SCL_PIN            18   // IO18 - I2C Clock

// USB Communication (if used)
#define USB_DP_PIN             19   // IO19 - USB D+
#define USB_DN_PIN             20   // IO20 - USB D-

// Audio & Analog
#define BUZZER_PIN             40   // IO40 - Buzzer PWM output
#define SPL_MICROPHONE_PIN     41   // IO41 - Sound level microphone (ADC)

#endif // PINS_H
