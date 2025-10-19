

-----

````markdown
# Prompt for GitHub Copilot: Create ESP32 Code for Django MQTT LED Control

**Goal:** Create a complete and robust ESP32 application using PlatformIO and the Arduino framework. The application must connect to WiFi and an MQTT broker to allow the Django dashboard to control the onboard LED (GPIO2).

The code should listen for commands on a `control` topic and report the LED's status back on a `state` topic.

---

### 1. PlatformIO Configuration (`platformio.ini`)

Set up the `platformio.ini` file to include the necessary library for MQTT communication.

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200

# Library dependencies for MQTT
lib_deps =
  knolleary/PubSubClient@^2.8
````

-----

### 2\. Main Application Code (`src/main.cpp`)

Generate the complete C++ code for `src/main.cpp` based on the following detailed requirements.

#### a. Header Includes

Include `Arduino.h`, `WiFi.h`, and `PubSubClient.h`.

#### b. Pin, WiFi, and MQTT Definitions

Define all necessary constants. The MQTT topics must exactly match the ones used in the Django project.

```cpp
// ===== Pin Definitions =====
#define LED_PIN 2 // The onboard LED is on GPIO2 (Active High)

// ===== WiFi Configuration =====
// IMPORTANT: Replace with your actual WiFi credentials
const char* WIFI_SSID = "myHome_2.4GHz";
const char* WIFI_PASSWORD = "0939391546";

// ===== MQTT Configuration =====
const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "ESP_ThaiTechZone_LED_Controller_01"; // A unique name

// --- MQTT Topics (Matching the Django Dashboard) ---
// Topic this ESP32 LISTENS to for commands
const char* LED_CONTROL_TOPIC = "thaitechzone/v2_board/control/led";
// Topic this ESP32 PUBLISHES its status to
const char* LED_STATE_TOPIC = "thaitechzone/v2_board/state/led";
```

#### c. Global Objects and Function Declarations

Instantiate the `WiFiClient` and `PubSubClient` objects, and declare the helper functions.

```cpp
// ===== Global Objects =====
WiFiClient espClient;
PubSubClient mqttClient(espClient);
long last_reconnect_attempt = 0;

// ===== Function Declarations =====
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT();
void publishLedState();
```

#### d. `setup()` Function

The `setup()` function should:

1.  Initialize the Serial Monitor at 115200 baud.
2.  Configure the `LED_PIN` as an `OUTPUT` and set its initial state to `LOW` (OFF).
3.  Call `setup_wifi()` to connect to the network.
4.  Configure the MQTT client with `setServer()` and `setCallback()`.

#### e. `loop()` Function

The `loop()` function's main responsibilities are:

1.  Check if the MQTT client is connected. If not, call `reconnectMQTT()`.
2.  Continuously call `mqttClient.loop()` to process MQTT traffic.

#### f. Helper Functions

1.  **`callback(char* topic, byte* payload, unsigned int length)`**

      * This is the most important function for receiving commands.
      * It must convert the `payload` to an uppercase `String`.
      * Check if the `topic` matches `LED_CONTROL_TOPIC`.
      * If the message is "ON", turn the LED `HIGH`.
      * If the message is "OFF", turn the LED `LOW`.
      * **Crucially**, after changing the LED state, it must immediately call `publishLedState()` to report the new status back to the dashboard.

2.  **`publishLedState()`**

      * This function reads the physical state of the `LED_PIN` using `digitalRead()`.
      * It determines if the current state is "ON" (`HIGH`) or "OFF" (`LOW`).
      * It publishes this state as a string to the `LED_STATE_TOPIC`. The message should be retained (`true`).

3.  **`reconnectMQTT()`**

      * Implement a non-blocking reconnect logic that attempts to connect every 5 seconds if disconnected.
      * When a connection is successfully established, it must **subscribe** to the `LED_CONTROL_TOPIC`.
      * It should also immediately call `publishLedState()` upon connecting to ensure the dashboard shows the correct initial state.

4.  **`setup_wifi()`**

      * A standard function to connect to the WiFi network specified in the constants. It should print the IP address to the Serial Monitor upon successful connection.

---

### 3. Complete Implementation Code

Here's the complete implementation for `src/main.cpp`:

```cpp

```

---

### 4. Installation and Testing Guide

#### Step 1: Hardware Setup
1. Connect your ESP32 to your computer via USB
2. The built-in LED is on GPIO2 (no additional wiring needed)

#### Step 2: Software Setup
1. Install PlatformIO IDE or VS Code with PlatformIO extension
2. Create a new PlatformIO project
3. Copy the `platformio.ini` configuration
4. Copy the complete code to `src/main.cpp`

#### Step 3: Configuration
1. **Update WiFi credentials** in the code:
   ```cpp
   const char* WIFI_SSID = "YOUR_WIFI_NAME";
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
   ```

2. **Optional: Change MQTT Client ID** to make it unique:
   ```cpp
   const char* MQTT_CLIENT_ID = "ESP_YourName_LED_Controller_01";
   ```

#### Step 4: Upload and Test
1. Build and upload the code to ESP32
2. Open Serial Monitor (115200 baud)
3. Verify WiFi and MQTT connections
4. Test with Django Dashboard or MQTT Explorer

#### Step 5: Troubleshooting
- **WiFi not connecting**: Check SSID and password
- **MQTT not connecting**: Verify broker address and port
- **LED not responding**: Check GPIO pin number and wiring
- **Dashboard not updating**: Verify MQTT topics match exactly

---

### 5. MQTT Topic Reference

| Topic | Direction | Purpose | Message Format |
|-------|-----------|---------|----------------|
| `thaitechzone/v2_board/control/led` | Django → ESP32 | Control LED | `ON` or `OFF` |
| `thaitechzone/v2_board/state/led` | ESP32 → Django | Report LED status | `ON` or `OFF` |

---

### 6. Testing with MQTT Explorer

Before testing with Django Dashboard, you can use MQTT Explorer:

1. Download and install MQTT Explorer
2. Connect to `broker.hivemq.com:1883`
3. Subscribe to `thaitechzone/v2_board/state/led`
4. Publish to `thaitechzone/v2_board/control/led` with message `ON` or `OFF`
5. Observe LED changes and state reports

---

### 7. Integration with Django Dashboard

Once the ESP32 is working:

1. Start your Django server: `python manage.py runserver`
2. Open browser to `http://127.0.0.1:8000/`
3. Start MQTT listener: `python manage.py mqtt_listener`
4. Click LED control buttons and observe real-time updates

---

**Created by**: ThaiTechZone  
**Date**: 2025-10-18  
**Version**: 1.0  
**Compatible with**: Django Dashboard Framework v2.0