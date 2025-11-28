// Konfigurasi dasar
const API_BASE_URL = 'http://localhost/FLOW_METER_SENSOR_2.0/flow_meter_app/backend/api';

// Variabel untuk menyimpan data dan chart
let flowChart = null;
let latestReadings = [];

// Fungsi untuk mengambil data pembacaan dari API
async function fetchReadings(deviceId = null, limit = 50) {
    try {
        let url = `${API_BASE_URL}/get_readings.php?limit=${limit}`;
        if (deviceId) {
            url += `&device_id=${deviceId}`;
        }
        
        const response = await fetch(url);
        const result = await response.json();
        
        if (result.success) {
            latestReadings = result.data.readings;
            updateDashboard(latestReadings);
            updateReadingsTable(latestReadings);
            updateChart(latestReadings);
        } else {
            console.error('Gagal mengambil data:', result.message);
        }
    } catch (error) {
        console.error('Error saat mengambil data:', error);
    }
}

// Fungsi untuk memperbarui elemen-elemen dashboard
function updateDashboard(readings) {
    if (readings && readings.length > 0) {
        const latest = readings[0]; // Data terbaru
        
        // Update flow rate
        document.getElementById('flow-rate-value').innerHTML = 
            `${latest.flow_rate.toFixed(2)} <small class="fs-6">L/min</small>`;
        
        // Update total volume
        document.getElementById('total-volume-value').innerHTML = 
            `${latest.total_volume.toFixed(2)} <small class="fs-6">L</small>`;
        
        // Update status perangkat (berdasarkan apakah data terbaru dalam 5 menit terakhir)
        const readingTime = new Date(latest.reading_time);
        const now = new Date();
        const timeDiff = (now - readingTime) / (1000 * 60); // Selisih dalam menit
        
        const deviceStatus = document.getElementById('device-status');
        if (timeDiff < 5) {
            deviceStatus.textContent = 'Online';
            deviceStatus.className = 'status-online';
        } else {
            deviceStatus.textContent = 'Offline';
            deviceStatus.className = 'status-offline';
        }
        
        // Update estimasi biaya (asumsi tarif Rp 10.000 per m³)
        const costEstimate = (latest.total_volume / 1000) * 10000; // Konversi L ke m³ dan kalikan tarif
        document.getElementById('cost-estimate').textContent = 
            `Rp ${costEstimate.toLocaleString('id-ID')}`;
    }
}

// Fungsi untuk memperbarui tabel riwayat pembacaan
function updateReadingsTable(readings) {
    const tableBody = document.getElementById('readings-table-body');
    tableBody.innerHTML = ''; // Kosongkan tabel
    
    // Hanya tampilkan 20 data terbaru di tabel
    const displayReadings = readings.slice(0, 20);
    
    displayReadings.forEach(reading => {
        const row = document.createElement('tr');
        row.innerHTML = `
            <td>${reading.id}</td>
            <td>${reading.device_id}</td>
            <td>${parseFloat(reading.flow_rate).toFixed(2)}</td>
            <td>${parseFloat(reading.total_volume).toFixed(2)}</td>
            <td>${new Date(reading.reading_time).toLocaleString('id-ID')}</td>
        `;
        tableBody.appendChild(row);
    });
}

// Fungsi untuk memperbarui grafik
function updateChart(readings) {
    const ctx = document.getElementById('flowChart').getContext('2d');
    
    // Ambil data terbaru untuk grafik (maksimal 20 titik)
    const chartData = readings.slice(0, 20).reverse(); // Urutan terbalik untuk grafik
    
    const labels = chartData.map(item => new Date(item.reading_time).toLocaleTimeString('id-ID'));
    const flowRates = chartData.map(item => parseFloat(item.flow_rate));
    const volumes = chartData.map(item => parseFloat(item.total_volume));
    
    // Hapus chart sebelumnya jika ada
    if (flowChart) {
        flowChart.destroy();
    }
    
    // Buat chart baru
    flowChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: labels,
            datasets: [
                {
                    label: 'Flow Rate (L/min)',
                    data: flowRates,
                    borderColor: 'rgb(75, 192, 192)',
                    backgroundColor: 'rgba(75, 192, 192, 0.2)',
                    yAxisID: 'y'
                },
                {
                    label: 'Total Volume (L)',
                    data: volumes,
                    borderColor: 'rgb(255, 99, 132)',
                    backgroundColor: 'rgba(255, 99, 132, 0.2)',
                    yAxisID: 'y1'
                }
            ]
        },
        options: {
            responsive: true,
            interaction: {
                mode: 'index',
                intersect: false
            },
            scales: {
                y: {
                    type: 'linear',
                    display: true,
                    position: 'left',
                    title: {
                        display: true,
                        text: 'Flow Rate (L/min)'
                    }
                },
                y1: {
                    type: 'linear',
                    display: true,
                    position: 'right',
                    title: {
                        display: true,
                        text: 'Volume (L)'
                    },
                    grid: {
                        drawOnChartArea: false,
                    },
                }
            }
        }
    });
}

// Fungsi untuk mengekspor data ke CSV
function exportToCSV() {
    if (latestReadings.length === 0) {
        alert('Tidak ada data untuk diekspor');
        return;
    }
    
    // Header CSV
    const headers = ['ID', 'Device ID', 'Flow Rate (L/min)', 'Total Volume (L)', 'Waktu Pembacaan'];
    
    // Data pembacaan
    const rows = latestReadings.map(reading => [
        reading.id,
        reading.device_id,
        reading.flow_rate,
        reading.total_volume,
        reading.reading_time
    ]);
    
    // Gabungkan header dan data
    const csvContent = [
        headers.join(','),
        ...rows.map(row => row.join(','))
    ].join('\n');
    
    // Buat file CSV
    const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' });
    const link = document.createElement('a');
    const url = URL.createObjectURL(blob);
    
    link.setAttribute('href', url);
    link.setAttribute('download', `flow_meter_readings_${new Date().toISOString().slice(0, 10)}.csv`);
    link.style.visibility = 'hidden';
    
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
}

// Event listeners
document.addEventListener('DOMContentLoaded', function() {
    // Ambil data saat halaman dimuat
    const deviceId = document.getElementById('device-select').value;
    fetchReadings(deviceId);
    
    // Update data setiap 30 detik
    setInterval(() => {
        const selectedDevice = document.getElementById('device-select').value;
        fetchReadings(selectedDevice);
    }, 30000); // 30 detik
    
    // Event listener untuk tombol refresh
    document.getElementById('refresh-btn').addEventListener('click', () => {
        const selectedDevice = document.getElementById('device-select').value;
        fetchReadings(selectedDevice);
    });
    
    // Event listener untuk filter device
    document.getElementById('device-select').addEventListener('change', () => {
        const selectedDevice = document.getElementById('device-select').value;
        fetchReadings(selectedDevice);
    });
    
    // Event listener untuk tombol export
    document.getElementById('export-btn').addEventListener('click', exportToCSV);
});