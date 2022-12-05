#include <stdlib.h>
#include <stdint.h>

class CommunicationModule {
  public:

    uint16_t packetCount = 0;
    // TelemetryPacketQueue telemetryPacketQueue = TelemetryPacketQueue();

    void CommunicationModule::init(){
      // Init connection to radio
      Serial.println("Communication Module initialized.");
    }

    void sendTelemetryPacketToGround(uint8_t* telPacketString, uint8_t telPacketLength){
      Serial.write(telPacketString, telPacketLength);
      Serial.write('\n');
      packetCount++;
      // groundRequestObj = ZBTxRequest(groundAddress, telemetryPacketQueue.head->data, telemetryPacketQueue.head->dataLength);
      // groundXBee.send(groundRequestObj);
      // packetCount++;
    }

};
