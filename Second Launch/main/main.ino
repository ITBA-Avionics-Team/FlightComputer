#include "Logger.h"
#include "StorageModule.h"
#include "CommunicationModule.h"
#include "ElectromechanicalModule.h"
#include "SensorModule.h"
#include <stdint.h>

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
        
        if (detectLaunch(altitude, acceleration)){
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

            createTelemetryPacketStr(communicationModule.packetCount, currentState, altitude, 0, 0);
            storageModule.addTelemetryPacketToFlightLog(telPacketString);
            communicationModule.sendTelemetryPacketToGround(telPacketString, TELEMETRY_PACKET_STRING_LENGTH);

            if (detectLanding(altitude, acceleration)){
              Serial.println("Landed. :)");
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
  Serial.println("Detecting launch...");
  return true;
}

bool detectApogee(float currAltitude, float currAccel) {
  return false;
}

bool detectLanding(float currAltitude, float currAccel) {
  return false;
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