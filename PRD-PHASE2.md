# Product Requirements Document: The Scout - Phase 2

## 1. Phase Goal: Implement a Robust WiFi Manager

The primary goal of this phase is to create a user-friendly and reliable WiFi setup mechanism for "The Scout" device. The device must be able to connect to a user's local WiFi network through a captive portal. This phase focuses exclusively on network connectivity and initial device configuration.

---

## 2. Core Feature: Branded Captive Portal for WiFi Setup

### User Story
**As a new user**, I want to set up my Scout device easily by selecting my home WiFi network from a list and entering my password, so I can get it online without technical expertise or typing errors.

### Acceptance Criteria
1.  **AP Mode Trigger**: On first boot, or if saved WiFi credentials fail to connect, the device must create its own WiFi Access Point (AP).
    * The AP SSID should be `Guardian-Scout-Setup`.
2.  **Automatic Captive Portal**: When a user connects to the `Guardian-Scout-Setup` AP with a phone or laptop, a captive portal should open automatically, directing them to the configuration page.
3.  **Branded Configuration Page**: The web page must be visually appealing and clearly branded.
    * **Header**: "Guardian Security System"
    * **Sub-header**: "The Scout: Network Setup"
    * **Introduction**: A brief (2-3 sentences) explanation, e.g., "Welcome! Let's get your Scout connected. Please select your home WiFi network from the list below and enter the password."
    * **WiFi Scanning**: The device must perform a WiFi scan and dynamically populate a dropdown menu on the webpage with the SSIDs of all detected networks. This allows the user to select their network instead of typing the name manually.
    * The page must provide a password input field. When a network is selected from the dropdown, its name should fill a (potentially read-only) SSID field.
4.  **Credential Handling**:
    * Upon submission, the device will attempt to connect to the selected WiFi network.
    * The WiFi credentials (SSID and password) **must** be securely stored in the ESP32's non-volatile memory using the `Preferences` library.
5.  **Unique Device ID**: After a successful first-time connection, the device must:
    * Generate a unique `DeviceID`.
    * The format must be `TheScout-XXXXXX`, where `XXXXXX` is a unique identifier (e.g., from the last 3 bytes of the MAC address).
    * This `DeviceID` **must** also be saved using the `Preferences` library.
6.  **User Feedback (Using Phase 1 Modules)**:
    * **System LED (IO09)**:
        * Use the `Feedback` module to show a *Pulsing Blue* pattern while in AP/Setup mode.
        * Use the `Feedback` module to show a *Blinking Yellow* pattern while attempting to connect.
        * Use the `Feedback` module to show a *Solid Green for 5 seconds* pattern on success.
    * **Buzzer (IO40)**:
        * Use the `Feedback` module to `playSuccessChime()` upon successful WiFi connection.
        * Use the `Feedback` module to `playFailureTone()` if the connection fails.
7.  **Success & Reboot**: After a successful connection, the device should display a success page ("Setup complete! Your Scout is now connected.") for 10 seconds and then reboot to begin normal operation.

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Integrate Phase 1 Modules**: You must `#include` and use the `Feedback` module created in Phase 1 to provide user feedback.
2.  **Use Specified Libraries**:
    * `WiFi.h`, `WebServer.h`, `DNSServer.h` for the captive portal functionality.
    * `Preferences.h` for storing all configuration data (WiFi credentials, DeviceID).
3.  **Code Structure**:
    * Create a dedicated `Network` module (`Network.h`/`.cpp`) to encapsulate all setup logic.
    * The HTML/CSS for the web page should be stored in a separate `.h` file as a `const char[]` PROGMEM variable. This HTML must include JavaScript to fetch the list of WiFi networks from a specific endpoint (e.g., `/scan`) and populate the dropdown.
4.  **Verification**: After generating the code, you must verify it by running the command `pio run`. The build **must** succeed without any errors. The final test is to confirm the captive portal works, it correctly lists nearby WiFi networks, and the device successfully connects after submission.