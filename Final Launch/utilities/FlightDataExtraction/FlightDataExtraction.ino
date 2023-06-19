#include "Logger.h"
#include <stdlib.h>
#include <Preferences.h>


char packetCountString[6];

Preferences preferences;

void setup(){
  Serial.begin(9600);
  Logger::log("Connecting to preferences for topic lanzamiento00");
  preferences.begin("lanzamiento00", false);

  bool telemetryPacketsRemaining = true;
  int numOfEmptyPackets = 0;

  // int packetCount = 0;

  for (int packetCount = 0; packetCount < 1000; packetCount++) {
    sprintf(packetCountString, "%05d", packetCount);
    // Logger::log(packetCountString);
    String telemetryPacket = preferences.getString(packetCountString, "NO-DATA");
    Logger::log(telemetryPacket);
  }
  // while (telemetryPacketsRemaining) {
  //   sprintf(packetCountString, "%05d", packetCount);
  //   String telemetryPacket = preferences.getString(packetCountString, "NO-DATA");
  //   Logger::log(telemetryPacket);
  //   packetCount++;
  //   if (telemetryPacket == "NO-DATA") {
  //       numOfEmptyPackets++;
  //       if (numOfEmptyPackets > 2) telemetryPacketsRemaining = false;
  //   }
  // }
}

void loop() {
  //
}
