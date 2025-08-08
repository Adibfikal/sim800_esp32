/*
 * ESP32 SIM800 Comprehensive Test
 * 
 * Converted from PlatformIO to Arduino IDE
 * This sketch demonstrates all SIM800 functionality including GPS, GPRS, HTTP, and MQTT
 */

#include "config.h"
#include "SIM800Manager.h"

// Global SIM800 manager instance
SIM800Manager sim800;

// Test result tracking
struct TestResults {
  bool basicATTests = false;
  bool simCardTest = false;
  bool networkTest = false;
  bool gpsTest = false;
  bool gprsTest = false;
  bool httpGetTest = false;
  bool httpPostTest = false;
  bool mqttTest = false;
};

TestResults testResults;

void setup() {
  // Initialize serial for debug output
  Serial.begin(DEBUG_BAUD);
  delay(2000);
  
  DEBUG_PRINTLN("=================================");
  DEBUG_PRINTLN("ESP32 SIM800 Comprehensive Test");
  DEBUG_PRINTLN("=================================");
  
  // Initialize SIM800
  DEBUG_PRINTLN("\n1. Initializing SIM800 Module...");
  if (!sim800.begin()) {
    DEBUG_PRINTLN("FAILED: Could not initialize SIM800 module");
    DEBUG_PRINTLN("Check wiring and power supply");
    while(1) {
      delay(1000);
      DEBUG_PRINT(".");
    }
  }
  
  DEBUG_PRINTLN("SUCCESS: SIM800 module initialized");
  delay(2000);
  
  // Run comprehensive tests
  runBasicATTests();
  runSIMCardTest();
  runNetworkTests();
  runGPSTests();
  runGPRSTests();
  runHTTPTests();
  runMQTTTests();
  
  // Print final results
  printTestSummary();
}

void loop() {
  // Keep the system alive and periodically check status
  delay(10000);
  
  static unsigned long lastStatusCheck = 0;
  if (millis() - lastStatusCheck > 60000) {  // Every 60 seconds
    DEBUG_PRINTLN("\n=== Periodic Status Check ===");
    sim800.printStatus();
    
    // Test GPS if enabled
    if (sim800.isGPSEnabled()) {
      GPSData gpsData;
      if (sim800.getGPSData(gpsData)) {
        if (gpsData.isValid) {
          DEBUG_PRINTF("GPS: Lat=%.6f, Lon=%.6f, Alt=%.2f, Sats=%d\n", 
                      gpsData.latitude, gpsData.longitude, gpsData.altitude, gpsData.satellites);
        } else {
          DEBUG_PRINTLN("GPS: Searching for satellites...");
        }
      }
    }
    
    lastStatusCheck = millis();
  }
}

void runBasicATTests() {
  DEBUG_PRINTLN("\n2. Running Basic AT Command Tests...");
  
  // Test basic AT communication
  DEBUG_PRINT("  Testing AT command... ");
  if (sim800.testAT()) {
    DEBUG_PRINTLN("PASS");
  } else {
    DEBUG_PRINTLN("FAIL");
    return;
  }
  
  // Get module information
  DEBUG_PRINT("  Getting module info... ");
  String moduleInfo;
  if (sim800.getModuleInfo(moduleInfo)) {
    DEBUG_PRINTLN("PASS");
    DEBUG_PRINTLN("    Module: " + moduleInfo);
  } else {
    DEBUG_PRINTLN("FAIL");
  }
  
  // Get IMEI
  DEBUG_PRINT("  Getting IMEI... ");
  String imei;
  if (sim800.getIMEI(imei)) {
    DEBUG_PRINTLN("PASS");
    DEBUG_PRINTLN("    IMEI: " + imei);
  } else {
    DEBUG_PRINTLN("FAIL");
  }
  
  // Get ICCID
  DEBUG_PRINT("  Getting ICCID... ");
  String iccid;
  if (sim800.getICCID(iccid)) {
    DEBUG_PRINTLN("PASS");
    DEBUG_PRINTLN("    ICCID: " + iccid);
  } else {
    DEBUG_PRINTLN("FAIL");
  }
  
  testResults.basicATTests = true;
  DEBUG_PRINTLN("SUCCESS: Basic AT tests completed");
}

void runSIMCardTest() {
  DEBUG_PRINTLN("\n3. Testing SIM Card...");
  
  DEBUG_PRINT("  Checking SIM card status... ");
  if (sim800.checkSIMCard()) {
    DEBUG_PRINTLN("PASS - SIM card ready");
    testResults.simCardTest = true;
  } else {
    DEBUG_PRINTLN("FAIL - SIM card not ready or missing");
    DEBUG_PRINTLN("  Please check SIM card installation");
  }
}

void runNetworkTests() {
  DEBUG_PRINTLN("\n4. Testing Network Registration...");
  
  // Check signal quality
  DEBUG_PRINT("  Checking signal quality... ");
  SignalQuality sq = sim800.getSignalQuality();
  if (sq.isValid) {
    DEBUG_PRINTLN("PASS");
    DEBUG_PRINTF("    RSSI: %d dBm, BER: %d\n", sq.rssi, sq.ber);
    
    // Interpret RSSI
    if (sq.rssi >= 20) {
      DEBUG_PRINTLN("    Signal: Excellent");
    } else if (sq.rssi >= 15) {
      DEBUG_PRINTLN("    Signal: Good");
    } else if (sq.rssi >= 10) {
      DEBUG_PRINTLN("    Signal: Fair");
    } else if (sq.rssi >= 5) {
      DEBUG_PRINTLN("    Signal: Poor");
    } else {
      DEBUG_PRINTLN("    Signal: Very Poor");
    }
  } else {
    DEBUG_PRINTLN("FAIL - No signal detected");
  }
  
  // Check network registration
  DEBUG_PRINT("  Checking network registration... ");
  NetworkStatus netStatus = sim800.getNetworkStatus();
  switch (netStatus) {
    case NET_NOT_REGISTERED:
      DEBUG_PRINTLN("FAIL - Not registered");
      break;
    case NET_REGISTERED_HOME:
      DEBUG_PRINTLN("PASS - Registered (home network)");
      testResults.networkTest = true;
      break;
    case NET_SEARCHING:
      DEBUG_PRINTLN("SEARCHING - Still searching for network");
      break;
    case NET_REGISTRATION_DENIED:
      DEBUG_PRINTLN("FAIL - Registration denied");
      break;
    case NET_REGISTERED_ROAMING:
      DEBUG_PRINTLN("PASS - Registered (roaming)");
      testResults.networkTest = true;
      break;
    default:
      DEBUG_PRINTLN("UNKNOWN - Unknown status");
      break;
  }
  
  if (!testResults.networkTest) {
    DEBUG_PRINTLN("  Network tests failed - some features may not work");
  }
}

void runGPSTests() {
  DEBUG_PRINTLN("\n5. Testing GPS Functionality...");
  
  // Enable GPS
  DEBUG_PRINT("  Enabling GPS... ");
  if (sim800.enableGPS()) {
    DEBUG_PRINTLN("PASS");
  } else {
    DEBUG_PRINTLN("FAIL - Could not enable GPS");
    return;
  }
  
  delay(2000);  // Wait for GPS to stabilize
  
  // Try to get GPS fix
  DEBUG_PRINT("  Waiting for GPS fix (30s timeout)... ");
  if (sim800.waitForGPSFix(30000)) {
    DEBUG_PRINTLN("PASS");
    
    // Get detailed GPS data
    GPSData gpsData;
    if (sim800.getGPSData(gpsData)) {
      DEBUG_PRINTLN("  GPS Data:");
      DEBUG_PRINTF("    Latitude: %.6f\n", gpsData.latitude);
      DEBUG_PRINTF("    Longitude: %.6f\n", gpsData.longitude);
      DEBUG_PRINTF("    Altitude: %.2f m\n", gpsData.altitude);
      DEBUG_PRINTF("    Speed: %.2f km/h\n", gpsData.speed);
      DEBUG_PRINTF("    Satellites: %d\n", gpsData.satellites);
      DEBUG_PRINTLN("    Timestamp: " + gpsData.timestamp);
      
      testResults.gpsTest = true;
    }
  } else {
    DEBUG_PRINTLN("TIMEOUT - Could not get GPS fix");
    DEBUG_PRINTLN("  This may be normal indoors or without clear sky view");
    
    // Still try to get raw GPS data
    GPSData gpsData;
    if (sim800.getGPSData(gpsData)) {
      DEBUG_PRINTF("  Raw GPS data - Satellites visible: %d\n", gpsData.satellites);
    }
  }
}

void runGPRSTests() {
  DEBUG_PRINTLN("\n6. Testing GPRS Connection...");
  
  if (!testResults.networkTest) {
    DEBUG_PRINTLN("SKIP - Network not registered, skipping GPRS tests");
    return;
  }
  
  // Configure APN
  DEBUG_PRINT("  Configuring APN... ");
  if (sim800.configureAPN(DEFAULT_APN, APN_USER, APN_PASS)) {
    DEBUG_PRINTLN("PASS");
  } else {
    DEBUG_PRINTLN("FAIL - Could not configure APN");
    return;
  }
  
  // Connect to GPRS
  DEBUG_PRINT("  Connecting to GPRS... ");
  if (sim800.connectGPRS()) {
    DEBUG_PRINTLN("PASS");
    
    // Get IP address
    String ipAddress;
    if (sim800.getIPAddress(ipAddress)) {
      DEBUG_PRINTLN("  IP Address: " + ipAddress);
    }
    
    testResults.gprsTest = true;
  } else {
    DEBUG_PRINTLN("FAIL - Could not connect to GPRS");
    DEBUG_PRINTLN("  Check APN settings and network coverage");
  }
}

void runHTTPTests() {
  DEBUG_PRINTLN("\n7. Testing HTTP Functionality...");
  
  if (!testResults.gprsTest) {
    DEBUG_PRINTLN("SKIP - GPRS not connected, skipping HTTP tests");
    return;
  }
  
  // HTTP GET Test
  DEBUG_PRINT("  Testing HTTP GET... ");
  String getResponse;
  if (sim800.httpGet(HTTP_TEST_HOST, HTTP_TEST_PORT, HTTP_GET_PATH, getResponse)) {
    DEBUG_PRINTLN("PASS");
    DEBUG_PRINTLN("  Response preview: " + getResponse.substring(0, 100) + "...");
    testResults.httpGetTest = true;
  } else {
    DEBUG_PRINTLN("FAIL - HTTP GET request failed");
  }
  
  delay(2000);  // Wait between requests
  
  // HTTP POST Test
  DEBUG_PRINT("  Testing HTTP POST... ");
  String postResponse;
  if (sim800.httpPost(HTTP_TEST_HOST, HTTP_TEST_PORT, HTTP_POST_PATH, HTTP_POST_DATA, postResponse)) {
    DEBUG_PRINTLN("PASS");
    DEBUG_PRINTLN("  Response preview: " + postResponse.substring(0, 100) + "...");
    testResults.httpPostTest = true;
  } else {
    DEBUG_PRINTLN("FAIL - HTTP POST request failed");
  }
}

void runMQTTTests() {
  DEBUG_PRINTLN("\n8. Testing MQTT Functionality...");
  
  if (!testResults.gprsTest) {
    DEBUG_PRINTLN("SKIP - GPRS not connected, skipping MQTT tests");
    return;
  }
  
  // MQTT Connect Test
  DEBUG_PRINT("  Connecting to MQTT broker... ");
  if (sim800.mqttConnect(MQTT_BROKER, MQTT_PORT, MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    DEBUG_PRINTLN("PASS");
    
    delay(2000);
    
    // MQTT Publish Test
    DEBUG_PRINT("  Testing MQTT publish... ");
    String testMessage = "{\"device\":\"ESP32-SIM800\",\"timestamp\":" + String(millis()) + ",\"test\":\"mqtt_publish\"}";
    
    if (sim800.mqttPublish(MQTT_TOPIC_PUB, testMessage)) {
      DEBUG_PRINTLN("PASS");
      DEBUG_PRINTLN("  Published: " + testMessage);
      testResults.mqttTest = true;
    } else {
      DEBUG_PRINTLN("FAIL - MQTT publish failed");
    }
    
    delay(1000);
    
    // MQTT Subscribe Test (simplified)
    DEBUG_PRINT("  Testing MQTT subscribe... ");
    if (sim800.mqttSubscribe(MQTT_TOPIC_SUB)) {
      DEBUG_PRINTLN("PASS");
    } else {
      DEBUG_PRINTLN("FAIL - MQTT subscribe failed");
    }
    
    delay(2000);
    
    // Disconnect MQTT
    sim800.mqttDisconnect();
    DEBUG_PRINTLN("  MQTT disconnected");
    
  } else {
    DEBUG_PRINTLN("FAIL - Could not connect to MQTT broker");
    DEBUG_PRINTLN("  Check broker settings and internet connectivity");
  }
}

void printTestSummary() {
  DEBUG_PRINTLN("\n=================================");
  DEBUG_PRINTLN("        TEST SUMMARY");
  DEBUG_PRINTLN("=================================");
  
  int passCount = 0;
  int totalTests = 8;
  
  DEBUG_PRINTLN("Basic AT Tests:     " + String(testResults.basicATTests ? "PASS" : "FAIL"));
  if (testResults.basicATTests) passCount++;
  
  DEBUG_PRINTLN("SIM Card Test:      " + String(testResults.simCardTest ? "PASS" : "FAIL"));
  if (testResults.simCardTest) passCount++;
  
  DEBUG_PRINTLN("Network Test:       " + String(testResults.networkTest ? "PASS" : "FAIL"));
  if (testResults.networkTest) passCount++;
  
  DEBUG_PRINTLN("GPS Test:           " + String(testResults.gpsTest ? "PASS" : "FAIL"));
  if (testResults.gpsTest) passCount++;
  
  DEBUG_PRINTLN("GPRS Test:          " + String(testResults.gprsTest ? "PASS" : "FAIL"));
  if (testResults.gprsTest) passCount++;
  
  DEBUG_PRINTLN("HTTP GET Test:      " + String(testResults.httpGetTest ? "PASS" : "FAIL"));
  if (testResults.httpGetTest) passCount++;
  
  DEBUG_PRINTLN("HTTP POST Test:     " + String(testResults.httpPostTest ? "PASS" : "FAIL"));
  if (testResults.httpPostTest) passCount++;
  
  DEBUG_PRINTLN("MQTT Test:          " + String(testResults.mqttTest ? "PASS" : "FAIL"));
  if (testResults.mqttTest) passCount++;
  
  DEBUG_PRINTLN("=================================");
  DEBUG_PRINTF("Overall Result: %d/%d tests passed\n", passCount, totalTests);
  
  if (passCount == totalTests) {
    DEBUG_PRINTLN("🎉 ALL TESTS PASSED! 🎉");
    DEBUG_PRINTLN("Your SIM800 module is working perfectly!");
  } else if (passCount >= 5) {
    DEBUG_PRINTLN("✅ Most tests passed - SIM800 is functional");
    DEBUG_PRINTLN("Some advanced features may need configuration");
  } else if (passCount >= 3) {
    DEBUG_PRINTLN("⚠️  Basic functionality working");
    DEBUG_PRINTLN("Check network coverage and APN settings");
  } else {
    DEBUG_PRINTLN("❌ Major issues detected");
    DEBUG_PRINTLN("Check hardware connections and SIM card");
  }
  
  DEBUG_PRINTLN("=================================");
  
  // Provide next steps
  DEBUG_PRINTLN("\nNext Steps:");
  if (!testResults.simCardTest) {
    DEBUG_PRINTLN("- Insert a valid SIM card with active service");
  }
  if (!testResults.networkTest) {
    DEBUG_PRINTLN("- Check antenna connection and network coverage");
  }
  if (!testResults.gpsTest) {
    DEBUG_PRINTLN("- Move to location with clear sky view for GPS");
  }
  if (!testResults.gprsTest) {
    DEBUG_PRINTLN("- Verify APN settings for your carrier");
  }
  if (!testResults.httpGetTest || !testResults.httpPostTest) {
    DEBUG_PRINTLN("- Check internet connectivity and firewall settings");
  }
  if (!testResults.mqttTest) {
    DEBUG_PRINTLN("- Verify MQTT broker settings and credentials");
  }
  
  DEBUG_PRINTLN("\nTest completed. System will continue monitoring...");
}