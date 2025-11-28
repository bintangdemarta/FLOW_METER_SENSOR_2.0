<?php
require_once 'config.php';

// Hanya menerima metode GET
if ($_SERVER['REQUEST_METHOD'] !== 'GET') {
    sendResponse(false, 'Metode request tidak diizinkan');
}

// Parameter opsional untuk filter
$device_id = $_GET['device_id'] ?? null;
$limit = intval($_GET['limit'] ?? 100); // Default 100 data terbaru
$from_date = $_GET['from'] ?? null;
$to_date = $_GET['to'] ?? null;

$conn = getConnection();

// Membangun query berdasarkan parameter
$sql = "SELECT * FROM readings";
$params = [];
$types = "";

$where_conditions = [];

if ($device_id) {
    $where_conditions[] = "device_id = ?";
    $params[] = $device_id;
    $types .= "s";
}

if ($from_date) {
    $where_conditions[] = "reading_time >= ?";
    $params[] = $from_date;
    $types .= "s";
}

if ($to_date) {
    $where_conditions[] = "reading_time <= ?";
    $params[] = $to_date;
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
    $readings[] = $row;
}

sendResponse(true, 'Data berhasil diambil', [
    'count' => count($readings),
    'readings' => $readings
]);

$conn->close();
?>