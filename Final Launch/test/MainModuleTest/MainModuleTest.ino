#include "Logger.h"
#include "StorageModule.h"
#include "ElectromechanicalModule.h"
#include "MockSensorModule.h"
#include <stdint.h>

#include <vector>

// Enums
#define STATE_STARTUP 0
#define STATE_PRE_APOGEE 1
#define STATE_DROGUE_DEPLOYED 2
#define STATE_MAIN_DEPLOYED 3
#define STATE_LANDED 4

// Telemetry packet related data
#define TELEMETRY_PACKET_STRING_LENGTH 23

static const char startupStr[2] = {'S', 'T'};
static const char preApogeeStr[2] = {'P', 'R'};
static const char drogueDeployedStr[2] = {'D', 'D'};
static const char mainDeployedStr[2] = {'M', 'D'};
static const char landedStr[2] = {'L', 'A'};

const char* STATE_STRING_ARRAY[5] = {startupStr, preApogeeStr, drogueDeployedStr, mainDeployedStr, landedStr};


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
#define MAIN_PARACHUTE_DEPLOYMENT_ALTITUDE 300 // In meters

std::vector<float> latestAltitudes;


// Modules
StorageModule storageModule = StorageModule();
ElectromechanicalModule electromechanicalModule = ElectromechanicalModule();
SensorModule sensorModule = SensorModule();

void setup() {
  Serial.begin(9600);
  
  storageModule.init();
  sensorModule.init();
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

        createTelemetryPacketStr(storageModule.packetCount, currentState, altitude, acceleration);
        storageModule.addTelemetryPacketToFlightLog(telPacketString);

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

        latestAltitudes.push_back(altitude);
        if (latestAltitudes.size() > MAX_LATEST_ALTITUDES) latestAltitudes.erase(latestAltitudes.begin());

        createTelemetryPacketStr(storageModule.packetCount, currentState, altitude, acceleration);
        storageModule.addTelemetryPacketToFlightLog(telPacketString);

        if (detectApogee(altitude, acceleration)){
          Logger::log("Apogee reached.");
          Logger::debug("Altitude: " + String(altitude));
          switchToState(STATE_DROGUE_DEPLOYED);
        }
        lastMilis = currMillis;
      }
      break;
    case STATE_DROGUE_DEPLOYED:
      if (currMillis - lastMilis > 500) {
        float altitude = sensorModule.getAltitude();
        float acceleration = sensorModule.getAcceleration();

        latestAltitudes.push_back(altitude);
        if (latestAltitudes.size() > MAX_LATEST_ALTITUDES_AFTER_APOGEE) latestAltitudes.erase(latestAltitudes.begin());

        createTelemetryPacketStr(storageModule.packetCount, currentState, altitude, acceleration);
        storageModule.addTelemetryPacketToFlightLog(telPacketString);

        if (detectMainDeployment(altitude, acceleration)){
          Logger::log("Main parachute altitude (300m) reached.");
          switchToState(STATE_MAIN_DEPLOYED);
        }
        lastMilis = currMillis;
      }
      break;
    case STATE_MAIN_DEPLOYED:
      if (currMillis - lastMilis > 500) {
        float altitude = sensorModule.getAltitude();
        float acceleration = sensorModule.getAcceleration();

        latestAltitudes.push_back(altitude);
        if (latestAltitudes.size() > MAX_LATEST_ALTITUDES_AFTER_APOGEE) latestAltitudes.erase(latestAltitudes.begin());

        createTelemetryPacketStr(storageModule.packetCount, currentState, altitude, acceleration);
        storageModule.addTelemetryPacketToFlightLog(telPacketString);

        if (detectLanding(altitude, acceleration)){
          Logger::log("Vehicle landed :)");
          switchToState(STATE_LANDED);
        }
        lastMilis = currMillis;
      }
      break;
    case STATE_LANDED:
      if (currMillis - lastMilis > 2000) {
        float altitude = sensorModule.getAltitude();

        createTelemetryPacketStr(storageModule.packetCount, currentState, 0, 0);
        storageModule.addTelemetryPacketToFlightLog(telPacketString);

        lastMilis = currMillis;
      }
      break;
  }
}

void switchToState(int8_t newState) {
  currentState = newState;
  Logger::log("Switching to state: " + String(currentState));
  storageModule.saveCurrentState(currentState);
  switch (currentState) {
    case STATE_DROGUE_DEPLOYED:
      electromechanicalModule.deployDrogueParachute();
      break;
    case STATE_MAIN_DEPLOYED:
      electromechanicalModule.deployMainParachute();
      break;
  }
}

bool detectLaunch(float currAltitude, float currAccel) {
  if (latestAltitudes.size() < MAX_LATEST_ALTITUDES) return false;
  if (biggerThan(latestAltitudes.at(latestAltitudes.size()-1), latestAltitudes.at(latestAltitudes.size()-2), LAUNCH_DETECTION_MARGIN))
      return true;
  return false;
}

bool detectApogee(float currAltitude, float currAccel) {
  if (latestAltitudes.size() < MAX_LATEST_ALTITUDES) return false;
  if (smallerThan(latestAltitudes.at(2), latestAltitudes.at(1), APOGEE_DETECTION_MARGIN) &&
      smallerThan(latestAltitudes.at(1), latestAltitudes.at(0), APOGEE_DETECTION_MARGIN))
      return true;
  return false;
}

bool detectLanding(float currAltitude, float currAccel) {
  Logger::debug("Detecting landing...");
  return latestAltitudes.back() < LANDING_ALTITUDE;
}

bool detectMainDeployment(float currAltitude, float currAccel) {
  Logger::debug("Detecting main deployment...");
  return latestAltitudes.back() < MAIN_PARACHUTE_DEPLOYMENT_ALTITUDE;
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

// Format is <PACKET_COUNT>,<STATE>,<ALTITUDE>,<ACCELERATION>
// Example result: 10000,ST,9999.9,9999.9
//                 00021,PO,0604.2,9999.9
char* createTelemetryPacketStr(uint16_t packetCount, uint8_t currentState, float altitude, float acceleration) {
  Logger::debug("Creating telemetry packet...");

  sprintf(telPacketString, "%05d", packetCount);
  
  telPacketString[5] = ',';

  memcpy(telPacketString + 6, STATE_STRING_ARRAY[currentState], 2);
  
  telPacketString[8] = ',';
  
  sprintf(telPacketString + 9, "%06.1f", altitude);
  
  telPacketString[15] = ',';

  sprintf(telPacketString + 16, "%06.1f", acceleration);
  
  telPacketString[22] = '\0';
  
  // if (gpsLat < 0){
  //   sprintf(telPacketString + 16, "%08.5f", gpsLat);
  // } else {
  //   telPacketString[16] = ' ';
  //   sprintf(telPacketString + 17, "%08.5f", gpsLat);
  // }
  
  // telPacketString[25] = ',';

  // if (gpsLng < 0) {
  //   sprintf(telPacketString + 26, "%08.5f", gpsLng);
  // } else {
  //   telPacketString[26] = ' ';
  //   sprintf(telPacketString + 27, "%08.5f", gpsLng);    
  // }
  
  // telPacketString[16] = '\0';
  
  return telPacketString;
}