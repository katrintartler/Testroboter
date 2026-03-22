//#include <functions.hpp>
#include <SchedTask.h>
#include <SchedTaskT.h>
#include <ServoEasing.hpp>
#include <NimBLEDevice.h>
#include <DFRobotDFPlayerMini.h> 

//forward dec.
void Servo1TargetReachedHandler(ServoEasing *aServoEasingInstance);
void Servo2TargetReachedHandler(ServoEasing *aServoEasingInstance);
void Servo3TargetReachedHandler(ServoEasing *aServoEasingInstance);
void Servo4TargetReachedHandler(ServoEasing *aServoEasingInstance);
void ServoSM(); 
void BleCmd(); 
void HeatMonitor(); 
//void Speaker(); 


// pins regarding Heating 
#define HeatControl 5
#define HeatMeassure 34

//Statemachine LUTs parameters
#define SEQUENCES 4
#define SERVOS 4

//Servos themselfs setups 
ServoEasing Servo1;
ServoEasing Servo2;
ServoEasing Servo3; 
ServoEasing Servo4; 

//Speaker
HardwareSerial FPSerial(2); 
DFRobotDFPlayerMini myPlayer;

//for cmd executing 
volatile bool tw_flag = false; // Twitching mode (Arms and Legs, Servos 1-4)
volatile bool gm_flag = false; // Gross Movement (both Legs)
volatile bool lm_l_flag = false; // Localized Movement (left Leg)
volatile bool lm_r_flag = false; // Localized Movement (right leg)
//volatile so callback and Sched can run parallel 

volatile bool crying_flag = false; 
volatile bool brabbeln_flag = false; 
volatile bool coughing_flag = false; 
volatile bool sneezing_flag = false; 

volatile bool heat_on_flag = false; 
volatile bool heat_off_flag = false; 

// BLE part 
class CommandCallback : 
  public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* c, NimBLEConnInfo& coninfo) override {
      
      uint8_t cmd = c->getValue()[0];
      //Serial.print("Received BLE command: ");
      //Serial.println(cmd.c_str());

      switch(cmd)
      {
        case 1: 
          tw_flag = true; 
        break; 

        case 2: 
          gm_flag = true; 
        break; 

        case 3:
          lm_l_flag = true; 
        break; 

        case 4:
          lm_r_flag = true; 
        break; 

        case 5: 
          crying_flag = true; 
        break; 

        case 6: 
          brabbeln_flag = true; 
        break; 

        case 7: 
          coughing_flag = true; 
        break; 

        case 8: 
          sneezing_flag = true; 
        break; 

        case 9:
          heat_on_flag = true;
        break; 

        case 10: 
          heat_off_flag = true; 
      } 

  }
}; 


void setup() {
  Serial.begin(115200);

  // Pins regarding heating init
  pinMode(HeatControl, OUTPUT); 
  pinMode(HeatMeassure, INPUT); 

  
  FPSerial.begin(9600, SERIAL_8N1, /*RX=*/ 18, /*TX=*/ 19);
  if (!myPlayer.begin(FPSerial, true, true)) {
    Serial.println("DFPlayer nicht gefunden");
    while (true);
  }

  // BLE Setup 
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

  Servo1.attach(26,0); // left leg
  Servo2.attach(27,0); // right leg
  Servo3.attach(14,0); // left arm
  Servo4.attach(12,0); // right arm

 // assigning callback functions to Servo Callbacks 
  Servo1.setTargetPositionReachedHandler(Servo1TargetReachedHandler);
  Servo1.setSpeed(90);                        // This speed is taken if no further speed argument is given.
  //Servo1TargetReachedHandler(&Servo1); // Start by calling handler which in turn calls Servo1.startEaseTo(tTargetDegree)
  Servo2.setTargetPositionReachedHandler(Servo2TargetReachedHandler);
  Servo2.setSpeed(90);      

  Servo3.setTargetPositionReachedHandler(Servo3TargetReachedHandler);
  Servo3.setSpeed(90);      

  Servo3.setTargetPositionReachedHandler(Servo4TargetReachedHandler);
  Servo3.setSpeed(90);      
}

void loop() {

SchedBase::dispatcher(); 

}


//Schedueled Tasks
SchedTask Heatcontrol(0, 300, HeatMonitor); 
SchedTask BLECommands(0,50,BleCmd); 
SchedTask Servos(0,200,ServoSM); //beginning at 0 (startpoint), every 200ms, void ServoSM will be done 
//SchedTask Speaker(0,100,Speaker); bringt no instance Fehler??


void HeatMonitor(){
  // hier die Berechung der Pt100 Werte 
  // und das Abschalten falls es kritisch hoch werden sollte 
}

//Statemaschine
typedef enum { // welche zustände hat die Statemachine? 
  IDLE,
  TWITCHING, // "zappeln" Arms and Legs (4Servos) 
  GM,
  LM_L_LEFT,
  LM_L_RIGHT,
}t_states;

typedef enum {
  CMD_IDLE,
  CMD_TWITCHING,
  CMD_GM,
  CMD_LM_L_LEFT,
  CMD_LM_L_RIGHT,
}t_state_commands; 

typedef enum {
  ENTRY,
  DURING,
}t_state_executionstate; 

t_states states = IDLE; 
t_state_commands state_commands /*=CMD_TWITCHING*/; // jz nur mal zum Ausprobieren sonst is IDLE
t_state_executionstate state_executionstate; 

//BLE 
void BleCmd(){
  if (tw_flag){
    tw_flag = false; 
    state_commands = CMD_TWITCHING; 
  }
  if (gm_flag){
    gm_flag = false; 
    state_commands = CMD_GM; 
  }
  if (lm_l_flag){
    lm_l_flag = false; 
    state_commands = CMD_LM_L_LEFT; 
  }
  if (lm_r_flag){
    lm_r_flag = false; 
    state_commands = CMD_LM_L_RIGHT; 
  }
  if(crying_flag){
    // AudioTask auch in SM geben? 
    // Wenn eine audio ausgeführt wird kann man nicht motorsteuerungs befehle senden : optimierbar 
    myPlayer.volume(10);
    myPlayer.play(1);
    crying_flag = false; 
  }
  if(brabbeln_flag){
    brabbeln_flag = false; 
    myPlayer.volume(10);
    myPlayer.play(2);
  }
  if(coughing_flag){
    coughing_flag = false; 
    myPlayer.volume(10);
    myPlayer.play(3);
  }
  if(sneezing_flag){
    sneezing_flag = false; 
    myPlayer.volume(10);
    myPlayer.play(4);
  }
  if(heat_on_flag){
    heat_on_flag = false; 
    digitalWrite(HeatControl,HIGH); 
  }
  if (heat_off_flag){
    heat_off_flag = false; 
    digitalWrite(HeatControl,HIGH); 
  }
}

//Speaker
//void Speaker(){} derweil ist die Speakerfunktion im BLE Task 


//LUTs
int TWITCHING_lut[SEQUENCES][SERVOS] = {
 //S1 S2 S3 S4 
  {50,0,0,50},
  {0,50,50,0},
  {50,0,0,50},
  {0,50,50,0}
}; 
int GM_lut[SEQUENCES][SERVOS-2] = { // only Servo1 and Servo2 needed
  {50,0},
  {0,50},
  {50,0},
  {0,50}
}; 
int LM_L_LEFT_lut [SEQUENCES] = {50,0,50,0}; // only one (Servo1 Left Leg) needed
int LM_L_RIGHT_lut[SEQUENCES] = {50,0,50,0}; // Servo2 Right Leg 

int counter1 = 0; //Servo1 Callback
int counter2 = 0; //Servo2 Callback 
int counter3 = 0; //Servo3 Callback
int counter4 = 0; //Servo   4 Callback

void Servo1TargetReachedHandler(ServoEasing *aServoEasingInstance){
  switch(states){
      // was muss der Servo1 also Linkes Bein machen bei Twitching, wir gehen nach dem LUT
    // Servo 1 winkel sind in der ersten Spalte des LUT geschrieben 

    case TWITCHING:
      if (counter1 < SEQUENCES){
        Servo1.startEaseTo(TWITCHING_lut[counter1][0]);
        counter1++; 
        Serial.print("hallo");
        state_commands = CMD_IDLE; 
      }
      else {
        states = IDLE; 
        counter1 = 0; 
      }
    break; 

    case GM:
       if(counter1 < SEQUENCES){
        Servo1.startEaseTo(GM_lut[counter1][0]); 
        counter1++; 
        state_commands = CMD_IDLE; 
       }
       else{
        states = IDLE; 
        counter1 = 0; 
       }
    break; 

    case LM_L_LEFT: 
      if(counter1 < SEQUENCES){
        Servo1.startEaseTo(LM_L_LEFT_lut[counter1]); 
        counter1++; 
        state_commands = CMD_IDLE; 
      }
      else{
        states = IDLE; 
        counter1 = 0; 
      }
    break; 
    }
}
void Servo2TargetReachedHandler(ServoEasing *aServoEasingInstance){
  switch(states) {

    case TWITCHING:
      if(counter2 <= 3){
        Servo2.setEaseTo(TWITCHING_lut[counter2][1]); // second col for Servo2 deg
        counter2++; 
        state_commands = CMD_IDLE; 
      }
      else {
        states = IDLE; 
        counter2 = 0; 
      }
    break; 

    case GM: 
      if(counter2 < SEQUENCES){
        Servo2.startEaseTo(GM_lut[counter2][1]); 
        counter2++; 
        state_commands = CMD_IDLE; 
      }
      else{
        states = IDLE;
        counter2 = 0; 
      }
    break; 

    case LM_L_RIGHT:
      if(counter2 < SEQUENCES){
        Servo2.startEaseTo(LM_L_RIGHT_lut[counter2]); 
        counter2++; 
        state_commands = CMD_IDLE; 
      }
      else{
        states = IDLE; 
        counter2 = 0; 
      }
    break; 

  }
}
void Servo3TargetReachedHandler(ServoEasing *aServoEasingInstance){
  
  switch (states){

    case TWITCHING:
    if(counter3 < SEQUENCES){
      Servo3.startEaseTo(TWITCHING_lut[counter3][2]); 
      counter3++; 
      state_commands = CMD_IDLE; 
    } 
    else{
      states = IDLE; 
      counter3 = 0; 
    }
  break; 
  }

}
void Servo4TargetReachedHandler(ServoEasing *aServoEasingInstance){
  switch (states){
    
    case TWITCHING: 
    if(counter4 < SEQUENCES){
      Servo4.startEaseTo(TWITCHING_lut[counter4][3]); 
      counter4++; 
      state_commands = CMD_IDLE; 
    }
    else {
      states = IDLE; 
      counter4 = 0; 
    }
  break; 
  }

}

void ServoSM() {

  switch (states) {

    case IDLE:

    switch (state_commands) {

      case CMD_IDLE:
      break; 

      case CMD_TWITCHING:
      states = TWITCHING; 
      state_executionstate = ENTRY; 
      break; 
      
      case CMD_GM:
      states = GM;
      state_executionstate = ENTRY;
      break; 

      case CMD_LM_L_LEFT:
      states = LM_L_LEFT; 
      state_executionstate = ENTRY; 
      break; 

      case CMD_LM_L_RIGHT: 
      states = LM_L_RIGHT; 
      state_executionstate = ENTRY; 
      break; 

      default:
      break; 
    }
    break; 
    
    case TWITCHING:
    if(state_executionstate == ENTRY){
      Serial.print("in Tw");
      state_executionstate = DURING;
      Servo1TargetReachedHandler(&Servo1); 
      Servo2TargetReachedHandler(&Servo2); 
      Servo3TargetReachedHandler(&Servo3); 
      Servo4TargetReachedHandler(&Servo4); 
      // fehtl Servo3 und Servo4
    }
    break; 

    case GM: 
    if(state_executionstate == ENTRY){
      state_executionstate = DURING; 
      // Servo callbacks initi (also das erste man aufrufen dass sie dann von selber weiter callbacken)
      Servo1TargetReachedHandler(&Servo1); 
      Servo2TargetReachedHandler(&Servo2); 
    }
    break; 

    case LM_L_LEFT:
    if(state_executionstate == ENTRY){
      state_executionstate = DURING; 
      Servo1TargetReachedHandler(&Servo1); 
    }
    break; 

    case LM_L_RIGHT: 
    if (state_executionstate == ENTRY){
      state_executionstate = DURING; 
      Servo2TargetReachedHandler(&Servo2); 
    }
    break; 

    default:
    break; 
  }
}