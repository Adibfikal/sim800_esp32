/*
 * ESP32 SIM800 Basic Usage Example
 * 
 * This example demonstrates the basic usage of the SIM800Manager class
 * for simple IoT applications like sensor data transmission.
 * 
 * This is a simplified version of the main test suite, focusing on
 * practical usage rather than comprehensive testing.
 * 
 * Hardware Connections:
 * - SIM800 RX -> ESP32 GPIO16
 * - SIM800 TX -> ESP32 GPIO17
 * - SIM800 VCC -> 5V (or 3.7V LiPo)
 * - SIM800 GND -> GND
 * - Optional: Reset pin -> GPIO4
 * - Optional: Power pin -> GPIO2
 */

#include "config.h"
#include "SIM800Manager.h"

SIM800Manager sim800;

void setup() {
  Serial.begin(DEBUG_BAUD);
  delay(2000);
  
  Serial.println("ESP32 SIM800 Basic Usage Example");
  Serial.println("=================================");
  
  // Initialize SIM800
  Serial.println("Initializing SIM800...");
  if (!sim800.begin()) {
    Serial.println("Failed to initialize SIM800!");
    while(1) delay(1000);
  }
  Serial.println("SIM800 initialized successfully");
  
  // Check SIM card
  if (!sim800.checkSIMCard()) {
    Serial.println("Warning: SIM card not ready");
  }
  
  // Wait for network registration
  Serial.println("Waiting for network registration...");
  int attempts = 0;
  while (sim800.getNetworkStatus() != NET_REGISTERED_HOME && 
         sim800.getNetworkStatus() != NET_REGISTERED_ROAMING && 
         attempts < 30) {
    delay(2000);
    attempts++;
    Serial.print(".");
  }
  
  if (attempts >= 30) {
    Serial.println("\nNetwork registration failed!");
    while(1) delay(1000);
  }
  Serial.println("\nNetwork registered successfully");
  
  // Configure and connect GPRS
  Serial.println("Connecting to internet...");
  if (sim800.configureAPN(DEFAULT_APN, APN_USER, APN_PASS)) {
    if (sim800.connectGPRS()) {
      String ip;
      if (sim800.getIPAddress(ip)) {
        Serial.println("Internet connected! IP: " + ip);
      }
    } else {
      Serial.println("Failed to connect to internet");
      while(1) delay(1000);
    }
  }
  
  Serial.println("Setup complete - starting main loop");
}

void loop() {
  // Simulate reading sensor data
  float temperature = 25.5 + (random(-50, 50) / 10.0);  // Simulated temperature
  float humidity = 60.0 + (random(-200, 200) / 10.0);   // Simulated humidity
  
  // Create JSON payload
  String jsonData = "{";
  jsonData += "\"device\":\"ESP32-SIM800\",";
  jsonData += "\"timestamp\":" + String(millis()) + ",";
  jsonData += "\"temperature\":" + String(temperature) + ",";
  jsonData += "\"humidity\":" + String(humidity);
  jsonData += "}";
  
  Serial.println("Sending sensor data: " + jsonData);
  
  // Send HTTP POST request
  String response;
  if (sim800.httpPost("httpbin.org", 80, "/post", jsonData, response)) {
    Serial.println("Data sent successfully!");
    // You can parse the response here if needed
  } else {
    Serial.println("Failed to send data");
  }
  
  // Alternatively, send via MQTT
  /*
  if (sim800.mqttConnect(MQTT_BROKER, MQTT_PORT, MQTT_CLIENT_ID)) {
    if (sim800.mqttPublish(MQTT_TOPIC_PUB, jsonData)) {
      Serial.println("MQTT data published!");
    }
    sim800.mqttDisconnect();
  }
  */
  
  // Wait 30 seconds before next transmission
  Serial.println("Waiting 30 seconds...\n");
  delay(30000);
}

/*
 * Additional utility functions you might need:
 */

// Function to send GPS data
void sendGPSData() {
  if (!sim800.isGPSEnabled()) {
    sim800.enableGPS();
    delay(5000);
  }
  
  GPSData gpsData;
  if (sim800.getGPSData(gpsData) && gpsData.isValid) {
    String gpsJson = "{";
    gpsJson += "\"latitude\":" + String(gpsData.latitude, 6) + ",";
    gpsJson += "\"longitude\":" + String(gpsData.longitude, 6) + ",";
    gpsJson += "\"altitude\":" + String(gpsData.altitude, 2) + ",";
    gpsJson += "\"satellites\":" + String(gpsData.satellites);
    gpsJson += "}";
    
    String response;
    if (sim800.httpPost("your-server.com", 80, "/gps", gpsJson, response)) {
      Serial.println("GPS data sent: " + gpsJson);
    }
  }
}

// Function to check connection health
bool checkConnectionHealth() {
  // Check if GPRS is still connected
  if (!sim800.isGPRSConnected()) {
    Serial.println("GPRS connection lost, reconnecting...");
    return sim800.connectGPRS();
  }
  
  // Check signal quality
  SignalQuality sq = sim800.getSignalQuality();
  if (sq.isValid && sq.rssi < 5) {
    Serial.println("Warning: Poor signal quality (RSSI: " + String(sq.rssi) + ")");
    return false;
  }
  
  return true;
}

// Function to handle emergency/alert messages
void sendAlert(String alertMessage) {
  String alertJson = "{";
  alertJson += "\"type\":\"alert\",";
  alertJson += "\"message\":\"" + alertMessage + "\",";
  alertJson += "\"timestamp\":" + String(millis());
  alertJson += "}";
  
  // Try HTTP first
  String response;
  if (!sim800.httpPost("your-server.com", 80, "/alert", alertJson, response)) {
    // If HTTP fails, try MQTT
    if (sim800.mqttConnect(MQTT_BROKER, MQTT_PORT, MQTT_CLIENT_ID)) {
      sim800.mqttPublish("alerts/" MQTT_CLIENT_ID, alertJson);
      sim800.mqttDisconnect();
    }
  }
}

/*
 * Configuration tips:
 * 
 * 1. Update DEFAULT_APN in config.h for your carrier
 * 2. Change HTTP endpoint to your own server
 * 3. Adjust transmission interval based on your needs
 * 4. Add error handling for production use
 * 5. Consider adding sleep modes for battery operation
 * 
 * For production deployment:
 * - Add watchdog timer for reliability
 * - Implement proper error recovery
 * - Add data buffering for offline scenarios  
 * - Use secure connections (HTTPS/TLS)
 * - Implement proper power management
 */