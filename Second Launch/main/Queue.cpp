#include "Queue.h"

//Container tlemetry example 10000,ST,9999,42.30402,34.30402
void TelemetryPacketQueue::add(uint8_t nextPacket[], uint8_t nextPacketLength) {
  TelemetryPacketQueueNode *tmp = new TelemetryPacketQueueNode;
  tmp->data = nextPacket;
  tmp->dataLength = nextPacketLength;
  tmp->next = NULL;
  if(head == NULL)
  {
      head = tmp;
      tail = tmp;
  }
  else
  {
      tail->next = tmp;
      tail = tail->next;
  }
  length++;
}
void TelemetryPacketQueue::removeHead() {
  if (isEmpty()){
    return;
  }
  TelemetryPacketQueueNode* temp = head->next;
  delete head;
  head = temp;
  length--;
}
bool TelemetryPacketQueue::isEmpty(){
  return length == 0;
}