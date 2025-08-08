# ESP32 SIM800 Comprehensive Test Suite

This project provides a complete test suite for integrating SIM800 GSM/GPRS/GPS modules with ESP32 microcontrollers using the Arduino framework. It tests all major functionalities including AT commands, GPS positioning, HTTP requests, and MQTT communication.

## 🚀 Features

- **Complete AT Command Testing**: Basic communication, module info, signal quality, network registration
- **GPS Functionality**: GPS power control, location tracking, satellite information  
- **GPRS Connectivity**: APN configuration, internet connection establishment
- **HTTP Client**: GET and POST request capabilities
- **MQTT Communication**: Connect, publish, and subscribe over TCP
- **Comprehensive Diagnostics**: Detailed status monitoring and error reporting
- **Flexible Configuration**: Easy customization for different carriers and use cases

## 📋 Hardware Requirements

### Components Needed
- **ESP32 Development Board** (ESP32 DevKit v1 or compatible)
- **SIM800L Module** or SIM800A/SIM800C
- **SIM Card** with active data plan
- **External Antenna** for GSM and GPS (if not integrated)
- **Power Supply**: 3.7V Li-ion battery or 5V power adapter
- **Jumper Wires**

### Power Considerations
⚠️ **Important**: SIM800 modules require significant power during transmission:
- Peak current: 2A during transmission bursts
- Operating voltage: 3.4V - 4.4V
- Use external power supply or high-capacity battery
- Add bulk capacitors (1000µF) near the SIM800 module

## 🔌 Wiring Diagram

### ESP32 DevKit v1 to SIM800L Connection

```
ESP32 DevKit v1    |    SIM800L Module
-------------------|-------------------
GPIO 16 (RX2)      |    TX
GPIO 17 (TX2)      |    RX  
GND                |    GND
3.3V/VIN           |    VCC (through level shifter if needed)
GPIO 4 (optional)  |    RST (Reset pin)
GPIO 2 (optional)  |    PWR (Power control)
```

### Detailed Pin Connections

| ESP32 Pin | SIM800 Pin | Function | Notes |
|-----------|------------|----------|-------|
| GPIO 16   | TX         | UART RX  | Serial communication |
| GPIO 17   | TX         | UART TX  | Serial communication |
| GND       | GND        | Ground   | Common ground |
| 3.3V/5V   | VCC        | Power    | Use appropriate voltage |
| GPIO 4    | RST        | Reset    | Optional hardware reset |
| GPIO 2    | PWR        | Power Control | Optional power control |

### Power Supply Options

**Option 1: External 5V Supply**
```
5V Supply → SIM800 VCC
GND → Common Ground
```

**Option 2: Li-ion Battery (Recommended)**
```
3.7V Li-ion → SIM800 VCC
GND → Common Ground
```

**Option 3: USB Power (with proper regulation)**
```
USB 5V → Buck Converter → 4.2V → SIM800 VCC
```

## ⚙️ Software Setup

### PlatformIO (Recommended)

1. **Install PlatformIO**:
   - Install Visual Studio Code
   - Add PlatformIO IDE extension

2. **Create Project**:
   ```bash
   mkdir esp32-sim800-test
   cd esp32-sim800-test
   pio init --board esp32dev
   ```

3. **Copy Files**:
   - Copy all source files to `src/` directory
   - Use the provided `platformio.ini` configuration

4. **Configure Settings**:
   - Edit `src/config.h` for your specific requirements
   - Set your carrier's APN settings
   - Configure HTTP and MQTT endpoints

5. **Build and Upload**:
   ```bash
   pio run --target upload
   pio device monitor
   ```

### Arduino IDE

1. **Install ESP32 Board Package**:
   - Add `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` to Additional Board Manager URLs
   - Install "ESP32 by Espressif Systems"

2. **Select Board**:
   - Tools → Board → ESP32 Arduino → ESP32 Dev Module

3. **Configure Port**:
   - Tools → Port → (Select your ESP32 port)

4. **Copy Code**:
   - Create new sketch and copy `main.cpp` content
   - Create tabs for `SIM800Manager.h`, `SIM800Manager.cpp`, and `config.h`

## 🔧 Configuration

### Carrier APN Settings

Edit `src/config.h` and update these values for your carrier:

```cpp
// Common APN Settings by Carrier
// Replace with your carrier's settings

// T-Mobile: "fast.t-mobile.com"
// AT&T: "phone" or "wap.cingular"
// Verizon: "vzwinternet"  
// Vodafone: "internet.vodafone.net"
// Orange: "orange.fr" or "orange"

#define DEFAULT_APN "internet"        // Your carrier's APN
#define APN_USER ""                   // Username (if required)
#define APN_PASS ""                   // Password (if required)
```

### HTTP Testing Configuration

```cpp
#define HTTP_TEST_HOST "httpbin.org"
#define HTTP_TEST_PORT 80
#define HTTP_GET_PATH "/get"
#define HTTP_POST_PATH "/post"
```

### MQTT Configuration

```cpp
#define MQTT_BROKER "broker.emqx.io"    // Public broker for testing
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "ESP32-SIM800-Test"
#define MQTT_TOPIC_PUB "esp32/sim800/data"
#define MQTT_TOPIC_SUB "esp32/sim800/commands"
```

## 🧪 Test Sequence

The system runs comprehensive tests in the following order:

### 1. Basic AT Command Tests
- AT communication verification
- Module information retrieval
- IMEI and ICCID reading
- **Expected Result**: All commands respond correctly

### 2. SIM Card Verification
- SIM card presence and readiness check
- **Expected Result**: SIM card status "READY"

### 3. Network Registration Tests
- Signal quality measurement (RSSI, BER)
- Network registration status
- **Expected Result**: Registered on home or roaming network

### 4. GPS Functionality Tests
- GPS power-on and initialization
- Satellite acquisition (30-second timeout)
- Location data parsing
- **Expected Result**: GPS fix with 4+ satellites

### 5. GPRS Connection Tests  
- APN configuration
- GPRS connection establishment
- IP address assignment
- **Expected Result**: Valid IP address assigned

### 6. HTTP Client Tests
- HTTP GET request to test endpoint
- HTTP POST request with JSON data
- **Expected Result**: Successful HTTP 200 responses

### 7. MQTT Communication Tests
- MQTT broker connection
- Message publishing
- Topic subscription
- **Expected Result**: Successful MQTT operations

## 📊 Understanding Test Results

### Serial Monitor Output

The system provides detailed output during testing:

```
=================================
ESP32 SIM800 Comprehensive Test
=================================

1. Initializing SIM800 Module...
SUCCESS: SIM800 module initialized

2. Running Basic AT Command Tests...
  Testing AT command... PASS
  Getting module info... PASS
    Module: SIM800L R14.18
  Getting IMEI... PASS
    IMEI: 123456789012345

...

=================================
        TEST SUMMARY
=================================
Basic AT Tests:     PASS
SIM Card Test:      PASS
Network Test:       PASS
GPS Test:           PASS
GPRS Test:          PASS
HTTP GET Test:      PASS
HTTP POST Test:     PASS
MQTT Test:          PASS
=================================
Overall Result: 8/8 tests passed
🎉 ALL TESTS PASSED! 🎉
```

### Status Indicators

- **PASS**: Test completed successfully
- **FAIL**: Test failed (check connections/configuration)
- **TIMEOUT**: Operation timed out (normal for GPS indoors)
- **SKIP**: Test skipped due to prerequisite failure

## 🔍 Troubleshooting

### Common Issues and Solutions

#### 1. SIM800 Not Responding
```
Symptoms: "FAILED: Could not initialize SIM800 module"
Solutions:
- Check power supply (minimum 2A capability)
- Verify wiring connections
- Ensure baud rate is correct (9600)
- Try hardware reset
```

#### 2. No Network Registration
```
Symptoms: "FAIL - Not registered" or poor signal
Solutions:
- Check antenna connection
- Move to area with better coverage
- Verify SIM card has active service
- Check if PIN is required
```

#### 3. GPRS Connection Failed
```
Symptoms: "FAIL - Could not connect to GPRS"
Solutions:
- Verify APN settings for your carrier
- Check if data plan is active
- Ensure network registration is successful
- Try different APN settings
```

#### 4. GPS No Fix
```
Symptoms: "TIMEOUT - Could not get GPS fix"
Solutions:
- Move outdoors with clear sky view
- Wait longer (GPS cold start can take 5+ minutes)
- Check GPS antenna connection
- Verify GPS is enabled in code
```

#### 5. HTTP/MQTT Tests Failing
```
Symptoms: Connection timeouts or failed requests
Solutions:
- Confirm GPRS connection is working
- Check internet connectivity
- Verify endpoint URLs are accessible
- Test with simple HTTP endpoints first
```

### Debug Options

Enable detailed debugging in `config.h`:

```cpp
#define DEBUG_ENABLED 1
#define DEBUG_AT_COMMANDS 1    // Show all AT command exchanges
#define DEBUG_GPS 1            // GPS-specific debugging
#define DEBUG_HTTP 1           // HTTP request/response details
#define DEBUG_MQTT 1           // MQTT communication details
```

### Power Supply Issues

SIM800 modules are sensitive to power supply quality:

- **Insufficient current**: Module resets during transmission
- **Voltage drops**: Communication errors and resets  
- **Poor regulation**: Erratic behavior and failed connections

**Solutions**:
- Use dedicated 4.2V/2A power supply
- Add large capacitors (1000µF+) near module
- Use short, thick power wires
- Consider external power supply for testing

## 📱 Carrier-Specific Notes

### Popular Carrier Settings

**T-Mobile (US)**:
```cpp
#define DEFAULT_APN "fast.t-mobile.com"
#define APN_USER ""
#define APN_PASS ""
```

**AT&T (US)**:
```cpp
#define DEFAULT_APN "phone"
#define APN_USER ""
#define APN_PASS ""
```

**Verizon (US)**:
```cpp
#define DEFAULT_APN "vzwinternet"
#define APN_USER ""
#define APN_PASS ""
```

**Vodafone (EU)**:
```cpp
#define DEFAULT_APN "internet.vodafone.net"
#define APN_USER ""
#define APN_PASS ""
```

## 🚀 Advanced Usage

### Integration into Your Projects

```cpp
#include "SIM800Manager.h"

SIM800Manager sim800;

void setup() {
    Serial.begin(115200);
    
    // Initialize SIM800
    if (sim800.begin()) {
        Serial.println("SIM800 ready!");
        
        // Connect to network
        if (sim800.configureAPN("your-apn")) {
            if (sim800.connectGPRS()) {
                Serial.println("Internet connected!");
                
                // Your application code here
                sendSensorData();
            }
        }
    }
}

void sendSensorData() {
    String jsonData = "{\"temperature\":25.6,\"humidity\":60}";
    String response;
    
    if (sim800.httpPost("api.yourserver.com", 80, "/data", jsonData, response)) {
        Serial.println("Data sent successfully!");
    }
}
```

### Custom MQTT Implementation

For production MQTT usage, consider using the PubSubClient library alongside this code for more robust MQTT functionality.

## 📄 API Reference

### Core Methods

#### Initialization
- `begin()` - Initialize SIM800 communication
- `reset()` - Hardware/software reset
- `isReady()` - Check if module is ready

#### Network Operations
- `checkSIMCard()` - Verify SIM card status
- `getSignalQuality()` - Get RSSI and BER values
- `getNetworkStatus()` - Check network registration
- `configureAPN()` - Set carrier APN settings
- `connectGPRS()` - Establish internet connection

#### GPS Operations
- `enableGPS()` - Power on GPS
- `getGPSData()` - Retrieve location information
- `waitForGPSFix()` - Wait for satellite lock

#### Communication
- `httpGet()` - Perform HTTP GET request
- `httpPost()` - Perform HTTP POST request  
- `mqttConnect()` - Connect to MQTT broker
- `mqttPublish()` - Publish MQTT message

## 🤝 Contributing

Contributions are welcome! Please read our contributing guidelines and submit pull requests for any improvements.

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🙏 Acknowledgments

- ESP32 Arduino Core team
- SIM800 module documentation and community
- MQTT and HTTP protocol specifications
- Open source contributors and testers

## 📞 Support

If you encounter issues:

1. Check the troubleshooting section
2. Enable debug output to get detailed information
3. Verify your hardware connections and power supply
4. Test with known working carrier settings
5. Create an issue with detailed debug output

Happy coding! 🚀📡