#include "Logger.h"
#include <stdlib.h>
#include <Preferences.h>


char packetCountString[6];

Preferences preferences;

void setup(){
  Serial.begin(9600);
  Logger::log("Connecting to preferences for topic lanzamiento00");
  preferences.begin("lanzamiento00", false);

  for (int i = 0; i < 16; i++) {
    char blobIdStr[4];
    sprintf(blobIdStr, "%03d", i);
    // Logger::log(packetCountString);
    String telemetryPacketBlob = preferences.getString(blobIdStr, "NO-DATA");
    Logger::log(telemetryPacketBlob);
  }
}

void loop() {
  //
}
