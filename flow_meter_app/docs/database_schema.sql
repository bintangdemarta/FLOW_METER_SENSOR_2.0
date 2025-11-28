-- Database: db_flowmeter
CREATE DATABASE IF NOT EXISTS db_flowmeter;
USE db_flowmeter;

-- Tabel devices
-- Menyimpan identitas mikrokontroler (berguna untuk multi-sensor)
CREATE TABLE devices (
    device_id VARCHAR(50) PRIMARY KEY, -- ID Unik (cth: ESP32_DAPUR)
    location VARCHAR(100),             -- Lokasi sensor (cth: Wastafel)
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Tabel readings
-- Menyimpan log data yang dikirim oleh sensor
CREATE TABLE readings (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50),
    flow_rate FLOAT,       -- Debit air sesaat (L/min)
    total_volume FLOAT,    -- Volume kumulatif (Liter)
    reading_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (device_id) REFERENCES devices(device_id)
);

-- Menambahkan data contoh ke tabel devices
INSERT INTO devices (device_id, location, description) VALUES 
('ESP32_DAPUR', 'Dapur', 'Sensor flow meter di dapur'),
('ESP32_KAMAR_MANDI', 'Kamar Mandi', 'Sensor flow meter di kamar mandi'),
('ESP32_HALAMAN', 'Halaman', 'Sensor flow meter utama');

-- Indeks untuk meningkatkan performa query
CREATE INDEX idx_device_id ON readings(device_id);
CREATE INDEX idx_reading_time ON readings(reading_time);