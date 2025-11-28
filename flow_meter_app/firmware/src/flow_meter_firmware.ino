/*
 * Firmware IoT Water Flow Meter untuk ESP32
 * 
 * Firmware ini membaca sinyal dari sensor flow meter YF-S201 melalui interrupt
 * dan mengirimkan data ke server backend melalui HTTP POST
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Konfigurasi WiFi
const char* ssid = "YOUR_WIFI_SSID";        // Ganti dengan SSID WiFi Anda
const char* password = "YOUR_WIFI_PASSWORD"; // Ganti dengan password WiFi Anda

// Konfigurasi server
const char* serverUrl = "http://localhost/FLOW_METER_SENSOR_2.0/flow_meter_app/backend/api/receive_data.php";
const char* deviceId = "ESP32_FLOW_METER_001"; // Ganti dengan ID unik perangkat Anda

// Konfigurasi pin
const int FLOW_SENSOR_PIN = 4; // Pin GPIO 4 sebagai contoh

// Variabel untuk perhitungan flow rate
volatile unsigned long pulseCount = 0;
unsigned long lastPulseCount = 0;
unsigned long currentTime = 0;
unsigned long lastTime = 0;
float flowRate = 0.0;
float totalVolume = 0.0;

// Faktor kalibrasi sensor (Q Factor) - untuk YF-S201 umumnya 7.5
const float CALIBRATION_FACTOR = 7.5;

// Fungsi interrupt untuk membaca pulse dari sensor
void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);
  
  // Setup pin sensor
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  
  // Setup interrupt pada pin sensor
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, RISING);
  
  // Setup WiFi
  WiFi.begin(ssid, password);
  Serial.println("Menghubungkan ke WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nTerhubung ke WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Inisialisasi waktu
  lastTime = millis();
}

void loop() {
  currentTime = millis();
  
  // Hitung per 1 detik (1000ms) untuk menghitung flow rate
  if (currentTime - lastTime >= 1000) {
    // Hitung jumlah pulse per detik
    unsigned long pulsesThisSecond = pulseCount - lastPulseCount;
    
    // Hitung flow rate dalam L/min
    // Rumus: FlowRate (L/min) = (Frekuensi (Hz)) / 7.5
    flowRate = (pulsesThisSecond / CALIBRATION_FACTOR) * 60.0;
    
    // Tambahkan volume berdasarkan flow rate
    totalVolume += (flowRate / 60.0) / 1000.0; // Konversi ke liter
    
    // Update nilai sebelumnya
    lastPulseCount = pulseCount;
    lastTime = currentTime;
    
    // Cetak data ke serial untuk debugging
    Serial.print("Flow Rate: ");
    Serial.print(flowRate, 2);
    Serial.print(" L/min, Total Volume: ");
    Serial.print(totalVolume, 2);
    Serial.print(" L, Pulse Count: ");
    Serial.println(pulseCount);
    
    // Kirim data ke server
    sendDataToServer();
  }
  
  // Delay kecil untuk stabilitas
  delay(10);
}

void sendDataToServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Buat URL server
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    // Buat payload JSON
    DynamicJsonDocument doc(256);
    doc["device_id"] = deviceId;
    doc["flow_rate"] = flowRate;
    doc["total_volume"] = totalVolume;
    
    String jsonData;
    serializeJson(doc, jsonData);
    
    // Kirim POST request
    int httpResponseCode = http.POST(jsonData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Response: ");
      Serial.println(response);
    } else {
      Serial.print("Error mengirim data. Kode: ");
      Serial.println(httpResponseCode);
    }
    
    // Bersihkan koneksi
    http.end();
  } else {
    Serial.println("Tidak terhubung ke WiFi, tidak bisa kirim data");
  }
}