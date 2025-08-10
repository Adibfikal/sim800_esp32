#ifndef CONFIG_H
#define CONFIG_H

// Hardware Configuration
#define SIM800_RX_PIN 16
#define SIM800_TX_PIN 17
#define SIM800_BAUD 9600
#define SIM800_RESET_PIN 4  // Optional reset pin
#define SIM800_PWR_PIN 2    // Optional power control pin

// Serial Configuration
#define DEBUG_BAUD 115200
#define SIM800_CMD_TIMEOUT 10000  // 10 seconds timeout for commands
#define GPS_TIMEOUT 30000     // 30 seconds for GPS fix

// Network Configuration
#define DEFAULT_APN "internet"           // Change this to your carrier's APN
#define APN_USER ""                      // APN username (if required)
#define APN_PASS ""                      // APN password (if required)

// HTTP Test Configuration
#define HTTP_TEST_HOST "httpbin.org"
#define HTTP_TEST_PORT 80
#define HTTP_GET_PATH "/get"
#define HTTP_POST_PATH "/post"
#define HTTP_POST_DATA "{\"device\":\"ESP32-SIM800\",\"test\":\"post_request\"}"

// MQTT Configuration
#define MQTT_BROKER "broker.emqx.io"    // Public MQTT broker for testing
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "ESP32-SIM800-Test"
#define MQTT_USERNAME ""                 // Leave empty for anonymous
#define MQTT_PASSWORD ""                 // Leave empty for anonymous
#define MQTT_TOPIC_PUB "esp32/sim800/data"
#define MQTT_TOPIC_SUB "esp32/sim800/commands"

// GPS Configuration
#define GPS_UPDATE_INTERVAL 5000         // GPS update interval in ms
#define GPS_MIN_SATELLITES 4             // Minimum satellites for valid fix

// PlatformIO Build Flags converted to defines
#define CORE_DEBUG_LEVEL 1
#define BOARD_HAS_PSRAM
// Note: -mfix-esp32-psram-cache-issue is a compiler flag, not a define

// Debug Configuration (can be controlled by Arduino IDE Tools menu)
// Uncomment the following lines to enable debug features:
#define DEBUG_ENABLED 1
#define DEBUG_AT_COMMANDS 1
#define DEBUG_GPS 1
#define DEBUG_HTTP 1
#define DEBUG_MQTT 1

// Alternative: Use these for release builds
// #define DEBUG_ENABLED 0
// #define DEBUG_AT_COMMANDS 0
// #define DEBUG_GPS 0
// #define DEBUG_HTTP 0
// #define DEBUG_MQTT 0

// Timing Configuration
#define BOOT_DELAY 5000                  // Initial boot delay
#define COMMAND_DELAY 1000               // Delay between commands
#define RETRY_COUNT 3                    // Number of retries for failed commands

// Debug macros
#if DEBUG_ENABLED
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(format, ...) Serial.printf(format, ##__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(format, ...)
#endif

/*
 * Configuration Notes for Arduino IDE:
 * 
 * 1. Update DEFAULT_APN for your carrier:
 *    - Telkomsel: "internet"
 *    - Indosat: "indosatgprs"
 *    - XL: "www.xlgprs.net"
 *    - Tri/3: "3gprs"
 *    - Smartfren: "smart"
 * 
 * 2. ESP32 Board Selection:
 *    - Use "ESP32 Dev Module" or "DOIT ESP32 DEVKIT V1"
 *    - Set Flash Size: 4MB
 *    - Partition Scheme: Default 4MB
 * 
 * 3. For debugging:
 *    - Enable DEBUG_ENABLED for detailed output
 *    - Disable for production to save memory
 * 
 * 4. Pin Configuration:
 *    - SIM800 RX -> ESP32 GPIO16
 *    - SIM800 TX -> ESP32 GPIO17
 *    - Optional: Reset pin -> GPIO4
 *    - Optional: Power pin -> GPIO2
 */

#endif // CONFIG_H