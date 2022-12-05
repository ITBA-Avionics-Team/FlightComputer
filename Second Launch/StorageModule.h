#include <stdlib.h>
#include <stdint.h>

#include <Preferences.h>

#define CURRENT_STATE_KEY "curr_state"

class StorageModule {
  Preferences preferences;
  public:

    void StorageModule::init(uint8_t* currentState, uint8_t defaultState){
      preferences.begin("lanzamiento00", false);
      uint8_t previouslySavedState = preferences.getUChar(CURRENT_STATE_KEY, 255); // 255 is the default value returned if no saved state is found
      if (previouslySavedState == 255) {
        *currentState = defaultState;
      } else {
        *currentState = previouslySavedState;
      }
      Serial.println("Storage Module initialized.");

    }

    void addTelemetryPacketToFlightLog(uint8_t8* telemetryPacket) {
      char packetCountBuff[6];
      memcpy( packetCountBuff, telemetryPacket, 5);
      subbuff[5] = '\0';
      preferences.putString(packetCountBuff, telemetryPacket);
    }

    void saveCurrentState(uint8_t currentState) {
      preferences.putString(CURRENT_STATE_KEY, currentState);
    }


};
