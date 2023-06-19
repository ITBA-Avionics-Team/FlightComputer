#include "Logger.h"



void setup(){
  Serial.begin(9600);
  Logger::log("Connecting to preferences for topic lanzamiento00");
  preferences.begin("lanzamiento00", false);

  bool telemetryPackagesRemaining = true;

  while (telemetryPackagesRemaining) {
    String telemetryPackage = preferences.getString(CURRENT_STATE_KEY, 255); // 255 is the default value returned if no saved state is found
  }


}

void loop() {}

void testSaveCurrentState() {
  Logger::log("Testing saveCurrentState()...");
  uint8_t currentStateTestValue = 42;
  storageModule.saveCurrentState(currentStateTestValue);
  Logger::log("Saved state " + String(currentStateTestValue));
}

void testLoadCurrentState() {
  Logger::log("Testing loadCurrentState()...");
  uint8_t defaultCurrentState = 13;
  uint8_t loadedState = storageModule.loadCurrentState(defaultCurrentState);
  Logger::log("Loaded state " + String(loadedState));
}

void testAddTelemetryPacketToFlightLog() {
  Logger::log("Testing addTelemetryPacketToFlightLog()...");
  char* telemetryPacketTestValue = "10000,ST,9999,42.30402,34.30402";
  storageModule.addTelemetryPacketToFlightLog(telemetryPacketTestValue);
  Logger::debug("Saving telemetry packet to storage: " + String(telemetryPacketTestValue));
}

void testSaveTelemetryPacketCount() {
  Logger::log("Testing saveTelemetryPacketCount()...");
  uint8_t telemPacketCountTestValue = 1202;
  storageModule.saveTelemetryPacketCount(telemPacketCountTestValue);
  Logger::log("Saved telemetry package count " + String(telemPacketCountTestValue));
}

void testLoadTelemetryPacketCount() {
  Logger::log("Testing loadTelemetryPacketCount()...");
  uint8_t loadedTelPacketCount = storageModule.loadTelemetryPacketCount();
  Logger::log("Loaded packet count " + String(loadedTelPacketCount));
}

    
