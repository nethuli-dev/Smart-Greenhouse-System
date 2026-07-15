
// Smart Greenhouse Dashboard - JavaScript

// Global variables
let updateInterval = null;
let dataPoints = 0;
let startTime = Date.now();

// Initialization
document.addEventListener('DOMContentLoaded', function() {
    console.log('🌱 Smart Greenhouse Dashboard initialized');
    
    // Start auto-update
    updateData();
    updateInterval = setInterval(updateData, 2000);
    
    // Update uptime every second
    setInterval(updateUptime, 1000);
});

// Data Fetching and Display
function updateData() {
    fetch('/data')
        .then(response => {
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            return response.json();
        })
        .then(data => {
            // Update sensor readings
            updateSensorDisplay(data);
            
            // Update device status
            updateDeviceStatus(data);
            
            // Update system info
            updateSystemInfo(data);
            
            // Update UI indicators
            updateIndicators(data);
            
            // Increment data points
            dataPoints++;
            
            // Update timestamp
            const now = new Date();
            document.getElementById('updateTime').textContent = now.toLocaleTimeString();
            document.getElementById('lastReading').textContent = now.toLocaleTimeString();
            document.getElementById('dataPoints').textContent = dataPoints;
            
            // Reset connection status
            updateConnectionStatus(true);
        })
        .catch(error => {
            console.error('Error fetching data:', error);
            updateConnectionStatus(false);
            showToast('Failed to fetch data. Check connection!', 'error');
        });
}

function updateSensorDisplay(data) {
    // Temperature
    const temp = data.temperature || 0;
    document.getElementById('temperature').textContent = temp.toFixed(1);
    const tempPercent = Math.min((temp / 50) * 100, 100);
    document.getElementById('tempProgress').style.width = tempPercent + '%';
    
    // Humidity
    const humidity = data.humidity || 0;
    document.getElementById('humidity').textContent = humidity.toFixed(1);
    document.getElementById('humidityProgress').style.width = Math.min(humidity, 100) + '%';
    
    // Soil Moisture
    const soil = data.soilMoisture || 0;
    document.getElementById('soilMoisture').textContent = soil;
    document.getElementById('soilProgress').style.width = Math.min(soil, 100) + '%';
}

function updateDeviceStatus(data) {
    // Pump
    const pump = data.pump || false;
    const pumpStatus = document.getElementById('pumpStatus');
    const pumpLED = document.getElementById('pumpLED');
    const pumpText = document.getElementById('pumpStateText');
    
    if (pump) {
        pumpStatus.textContent = 'ON';
        pumpStatus.className = 'status-value active';
        pumpLED.className = 'indicator-led on';
        pumpText.textContent = 'Running 💧';
    } else {
        pumpStatus.textContent = 'OFF';
        pumpStatus.className = 'status-value inactive';
        pumpLED.className = 'indicator-led off';
        pumpText.textContent = 'Idle ⏸️';
    }
    
    // Fan
    const fan = data.fan || false;
    const fanStatus = document.getElementById('fanStatus');
    const fanLED = document.getElementById('fanLED');
    const fanText = document.getElementById('fanStateText');
    
    if (fan) {
        fanStatus.textContent = 'ON';
        fanStatus.className = 'status-value active';
        fanLED.className = 'indicator-led on';
        fanText.textContent = 'Running 🌀';
    } else {
        fanStatus.textContent = 'OFF';
        fanStatus.className = 'status-value inactive';
        fanLED.className = 'indicator-led off';
        fanText.textContent = 'Idle ⏸️';
    }
}

function updateSystemInfo(data) {
    // Update auto mode status based on conditions
    const temp = data.temperature || 0;
    const soil = data.soilMoisture || 0;
    
    let autoStatus = 'Active ✅';
    if (temp > 35 || soil < 20) {
        autoStatus = 'Active - Alert ⚠️';
    }
    document.getElementById('autoMode').textContent = autoStatus;
}

function updateIndicators(data) {
    // Update card border colors based on values
    const temp = data.temperature || 0;
    const humidity = data.humidity || 0;
    const soil = data.soilMoisture || 0;
    
    // Temperature indicator
    const tempCard = document.querySelector('.temperature');
    if (temp > 30) {
        tempCard.style.borderLeft = '4px solid #ff6b6b';
    } else if (temp > 25) {
        tempCard.style.borderLeft = '4px solid #fbbf24';
    } else {
        tempCard.style.borderLeft = '4px solid #4ade80';
    }
    
    // Soil moisture indicator
    const soilCard = document.querySelector('.soil-moisture');
    if (soil < 30) {
        soilCard.style.borderLeft = '4px solid #ff6b6b';
    } else if (soil < 50) {
        soilCard.style.borderLeft = '4px solid #fbbf24';
    } else {
        soilCard.style.borderLeft = '4px solid #4ade80';
    }
}

function updateConnectionStatus(connected) {
    const statusText = document.getElementById('connectionStatus');
    const dot = document.querySelector('.dot');
    
    if (connected) {
        statusText.textContent = 'Connected';
        dot.style.background = '#4ade80';
    } else {
        statusText.textContent = 'Disconnected';
        dot.style.background = '#f87171';
    }
}

function updateUptime() {
    const uptime = Math.floor((Date.now() - startTime) / 1000);
    const hours = Math.floor(uptime / 3600);
    const minutes = Math.floor((uptime % 3600) / 60);
    const seconds = uptime % 60;
    
    document.getElementById('uptime').textContent = 
        `${hours}h ${minutes}m ${seconds}s`;
}

// Device Control Functions
function controlDevice(command) {
    console.log(`📡 Sending command: ${command}`);
    
    // Show loading state
    showToast(`Sending ${command}...`, 'info');
    
    fetch(`/control?cmd=${command}`)
        .then(response => {
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            return response.text();
        })
        .then(data => {
            console.log('✅ Command sent successfully:', data);
            showToast(`✅ ${command} executed successfully!`, 'success');
            
            // Update data after command
            setTimeout(updateData, 500);
        })
        .catch(error => {
            console.error('❌ Error sending command:', error);
            showToast(`❌ Failed to send ${command}. Check connection!`, 'error');
        });
}

// Settings Functions
function updateSettings() {
    // Get values from inputs
    const tempHigh = document.getElementById('tempHigh').value;
    const tempLow = document.getElementById('tempLow').value;
    const soilDry = document.getElementById('soilDry').value;
    const soilWet = document.getElementById('soilWet').value;
    
    // Validate values
    if (parseFloat(tempHigh) <= parseFloat(tempLow)) {
        showToast('❌ Temperature High must be greater than Low!', 'error');
        return;
    }
    
    if (parseInt(soilDry) >= parseInt(soilWet)) {
        showToast('❌ Soil Dry must be less than Wet!', 'error');
        return;
    }
    
    // Send settings to server
    const settings = {
        tempHigh: parseFloat(tempHigh),
        tempLow: parseFloat(tempLow),
        soilDry: parseInt(soilDry),
        soilWet: parseInt(soilWet)
    };
    
    console.log('📤 Updating settings:', settings);
    
    fetch('/settings', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(settings)
    })
    .then(response => response.text())
    .then(data => {
        console.log('✅ Settings updated:', data);
        showToast('✅ Settings saved successfully!', 'success');
    })
    .catch(error => {
        console.error('❌ Error updating settings:', error);
        showToast('❌ Failed to save settings!', 'error');
    });
}

// UI Helper Functions

function showToast(message, type = 'info') {
    // Remove existing toast if any
    const existingToast = document.querySelector('.toast');
    if (existingToast) {
        existingToast.remove();
    }
    
    // Create toast
    const toast = document.createElement('div');
    toast.className = `toast ${type}`;
    toast.textContent = message;
    document.body.appendChild(toast);
    
    // Show with animation
    setTimeout(() => {
        toast.classList.add('show');
    }, 100);
    
    // Auto-hide after 3 seconds
    setTimeout(() => {
        toast.classList.remove('show');
        setTimeout(() => {
            toast.remove();
        }, 300);
    }, 3000);
}


// Keyboard Shortcuts

document.addEventListener('keydown', function(event) {
    // Ctrl+R = Refresh data
    if (event.ctrlKey && event.key === 'r') {
        event.preventDefault();
        updateData();
        showToast('🔄 Data refreshed!', 'info');
    }
    
    // Ctrl+P = Toggle Pump
    if (event.ctrlKey && event.key === 'p') {
        event.preventDefault();
        const pumpStatus = document.getElementById('pumpStatus').textContent;
        controlDevice(pumpStatus === 'ON' ? 'PUMP_OFF' : 'PUMP_ON');
    }
    
    // Ctrl+F = Toggle Fan
    if (event.ctrlKey && event.key === 'f') {
        event.preventDefault();
        const fanStatus = document.getElementById('fanStatus').textContent;
        controlDevice(fanStatus === 'ON' ? 'FAN_OFF' : 'FAN_ON');
    }
});

// Console Help

console.log(`
🌱 Smart Greenhouse Dashboard - Help
====================================
Commands:
  - updateData()      : Refresh sensor data
  - controlDevice('PUMP_ON')  : Turn pump ON
  - controlDevice('PUMP_OFF') : Turn pump OFF
  - controlDevice('FAN_ON')   : Turn fan ON
  - controlDevice('FAN_OFF')  : Turn fan OFF
  - showToast('msg', 'type')  : Show notification

Keyboard Shortcuts:
  - Ctrl+R : Refresh data
  - Ctrl+P : Toggle Pump
  - Ctrl+F : Toggle Fan
`);

// Export functions for debugging
window.updateData = updateData;
window.controlDevice = controlDevice;
window.showToast = showToast;
window.updateSettings = updateSettings;