#include "Logger.h"
#include "ElectromechanicalModule.h"


ElectromechanicalModule electromechanicalModule = ElectromechanicalModule();

void setup(){
  Logger::log("Initializing electromechanical module");
  electromechanicalModule.init();
}

void loop() {
  testDeployDrogueParachute();
  delay(1000);
  testDeployMainParachute();
  delay(1000);
}

void testDeployDrogueParachute() {
  Logger::log("Testing deployDroguePrachute()...");
  electromechanicalModule.deployDrogueParachute();
}

void testDeployMainParachute() {
  Logger::log("Testing deployMainPrachute()...");
  electromechanicalModule.deployMainParachute();
}