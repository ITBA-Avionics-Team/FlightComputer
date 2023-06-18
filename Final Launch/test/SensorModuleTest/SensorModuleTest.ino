#include "Logger.h"
#include "SensorModule.h"


SensorModule sensorModule = SensorModule();

void setup(){
  Logger::log("Initializing sensor module");
  sensorModule.init();
}

void loop() {
  testGetAltitude();
  delay(1000);
  testGetRawAltitude();
  delay(1000);
  testGetAcceleration();
  delay(1000);
}


void testGetAltitude() {
  Logger::log("Testing getAltitude()...");
  float altitude = sensorModule.getAltitude();
  Logger::log("Altitude measured: " + String(altitude));
}

void testGetRawAltitude() {
  Logger::log("Testing getRawAltitude()...");
  float altitude = sensorModule.getRawAltitude();
  Logger::log("Raw altitude measured: " + String(altitude));
}

void testGetAcceleration() {
  Logger::log("Testing getAcceleration()...");
  float acceleration = sensorModule.getAcceleration();
  Logger::log("Acceleration measured: " + String(acceleration));
}
