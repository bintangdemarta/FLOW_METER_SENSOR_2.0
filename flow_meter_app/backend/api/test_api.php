<?php
// File uji untuk menguji API backend
require_once 'config.php';

// Contoh data yang akan dikirimkan seolah-olah dari ESP32
$testData = [
    'device_id' => 'ESP32_DAPUR',
    'flow_rate' => 2.5,
    'total_volume' => 10.3
];

echo "Menguji API receive_data.php...\n";

// Simulasi POST request ke API
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, 'http://localhost/FLOW_METER_SENSOR_2.0/flow_meter_app/backend/api/receive_data.php');
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($testData));
curl_setopt($ch, CURLOPT_HTTPHEADER, ['Content-Type: application/json']);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

$response = curl_exec($ch);
$httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);

curl_close($ch);

echo "HTTP Code: " . $httpCode . "\n";
echo "Response: " . $response . "\n";

if ($httpCode === 200) {
    $responseData = json_decode($response, true);
    if ($responseData['success']) {
        echo "✅ API test berhasil: " . $responseData['message'] . "\n";
    } else {
        echo "❌ API test gagal: " . $responseData['message'] . "\n";
    }
} else {
    echo "❌ HTTP error: " . $httpCode . "\n";
}

// Cek data terbaru di database
$conn = getConnection();
$stmt = $conn->prepare("SELECT * FROM readings WHERE device_id = ? ORDER BY reading_time DESC LIMIT 1");
$stmt->bind_param("s", $testData['device_id']);
$stmt->execute();
$result = $stmt->get_result();
$row = $result->fetch_assoc();

if ($row) {
    echo "\nData terbaru di database:\n";
    echo "ID: " . $row['id'] . "\n";
    echo "Device: " . $row['device_id'] . "\n";
    echo "Flow Rate: " . $row['flow_rate'] . " L/min\n";
    echo "Total Volume: " . $row['total_volume'] . " L\n";
    echo "Waktu: " . $row['reading_time'] . "\n";
} else {
    echo "\n⚠️ Tidak ada data ditemukan di database\n";
}

$conn->close();
?>