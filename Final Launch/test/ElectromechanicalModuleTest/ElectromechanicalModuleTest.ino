#include "ElectromechanicalModule.h"
#include "Logger.h"


ElectromechanicalModule electromechanicalModule = ElectromechanicalModule();

void setup(){
  Logger::log("Initializing electromechanical module");
  electromechanicalModule.init();
}