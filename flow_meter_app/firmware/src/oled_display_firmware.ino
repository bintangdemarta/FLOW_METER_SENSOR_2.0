/*
 * Firmware Display IoT Water Flow Meter untuk ESP32
 *
 * Firmware ini mengambil data dari API server backend dan menampilkannya
 * pada layar OLED SSD1306 0.96" I2C
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Konfigurasi WiFi
const char* ssid = "YOUR_WIFI_SSID";        // Ganti dengan SSID WiFi Anda
const char* password = "YOUR_WIFI_PASSWORD"; // Ganti dengan password WiFi Anda

// Konfigurasi server API
const char* serverUrl = "http://192.168.1.10/FLOW_METER_SENSOR_2.0/flow_meter_app/backend/api/get_latest_reading.php"; // Ganti dengan IP server Anda
const char* deviceId = "ESP32_FLOW_METER_001"; // Ganti dengan ID perangkat Anda

// Konfigurasi layar OLED SSD1306 0.96"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Interval pengambilan data (dalam ms)
const unsigned long UPDATE_INTERVAL = 5000; // Ambil data setiap 5 detik
unsigned long lastUpdate = 0;

// Variabel untuk menyimpan data dari API
float flow_rate = 0.0;
float total_volume = 0.0;
String timestamp = "N/A";

void setup() {
  Serial.begin(115200);

  // Inisialisasi layar OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Alamat I2C 0x3C adalah default untuk SSD1306
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Hentikan program jika layar tidak ditemukan
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Initializing...");
  display.display();
  delay(2000);

  // Setup WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Connecting to");
  display.println(ssid);
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Connected!");
  display.println(WiFi.localIP());
  display.display();
  delay(2000);
}

void loop() {
  unsigned long currentTime = millis();

  // Ambil data dari server secara berkala
  if (currentTime - lastUpdate >= UPDATE_INTERVAL) {
    fetchDataFromAPI();
    lastUpdate = currentTime;
  }

  // Tampilkan data terbaru di layar
  displayDataOnScreen();

  delay(100); // Delay kecil untuk stabilitas
}

void fetchDataFromAPI() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // URL untuk mengambil data dengan filter device_id (endpoint baru lebih ringkas)
    String requestUrl = String(serverUrl) + "?device_id=" + deviceId;

    http.begin(requestUrl);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);

      // Parse JSON response
      DynamicJsonDocument doc(512); // Buffer lebih kecil karena data lebih ringkas
      DeserializationError error = deserializeJson(doc, response);

      if (error == DeserializationError::Ok) {
        // Endpoint baru langsung mengembalikan objek data bukan array
        JsonObject root = doc.as<JsonObject>();

        // Ambil data dari JSON
        flow_rate = root["flow_rate"] | 0.0;
        total_volume = root["total_volume"] | 0.0;
        timestamp = root["reading_time"] | "N/A";

        Serial.println("Flow Rate: " + String(flow_rate, 2) + " L/min");
        Serial.println("Total Volume: " + String(total_volume, 2) + " L");
        Serial.println("Timestamp: " + timestamp);
      } else {
        Serial.println("JSON Parse Error: " + String(error.c_str()));
        setErrorData();
      }
    } else {
      Serial.print("Error fetching data. HTTP Code: ");
      Serial.println(httpResponseCode);
      setErrorData();
    }

    http.end();
  } else {
    Serial.println("WiFi not connected, cannot fetch data");
    setErrorData();
  }
}

void setErrorData() {
  // Set nilai default ketika terjadi error
  flow_rate = 0.0;
  total_volume = 0.0;
  timestamp = "Error";
}

void displayDataOnScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  // Header
  display.println("FLOW METER DATA");
  display.drawLine(0, 12, 127, 12, SSD1306_WHITE);

  // Flow Rate
  display.setCursor(0, 16);
  display.print("Flow Rate: ");
  display.setTextSize(1);
  display.println(String(flow_rate, 2) + " L/min");

  // Total Volume
  display.setCursor(0, 26);
  display.setTextSize(1);
  display.print("Total Vol: ");
  display.println(String(total_volume, 2) + " L");

  // Timestamp
  display.setCursor(0, 36);
  display.setTextSize(1);
  display.print("Time: ");
  if (timestamp.length() > 10) {
    // Ambil tanggal dan waktu saja (YYYY-MM-DD HH:MM:SS)
    display.println(timestamp.substring(0, 19));
  } else {
    display.println(timestamp);
  }

  // Status
  display.setCursor(0, 46);
  display.setTextSize(1);
  if (timestamp == "Error") {
    display.println("Status: Error");
  } else if (flow_rate > 0) {
    display.println("Status: Flowing");
  } else {
    display.println("Status: Idle");
  }

  // IP Address
  display.setCursor(0, 56);
  display.setTextSize(1);
  display.println("IP: " + WiFi.localIP().toString());

  display.display();
}