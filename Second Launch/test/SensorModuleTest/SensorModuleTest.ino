#include "SensorModule.h"
#include "Logger.h"


SensorModule sensorModule = SensorModule();

void setup(){
  Logger::log("Initializing sensor module");
  sensorModule.init();
  testGetAltitude();
  testGetAcceleration();
}


void testGetAltitude() {
  Logger::log("Testing getAltitude()...");
  float altitude = sensorModule.getAltitude();
  Logger::log("Altitude measured: " + String(altitude));
}

void testGetAcceleration() {
  Logger::log("Testing getAcceleration()...");
  float acceleration = sensorModule.getAcceleration();
  Logger::log("Acceleration measured: " + String(acceleration));
}
