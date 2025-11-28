/*
 * Firmware Display Multi-IoT Water Flow Meter untuk ESP32
 *
 * Firmware ini mengambil data dari API server backend dan menampilkannya
 * pada layar OLED SSD1306 0.96" I2C untuk beberapa perangkat
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

// Daftar device ID yang akan ditampilkan (ubah sesuai kebutuhan)
const int MAX_DEVICES = 3; // Maksimum 3 perangkat
String deviceIds[MAX_DEVICES] = {"ESP32_FLOW_METER_001", "ESP32_DAPUR", "ESP32_KAMAR_MANDI"};
int validDeviceCount = 3; // Jumlah perangkat valid

// Struktur untuk menyimpan data per perangkat
struct DeviceData {
  String deviceId;
  float flow_rate;
  float total_volume;
  String timestamp;
  bool valid;
};

DeviceData devices[MAX_DEVICES];

// Konfigurasi layar OLED SSD1306 0.96"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Interval pengambilan data (dalam ms)
const unsigned long UPDATE_INTERVAL = 10000; // Ambil data setiap 10 detik
unsigned long lastUpdate = 0;

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

  // Inisialisasi data perangkat
  for(int i = 0; i < MAX_DEVICES; i++) {
    devices[i].deviceId = deviceIds[i];
    devices[i].flow_rate = 0.0;
    devices[i].total_volume = 0.0;
    devices[i].timestamp = "N/A";
    devices[i].valid = false;
  }
}

void loop() {
  unsigned long currentTime = millis();

  // Ambil data dari server secara berkala
  if (currentTime - lastUpdate >= UPDATE_INTERVAL) {
    fetchAllDataFromAPI();
    lastUpdate = currentTime;
  }

  // Tampilkan data terbaru di layar
  displayMultiDeviceData();

  delay(100); // Delay kecil untuk stabilitas
}

void fetchAllDataFromAPI() {
  Serial.println("Fetching data for all devices...");
  
  for(int i = 0; i < validDeviceCount; i++) {
    fetchDataForDevice(deviceIds[i], i);
    delay(1000); // Delay antar permintaan untuk menghindari flooding server
  }
}

void fetchDataForDevice(String deviceId, int deviceIndex) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // URL untuk mengambil data dengan filter device_id (endpoint baru lebih ringkas)
    String requestUrl = String(serverUrl) + "?device_id=" + deviceId;

    http.begin(requestUrl);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code for " + deviceId + ": " + String(httpResponseCode));
      Serial.println("Response: " + response);

      // Parse JSON response
      DynamicJsonDocument doc(512); // Buffer lebih kecil karena data lebih ringkas
      DeserializationError error = deserializeJson(doc, response);

      if (error == DeserializationError::Ok) {
        // Endpoint baru langsung mengembalikan objek data bukan array
        JsonObject root = doc.as<JsonObject>();

        // Ambil data dari JSON
        devices[deviceIndex].flow_rate = root["flow_rate"] | 0.0;
        devices[deviceIndex].total_volume = root["total_volume"] | 0.0;
        devices[deviceIndex].timestamp = root["reading_time"] | "N/A";
        devices[deviceIndex].valid = true;

        Serial.println("Device " + deviceId + " - Flow Rate: " + String(devices[deviceIndex].flow_rate, 2) + " L/min");
        Serial.println("Device " + deviceId + " - Total Volume: " + String(devices[deviceIndex].total_volume, 2) + " L");
        Serial.println("Device " + deviceId + " - Timestamp: " + devices[deviceIndex].timestamp);
      } else {
        Serial.println("JSON Parse Error for " + deviceId + ": " + String(error.c_str()));
        setErrorDataForDevice(deviceIndex);
      }
    } else {
      Serial.print("Error fetching data for " + deviceId + ". HTTP Code: ");
      Serial.println(httpResponseCode);
      setErrorDataForDevice(deviceIndex);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected, cannot fetch data for " + deviceId);
    setErrorDataForDevice(deviceIndex);
  }
}

void setErrorDataForDevice(int deviceIndex) {
  // Set nilai default ketika terjadi error
  devices[deviceIndex].flow_rate = 0.0;
  devices[deviceIndex].total_volume = 0.0;
  devices[deviceIndex].timestamp = "Error";
  devices[deviceIndex].valid = false;
}

void displayMultiDeviceData() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  // Header
  display.println("FLOW METERS");
  display.drawLine(0, 12, 127, 12, SSD1306_WHITE);

  int yPos = 16;
  int devicesDisplayed = 0;

  // Tampilkan data untuk setiap perangkat
  for(int i = 0; i < validDeviceCount && devicesDisplayed < 2; i++) { // Batasi tampilan untuk 2 perangkat agar tidak terlalu padat
    if(yPos > 50) break; // Jika sudah hampir penuh, hentikan
    
    display.setCursor(0, yPos);
    
    String status = devices[i].valid ? "OK" : "ERR";
    String deviceName = devices[i].deviceId.substring(0, 8); // Ambil 8 karakter pertama
    
    display.print(deviceName + ":");
    display.setCursor(50, yPos);
    display.print(String(devices[i].flow_rate, 1) + "L/m");
    display.setCursor(90, yPos);
    display.print(status);
    
    yPos += 10; // Pindah baris
    devicesDisplayed++;
  }

  // Jika masih ada ruang, tampilkan informasi tambahan
  if(yPos < 50) {
    display.setCursor(0, yPos);
    display.print("Vol:");
    display.setCursor(30, yPos);
    display.print(String(devices[0].total_volume, 1) + "L");
    
    display.setCursor(70, yPos);
    display.print("IP:");
    display.setCursor(95, yPos);
    display.print(WiFi.localIP().toString().substring(0, 5) + "..");
  }

  display.display();
}