# OLED Display Firmware Instructions

This firmware fetches data from the flow meter API and displays it on an SSD1306 OLED display.

## Hardware Requirements
- ESP32 development board
- SSD1306 0.96" I2C OLED display
- Jumper wires for connecting the OLED to ESP32

## Wiring Connection
- OLED VCC → ESP32 3.3V
- OLED GND → ESP32 GND
- OLED SDA → ESP32 GPIO 21
- OLED SCL → ESP32 GPIO 22

## Required Libraries
Before uploading the firmware, install these libraries via Arduino IDE Library Manager:
- Adafruit SSD1306
- Adafruit GFX Library
- ArduinoJson
- WiFi (part of ESP32 core)
- HTTPClient (part of ESP32 core)
- Wire (for I2C)

## Setup Instructions

1. Install the required libraries mentioned above
2. Open `oled_display_firmware.ino` in Arduino IDE
3. Modify the following parameters at the top of the file:

```cpp
const char* ssid = "YOUR_WIFI_SSID";        // Ganti dengan SSID WiFi Anda
const char* password = "YOUR_WIFI_PASSWORD"; // Ganti dengan password WiFi Anda
const char* serverUrl = "http://192.168.1.10/FLOW_METER_SENSOR_2.0/flow_meter_app/backend/api/get_readings.php"; // Ganti dengan IP server Anda
const char* deviceId = "ESP32_FLOW_METER_001"; // Ganti dengan ID perangkat Anda
```

4. Upload the firmware to your ESP32

## Display Layout
The OLED display will show:
- Current flow rate (L/min)
- Total volume (L)
- Timestamp of last reading
- Status (Flowing/Idle/Error)
- ESP32 IP address

## Features
- Automatically connects to WiFi
- Fetches data from the server API every 5 seconds
- Displays flow meter readings in real-time
- Shows status information
- Displays ESP32 IP address for troubleshooting