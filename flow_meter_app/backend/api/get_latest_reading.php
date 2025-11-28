<?php
// Simple API endpoint specifically for microcontroller OLED display
// Returns only the most essential data in a compact format

require_once 'config.php';

// Hanya menerima metode GET
if ($_SERVER['REQUEST_METHOD'] !== 'GET') {
    header('Content-Type: application/json');
    echo json_encode(['error' => 'Method not allowed']);
    exit();
}

// Parameter opsional untuk filter
$device_id = $_GET['device_id'] ?? null;
$limit = intval($_GET['limit'] ?? 1); // Hanya ambil 1 data terbaru secara default

$conn = getConnection();

// Query sederhana untuk data terbaru
$sql = "SELECT flow_rate, total_volume, reading_time FROM readings";

$params = [];
$types = "";
$where_conditions = [];

if ($device_id) {
    $where_conditions[] = "device_id = ?";
    $params[] = $device_id;
    $types .= "s";
}

if (!empty($where_conditions)) {
    $sql .= " WHERE " . implode(" AND ", $where_conditions);
}

$sql .= " ORDER BY reading_time DESC LIMIT ?";

// Menambahkan limit ke parameter
$params[] = $limit;
$types .= "i";

$stmt = $conn->prepare($sql);

if (!empty($params)) {
    $stmt->bind_param($types, ...$params);
}

$stmt->execute();
$result = $stmt->get_result();

$readings = [];
while ($row = $result->fetch_assoc()) {
    $readings[] = [
        'flow_rate' => floatval($row['flow_rate']),
        'total_volume' => floatval($row['total_volume']),
        'reading_time' => $row['reading_time']
    ];
}

header('Content-Type: application/json');
if (count($readings) > 0) {
    // Jika hanya satu data, kembalikan langsung
    if ($limit == 1 && count($readings) == 1) {
        echo json_encode($readings[0]);
    } else {
        // Jika banyak data, kembalikan dalam array
        echo json_encode(['readings' => $readings]);
    }
} else {
    // Jika tidak ada data, kembalikan nilai default
    echo json_encode([
        'flow_rate' => 0.0,
        'total_volume' => 0.0,
        'reading_time' => 'N/A'
    ]);
}

$conn->close();
?>