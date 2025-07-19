# Product Requirements Document: The Scout - Phase 2 (Revision 3)

## 1. Phase Goal: Implement a Robust WiFi Manager

The primary goal of this phase is to create a user-friendly and reliable WiFi setup mechanism for "The Scout" device. The device must be able to connect to a user's local WiFi network through a captive portal if and only if it has no credentials stored. If credentials are stored, it must attempt to connect indefinitely without starting the portal.

## 2. Core Features & Logic

### A. Connection Logic Flow
The device's startup and connection logic must follow this exact sequence:

1.  **Check for Credentials**: On boot, check the `Preferences` library for stored WiFi credentials.
2.  **Scenario A: Credentials Exist**
    * Attempt to connect to the stored WiFi network.
    * **On Success**: Proceed to normal operation.
    * **On Failure**: Begin the "Persistent Retry" strategy (see Section 2B). **Crucially, do not fall back to AP/Captive Portal mode.**
3.  **Scenario B: No Credentials Exist**
    * Immediately start the Captive Portal / AP Mode (see Section 2C).

### B. Persistent Retry Strategy (For Saved Credentials)
This strategy only applies if the device has successfully connected to a network at least once in the past and is now failing to reconnect.

1.  **Initial Attempts**: Try to reconnect 5 times, with a 10-second delay between each attempt.
2.  **Long-Term Attempts**: If the initial 5 attempts fail, switch to a long-delay retry strategy, attempting to reconnect once every 5 minutes.

### C. Captive Portal / AP Mode
This mode should only activate if no credentials are found in `Preferences`.

#### C.1. Webpage User Interface & Functionality Requirements
The captive portal page must be implemented with the following specific UI/UX features:

1.  **Branding & Text**:
    * **Main Header**: `The Scout`
    * **Introductory Text**: Replace the sub-header with a short introduction. For example: "Welcome to your new Scout device. To get started, please select your home WiFi network from the list below and enter the password to bring it online."
    * **Connect Button Text**: `Connect to Network`
2.  **WiFi Network Selection**:
    * **Automatic Scan**: On page load, the page must immediately display a "Scanning for networks..." loader overlay. The scan for WiFi networks must start automatically without requiring a button press.
    * **Clean SSID List**: The page must use JavaScript to fetch a list of available SSIDs from a dedicated endpoint on the ESP32 (e.g., `/api/scan`). The server response must be processed to create a **clean, de-duplicated list showing only the SSID names**. This list populates a dropdown menu (`<select>` element).
3.  **Password Input**:
    * The password field must include a button or icon that, when pressed, toggles the input field's type between `password` and `text` to allow the user to check their entry.
4.  **Button State**:
    * The "Connect to Network" button must be disabled by default.
    * It should only become enabled after the user has selected a network from the dropdown list.

#### C.2. Visual Target
The final rendered webpage should look exactly like this screenshot.

![Captive Portal UI Target](images/captive_portal_target.png)
*(Note: You will need to create this screenshot and place it in an `images` folder in your project for this link to work.)*

#### C.3. Webpage Post-Submission Flow
1.  After the user clicks "Connect to Network," the device should serve a new page with a "Connecting... Please wait." message.
2.  **On First-Time Success**: The device saves the credentials and reboots into normal operation.
3.  **On First-Time Failure**: If the connection fails during this initial setup, the device must **not** enter the persistent retry loop. Instead, it must immediately restart the AP and return the user to the initial configuration page with an error message (e.g., "Connection failed. Please check the password and try again.").

### D. Credential & Device Name Management
1.  **Storage**: All credentials and the device name **must** be stored using the `Preferences` library.
2.  **Device Name Generation**: During the very first successful setup via the captive portal, the device must:
    * Check if a device name already exists in `Preferences`.
    * If not, generate a unique device name in the format `TheScout-XXXXXX`, where `XXXXXX` is a random 6-digit hexadecimal string.
    * Store this name in `Preferences`.

### E. User Feedback (Integration with Phase 1 Modules)
The `Feedback` module must be used to provide clear status indications:

| State                               | System LED (IO09) Pattern         | Buzzer Action                       |
| ----------------------------------- | --------------------------------- | ----------------------------------- |
| In AP / Captive Portal Mode         | Pulsing Blue                      | None                                |
| Connecting (with saved credentials) | Flashing Blue                     | None                                |
| Connection Succeeded                | Flash Green twice                 | `playSuccessChime()`                |
| Connection Failed (with credentials)| Flash Red twice (at end of 5 tries) | `playFailureTone()` (at end of 5 tries) |

## 3. Deferred Functionality

* **Factory Reset**: The logic for the factory reset button (IO02) is **out of scope** for this phase and will be implemented later.

## 4. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Implement the Logic Flow**: Adhere strictly to the connection and failure logic defined in Sections 2A, 2B, and 2C.3.
2.  **Web Server Endpoints**: The web server must handle requests for `/`, `/api/scan`, and the form submission. The `/api/scan` endpoint must return a JSON array of unique SSID strings.
3.  **Code Structure**: Create a dedicated `Network` module (`Network.h`/`.cpp`) to encapsulate all setup and connection logic. The HTML, CSS, and JavaScript for the portal should be in a separate `.h` file.
4.  **Verification**: After generating the code, you must verify it by running the command `pio run`. The build **must** succeed without any errors. The final test is to confirm all UI elements and logic flows work exactly as described.