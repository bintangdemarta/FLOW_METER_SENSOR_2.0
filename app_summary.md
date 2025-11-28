# 🚰 IoT Water Flow Meter Monitoring System

**IoT Water Flow Meter** adalah sistem pemantauan penggunaan air end-to-end berbasis Internet of Things. Proyek ini menghubungkan sensor fisik (hardware) ke antarmuka web (dashboard) untuk memantau debit air, volume penggunaan, dan estimasi biaya secara *real-time*.

---

## 📑 Daftar Isi
- [Gambaran Sistem](#-gambaran-sistem)
- [Fitur Utama](#-fitur-utama)
- [Alur Kerja Aplikasi (Architecture)](#-alur-kerja-aplikasi-architecture)
- [Tech Stack](#-tech-stack)
- [Skema Database](#-skema-database)
- [Konfigurasi Hardware](#-konfigurasi-hardware)
- [Logika Konversi Sensor](#-logika-konversi-sensor)

---

## 🔭 Gambaran Sistem
Sistem ini dirancang untuk membaca sinyal *pulse* dari sensor Flow Meter menggunakan mikrokontroler ESP32. Data tersebut diproses menjadi satuan volume (Liter) dan dikirim ke server lokal/cloud melalui protokol HTTP untuk disimpan di database MySQL. Pengguna dapat mengakses Dashboard Web untuk melihat statistik penggunaan air.

---

## ✨ Fitur Utama

### 1. Dashboard Monitoring
- **Real-time Flow Rate:** Menampilkan kecepatan aliran air saat ini (Liter/menit).
- **Total Volume:** Akumulasi total air yang telah dikonsumsi (M³ atau Liter).
- **Status Perangkat:** Indikator konektivitas sensor (Online/Offline).

### 2. Analisis Data
- **Historical Charts:** Grafik interaktif penggunaan air per jam, harian, dan bulanan (menggunakan Chart.js).
- **Cost Estimation:** Kalkulasi otomatis estimasi tagihan air (Rupiah) berdasarkan tarif per meter kubik (M³).

### 3. Keamanan & Utilitas
- **Leak Detection:** Logika deteksi anomali (misal: aliran air terus menerus pada jam 02.00 - 04.00 pagi).
- **Data Export:** Fitur unduh laporan riwayat penggunaan ke format Excel/CSV.

---

## 🔄 Alur Kerja Aplikasi (Architecture)

Sistem bekerja dengan 5 tahapan utama:

1.  **Sensing (Pembacaan):**
    Sensor Flow Meter (Hall Effect) berputar saat air lewat, menghasilkan sinyal digital (Pulse).
2.  **Processing (Edge Computing):**
    ESP32 menangkap *Pulse* melalui Interrupt, menghitung frekuensi, dan mengonversinya menjadi *Flow Rate* & *Volume*.
3.  **Transmission (Pengiriman):**
    ESP32 mengirim data via WiFi menggunakan **HTTP POST** Request ke API Server (PHP/Node.js).
4.  **Storing (Penyimpanan):**
    Backend menerima data, melakukan validasi, dan menyimpannya ke tabel `readings` di **MySQL**.
5.  **Visualizing (Tampilan):**
    Web Dashboard me-request data dari database dan merendernya menjadi angka dan grafik visual.

---

## 🛠 Tech Stack

| Komponen | Teknologi |
| :--- | :--- |
| **Hardware** | ESP32 DevKit V1, Sensor YF-S201 / DN40 |
| **Firmware** | C++ (Arduino IDE / PlatformIO) |
| **Backend** | PHP Native (atau Framework Laravel/CI) |
| **Database** | MySQL / MariaDB |
| **Frontend** | HTML5, Bootstrap 5, Javascript (Chart.js / jQuery) |

---

## 🗄 Skema Database

Nama Database: `db_flowmeter`

### 1. Tabel `devices`
Menyimpan identitas mikrokontroler (berguna untuk multi-sensor).

```sql
CREATE TABLE devices (
    device_id VARCHAR(50) PRIMARY KEY, -- ID Unik (cth: ESP32_DAPUR)
    location VARCHAR(100),             -- Lokasi sensor (cth: Wastafel)
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
2. Tabel readingsMenyimpan log data yang dikirim oleh sensor.SQLCREATE TABLE readings (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50),
    flow_rate FLOAT,       -- Debit air sesaat (L/min)
    total_volume FLOAT,    -- Volume kumulatif (Liter)
    reading_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (device_id) REFERENCES devices(device_id)
);
🔌 Konfigurasi HardwarePin Mapping (ESP32 vs Sensor YF-S201)Kabel SensorFungsiPin ESP32CatatanMerahVCC (+5V)VIN / 5VButuh supply 5V agar akuratHitamGNDGNDGround harus disatukanKuningSignal (Pulse)GPIO 4Gunakan pin yang mendukung InterruptPenting: Karena ESP32 bekerja pada logika 3.3V dan sensor seringkali mengeluarkan sinyal 5V, disarankan menggunakan Voltage Divider (Resistor) pada kabel sinyal Kuning sebelum masuk ke GPIO 4 untuk keamanan jangka panjang.🧮 Logika Konversi SensorUntuk mengubah frekuensi pulse menjadi Liter/menit, gunakan rumus kalibrasi sensor (contoh untuk YF-S201):$$FlowRate (L/min) = \frac{Frekuensi (Hz)}{7.5}$$Dimana 7.5 adalah faktor kalibrasi (Q Factor) standar YF-S201. Angka ini bisa berbeda tergantung jenis sensor.🚀 Cara InstalasiDatabase: Import skema SQL di atas ke phpMyAdmin/MySQL Workbench.Backend: Upload file API (PHP) ke folder htdocs (XAMPP) atau server hosting.Firmware:Buka codingan .ino di Arduino IDE.Edit bagian ssid, password, dan serverUrl.Upload ke ESP32.Frontend: Buka index.php atau index.html di browser untuk melihat dashboard.