#include "Logger.h"
#include "MockStorageModule.h"
#include "MockCommunicationModule.h"
#include "MockElectromechanicalModule.h"
#include "MockSensorModule.h"
#include <stdint.h>

#include <vector>

// Enums
#define STATE_STARTUP 0
#define STATE_PRE_APOGEE 1
#define STATE_POST_APOGEE 2
#define STATE_LANDED 3

// Telemetry packet related data
#define TELEMETRY_PACKET_STRING_LENGTH 34

static const char startupStr[2] = {'S', 'T'};
static const char preApogeeStr[10] = {'P', 'R'};
static const char postApogeeStr[10] = {'P', 'O'};
static const char landedStr[10] = {'L', 'A'};

const char* STATE_STRING_ARRAY[4] = {startupStr, preApogeeStr, postApogeeStr, landedStr};


// State variables
uint8_t currentState = STATE_STARTUP;
char telPacketString[TELEMETRY_PACKET_STRING_LENGTH]; // Assumed null terminated
unsigned long lastMilis = 0;


// Launch/Apogee detection algorithms
#define MAX_LATEST_ALTITUDES 3
#define MAX_LATEST_ALTITUDES_AFTER_APOGEE 7
#define LAUNCH_DETECTION_MARGIN 5
#define APOGEE_DETECTION_MARGIN 0.8
#define LANDING_DETECTION_MARGIN 0.8
#define LANDING_ALTITUDE 3
std::vector<float> latestAltitudes;


// Modules
StorageModule storageModule = StorageModule();
CommunicationModule communicationModule = CommunicationModule();
ElectromechanicalModule electromechanicalModule = ElectromechanicalModule();
SensorModule sensorModule = SensorModule();

void setup() {
  Serial.begin(9600);
  
  storageModule.init();
  sensorModule.init();
  communicationModule.init(storageModule);
  electromechanicalModule.init();

  lastMilis = millis();

  switchToState(storageModule.loadCurrentState(STATE_STARTUP));
}

void loop() {
  unsigned long currMillis = millis();
  
  switch(currentState) {
    case STATE_STARTUP:
      if (currMillis - lastMilis > 100) {
        float altitude = sensorModule.getAltitude();
        float acceleration = sensorModule.getAcceleration();

        latestAltitudes.push_back(altitude);
        if (latestAltitudes.size() > MAX_LATEST_ALTITUDES) latestAltitudes.erase(latestAltitudes.begin());
        
        if (detectLaunch(altitude, acceleration)){
          Logger::log("Vehicle launched.");
          switchToState(STATE_PRE_APOGEE);
        }
        lastMilis = currMillis;
      }
      break;
    case STATE_PRE_APOGEE:
      if (currMillis - lastMilis > 500) {
          float altitude = sensorModule.getAltitude();
          float acceleration = sensorModule.getAcceleration();
          double gpsLat = sensorModule.gps.location.lat(); // TODO: We could avoid sensing GPS before apogee
          double gpsLng = sensorModule.gps.location.lng();

          latestAltitudes.push_back(altitude);
          if (latestAltitudes.size() > MAX_LATEST_ALTITUDES) latestAltitudes.erase(latestAltitudes.begin());

          createTelemetryPacketStr(communicationModule.packetCount, currentState, altitude, 0, 0);
          storageModule.addTelemetryPacketToFlightLog(telPacketString);
          communicationModule.sendTelemetryPacketToGround(telPacketString, TELEMETRY_PACKET_STRING_LENGTH);

          if (detectApogee(altitude, acceleration)){
            Logger::log("Apogee reached.");
            Logger::debug("Altitude: " + String(altitude));
            switchToState(STATE_POST_APOGEE);
          }
          lastMilis = currMillis;
      }
      break;
    case STATE_POST_APOGEE:
      if (currMillis - lastMilis > 500) {
            float altitude = sensorModule.getAltitude();
            float acceleration = sensorModule.getAcceleration();
            double gpsLat = sensorModule.gps.location.lat();
            double gpsLng = sensorModule.gps.location.lng();

            latestAltitudes.push_back(altitude);
            if (latestAltitudes.size() > MAX_LATEST_ALTITUDES_AFTER_APOGEE) latestAltitudes.erase(latestAltitudes.begin());

            createTelemetryPacketStr(communicationModule.packetCount, currentState, altitude, 0, 0);
            storageModule.addTelemetryPacketToFlightLog(telPacketString);
            communicationModule.sendTelemetryPacketToGround(telPacketString, TELEMETRY_PACKET_STRING_LENGTH);

            if (detectLanding(altitude, acceleration)){
              Logger::log("Vehicle landed :)");
              Serial.println(altitude);
              switchToState(STATE_LANDED);
            }
            lastMilis = currMillis;
        }
      break;
    case STATE_LANDED:
      if (currMillis - lastMilis > 2000) {
            double gpsLat = sensorModule.gps.location.lat();
            double gpsLng = sensorModule.gps.location.lng();

            createTelemetryPacketStr(communicationModule.packetCount, currentState, 0, 0, 0);
            communicationModule.sendTelemetryPacketToGround(telPacketString, TELEMETRY_PACKET_STRING_LENGTH);

            lastMilis = currMillis;
        }
      break;
  }
}

void switchToState(int8_t newState) {
  currentState = newState;
  Logger::log("Switching to state: " + String(currentState));
  storageModule.saveCurrentState(currentState);
  if (currentState == STATE_POST_APOGEE){
    Serial.println("Deploying parachute");
    electromechanicalModule.deployParachute();
  }
}

bool detectLaunch(float currAltitude, float currAccel) {
  Logger::debug("Detecting launch...");
  if (latestAltitudes.size() < MAX_LATEST_ALTITUDES) return false;
  if (biggerThan(latestAltitudes.at(latestAltitudes.size()-1), latestAltitudes.at(latestAltitudes.size()-2), LAUNCH_DETECTION_MARGIN))
      return true;
  return false;
}

bool detectApogee(float currAltitude, float currAccel) {
  Logger::debug("Detecting apogee...");
  if (latestAltitudes.size() < MAX_LATEST_ALTITUDES) return false;
  if (smallerThan(latestAltitudes.at(2), latestAltitudes.at(1), APOGEE_DETECTION_MARGIN) &&
      smallerThan(latestAltitudes.at(1), latestAltitudes.at(0), APOGEE_DETECTION_MARGIN))
      return true;
  return false;
}

bool detectLanding(float currAltitude, float currAccel) {
  Logger::debug("Detecting landing...");
  return latestAltitudes.back() < LANDING_ALTITUDE;
  // if (latestAltitudes.size() < MAX_LATEST_ALTITUDES_AFTER_APOGEE) return false;
  // for (int i = 1; i < latestAltitudes.size(); i++) {
  //   if (!equals(latestAltitudes.at(i), latestAltitudes.at(i-1), LANDING_DETECTION_MARGIN))
  //     return false;
  // }
  // return true;
}

bool biggerThan(float v1, float v2, float margin) {
  return v1 - v2 > margin;
}

bool smallerThan(float v1, float v2, float margin) {
  return v2 - v1 > margin;
}

bool equals(float v1, float v2, float margin) {
  return abs(v1 - v2) < margin;
}

// Format is <PACKET_COUNT>,<STATE>,<ALTITUDE>,<GPS_LATITUDE>,<GPS_LONGITUDE>
// Example result: 10000,ST,9999,42.30402,34.30402
char* createTelemetryPacketStr(uint16_t packetCount, uint8_t currentState, float altitude, double gpsLat, double gpsLng) {
  Logger::debug("Creating telemetry packet...");

  sprintf(telPacketString, "%05d", packetCount);
  
  telPacketString[5] = ',';

  memcpy(telPacketString + 6, STATE_STRING_ARRAY[currentState], 2);
  
  telPacketString[8] = ',';
  
  sprintf(telPacketString + 9, "%06.1f", altitude);
  
  telPacketString[15] = ',';
  
  sprintf(telPacketString + 16, "%08.5f", gpsLat);
  
  telPacketString[24] = ',';

  sprintf(telPacketString + 25, "%08.5f", gpsLng);
  
  telPacketString[33] = '\0';
  
  return telPacketString;
}