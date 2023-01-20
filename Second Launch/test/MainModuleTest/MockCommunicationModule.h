#include <stdint.h>

class CommunicationModule {
  StorageModule storage;
  public:

    uint16_t packetCount = 0;
    // TelemetryPacketQueue telemetryPacketQueue = TelemetryPacketQueue();

    void init(StorageModule storageModule){
      // Init connection to radio
      Logger::log("Communication Module initialized.");
      storage = storageModule;
      packetCount = storage.loadTelemetryPacketCount();
    }

    void sendTelemetryPacketToGround(char* telPacketString, uint8_t telPacketLength){
      Logger::debug("Sending telemetry packet to ground: " + String(telPacketString));
      packetCount++;
      storage.saveTelemetryPacketCount(packetCount);
      // groundRequestObj = ZBTxRequest(groundAddress, telemetryPacketQueue.head->data, telemetryPacketQueue.head->dataLength);
      // groundXBee.send(groundRequestObj);
      // packetCount++;
    }

};
