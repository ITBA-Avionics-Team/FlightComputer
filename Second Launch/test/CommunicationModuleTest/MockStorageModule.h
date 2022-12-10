#include <stdlib.h>
#include <stdint.h>
#include "Logger.h"
#include "StorageModule.h"

class MockStorageModule : StorageModule {
  public:

    void init(){
      Logger::log("Mock Storage Module initialized.");
    }

    uint8_t loadCurrentState(uint8_t defaultState) {
      Logger::log("[MockStorageModule] loadCurrentState() called.");
    }

    void saveCurrentState(uint8_t currentState) {
      Logger::log("[MockStorageModule] saveCurrentState() called.");
    }

    void addTelemetryPacketToFlightLog(char* telemetryPacket) {
      Logger::log("[MockStorageModule] addTelemetryPacketToFlightLog() called.");
    }

    uint16_t loadTelemetryPacketCount() {
      Logger::log("[MockStorageModule] loadTelemetryPacketCount() called.");
    }

    void saveTelemetryPacketCount(uint16_t count) {
      Logger::log("[MockStorageModule] saveTelemetryPacketCount() called.");
    }


};
