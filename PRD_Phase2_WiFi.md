# Product Requirements Document: The Scout - Phase 2 (Revision 7)

## 1. Phase Goal: Implement a Robust WiFi Manager

The primary goal of this phase is to implement the device's complete startup, connection, and setup logic, including detailed user feedback for each step.

## 2. Module Interactions & Dependencies
This phase involves implementing the logic within the **`WifiHandler`** module. This module **must** interact with:
* The **`FeedbackManager`** module (defined in `PRD_Phase1_Feedback.md`) to provide all user feedback.
* The **`Preferences`** library to manage credentials.

---

## 3. User Stories & Acceptance Criteria

### User Story 1: The First-Time Setup Experience
**As a new user**, I want a guided, visually clear setup process that helps me connect my Scout to my WiFi network easily, even if I have a hidden network, so that I can get the device online without frustration or needing a manual.

#### Acceptance Criteria for First-Time Setup:
* [ ] **Trigger**: This entire flow is triggered **only** if the `Preferences` library contains no saved WiFi credentials upon boot.
* [ ] **Captive Portal Activation**: The device must create an open WiFi Access Point (AP). When a user connects to this AP, a captive portal **must** open automatically on their device.
* [ ] **Welcome Overlay**:
    * Upon loading, the captive portal page **must** immediately display a welcome overlay.
    * This overlay will contain a welcome message and a short (2-3 sentences) introduction explaining that the user is about to connect their Scout to their home WiFi.
    * The user must manually close this overlay to proceed.
* [ ] **Network Discovery & Display**:
    * After the overlay is closed, the main setup form becomes visible.
    * The page **must** automatically scan for available WiFi networks.
    * The list of networks presented in the dropdown **must** be sorted by signal strength (strongest first) and **must not** contain duplicate SSIDs.
* [ ] **Form Interaction**:
    * When the user selects a network from the dropdown, the cursor's focus **must** automatically move to the password input field.
    * The form **must** include a checkbox labeled "Connect to a hidden network".
    * If the "hidden network" box is checked, the network dropdown **must** be hidden, and a new text input field for the SSID **must** appear. The form validation must adapt to this state.
* [ ] **Connection Process**:
    * After the user fills in the details and clicks "Connect", another overlay **must** appear, indicating that the device is "Connecting...".
    * If the connection is successful, this overlay **must** display a large green checkmark.
* [ ] **Finalization**:
    * Upon successful connection, the device **must** save the credentials to the `Preferences` library.
    * The device **must** provide the universal success feedback (see Section 3.C).
    * After a 3-second delay showing the success checkmark, the device **must** automatically restart.

### User Story 2: The Reliable Reconnection Experience
**As a user**, once my Scout is set up, I want it to automatically and reliably reconnect to my WiFi whenever it powers on or the connection drops, and to keep trying intelligently if my network is temporarily down, so that I can trust it will always be online.

#### Acceptance Criteria for Reconnection:
* [ ] **Trigger**: This flow is triggered **only** if the `Preferences` library contains saved WiFi credentials upon boot.
* [ ] **Initial Connection Attempt**: The device **must** immediately try to connect to the saved network.
* [ ] **Fast Retry Loop**:
    * If the initial connection attempt fails, the device **must** retry **3 more times** with a short delay between each attempt.
* [ ] **Exponential Backoff Retry Loop**:
    * If all 4 initial attempts fail, the device **must** switch to a long-delay retry strategy.
    * It will wait for `n + 1` minutes before the next retry attempt, where `n` is the number of failed retry blocks.
    * The value of `n` **must** be capped at a maximum of `10`, resulting in a maximum wait time of 11 minutes between retries.
    * The device **must** remain in this retry loop indefinitely and **must not** start the captive portal.

### C. Universal Feedback Logic
This section defines the specific LED and Buzzer feedback for the different states of the WiFi connection process.

| State / Event                 | System LED (`PIXEL_SYSTEM`) Feedback                                | Buzzer Feedback                               |
| ----------------------------- | ------------------------------------------------------------------- | --------------------------------------------- |
| **Device Powers On** | Off                                                                 | Play a single, short beep (100-200ms).        |
| **AP Mode is Active** | **Slowly Flashing Blue** (e.g., `PULSE_SLOW` with `HEARTHGUARD_BLUE`) | None.                                         |
| **Connecting to Saved WiFi** | **Fast Flashing Orange** (e.g., `SMOOTH_FLASH_FAST` with `HEARTHGUARD_ORANGE`) | None.                                         |
| **Connection Succeeded** | **Flash Green 3 times fast** (e.g., `SMOOTH_FLASH_FAST` with `HEARTHGUARD_GREEN`) | Play `playFanfare()` sound.                   |
| **Connection Retry Block Fails** | **Flash Red 3 times fast** (e.g., `SMOOTH_FLASH_FAST` with `HEARTHGUARD_RED`) | Play `playHornBlast()` sound.                 |

### D. Credential & Device Name Management
1.  **Storage**: All credentials and the device name **must** be stored using the `Preferences` library.
2.  **Namespace Requirement**: All interactions with the `Preferences` library for saving or loading WiFi credentials and the Device ID **must** use the `"credentials"` namespace (e.g., `preferences.begin("credentials", false);`). This is to keep them separate from user settings.
3.  **Device Name Generation**: During the very first successful setup, generate and store a unique device name in the format `TheScout-XXXXXX`.

---

## 4. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Implement the Exact Logic Flow**: Adhere strictly to the two distinct user stories and their acceptance criteria.
2.  **Implement Feedback Precisely**: The feedback calls to the `FeedbackManager` **must** match the states defined in the "Universal Feedback Logic" table (Section 3.C) exactly.
3.  **Use Specified Libraries**: You **must** use the `WiFiManager` library (`tzapu/WiFiManager`).
4.  **Code Structure**: All logic for this phase must be encapsulated within the `WifiHandler` class (`network/WifiHandler.h` & `.cpp`).
5.  **Verification**: After generating the code, you must verify it by running the command `pio run`. The build **must** succeed without any errors.