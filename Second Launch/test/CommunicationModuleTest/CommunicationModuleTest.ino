#include "CommunicationModule.h"
#include "StorageModule.h"
#include "MockStorageModule.h"
#include "Logger.h"


CommunicationModule communicationModule = CommunicationModule();
StorageModule storageModule = MockStorageModule();

void setup(){
  Logger::log("Initializing communication module");
  storageModule.init();
  communicationModule.init(storageModule);
}