# Panduan Instalasi dan Pengujian Aplikasi IoT Water Flow Meter

## 1. Instalasi dan Setup

### A. Database
1. Buka phpMyAdmin di XAMPP
2. Buat database baru bernama `db_flowmeter`, atau
3. Import file skrip database dari `docs/database_schema.sql` ke phpMyAdmin

### B. Backend (PHP)
1. Pastikan XAMPP dengan Apache dan MySQL sudah berjalan
2. Salin folder `backend` ke direktori `htdocs` di XAMPP
3. Sesuaikan konfigurasi database di file `backend/api/config.php` jika diperlukan

### C. Frontend (Dashboard)
1. Pastikan Apache server berjalan
2. Akses dashboard melalui browser: `http://localhost/FLOW_METER_SENSOR_2.0/flow_meter_app/frontend/index.html`

### D. Firmware (ESP32)
1. Install Arduino IDE dan tambahkan board ESP32
2. Install library ArduinoJson melalui Library Manager
3. Buka file `firmware/src/flow_meter_firmware.ino` di Arduino IDE
4. Edit konfigurasi:
   - SSID dan password WiFi
   - URL server (pastikan bisa diakses dari jaringan yang sama)
   - Device ID unik
5. Upload firmware ke ESP32

## 2. Pengujian Fungsi

### A. Pengujian Backend API
1. Buka Postman atau alat pengujian API lainnya
2. Lakukan POST request ke `http://localhost/FLOW_METER_SENSOR_2.0/flow_meter_app/backend/api/receive_data.php`
3. Kirim data dalam format JSON:
   ```json
   {
     "device_id": "ESP32_DAPUR",
     "flow_rate": 2.5,
     "total_volume": 10.3
   }
   ```
4. Pastikan respons API sukses dan data masuk ke database

### B. Pengujian Dashboard
1. Buka `http://localhost/FLOW_METER_SENSOR_2.0/flow_meter_app/frontend/index.html`
2. Pastikan dashboard bisa mengakses API untuk menampilkan data
3. Uji fungsi refresh dan filter perangkat
4. Pastikan grafik dan tabel data muncul dengan benar

### C. Pengujian Firmware
1. Hubungkan sensor flow meter ke ESP32 sesuai skema
2. Nyalakan perangkat dan amati output di Serial Monitor (baud rate: 115200)
3. Pastikan data dikirimkan ke server setiap detik
4. Cek database untuk memastikan data masuk dengan benar

## 3. Troubleshooting

### A. Tidak bisa kirim data dari ESP32
- Pastikan WiFi terhubung dengan benar
- Cek apakah server API bisa diakses dari perangkat lain di jaringan yang sama
- Pastikan Device ID sudah terdaftar di tabel `devices`

### B. Dashboard tidak menampilkan data
- Pastikan API endpoint bisa diakses dari browser
- Cek koneksi internet dan akses ke file JavaScript
- Pastikan database sudah terisi dengan data

### C. Pembacaan sensor tidak akurat
- Pastikan faktor kalibrasi disesuaikan dengan sensor yang digunakan
- Cek koneksi kabel dan tegangan input sensor
- Pastikan menggunakan voltage divider jika diperlukan