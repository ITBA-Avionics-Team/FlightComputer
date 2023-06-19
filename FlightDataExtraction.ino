#include "Logger.h"
#include <stdlib.h>

char packetCountString[6];

void setup(){
  Serial.begin(9600);
  Logger::log("Connecting to preferences for topic lanzamiento00");
  preferences.begin("lanzamiento00", false);

  bool telemetryPacketsRemaining = true;
  int numOfEmptyPackets = 0;

  int packetCount = 0;
  while (telemetryPacketsRemaining) {
    sprintf(packetCountString, "%05d", packetCount);
    String telemetryPacket = preferences.getString(packetCountString, "NO-DATA");
    Logger::log(telemetryPacket);
    packetCount++;
    if (telemetryPacket == "NO-DATA") {
        numOfEmptyPackets++;
        if (numOfEmptyPackets > 2) telemetryPacketsRemaining = false;
    }
  }
}
