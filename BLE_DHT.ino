#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h> 
#include <DHT.h>

#define EnvironmentalService_UUID "0000181A-0000-1000-8000-00805F9B34FB"
#define tempCharacteristic_UUID "00002A6E-0000-1000-8000-00805F9B34FB"
#define tempDescriptor_UUID     "00002901-0000-1000-8000-00805F9B34FB"
#define humidCharacteristic_UUID "00002A6F-0000-1000-8000-00805F9B34FB"
#define humidDescriptor_UUID     "00002901-0000-1000-8000-00805F9B34FB"

BLECharacteristic EnvironmentalTempCharacteristic(BLEUUID(tempCharacteristic_UUID), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
BLEDescriptor EnvironmentalTempDescriptor(BLEUUID(tempDescriptor_UUID));
BLECharacteristic EnvironmentalHumidCharacteristic(BLEUUID(humidCharacteristic_UUID), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
BLEDescriptor EnvironmentalHumidDescriptor(BLEUUID(humidDescriptor_UUID));

bool _BLEClientConnected = false;

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
float kelembaban;
float suhu;

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      _BLEClientConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      _BLEClientConnected = false;
    }
};

void InitBLE() {
  BLEDevice::init("BLE Temperature & Humidity");
  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pEnvironmental = pServer->createService(EnvironmentalService_UUID);
  
  pEnvironmental->addCharacteristic(&EnvironmentalTempCharacteristic);
  EnvironmentalTempDescriptor.setValue("Temperature Range (0 - 100)");
  EnvironmentalTempCharacteristic.addDescriptor(&EnvironmentalTempDescriptor);
  EnvironmentalTempCharacteristic.addDescriptor(new BLE2902());

  pEnvironmental->addCharacteristic(&EnvironmentalHumidCharacteristic);
  EnvironmentalHumidDescriptor.setValue("Temperature Range (0 - 100)");
  EnvironmentalHumidCharacteristic.addDescriptor(&EnvironmentalHumidDescriptor);
  EnvironmentalHumidCharacteristic.addDescriptor(new BLE2902());

  pServer->getAdvertising()->addServiceUUID(EnvironmentalService_UUID);
  pEnvironmental->start();
  // Start advertising
  pServer->getAdvertising()->start();
}

void setup () {
    Serial.begin(115200); 
    InitBLE();
    dht.begin();
}

void loop () {
  delay(10000);
   if (_BLEClientConnected) {
    suhu = dht.readTemperature();
    kelembaban = dht.readHumidity();
    char suhuconv [8];
    char humidconv [8];
    std::string suhuvalue = dtostrf(suhu, 3, 2, suhuconv);
    std::string kelembabanvalue = dtostrf(kelembaban, 3, 2, humidconv);
    std::string space = " ";
    EnvironmentalTempCharacteristic.setValue(suhu);
    EnvironmentalTempCharacteristic.notify(); //Send data to client
    EnvironmentalHumidCharacteristic.setValue(suhu);
    EnvironmentalHumidCharacteristic.notify();
    if (isnan(suhu) || isnan(kelembaban)) {// isnan, digunakan untuk melakukan cek value pada variable.
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
    }
  Serial.print(F("Temperature: "));
  Serial.print(suhu);
  Serial.print(F("°C || Humidity: "));
  Serial.print(kelembaban);
  Serial.print(F("%"));
  Serial.println();
  }
}
