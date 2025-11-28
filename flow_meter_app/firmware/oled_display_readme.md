# Flow Meter Sensor Project - OLED Display Integration

This project extends the existing flow meter sensor system to include an SSD1306 OLED display that shows real-time data fetched from the API.

## New Files Added

### Firmware Files
1. `oled_display_firmware.ino` - Basic OLED display firmware for single device
2. `multi_device_oled_display_firmware.ino` - Advanced OLED display firmware for multiple devices
3. `oled_display_instructions.md` - Setup and configuration instructions

## Features

### Single Device Display (`oled_display_firmware.ino`)
- Fetches data from the API endpoint
- Displays flow rate (L/min)
- Displays total volume (L)
- Shows timestamp of last reading
- Displays device status (Flowing/Idle/Error)
- Shows ESP32 IP address

### Multi-Device Display (`multi_device_oled_display_firmware.ino`)
- Fetches data for multiple devices
- Displays summary for up to 3 devices
- Shows flow rate and status for each device
- Efficient display layout for multiple sensors

## Hardware Setup

### Components Required
- ESP32 development board
- SSD1306 0.96" I2C OLED display (128x64 pixels)

### Wiring Connections
- OLED VCC → ESP32 3.3V
- OLED GND → ESP32 GND
- OLED SDA → ESP32 GPIO 21
- OLED SCL → ESP32 GPIO 22

## Software Setup

### Required Libraries
Install the following libraries using the Arduino IDE Library Manager:
- Adafruit SSD1306
- Adafruit GFX Library
- ArduinoJson
- WiFi (part of ESP32 core)
- HTTPClient (part of ESP32 core)
- Wire (for I2C, part of ESP32 core)

### Configuration
Before uploading the firmware, update these values in the code:

```cpp
const char* ssid = "YOUR_WIFI_SSID";        // Your WiFi SSID
const char* password = "YOUR_WIFI_PASSWORD"; // Your WiFi password
const char* serverUrl = "http://YOUR_SERVER_IP/FLOW_METER_SENSOR_2.0/flow_meter_app/backend/api/get_readings.php"; // Your server URL
const char* deviceId = "ESP32_FLOW_METER_001"; // Your device ID from the database
```

Change `YOUR_SERVER_IP` to the IP address of your computer running the XAMPP server.

## API Integration

The firmware fetches data from the existing API endpoint:
- Endpoint: `GET /backend/api/get_readings.php`
- Parameters: `?device_id=DEVICE_ID&limit=1`
- Returns: JSON with the latest flow meter reading

## Usage

1. Upload the chosen firmware to your ESP32
2. Power on the system
3. The OLED will show "Initializing..." while connecting to WiFi
4. Once connected, it will display the current IP address
5. After connecting to the API, it will start showing flow meter data
6. Data updates every 5-10 seconds depending on the firmware version

## Troubleshooting

- If the display shows "Error" status, check WiFi connectivity and API endpoint accessibility
- Ensure your server is accessible from the ESP32's network
- Verify the device ID exists in the database
- Check all wiring connections between ESP32 and OLED display
- Verify the API endpoint URL is correct and accessible