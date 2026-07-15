# 🌱 Smart Greenhouse Monitoring & Management System

An IoT-based automated greenhouse monitoring and control system developed as a university project. The system continuously tracks environmental parameters and automatically responds to maintain optimal growing conditions.

## 🎯 Project Overview

This system monitors soil moisture, temperature, and humidity in a greenhouse environment, automatically controlling irrigation and cooling systems. It provides both automated control and manual override capabilities through a web dashboard.

## ✨ Features

- **Real-time Monitoring**: Continuous tracking of temperature, humidity, and soil moisture
- **Automated Irrigation**: Water pump control based on soil moisture thresholds
- **Temperature Regulation**: Automated cooling fan control
- **Manual Control**: Web dashboard for manual override of pumps and fans
- **Data Visualization**: Live sensor readings displayed on dashboard
- **Wireless Communication**: Data transmission via ESP8266 Wi-Fi module

## 🛠️ Hardware Components

- **Arduino Uno R3** - Main controller
- **ESP8266** - Wi-Fi module for web interface
- **DHT11** - Temperature and humidity sensor
- **Soil Moisture Sensor** - Capacitive or resistive type
- **2-Channel Relay Module** - For pump and fan control
- **Water Pump** - For irrigation
- **Cooling Fan** - For temperature regulation
- **Breadboard & Jumper Wires**

## 📁 Project Structure

Smart-Greenhouse-System/
├── arduino/
│ └── greenhouse_controller/
│ ├── greenhouse_controller.ino
│ └── config.h
├── esp8266/
│ └── esp8266_webserver/
│ ├── esp8266_webserver.ino
│ └── config.h
├── docs/
│ ├── circuit_diagram.png
│ └── system_architecture.png
├── images/
│ ├── dashboard_screenshot.png
│ └── hardware_setup.jpg
└── README.md

## 🔧 Setup Instructions

### 1. Hardware Assembly

1. Connect sensors and components according to the circuit diagram
2. Power the Arduino Uno via USB or external power supply
3. Connect ESP8266 to Arduino via SoftwareSerial (pins 10 and 11)

### 2. Software Setup

**Arduino Uno Setup:**

1. Install Arduino IDE (version 1.8.19 or newer)
2. Install required libraries:
   - DHT sensor library by Adafruit
   - SoftwareSerial (built-in)
3. Open `arduino/greenhouse_controller/greenhouse_controller.ino`
4. Update `config.h` with your WiFi credentials
5. Upload the code to Arduino Uno

**ESP8266 Setup:**

1. Install ESP8266 board package in Arduino IDE
2. Open `esp8266/esp8266_webserver/esp8266_webserver.ino`
3. Update `config.h` with your WiFi credentials
4. Upload the code to ESP8266

### 3. Web Dashboard

After uploading both codes:

1. Open Serial Monitor to see ESP8266 IP address
2. Connect to the same WiFi network as ESP8266
3. Access the dashboard at `http://<ESP8266_IP>`

## 📱 Live Dashboard Preview

![Dashboard Screenshot](images/dashboard_screenshot.png)

The web dashboard provides:

- Real-time sensor readings (updated every 2 seconds)
- Visual status indicators for pump and fan
- Manual control buttons for irrigation and cooling
- System status information

## 🔬 Testing & Results

### Sensor Calibration

- Soil moisture sensor calibrated using dry air (1023) and water (300)
- Thresholds set based on plant requirements
- Temperature range validated with environmental conditions

### Performance Metrics

| Metric                  | Value     |
| ----------------------- | --------- |
| Sensor Reading Interval | 5 seconds |
| Web Update Interval     | 2 seconds |
| Temperature Accuracy    | ±2°C      |
| Humidity Accuracy       | ±5%       |
| Soil Moisture Accuracy  | ±10%      |

## 🤝 Team Contributions

- **System Architecture**: Design and planning
- **Hardware Integration**: Sensor and component interfacing
- **Software Development**: Arduino and ESP8266 programming
- **Web Dashboard**: UI/UX design and implementation
- **Testing**: System validation and debugging
- **Documentation**: Project documentation and report

## 📚 Lessons Learned

1. **Embedded Systems**: Understanding microcontroller programming and sensor interfacing
2. **IoT Integration**: Connecting hardware with web services
3. **Automation Logic**: Implementing control algorithms for environmental management
4. **Web Development**: Creating responsive dashboards for remote monitoring
5. **Team Collaboration**: Working effectively in a development team

## 🚀 Future Improvements

1. Add data logging to SD card or cloud storage
2. Implement predictive algorithms for temperature control
3. Add additional sensors (light intensity, CO2, etc.)
4. Integrate with smartphone app (Android/iOS)
5. Implement energy monitoring and optimization

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- University Faculty for guidance and support
- Adafruit for DHT sensor library
- ESP8266 community for valuable resources

---

_Made with ❤️ by FC08 Group_
