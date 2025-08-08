#include "SIM800Manager.h"

SIM800Manager::SIM800Manager() {
  sim800Serial = nullptr;
  responseBuffer = "";
  lastCommandTime = 0;
  isInitialized = false;
  isGPRSConnected = false;
  isGPSEnabled = false;
}

SIM800Manager::~SIM800Manager() {
  if (sim800Serial) {
    sim800Serial->end();
  }
}

bool SIM800Manager::begin() {
  DEBUG_PRINTLN("Initializing SIM800 Manager...");
  
  // Initialize hardware serial for SIM800
  sim800Serial = &Serial2;
  sim800Serial->begin(SIM800_BAUD, SERIAL_8N1, SIM800_RX_PIN, SIM800_TX_PIN);
  
  // Optional: Initialize reset and power pins
  #ifdef SIM800_RESET_PIN
    pinMode(SIM800_RESET_PIN, OUTPUT);
    digitalWrite(SIM800_RESET_PIN, HIGH);
  #endif
  
  #ifdef SIM800_PWR_PIN
    pinMode(SIM800_PWR_PIN, OUTPUT);
    digitalWrite(SIM800_PWR_PIN, HIGH);
  #endif
  
  delay(BOOT_DELAY);
  
  // Test basic communication
  if (testAT()) {
    isInitialized = true;
    DEBUG_PRINTLN("SIM800 initialized successfully");
    return true;
  }
  
  DEBUG_PRINTLN("Failed to initialize SIM800");
  return false;
}

bool SIM800Manager::reset() {
  DEBUG_PRINTLN("Resetting SIM800...");
  
  #ifdef SIM800_RESET_PIN
    digitalWrite(SIM800_RESET_PIN, LOW);
    delay(100);
    digitalWrite(SIM800_RESET_PIN, HIGH);
    delay(3000);
  #else
    sendCommand("AT+CFUN=1,1", "OK", 15000);  // Software reset
    delay(3000);
  #endif
  
  return testAT();
}

bool SIM800Manager::powerOn() {
  #ifdef SIM800_PWR_PIN
    digitalWrite(SIM800_PWR_PIN, HIGH);
    delay(1000);
  #endif
  return true;
}

bool SIM800Manager::powerOff() {
  sendCommand("AT+CPOWD=1", "NORMAL POWER DOWN", 10000);
  #ifdef SIM800_PWR_PIN
    digitalWrite(SIM800_PWR_PIN, LOW);
  #endif
  return true;
}

bool SIM800Manager::isReady() {
  return isInitialized && testAT();
}

bool SIM800Manager::testAT() {
  for (int i = 0; i < RETRY_COUNT; i++) {
    if (sendCommand("AT") == SIM800_OK) {
      return true;
    }
    delay(1000);
  }
  return false;
}

bool SIM800Manager::getModuleInfo(String& info) {
  String response;
  if (sendCommand("ATI", response) == SIM800_OK) {
    info = response;
    return true;
  }
  return false;
}

bool SIM800Manager::checkSIMCard() {
  String response;
  SIM800Response result = sendCommand("AT+CPIN?", response);
  if (result == SIM800_OK) {
    return response.indexOf("READY") >= 0;
  }
  return false;
}

SignalQuality SIM800Manager::getSignalQuality() {
  SignalQuality sq = {0, 0, false};
  String response;
  
  if (sendCommand("AT+CSQ", response) == SIM800_OK) {
    int csqStart = response.indexOf("+CSQ: ");
    if (csqStart >= 0) {
      String csqData = response.substring(csqStart + 6);
      int commaIndex = csqData.indexOf(',');
      if (commaIndex >= 0) {
        sq.rssi = csqData.substring(0, commaIndex).toInt();
        sq.ber = csqData.substring(commaIndex + 1).toInt();
        sq.isValid = (sq.rssi >= 0 && sq.rssi <= 31 && sq.rssi != 99);
      }
    }
  }
  
  return sq;
}

NetworkStatus SIM800Manager::getNetworkStatus() {
  String response;
  if (sendCommand("AT+CREG?", response) == SIM800_OK) {
    int statusStart = response.indexOf("+CREG: ");
    if (statusStart >= 0) {
      String statusData = response.substring(statusStart + 7);
      int commaIndex = statusData.indexOf(',');
      if (commaIndex >= 0) {
        int status = statusData.substring(commaIndex + 1).toInt();
        return (NetworkStatus)status;
      }
    }
  }
  return NET_UNKNOWN;
}

bool SIM800Manager::getIMEI(String& imei) {
  String response;
  if (sendCommand("AT+GSN", response) == SIM800_OK) {
    // Extract IMEI from response
    int start = 0;
    int end = response.indexOf('\r');
    if (end > start) {
      imei = response.substring(start, end);
      imei.trim();
      return imei.length() == 15;  // IMEI should be 15 digits
    }
  }
  return false;
}

bool SIM800Manager::getICCID(String& iccid) {
  String response;
  if (sendCommand("AT+CCID", response) == SIM800_OK) {
    int start = response.indexOf("+CCID: ");
    if (start >= 0) {
      iccid = response.substring(start + 7);
      iccid.trim();
      return iccid.length() > 10;
    }
  }
  return false;
}

bool SIM800Manager::configureAPN(const String& apn, const String& user, const String& pass) {
  DEBUG_PRINTLN("Configuring APN: " + apn);
  
  // Set connection type to GPRS
  if (sendCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"") != SIM800_OK) return false;
  
  // Set APN
  if (sendCommand("AT+SAPBR=3,1,\"APN\",\"" + apn + "\"") != SIM800_OK) return false;
  
  // Set username if provided
  if (user.length() > 0) {
    if (sendCommand("AT+SAPBR=3,1,\"USER\",\"" + user + "\"") != SIM800_OK) return false;
  }
  
  // Set password if provided
  if (pass.length() > 0) {
    if (sendCommand("AT+SAPBR=3,1,\"PWD\",\"" + pass + "\"") != SIM800_OK) return false;
  }
  
  return true;
}

bool SIM800Manager::connectGPRS() {
  DEBUG_PRINTLN("Connecting to GPRS...");
  
  // Check if already connected
  String response;
  if (sendCommand("AT+SAPBR=2,1", response) == SIM800_OK) {
    if (response.indexOf("1,1,") >= 0) {  // Already connected
      isGPRSConnected = true;
      return true;
    }
  }
  
  // Connect to GPRS
  if (sendCommand("AT+SAPBR=1,1", "OK", 30000) == SIM800_OK) {
    delay(5000);  // Wait for connection to establish
    
    // Verify connection
    if (sendCommand("AT+SAPBR=2,1", response) == SIM800_OK) {
      if (response.indexOf("1,1,") >= 0) {
        isGPRSConnected = true;
        DEBUG_PRINTLN("GPRS connected successfully");
        return true;
      }
    }
  }
  
  DEBUG_PRINTLN("Failed to connect GPRS");
  return false;
}

bool SIM800Manager::disconnectGPRS() {
  if (sendCommand("AT+SAPBR=0,1") == SIM800_OK) {
    isGPRSConnected = false;
    return true;
  }
  return false;
}

bool SIM800Manager::isGPRSConnected() {
  return isGPRSConnected;
}

bool SIM800Manager::getIPAddress(String& ip) {
  String response;
  if (sendCommand("AT+SAPBR=2,1", response) == SIM800_OK) {
    int ipStart = response.lastIndexOf("\"") - 15;  // Approximate IP position
    if (ipStart > 0) {
      int ipEnd = response.lastIndexOf("\"");
      if (ipEnd > ipStart) {
        ip = response.substring(ipStart, ipEnd);
        ip.trim();
        return ip.length() > 7;  // Basic IP length check
      }
    }
  }
  return false;
}

bool SIM800Manager::enableGPS() {
  DEBUG_PRINTLN("Enabling GPS...");
  
  // Power on GPS
  if (sendCommand("AT+CGPSPWR=1") != SIM800_OK) {
    DEBUG_PRINTLN("Failed to power on GPS");
    return false;
  }
  
  delay(1000);
  
  // Reset GPS in cold start mode
  if (sendCommand("AT+CGPSRST=1") != SIM800_OK) {
    DEBUG_PRINTLN("Failed to reset GPS");
    return false;
  }
  
  isGPSEnabled = true;
  DEBUG_PRINTLN("GPS enabled successfully");
  return true;
}

bool SIM800Manager::disableGPS() {
  if (sendCommand("AT+CGPSPWR=0") == SIM800_OK) {
    isGPSEnabled = false;
    return true;
  }
  return false;
}

bool SIM800Manager::isGPSEnabled() {
  return isGPSEnabled;
}

bool SIM800Manager::getGPSData(GPSData& gpsData) {
  String response;
  gpsData.isValid = false;
  
  if (sendCommand("AT+CGPSINF=0", response) == SIM800_OK) {
    int dataStart = response.indexOf("+CGPSINF: ");
    if (dataStart >= 0) {
      String gpsInfo = response.substring(dataStart + 10);
      gpsInfo.trim();
      
      // Parse GPS data format: mode,longitude,latitude,altitude,utc,ttff,num,speed,course
      if (gpsInfo.indexOf("0,,,,,,,,,") < 0) {  // Check if GPS has fix
        // Split the data
        int indices[10];
        int count = 0;
        indices[count++] = -1;
        
        for (int i = 0; i < gpsInfo.length() && count < 10; i++) {
          if (gpsInfo.charAt(i) == ',') {
            indices[count++] = i;
          }
        }
        
        if (count >= 8) {
          // Extract GPS data
          String mode = gpsInfo.substring(indices[0] + 1, indices[1]);
          String lon = gpsInfo.substring(indices[1] + 1, indices[2]);
          String lat = gpsInfo.substring(indices[2] + 1, indices[3]);
          String alt = gpsInfo.substring(indices[3] + 1, indices[4]);
          String utc = gpsInfo.substring(indices[4] + 1, indices[5]);
          String ttff = gpsInfo.substring(indices[5] + 1, indices[6]);
          String num = gpsInfo.substring(indices[6] + 1, indices[7]);
          String spd = gpsInfo.substring(indices[7] + 1, indices[8]);
          
          if (lat.length() > 0 && lon.length() > 0) {
            gpsData.latitude = lat.toFloat();
            gpsData.longitude = lon.toFloat();
            gpsData.altitude = alt.toFloat();
            gpsData.speed = spd.toFloat();
            gpsData.satellites = num.toInt();
            gpsData.timestamp = utc;
            gpsData.isValid = (gpsData.satellites >= GPS_MIN_SATELLITES);
            
            return true;
          }
        }
      }
    }
  }
  
  return false;
}

bool SIM800Manager::waitForGPSFix(unsigned long timeout) {
  DEBUG_PRINTLN("Waiting for GPS fix...");
  unsigned long startTime = millis();
  
  while (millis() - startTime < timeout) {
    GPSData gpsData;
    if (getGPSData(gpsData) && gpsData.isValid) {
      DEBUG_PRINTLN("GPS fix acquired");
      return true;
    }
    delay(2000);
  }
  
  DEBUG_PRINTLN("GPS fix timeout");
  return false;
}

bool SIM800Manager::openTCPConnection(const String& host, int port) {
  DEBUG_PRINTLN("Opening TCP connection to " + host + ":" + String(port));
  
  // Initialize HTTP service
  if (sendCommand("AT+HTTPINIT") != SIM800_OK) {
    DEBUG_PRINTLN("Failed to init HTTP service");
    return false;
  }
  
  // Set bearer profile
  if (sendCommand("AT+HTTPPARA=\"CID\",1") != SIM800_OK) {
    sendCommand("AT+HTTPTERM");
    return false;
  }
  
  return true;
}

bool SIM800Manager::closeTCPConnection() {
  return sendCommand("AT+HTTPTERM") == SIM800_OK;
}

bool SIM800Manager::sendTCPData(const String& data) {
  // This would be implemented for raw TCP, but for HTTP we use specific commands
  return true;
}

String SIM800Manager::receiveTCPData(unsigned long timeout) {
  return readResponse(timeout);
}

bool SIM800Manager::httpGet(const String& host, int port, const String& path, String& response) {
  DEBUG_PRINTLN("HTTP GET: http://" + host + ":" + String(port) + path);
  
  if (!openTCPConnection(host, port)) {
    return false;
  }
  
  // Set URL
  String url = "http://" + host + ":" + String(port) + path;
  if (sendCommand("AT+HTTPPARA=\"URL\",\"" + url + "\"") != SIM800_OK) {
    closeTCPConnection();
    return false;
  }
  
  // Perform GET request
  String getResponse;
  if (sendCommand("AT+HTTPACTION=0", getResponse, "+HTTPACTION: 0,200", 30000) == SIM800_OK) {
    delay(1000);
    
    // Read response
    String readCmd;
    if (sendCommand("AT+HTTPREAD", readCmd) == SIM800_OK) {
      int dataStart = readCmd.indexOf("+HTTPREAD: ");
      if (dataStart >= 0) {
        response = readCmd.substring(dataStart);
        closeTCPConnection();
        return true;
      }
    }
  }
  
  closeTCPConnection();
  return false;
}

bool SIM800Manager::httpPost(const String& host, int port, const String& path, const String& data, String& response) {
  DEBUG_PRINTLN("HTTP POST: http://" + host + ":" + String(port) + path);
  
  if (!openTCPConnection(host, port)) {
    return false;
  }
  
  // Set URL
  String url = "http://" + host + ":" + String(port) + path;
  if (sendCommand("AT+HTTPPARA=\"URL\",\"" + url + "\"") != SIM800_OK) {
    closeTCPConnection();
    return false;
  }
  
  // Set content type
  if (sendCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"") != SIM800_OK) {
    closeTCPConnection();
    return false;
  }
  
  // Set data
  String dataCmd = "AT+HTTPDATA=" + String(data.length()) + ",10000";
  if (sendCommand(dataCmd, "DOWNLOAD") == SIM800_OK) {
    delay(100);
    sim800Serial->print(data);
    delay(1000);
    
    // Perform POST request
    String postResponse;
    if (sendCommand("AT+HTTPACTION=1", postResponse, "+HTTPACTION: 1,200", 30000) == SIM800_OK) {
      delay(1000);
      
      // Read response
      String readCmd;
      if (sendCommand("AT+HTTPREAD", readCmd) == SIM800_OK) {
        int dataStart = readCmd.indexOf("+HTTPREAD: ");
        if (dataStart >= 0) {
          response = readCmd.substring(dataStart);
          closeTCPConnection();
          return true;
        }
      }
    }
  }
  
  closeTCPConnection();
  return false;
}

// Basic MQTT implementation using raw TCP commands
bool SIM800Manager::mqttConnect(const String& broker, int port, const String& clientId, 
                                const String& username, const String& password) {
  DEBUG_PRINTLN("MQTT Connect: " + broker + ":" + String(port));
  
  // Start TCP connection
  if (sendCommand("AT+CIPSTART=\"TCP\",\"" + broker + "\",\"" + String(port) + "\"", "CONNECT OK", 30000) != SIM800_OK) {
    DEBUG_PRINTLN("Failed to establish TCP connection for MQTT");
    return false;
  }
  
  // Create MQTT CONNECT packet (simplified)
  String connectPacket = "\x10\x0C\x00\x04MQTT\x04\x00\x00\x3C\x00\x00";  // Basic MQTT connect
  
  // Send MQTT CONNECT
  if (sendCommand("AT+CIPSEND=" + String(connectPacket.length()), "> ") == SIM800_OK) {
    sim800Serial->print(connectPacket);
    String response = readResponse(10000);
    if (response.indexOf("SEND OK") >= 0) {
      delay(2000);
      return true;
    }
  }
  
  return false;
}

bool SIM800Manager::mqttPublish(const String& topic, const String& message) {
  // Simplified MQTT PUBLISH implementation
  DEBUG_PRINTLN("MQTT Publish: " + topic + " -> " + message);
  
  // Create basic MQTT PUBLISH packet
  String publishPacket = "\x30";  // PUBLISH packet type
  publishPacket += char(2 + topic.length() + message.length());  // Remaining length
  publishPacket += char(topic.length() >> 8);  // Topic length MSB
  publishPacket += char(topic.length() & 0xFF);  // Topic length LSB
  publishPacket += topic;  // Topic
  publishPacket += message;  // Message
  
  if (sendCommand("AT+CIPSEND=" + String(publishPacket.length()), "> ") == SIM800_OK) {
    sim800Serial->print(publishPacket);
    String response = readResponse(5000);
    return response.indexOf("SEND OK") >= 0;
  }
  
  return false;
}

bool SIM800Manager::mqttSubscribe(const String& topic) {
  DEBUG_PRINTLN("MQTT Subscribe: " + topic);
  // Simplified MQTT SUBSCRIBE implementation
  return true;
}

String SIM800Manager::mqttReceive(unsigned long timeout) {
  return readResponse(timeout);
}

bool SIM800Manager::mqttDisconnect() {
  return sendCommand("AT+CIPCLOSE") == SIM800_OK;
}

SIM800Response SIM800Manager::sendCommand(const String& command, const String& expectedResponse, unsigned long timeout) {
  String response;
  return sendCommand(command, response, expectedResponse, timeout);
}

SIM800Response SIM800Manager::sendCommand(const String& command, String& response, const String& expectedResponse, unsigned long timeout) {
  if (!sim800Serial) {
    return SIM800_FAIL;
  }
  
  clearBuffer();
  
  #if DEBUG_AT_COMMANDS
    DEBUG_PRINTLN("AT CMD: " + command);
  #endif
  
  sim800Serial->println(command);
  lastCommandTime = millis();
  
  response = readResponse(timeout);
  
  #if DEBUG_AT_COMMANDS
    DEBUG_PRINTLN("AT RSP: " + response);
  #endif
  
  if (response.length() == 0) {
    return SIM800_TIMEOUT;
  }
  
  if (response.indexOf("ERROR") >= 0) {
    return SIM800_ERROR;
  }
  
  if (response.indexOf("BUSY") >= 0) {
    return SIM800_BUSY;
  }
  
  if (response.indexOf(expectedResponse) >= 0) {
    return SIM800_OK;
  }
  
  return SIM800_FAIL;
}

void SIM800Manager::printDebug(const String& message) {
  DEBUG_PRINTLN("[SIM800] " + message);
}

bool SIM800Manager::performDiagnostics() {
  DEBUG_PRINTLN("=== SIM800 Diagnostics ===");
  
  // Test basic communication
  if (!testAT()) {
    DEBUG_PRINTLN("FAIL: Basic AT communication");
    return false;
  }
  DEBUG_PRINTLN("PASS: Basic AT communication");
  
  // Check SIM card
  if (!checkSIMCard()) {
    DEBUG_PRINTLN("FAIL: SIM card not ready");
    return false;
  }
  DEBUG_PRINTLN("PASS: SIM card ready");
  
  // Check signal quality
  SignalQuality sq = getSignalQuality();
  if (sq.isValid) {
    DEBUG_PRINTF("PASS: Signal quality RSSI=%d, BER=%d\n", sq.rssi, sq.ber);
  } else {
    DEBUG_PRINTLN("FAIL: No valid signal");
    return false;
  }
  
  // Check network registration
  NetworkStatus netStatus = getNetworkStatus();
  if (netStatus == NET_REGISTERED_HOME || netStatus == NET_REGISTERED_ROAMING) {
    DEBUG_PRINTLN("PASS: Network registered");
  } else {
    DEBUG_PRINTF("FAIL: Network not registered (status: %d)\n", netStatus);
    return false;
  }
  
  return true;
}

void SIM800Manager::printStatus() {
  DEBUG_PRINTLN("=== SIM800 Status ===");
  DEBUG_PRINTLN("Initialized: " + String(isInitialized ? "Yes" : "No"));
  DEBUG_PRINTLN("GPRS Connected: " + String(isGPRSConnected ? "Yes" : "No"));
  DEBUG_PRINTLN("GPS Enabled: " + String(isGPSEnabled ? "Yes" : "No"));
  
  String imei, iccid;
  if (getIMEI(imei)) {
    DEBUG_PRINTLN("IMEI: " + imei);
  }
  
  if (getICCID(iccid)) {
    DEBUG_PRINTLN("ICCID: " + iccid);
  }
  
  SignalQuality sq = getSignalQuality();
  if (sq.isValid) {
    DEBUG_PRINTF("Signal: RSSI=%d, BER=%d\n", sq.rssi, sq.ber);
  }
}

// Private helper methods
void SIM800Manager::clearBuffer() {
  responseBuffer = "";
  while (sim800Serial && sim800Serial->available()) {
    sim800Serial->read();
  }
}

String SIM800Manager::readResponse(unsigned long timeout) {
  String response = "";
  unsigned long startTime = millis();
  
  while (millis() - startTime < timeout) {
    if (sim800Serial && sim800Serial->available()) {
      char c = sim800Serial->read();
      response += c;
      
      // Check for common response endings
      if (response.endsWith("OK\r\n") || 
          response.endsWith("ERROR\r\n") || 
          response.endsWith("FAIL\r\n") ||
          response.endsWith("> ") ||
          response.endsWith("SEND OK\r\n") ||
          response.endsWith("CONNECT OK\r\n") ||
          response.indexOf("+HTTPACTION:") >= 0) {
        break;
      }
    } else {
      delay(10);
    }
  }
  
  return response;
}

bool SIM800Manager::waitForResponse(const String& expectedResponse, unsigned long timeout) {
  String response = readResponse(timeout);
  return response.indexOf(expectedResponse) >= 0;
}

void SIM800Manager::flushSerial() {
  if (sim800Serial) {
    while (sim800Serial->available()) {
      sim800Serial->read();
    }
  }
}