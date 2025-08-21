# Product Requirements Document: The Scout - Phase 0

## 1. Phase Goal: Project Scaffolding & Core Architecture

The primary goal of this phase is to establish the complete, compilable skeleton of the firmware project. This involves creating the entire directory structure, all necessary configuration files, all core libraries, and all class files with empty "stub" methods. The project **must** compile successfully at the end of this phase.

## 2. Core Features & Architecture

### User Story
**As a developer**, I need the foundational structure of my project set up correctly from the start, so I can focus on implementing features in their logical place without worrying about project configuration.

### Acceptance Criteria

#### A. `platformio.ini` Configuration
1.  Create the `platformio.ini` file.
2.  It must define the correct `[env:esp32s3]` with the `platform`, `board`, and `framework`.
3.  It must include the `monitor_speed`.
4.  It must contain a `lib_deps` section that lists **all** of the "Core Libraries & Dependencies" specified in the `Master_PRD.md`. This will ensure PlatformIO automatically downloads them.

#### B. Directory and File Structure
1.  Create the complete directory structure as defined in the `Master_PRD.md`. This includes `include/`, `src/`, and all subdirectories (`config`, `feedback`, `network`, `setup`, `sensors`, `utilities`).
2.  Create all the `.h` and `.cpp` files for every class mentioned in the folder structure.
3.  Each `.h` file should contain the basic class definition with a public constructor, a `begin()` method, and an `update()` method.
4.  Each `.cpp` file should contain the implementations of these methods, which can be empty for now.

#### C. Configuration Files (`include/config/`)
1.  **`Pins.h`**: Create this file. It must contain `#define` statements for every hardware pin listed in the "Pin Mapping" table of the `Master_PRD.md`.
2.  **`Settings.h`**: Create this file. It can be mostly empty for now, but should include placeholders for future settings like `FIRMWARE_VERSION`.

#### D. Main Application Skeleton (`src/main.cpp`)
1.  **Includes**: `main.cpp` must `#include` all the manager/handler header files (e.g., `FeedbackManager.h`, `WifiHandler.h`, etc.).
2.  **Object Instantiation**: It must create global instances of each of the main manager classes.
    * `FeedbackManager feedbackManager;`
    * `WifiHandler wifiHandler;`
    * `DeviceManager deviceManager;`
    * `SensorManager sensorManager;`
    * `MqttHandler mqttHandler;`
3.  **State Machine**: It must define the `enum class SystemState` as described in the `copilot-instructions.md`. A global variable `currentState` should be initialized to a `BOOTING` or `SETUP` state.
4.  **`setup()` function**:
    * Must initialize the Serial monitor.
    * Must call the `begin()` method on each of the instantiated manager objects.
5.  **`loop()` function**:
    * Must contain a `switch` statement for the `SystemState`.
    * Must call the `update()` method for each of the instantiated manager objects on every loop.

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Follow the Blueprint**: Adhere strictly to the folder structure and file creation requirements listed above.
2.  **Create Stubs, Not Logic**: The goal is to create the structure. The methods inside the classes should be empty or contain a single `Serial.println("ClassName::methodName called");` for debugging. Do not implement any real logic.
3.  **Verification**: After generating all the files and code, you must verify it by running the command `pio run`. The build **must** succeed without any errors. This is the primary success metric for this phase.