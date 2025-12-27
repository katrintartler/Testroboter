#include <Arduino.h>
#include <NimBLEDevice.h>
#include <Stepper.h> 
 
const int stepsPerRevolution = 1024; 
const int IN1_1 = 14; 
const int IN1_2 = 27; 
const int IN1_3 = 26; 
const int IN1_4 = 25; 

const int IN2_1 = 16; 
const int IN2_2 = 17; 
const int IN2_3 = 33; 
const int IN2_4 = 32; 

Stepper myStepper1(stepsPerRevolution, IN1_1, IN1_2, IN1_3, IN1_4); 
Stepper myStepper2(stepsPerRevolution, IN2_1, IN2_2, IN2_3, IN2_4); 

bool rotate1_flag = false; 
bool rotate2_flag = false; 

class CommandCallback : 
  public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* c, NimBLEConnInfo& coninfo) override {
      std::string cmd = c->getValue();
      //Serial.print("Received BLE command: ");
      //Serial.println(cmd.c_str());

    if (cmd == "rotate1") {
      rotate1_flag = true; 
    }
    if (cmd == "rotate2") {
      rotate2_flag = true; 
    }
  }
}; 

void setup() {
  Serial.begin(115200);
  myStepper1.setSpeed(10); 
  myStepper2.setSpeed(10); 

  // BLE init 
  NimBLEDevice::init("Bby"); 
  Serial.print("Name vergeben"); 
  
  NimBLEAddress addr = NimBLEDevice::getAddress();
  Serial.print("BLE MAC:");
  Serial.print(addr.toString().c_str()); 

  NimBLEServer * server = NimBLEDevice::createServer();
  NimBLEService * service = server->createService("12345678-1234-1234-1234-1234567890ab"); 

   NimBLECharacteristic* characteristic =
      service->createCharacteristic(
          "abcd1234-ab12-34cd-56ef-1234567890ab",
          NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
      );

  characteristic->setCallbacks(new CommandCallback());
  characteristic->setValue("ready");

  service->start();

  NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
  adv->addServiceUUID(service->getUUID());
  adv->start();

  Serial.println("advertising");
}
void loop() {
 
  if (rotate1_flag) {
        rotate1_flag = false;
        myStepper1.step(stepsPerRevolution);
    }

  if (rotate2_flag) {
    rotate2_flag = false; 
    myStepper2.step(stepsPerRevolution); 
  }
}

