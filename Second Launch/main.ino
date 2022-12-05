#include "StorageModule.h"
#include "CommunicationModule.h"
#include "ElectromechanicalModule.h"
#include "SensorModule.h"
#include <stdint.h>
#include <stdio.h>
#include <Wire.h>
#include <TimerOne.h>

// External Component pins



// Enums
#define STATE_STARTUP 0
#define STATE_PRE_APOGEE 1
#define STATE_POST_APOGEE 2
#define STATE_LANDED 3


// Telemetry packet related data
#define TELEMETRY_PACKET_STRING_LENGTH 32

static const uint8_t startupStr[2] = {'S', 'T'};
static const uint8_t preApogeeStr[10] = {'P', 'R'};
static const uint8_t postApogeeStr[10] = {'P', 'O'};
static const uint8_t landedStr[10] = {'L', 'A'};

uint8_t* STATE_STRING_ARRAY[4] = {startupStr, preApogeeStr, postApogeeStr, landedStr};


// State variables
uint8_t currentState = STATE_STARTUP;
uint8_t telPacketString[TELEMETRY_PACKET_STRING_LENGTH]; // Assumed null terminated
unsigned long lastMilis = 0;


// Modules
StorageModule storageModule = StorageModule();
CommunicationModule communicationModule = CommunicationModule();
ElectromechanicalModule electromechanicalModule = ElectromechanicalModule();
SensorModule sensorModule = SensorModule();

void setup() {
  Serial.begin(9600);
  
  storageModule.init(&currentState, STATE_STARTUP);
  sensorModule.init();
  communicationModule.init();
  electromechanicalModule.init();

  lastMilis = millis();


  switchToState(currentState);
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
          double gpsLat = sensorModule.gps.location.lat();
          double gpsLng = sensorModule.gps.location.lng();

          createTelemetryPacketStr(communicationModule.packetCount, currentState, altitude, gpsLat, gpsLng);
          storageModule.addTelemetryPacketToFlightLog(telPacketString, communicationModule.packetCount);
          communicationModule.sendTelemetryPacketToGround(telPacketString, TELEMETRY_PACKET_STRING_LENGTH);

          if (detectApogee(altitude, acceleration)){
            Serial.println("Apogee reached.");
            Serial.println(altitude);
            switchToState(STATE_PRE_APOGEE);
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

            createTelemetryPacketStr(pcommunicationModule.acketCount, currentState, altitude, gpsLat, gpsLng);
            storageModule.addTelemetryPacketToFlightLog(telPacketString);
            communicationModule.telemetryPacketQueue.add(telPacketString, TELEMETRY_PACKET_STRING_LENGTH);

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

            createTelemetryPacketStr(communicationModule.packetCount, currentState, 0, gpsLat, gpsLng);
            communicationModule.telemetryPacketQueue.add(telPacketString, TELEMETRY_PACKET_STRING_LENGTH);

            lastMilis = currMillis;
        }
      break;
  }
}

void switchToState(int8_t newState) {
  currentState = newState;
  Serial.println(currentState);
  storageModule.saveCurrentState(currentState);
  if (currentState == STATE_POST_APOGEE){
    Serial.println("Deploying parachute");
    electromechanicalModule.deployParachute();
  }
}

function detectLaunch(float currAltitude, float currAccel) {

}

function detectApogee(float currAltitude, float currAccel) {

}

function detectLanding(float currAltitude, float currAccel) {

}

// Format is <PACKET_COUNT>,<STATE>,<ALTITUDE>,<GPS_LATITUDE>,<GPS_LONGITUDE>
// Example result: 10000,ST,9999,42.30402,34.30402
uint8_t* createTelemetryPacketStr(uint16_t packetCount, uint8_t currentState, float altitude, double gpsLat, double gpsLng) {
   uint8_t buffer[9];
   unsigned char precision = 1, latLngPrecision = 5, bufferPadding;
   
   itoa(packetCount, buffer, 10);
   bufferPadding =  5 - strlen(buffer);
   memcpy(telPacketString + bufferPadding, buffer, strlen(buffer));
   telPacketString[5] = ',';
   memcpy(telPacketString + 6, STATE_STRING_ARRAY[currentState], 2);
   telPacketString[8] = ',';
   dtostrf(altitude, 6, precision, telPacketString + 9);
   telPacketString[15] = ',';
   dtostrf(gpsLat, 8, latLngPrecision, telPacketString + 16);
   telPacketString[24] = ',';
   dtostrf(gpsLng, 8, latLngPrecision, telPacketString + 25);
   telPacketString[33] = '\0';
   return telPacketString;
}