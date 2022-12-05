#include "CommunicationModule.h"

void CommunicationModule::init() {
  // Init connection to radio
}


void CommunicationModule::sendTelemetryPacketToGround(uint8_t* telPacketString, uint8_t telPacketLength){
  Serial.write(telPacketString, telPacketLength);
  Serial.write('\n');
  // groundRequestObj = ZBTxRequest(groundAddress, telemetryPacketQueue.head->data, telemetryPacketQueue.head->dataLength);
  // groundXBee.send(groundRequestObj);
  // packetCount++;
}


