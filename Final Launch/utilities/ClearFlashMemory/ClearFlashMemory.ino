#include "Logger.h"
#include <Preferences.h>

Preferences preferences;

void setup(){
  Serial.begin(9600);
  Logger::log("Connecting to preferences for topic lanzamiento00");
  preferences.begin("lanzamiento00", false);
  Logger::log("Clearing all data on topic lanzamiento00");
  // Uncomment the line below to erase flash memory on init
  preferences.clear();
}

void loop() {
  
}
