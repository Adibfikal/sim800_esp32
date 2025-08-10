#ifndef SIM800MANAGER_H
#define SIM800MANAGER_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include "config.h"

// Response codes
enum SIM800Response {
  SIM800_OK = 0,
  SIM800_ERROR = 1,
  SIM800_TIMEOUT_RESP = 2,
  SIM800_FAIL = 3,
  SIM800_BUSY = 4
};

// Network status
enum NetworkStatus {
  NET_NOT_REGISTERED = 0,
  NET_REGISTERED_HOME = 1,
  NET_SEARCHING = 2,
  NET_REGISTRATION_DENIED = 3,
  NET_UNKNOWN = 4,
  NET_REGISTERED_ROAMING = 5
};

// GPS Data structure
struct GPSData {
  bool isValid;
  float latitude;
  float longitude;
  float altitude;
  float speed;
  int satellites;
  String timestamp;
  float hdop;  // Horizontal dilution of precision
};

// Signal quality structure
struct SignalQuality {
  int rssi;        // Received signal strength indicator
  int ber;         // Bit error rate
  bool isValid;
};

class SIM800Manager {
private:
  HardwareSerial* sim800Serial;
  String responseBuffer;
  unsigned long lastCommandTime;
  bool isInitialized;
  bool gprsConnected;
  bool gpsEnabled;
  
  // Private helper methods
  void clearBuffer();
  String readResponse(unsigned long timeout = SIM800_TIMEOUT);
  bool waitForResponse(const String& expectedResponse, unsigned long timeout = SIM800_TIMEOUT);
  void flushSerial();
  
public:
  SIM800Manager();
  ~SIM800Manager();
  
  // Initialization and basic control
  bool begin();
  bool reset();
  bool powerOn();
  bool powerOff();
  bool isReady();
  
  // Basic AT command testing
  bool testAT();
  bool getModuleInfo(String& info);
  bool checkSIMCard();
  SignalQuality getSignalQuality();
  NetworkStatus getNetworkStatus();
  bool getIMEI(String& imei);
  bool getICCID(String& iccid);
  
  // GPRS/Network functions
  bool configureAPN(const String& apn, const String& user = "", const String& pass = "");
  bool connectGPRS();
  bool disconnectGPRS();
  bool isGPRSConnected();
  bool getIPAddress(String& ip);
  
  // GPS functions
  bool enableGPS();
  bool disableGPS();
  bool isGPSEnabled();
  bool getGPSData(GPSData& gpsData);
  bool waitForGPSFix(unsigned long timeout = GPS_TIMEOUT);
  
  // TCP/HTTP functions
  bool openTCPConnection(const String& host, int port);
  bool closeTCPConnection();
  bool sendTCPData(const String& data);
  String receiveTCPData(unsigned long timeout = 5000);
  bool httpGet(const String& host, int port, const String& path, String& response);
  bool httpPost(const String& host, int port, const String& path, const String& data, String& response);
  
  // MQTT helper functions (basic TCP operations for MQTT)
  bool mqttConnect(const String& broker, int port, const String& clientId, 
                   const String& username = "", const String& password = "");
  bool mqttPublish(const String& topic, const String& message);
  bool mqttSubscribe(const String& topic);
  String mqttReceive(unsigned long timeout = 5000);
  bool mqttDisconnect();
  
  // Utility functions
  SIM800Response sendCommand(const String& command, const String& expectedResponse = "OK", 
                            unsigned long timeout = SIM800_TIMEOUT);
  SIM800Response sendCommand(const String& command, String& response, 
                            const String& expectedResponse = "OK", unsigned long timeout = SIM800_TIMEOUT);
  void printDebug(const String& message);
  
  // Status monitoring
  bool performDiagnostics();
  void printStatus();
};

#endif // SIM800MANAGER_H