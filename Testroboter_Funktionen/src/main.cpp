#include <Arduino.h> 
#include <NimBLEDevice.h>
#include <Stepper.h> 
#include <DFRobotDFPlayerMini.h> 
 

// MOTOR SETTINGS 
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

// SPEAKER SETTINGS 
HardwareSerial FPSerial(2); 
DFRobotDFPlayerMini myPlayer;

bool crying_flag = false; 
bool brabbeln_flag = false; 
bool coughing_flag = false; 
bool sneezing_flag = false; 

class CommandCallback : 
  public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* c, NimBLEConnInfo& coninfo) override {
      std::string cmd = c->getValue();
      //Serial.print("Received BLE command: ");
      //Serial.println(cmd.c_str());

    // MOTOR Callbacks
    if (cmd == "rotate1") {
      rotate1_flag = true; 
    }
    if (cmd == "rotate2") {
      rotate2_flag = true; 
    }
    // SPEAKER Callbacks 
    if (cmd == "crying"){
      crying_flag = true; 
    }
    if (cmd == "brabbeln") {
      brabbeln_flag = true; 
    }
    if (cmd == "coughing"){
      coughing_flag = true; 
    }
    if (cmd == "sneezing"){
      sneezing_flag = true; 
    }
  }
}; 




void setup() {
  Serial.begin(115200);
  // MOTOR
  myStepper1.setSpeed(10); 
  myStepper2.setSpeed(10); 
  // SPEAKER 
  FPSerial.begin(9600, SERIAL_8N1, /*RX=*/ 18, /*TX=*/ 19);
  if (!myPlayer.begin(FPSerial, true, true)) {
    Serial.println("DFPlayer nicht gefunden");
    while (true);
  }


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

  if (crying_flag){
    crying_flag = false; 
     myPlayer.volume(10);
     myPlayer.play(1);
  }
  if (brabbeln_flag){
    brabbeln_flag = false; 
    myPlayer.volume(10);
    myPlayer.play(2);
  }
  if (coughing_flag){
    coughing_flag = false; 
    myPlayer.volume(10);
    myPlayer.play(3);
  }
  if (sneezing_flag){
    sneezing_flag = false; 
    myPlayer.volume(10);
    myPlayer.play(4);
  }
}

