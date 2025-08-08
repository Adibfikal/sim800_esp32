# ESP32 SIM800 Arduino IDE Project

This project has been converted from PlatformIO to Arduino IDE compatible format. It provides comprehensive functionality for ESP32 with SIM800 GSM/GPRS module including GPS, HTTP, and MQTT capabilities.

## Hardware Requirements

- ESP32 development board (ESP32 Dev Module, DOIT ESP32 DEVKIT V1, etc.)
- SIM800L or SIM800A GSM/GPRS module
- Active SIM card with data plan
- Jumper wires for connections

## Hardware Connections

| SIM800 Pin | ESP32 Pin | Description |
|------------|-----------|-------------|
| VCC        | 5V/3.7V   | Power supply (check your module voltage) |
| GND        | GND       | Ground |
| RXD        | GPIO17    | Serial transmit from ESP32 |
| TXD        | GPIO16    | Serial receive to ESP32 |
| RST        | GPIO4     | Reset pin (optional) |
| PWR        | GPIO2     | Power control (optional) |

**Important Notes:**
- SIM800L requires 3.7-4.2V power supply with high current capability (2A peak)
- SIM800A can work with 5V supply
- Use proper power supply, USB power may not be sufficient
- Connect a good antenna for reliable operation

## Arduino IDE Setup

### 1. Install ESP32 Board Package
1. Open Arduino IDE
2. Go to File > Preferences
3. Add this URL to "Additional Boards Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to Tools > Board > Boards Manager
5. Search for "ESP32" and install "ESP32 by Espressif Systems"

### 2. Board Configuration
Select the following settings in Arduino IDE:
- **Board:** ESP32 Dev Module (or your specific ESP32 board)
- **Flash Size:** 4MB
- **Partition Scheme:** Default 4MB with spiffs
- **PSRAM:** Enabled (if your board has PSRAM)
- **Upload Speed:** 921600
- **CPU Frequency:** 240MHz (WiFi/BT)

### 3. Upload the Sketch
1. Open `ESP32_SIM800.ino` in Arduino IDE
2. Configure your APN settings in `config.h`:
   ```cpp
   #define DEFAULT_APN "your-carrier-apn"  // e.g., "internet" for Telkomsel
   ```
3. Connect your ESP32 via USB
4. Select the correct COM port
5. Click Upload

## Project Structure

```
ESP32_SIM800_Arduino/
├── ESP32_SIM800.ino          # Main sketch (comprehensive test)
├── config.h                  # Configuration settings
├── SIM800Manager.h           # SIM800 library header
├── SIM800Manager.cpp         # SIM800 library implementation
├── README.md                 # This file
└── examples/
    └── BasicUsage/
        ├── BasicUsage.ino    # Simple usage example
        ├── config.h          # Configuration for example
        ├── SIM800Manager.h   # Library header (copy)
        └── SIM800Manager.cpp # Library implementation (copy)
```

## Available Examples

### 1. Main Test Sketch (ESP32_SIM800.ino)
Comprehensive test suite that demonstrates all features:
- Basic AT communication
- SIM card detection
- Network registration
- Signal quality monitoring
- GPS functionality
- GPRS connection
- HTTP GET/POST requests
- Basic MQTT communication

### 2. Basic Usage Example (examples/BasicUsage/)
Simplified example for IoT applications:
- Sensor data simulation
- HTTP POST to web server
- Basic error handling
- Periodic data transmission

## Configuration

### APN Settings
Update the APN settings in `config.h` for your carrier:

```cpp
// Common APN settings for Indonesian carriers:
#define DEFAULT_APN "internet"      // Telkomsel
// #define DEFAULT_APN "indosatgprs"   // Indosat
// #define DEFAULT_APN "www.xlgprs.net" // XL
// #define DEFAULT_APN "3gprs"         // Tri/3
// #define DEFAULT_APN "smart"         // Smartfren
```

### Debug Configuration
Enable/disable debug output by modifying `config.h`:

```cpp
#define DEBUG_ENABLED 1        // Enable for debugging
#define DEBUG_AT_COMMANDS 1    // Show AT command communication
#define DEBUG_GPS 1            // Show GPS debug info
#define DEBUG_HTTP 1           // Show HTTP debug info
#define DEBUG_MQTT 1           // Show MQTT debug info
```

## Features

✅ **Basic GSM Functions**
- AT command communication
- SIM card detection
- Network registration
- Signal quality monitoring
- IMEI and ICCID reading

✅ **GPS Functions**
- GPS enable/disable
- Location data parsing
- Satellite count monitoring
- GPS fix waiting with timeout

✅ **GPRS Functions**
- APN configuration
- GPRS connection management
- IP address retrieval

✅ **HTTP Functions**
- HTTP GET requests
- HTTP POST requests with JSON data
- Response parsing

✅ **MQTT Functions**
- Basic MQTT connection
- Message publishing
- Topic subscription (basic)

## Usage Examples

### Basic HTTP POST
```cpp
String jsonData = "{\"temperature\":25.5,\"humidity\":60.0}";
String response;
if (sim800.httpPost("httpbin.org", 80, "/post", jsonData, response)) {
  Serial.println("Data sent successfully!");
}
```

### GPS Data Reading
```cpp
if (sim800.enableGPS()) {
  GPSData gpsData;
  if (sim800.getGPSData(gpsData) && gpsData.isValid) {
    Serial.printf("Location: %.6f, %.6f\n", gpsData.latitude, gpsData.longitude);
  }
}
```

### MQTT Publishing
```cpp
if (sim800.mqttConnect("broker.emqx.io", 1883, "ESP32-Client")) {
  sim800.mqttPublish("sensors/temperature", "25.5");
  sim800.mqttDisconnect();
}
```

## Troubleshooting

### Common Issues

1. **"Failed to initialize SIM800"**
   - Check wiring connections
   - Ensure proper power supply (minimum 3.7V, 2A capability)
   - Verify SIM800 module is receiving power

2. **"SIM card not ready"**
   - Insert SIM card properly
   - Ensure SIM card has active service
   - Check if SIM card PIN is disabled

3. **"Network registration failed"**
   - Check antenna connection
   - Ensure good signal coverage
   - Verify SIM card has active service

4. **"GPRS connection failed"**
   - Verify correct APN settings for your carrier
   - Ensure SIM card has data plan
   - Check network coverage

5. **"GPS timeout"**
   - Move to location with clear sky view
   - GPS may take 30+ seconds for first fix
   - Indoor GPS generally doesn't work

### Debug Steps

1. Enable full debugging in `config.h`
2. Open Serial Monitor at 115200 baud
3. Check AT command responses
4. Verify each step of initialization

## Original PlatformIO Configuration

This project was converted from PlatformIO with the following build flags:
- `CORE_DEBUG_LEVEL=1`
- `BOARD_HAS_PSRAM`
- Debug configurations for AT commands, GPS, HTTP, and MQTT

## License

This project maintains the same license as the original PlatformIO version.

## Contributing

When reporting issues, please include:
- Arduino IDE version
- ESP32 board type and version
- SIM800 module type
- Serial monitor output with debugging enabled
- Carrier and country information