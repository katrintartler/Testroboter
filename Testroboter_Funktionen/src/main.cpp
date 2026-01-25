#include <Arduino.h> 
#include <NimBLEDevice.h>
#include <DFRobotDFPlayerMini.h> 
#include <ESP32Servo.h> 
 
// SERVO SETTINGS 
Servo myServo1; 
Servo myServo2; 
#define SERVO1_PIN 26; // funktioniert stand 2501 nur mit magic numbers NOCHMAL ANSCHAUEN !!!
#define SERVO2_PIN 27; 


bool rotate_servo1_flag = false; 
bool rotate_servo2_flag = false; 
int pos = 0; 
int direction = -1; 
unsigned long lastMoveTime = 0;
const int moveInterval = 20; // 20ms movetime 


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

    // SERVO Callbacks
    if (cmd == "rotate_servo1") {
      rotate_servo1_flag == true; 
    }
     if (cmd == "rotate_servo2") {
      rotate_servo2_flag == true; 
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
 
  // SPEAKER 
  FPSerial.begin(9600, SERIAL_8N1, /*RX=*/ 18, /*TX=*/ 19);
  if (!myPlayer.begin(FPSerial, true, true)) {
    Serial.println("DFPlayer nicht gefunden");
    while (true);
  }

  // SERVO
  myServo1.attach(26); 
  myServo2.attach(27); 
  myServo1.write(0); 
  myServo2.write(0);


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
 
  // SERVO 
  if (rotate_servo1_flag) {
      rotate_servo1_flag = false; 
      if (millis() - lastMoveTime >= moveInterval) {
      lastMoveTime = millis();

      pos += direction;

      if (pos >= 90) {
        pos = 90;
        direction = -1;
      }
      if (pos <= 0) {
        pos = 0;
        direction = 1;
      }

      myServo1.write(pos);
    }
  }

  if (rotate_servo2_flag) {
      rotate_servo2_flag = false; 
      if (millis() - lastMoveTime >= moveInterval) {
      lastMoveTime = millis();

      pos += direction;

      if (pos >= 90) {
        pos = 90;
        direction = -1;
      }
      if (pos <= 0) {
        pos = 0;
        direction = 1;
      }

      myServo2.write(pos);
    }
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

