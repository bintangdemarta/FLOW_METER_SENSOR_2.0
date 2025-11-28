# Firmware ESP32 untuk IoT Water Flow Meter

Firmware ini dirancang untuk mikrokontroler ESP32 untuk membaca sinyal dari sensor flow meter YF-S201 dan mengirimkan data ke server backend.

## Fitur
- Pembacaan sinyal pulse dari sensor flow meter menggunakan interrupt
- Perhitungan flow rate dalam L/min
- Akumulasi total volume air yang mengalir
- Pengiriman data ke server backend via WiFi

## Konfigurasi Awal
Sebelum meng-upload firmware ke ESP32, pastikan untuk mengganti parameter berikut:

1. `ssid` dan `password` WiFi Anda
2. `serverUrl` dengan URL server backend Anda
3. `deviceId` dengan ID unik untuk perangkat Anda

## Cara Penggunaan
1. Install library yang diperlukan:
   - WiFi.h (sudah termasuk dalam framework ESP32)
   - HTTPClient.h (sudah termasuk dalam framework ESP32)
   - ArduinoJson.h (harus diinstall melalui Library Manager)

2. Buka file `flow_meter_firmware.ino` di Arduino IDE

3. Edit nilai berikut di bagian atas file:
   - `ssid`: Nama jaringan WiFi Anda
   - `password`: Password jaringan WiFi Anda
   - `serverUrl`: Alamat server tempat mengirim data
   - `deviceId`: ID unik perangkat Anda

4. Upload firmware ke ESP32

## Skema Koneksi
| Kabel Sensor | Fungsi | Pin ESP32 | Catatan |
|--------------|--------|-----------|---------|
| Merah | VCC (+5V) | VIN / 5V | Butuh supply 5V agar akurat |
| Hitam | GND | GND | Ground harus disatukan |
| Kuning | Signal (Pulse) | GPIO 4 | Gunakan pin yang mendukung Interrupt |

CATATAN: Karena ESP32 bekerja pada logika 3.3V dan sensor seringkali mengeluarkan sinyal 5V, disarankan menggunakan Voltage Divider (Resistor) pada kabel sinyal Kuning sebelum masuk ke GPIO 4 untuk keamanan jangka panjang.

## Prinsip Kerja
1. Firmware membaca sinyal pulse dari sensor melalui interrupt
2. Setiap detik, firmware menghitung jumlah pulse dan mengonversinya ke flow rate menggunakan rumus:
   `FlowRate (L/min) = (Frekuensi (Hz)) / 7.5`
3. Total volume dihitung berdasarkan flow rate
4. Data dikirim ke server dalam bentuk JSON setiap detik