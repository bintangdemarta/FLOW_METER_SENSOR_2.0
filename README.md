# 🚰 IoT Water Flow Meter Monitoring System

**IoT Water Flow Meter** adalah sistem pemantauan penggunaan air end-to-end berbasis Internet of Things. Proyek ini menghubungkan sensor fisik (hardware) ke antarmuka web (dashboard) untuk memantau debit air, volume penggunaan, dan estimasi biaya secara *real-time*.

## 📋 Daftar Isi
- [Deskripsi Proyek](#-deskripsi-proyek)
- [Fitur Utama](#-fitur-utama)
- [Arsitektur Sistem](#-arsitektur-sistem)
- [Struktur Proyek](#-struktur-proyek)
- [Persyaratan Sistem](#-persyaratan-sistem)
- [Instalasi](#-instalasi)
- [Penggunaan](#-penggunaan)
- [Testing](#-testing)

## 📘 Deskripsi Proyek
Sistem ini dirancang untuk membaca sinyal *pulse* dari sensor Flow Meter menggunakan mikrokontroler ESP32. Data tersebut diproses menjadi satuan volume (Liter) dan dikirim ke server lokal melalui protokol HTTP untuk disimpan di database MySQL. Pengguna dapat mengakses Dashboard Web untuk melihat statistik penggunaan air secara real-time.

## ✨ Fitur Utama
1. **Dashboard Monitoring Real-time**
   - Menampilkan flow rate saat ini (L/min)
   - Menampilkan total volume air terpakai
   - Indikator status konektivitas perangkat

2. **Analisis Data**
   - Grafik interaktif penggunaan air
   - Estimasi biaya penggunaan air
   - Riwayat pembacaan sensor

3. **Fungsionalitas Tambahan**
   - Deteksi kebocoran (rencana pengembangan lanjutan)
   - Ekspor data ke format CSV

## 🔄 Arsitektur Sistem
Sistem bekerja dengan 5 tahapan utama:

1. **Sensing (Pembacaan)**: Sensor Flow Meter (Hall Effect) menghasilkan sinyal digital (Pulse) saat air mengalir
2. **Processing (Edge Computing)**: ESP32 menangkap *Pulse* melalui Interrupt, menghitung frekuensi, dan mengonversinya menjadi *Flow Rate* & *Volume*
3. **Transmission (Pengiriman)**: ESP32 mengirim data via WiFi menggunakan **HTTP POST** Request ke API Server (PHP)
4. **Storing (Penyimpanan)**: Backend menerima data, melakukan validasi, dan menyimpannya ke tabel `readings` di **MySQL**
5. **Visualizing (Tampilan)**: Web Dashboard me-request data dari database dan merendernya menjadi angka dan grafik visual

## 📁 Struktur Proyek
```
flow_meter_app/
│
├── backend/
│   └── api/
│       ├── config.php          # Konfigurasi database
│       ├── receive_data.php    # API endpoint untuk menerima data dari ESP32
│       ├── get_readings.php    # API endpoint untuk mengambil data pembacaan
│       └── test_api.php        # File untuk menguji API
│
├── frontend/
│   ├── index.html              # Tampilan utama dashboard
│   ├── css/
│   │   └── style.css           # File CSS untuk tampilan
│   └── js/
│       └── dashboard.js        # Logika frontend dashboard
│
├── firmware/
│   ├── src/
│   │   └── flow_meter_firmware.ino  # Firmware untuk ESP32
│   └── firmware_docs.md        # Dokumentasi firmware
│
└── docs/
    ├── database_schema.sql     # Skrip pembuatan database
    └── installation_guide.md   # Panduan instalasi dan pengujian
```

## 🛠️ Persyaratan Sistem

### Hardware
- ESP32 DevKit V1
- Sensor Flow Meter YF-S201 / DN40
- Kabel jumper
- Breadboard (opsional)

### Software
- XAMPP (untuk server Apache & MySQL)
- Arduino IDE (untuk meng-upload firmware ke ESP32)
- NodeMCU atau ESP32 board package di Arduino IDE
- Library ArduinoJson (untuk firmware ESP32)

### Teknologi
- Backend: PHP Native
- Database: MySQL / MariaDB
- Frontend: HTML5, Bootstrap 5, JavaScript (Chart.js)
- Hardware: C++ (Arduino IDE / PlatformIO)

## 🚀 Instalasi

### A. Instalasi Backend
1. Pastikan XAMPP sudah terinstall dan berjalan
2. Salin folder `backend` ke direktori `htdocs`
3. Buka phpMyAdmin dan buat database `db_flowmeter`
4. Import skrip `docs/database_schema.sql` ke database

### B. Instalasi Frontend
1. Akses dashboard di `http://localhost/FLOW_METER_SENSOR_2.0/flow_meter_app/frontend/index.html`

### C. Instalasi Firmware ESP32
1. Buka file `firmware/src/flow_meter_firmware.ino` di Arduino IDE
2. Instal library ArduinoJson melalui Library Manager
3. Ganti SSID, password, dan serverUrl di bagian atas file
4. Upload firmware ke ESP32

Lihat file `docs/installation_guide.md` untuk panduan instalasi lengkap.

## 📊 Penggunaan

1. Pastikan server XAMPP berjalan (Apache & MySQL)
2. Upload firmware ke ESP32 dan pastikan terhubung ke WiFi
3. Buka dashboard di browser untuk melihat data real-time
4. Gunakan filter device untuk melihat data dari sensor berbeda
5. Gunakan tombol "Export to CSV" untuk menyimpan data

## 🧪 Testing

1. Uji API backend menggunakan file `backend/api/test_api.php`
2. Pastikan data masuk ke database dengan benar
3. Cek tampilan dashboard untuk memastikan data tampil dengan benar
4. Uji konektivitas ESP32 dan pengiriman data

Lihat file `docs/installation_guide.md` untuk panduan troubleshooting.