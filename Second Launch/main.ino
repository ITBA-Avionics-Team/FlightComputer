#include "ContainerCommunicationModule.h"
#include "ElectromechanicalModule.h"
#include "SensorModule.h"
#include <stdint.h>
#include <stdio.h>
#include <Wire.h>
#include <EEPROM.h>
#include <DS3231.h>
#include <TimerOne.h>

//External Component pins



//Enums
#define STATE_STARTUP 0
#define STATE_PRE_APOGEE 1
#define STATE_POST_APOGEE 2
#define STATE_LANDED 3

//ROM Memory addresses
#define PACKET_COUNT_EEPROM_ADDR 4
#define CURRENT_STATE_EEPROM_ADDR 0
#define ALTITUDE_EEPROM_ADDR 0
#define GPS_LATITUDE_EEPROM_ADDR 0
#define GPS_LONGITUDE_EEPROM_ADDR 0


#define ALTITUDE_LIST_LENGTH 6
#define TELEMETRY_PACKET_STRING_LENGTH 32

static const uint8_t startupStr[2] = {'S', 'T'};
static const uint8_t preApogeeStr[10] = {'P', 'R'};
static const uint8_t postApogeeStr[10] = {'P', 'O'};
static const uint8_t landedStr[10] = {'L', 'A'};

uint8_t* STATE_STRING_ARRAY[4] = {startupStr, preApogeeStr, postApogeeStr, landedStr};


//State variables
uint8_t currentState = STATE_STARTUP;
uint8_t telPacketString[TELEMETRY_PACKET_STRING_LENGTH]; // Assumed null terminated, I don't think we actually need it to be.
bool hasReachedApogee = false;
float latestAltitudes[ALTITUDE_LIST_LENGTH];
uint8_t latestAltitudesIndex = 0;


ContainerCommunicationModule communicationModule = ContainerCommunicationModule();
ElectromechanicalModule electromechanicalModule = ElectromechanicalModule();
SensorModule sensorModule = SensorModule();


// Sample result: '10000,ST,9999,42.30402,34.30402'
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
   telPacketString[33] = 0;
   return telPacketString;
}

void setup() {
  Serial.begin(9600);
  
  storageModule.init(&currentState); // Retrieve state variable from EEPROM
  sensorModule.init();
  communicationModule.init();
  electromechanicalModule.init();

  currentSec = seconds();
  lastMilis = millis();


  switchToState(currentState);
}


// void takeAllMeasurementsAndSendTelemetry(float altitude){
//   createTelemetryPacketStr(packetCount, currentState, altitude, sensorModule.gps.location.lat(), sensorModule.gps.location.lng());
//   Serial.write(telPacketString, TELEMETRY_PACKET_STRING_LENGTH);
//   Serial.write('\n');
//   communicationModule.telemetryPacketQueue.add(telPacketString, TELEMETRY_PACKET_STRING_LENGTH);
// }

// bool detectLanding(){
//   float max = latestAltitudes[0];
//   float min = latestAltitudes[0];
//   for (int i = 1; i < ALTITUDE_LIST_LENGTH; i++) {
//     if (latestAltitudes[i] < min) min = latestAltitudes[i];
//     if (latestAltitudes[i] < max) max = latestAltitudes[i];
//   }
//   Serial.println("Max - min:");
//   Serial.println(max - min);
//   Serial.println("Max:");
//   Serial.println(max);
//   return max - min < 5 && max < 5;
// }

function detectLaunch(float currAltitude, float currAccel) {

}

function detectApogee(float currAltitude, float currAccel) {

}

function detectLanding(float currAltitude, float currAccel) {

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

          createTelemetryPacketStr(packetCount, currentState, altitude, gpsLat, gpsLng);
          storageModule.addToFlashData(telPacketString);
          communicationModule.telemetryPacketQueue.add(telPacketString, TELEMETRY_PACKET_STRING_LENGTH);

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

            createTelemetryPacketStr(packetCount, currentState, altitude, gpsLat, gpsLng);
            storageModule.addToFlashData(telPacketString);
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

            createTelemetryPacketStr(packetCount, currentState, 0, gpsLat, gpsLng);
            communicationModule.telemetryPacketQueue.add(telPacketString, TELEMETRY_PACKET_STRING_LENGTH);

            lastMilis = currMillis;
        }
      break;
  }
}

void switchToState(int8_t newState) {
  if (newState == NULL){ // TODO: Verify what is the actual value stored in ROM if not previously set
    currentState = STATE_STARTUP;
  } else {
    currentState = newState;
  }
  Serial.println(currentState);
  storageModule.saveToROM(CURRENT_STATE_EEPROM_ADDR, currentState);
  if (currentState == STATE_POST_APOGEE){
    Serial.println("Deploying parachute");
    electromechanicalModule.deployParachute();
  }
}
