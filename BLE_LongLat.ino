#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h> //Library to use BLE as server
#include <BLE2902.h> 

#define IndoorService_UUID          "00001821-0000-1000-8000-00805F9B34FB"
#define LatitudeCharacteristic_UUID "00002AAE-0000-1000-8000-00805F9B34FB"
#define LatitudeDescriptor_UUID     "00002901-0000-1000-8000-00805F9B34FB"
#define LongitudeCharacteristic_UUID "00002AAF-0000-1000-8000-00805F9B34FB"
#define LongitudeDescriptor_UUID     "00002901-0000-1000-8000-00805F9B34FB"


BLECharacteristic IndoorLatitudeCharacteristic(BLEUUID(LatitudeCharacteristic_UUID), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_BROADCAST);
BLEDescriptor IndoorLatitudeDescriptor(BLEUUID(LatitudeDescriptor_UUID));
BLECharacteristic IndoorLongitudeCharacteristic(BLEUUID(LongitudeCharacteristic_UUID), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_BROADCAST);
BLEDescriptor IndoorLongitudeDescriptor(BLEUUID(LongitudeDescriptor_UUID));

bool _BLEClientConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      _BLEClientConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      _BLEClientConnected = false;
    }
};

void InitBLE() {
  BLEDevice::init("BLE LongLat");
  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pIndoor = pServer->createService(IndoorService_UUID);
  
  pIndoor->addCharacteristic(&IndoorLatitudeCharacteristic);
  IndoorLatitudeDescriptor.setValue("Latitude Range (-90) - 90");
  IndoorLatitudeCharacteristic.addDescriptor(&IndoorLatitudeDescriptor);
  IndoorLatitudeCharacteristic.addDescriptor(new BLE2902());
  
  pIndoor->addCharacteristic(&IndoorLongitudeCharacteristic);
  IndoorLongitudeDescriptor.setValue("Longitude Range -180 to 80");
  IndoorLongitudeCharacteristic.addDescriptor(&IndoorLongitudeDescriptor);
  IndoorLongitudeCharacteristic.addDescriptor(new BLE2902());

  pServer->getAdvertising()->addServiceUUID(IndoorService_UUID);

  pIndoor->start();
  // Start advertising
  pServer->getAdvertising()->start();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Latitude Indicator - BLE");
  InitBLE();
}
  
  float latitude = -6.175867;
  float longitude = - 176.523651;
void loop() {
  if (_BLEClientConnected) {
    IndoorLatitudeCharacteristic.setValue(latitude);
    IndoorLatitudeCharacteristic.notify();
    IndoorLongitudeCharacteristic.setValue(longitude);
    IndoorLongitudeCharacteristic.notify();
    delay(5000);
    
    Serial.println(latitude, 6);
    Serial.println(longitude, 6);
    latitude -=0.002005;
    longitude +=0.002005;
  }
}
