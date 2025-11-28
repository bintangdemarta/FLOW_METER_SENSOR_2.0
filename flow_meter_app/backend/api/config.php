<?php
// Konfigurasi koneksi database
define('DB_HOST', 'localhost');
define('DB_USER', 'root'); // Sesuaikan dengan konfigurasi XAMPP Anda
define('DB_PASS', '');     // Biasanya kosong untuk XAMPP default
define('DB_NAME', 'db_flowmeter');

// Fungsi untuk membuat koneksi ke database
function getConnection() {
    $conn = new mysqli(DB_HOST, DB_USER, DB_PASS, DB_NAME);
    
    if ($conn->connect_error) {
        die("Koneksi database gagal: " . $conn->connect_error);
    }
    
    return $conn;
}

// Fungsi untuk mengirim response JSON
function sendResponse($success, $message, $data = null) {
    header('Content-Type: application/json');
    echo json_encode([
        'success' => $success,
        'message' => $message,
        'data' => $data
    ]);
    exit();
}
?>