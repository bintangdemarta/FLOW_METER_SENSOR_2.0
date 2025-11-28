<?php
require_once 'config.php';

// Hanya menerima metode POST
if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    sendResponse(false, 'Metode request tidak diizinkan');
}

// Membaca data JSON dari request body
$input = json_decode(file_get_contents('php://input'), true);

// Validasi data
if (!isset($input['device_id']) || !isset($input['flow_rate']) || !isset($input['total_volume'])) {
    sendResponse(false, 'Data tidak lengkap', [
        'required_fields' => ['device_id', 'flow_rate', 'total_volume']
    ]);
}

$device_id = $input['device_id'];
$flow_rate = floatval($input['flow_rate']);
$total_volume = floatval($input['total_volume']);

// Validasi device_id ada di tabel devices
$conn = getConnection();
$stmt = $conn->prepare("SELECT device_id FROM devices WHERE device_id = ?");
$stmt->bind_param("s", $device_id);
$stmt->execute();
$result = $stmt->get_result();

if ($result->num_rows === 0) {
    sendResponse(false, 'Device ID tidak dikenali', ['received_device_id' => $device_id]);
}

// Menyimpan data ke tabel readings
$stmt = $conn->prepare("INSERT INTO readings (device_id, flow_rate, total_volume) VALUES (?, ?, ?)");
$stmt->bind_param("sff", $device_id, $flow_rate, $total_volume);

if ($stmt->execute()) {
    sendResponse(true, 'Data berhasil disimpan', [
        'device_id' => $device_id,
        'flow_rate' => $flow_rate,
        'total_volume' => $total_volume,
        'timestamp' => date('Y-m-d H:i:s')
    ]);
} else {
    sendResponse(false, 'Gagal menyimpan data', ['error' => $conn->error]);
}

$conn->close();
?>