//#include <functions.hpp>
#include <SchedTask.h>
#include <SchedTaskT.h>
#include <ServoEasing.hpp>

//forward dec.
void Servo1TargetReachedHandler(ServoEasing *aServoEasingInstance);
void Servo2TargetReachedHandler(ServoEasing *aServoEasingInstance);
void ServoSM(); 

#define SEQUENCES 4
#define SERVOS 4

ServoEasing Servo1;
ServoEasing Servo2;
ServoEasing Servo3; 
ServoEasing Servo4; 

void setup() {
  Serial.begin(115200);
  Servo1.attach(26,0); 
  Servo2.attach(25,0); 
 
  Servo1.setTargetPositionReachedHandler(Servo1TargetReachedHandler);
  Servo1.setSpeed(90);                        // This speed is taken if no further speed argument is given.
  //Servo1TargetReachedHandler(&Servo1); // Start by calling handler which in turn calls Servo1.startEaseTo(tTargetDegree)
  Servo2.setTargetPositionReachedHandler(Servo2TargetReachedHandler);
  Servo2.setSpeed(90);      
}

void loop() {

SchedBase::dispatcher(); 

}


//Statemachine with Scheduled Tasks für das Ausführen von Servo Befehlen

//Schedueled Task 
SchedTask Servos(0,200,ServoSM); // beginning at 0 (startpoint), every 200ms, void ServoSM will be done 

//Statemaschine
typedef enum { // welche zustände hat die Statemachine? 
  IDLE,
  TWITCHING, // "zappeln" Arms and Legs (4Servos)
}t_states;

typedef enum {
  CMD_IDLE,
  CMD_TWITCHING,
}t_state_commands; 

typedef enum {
  ENTRY,
  DURING,
}T_state_executionstate; 

t_states states = IDLE; 
t_state_commands state_commands = CMD_TWITCHING; // jz nur mal zum Ausprobieren sonst is IDLE
T_state_executionstate state_executionstate; 



//LUTs
int Twitching_lut[SEQUENCES][SERVOS] = {
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0}
}; 

int counter1 = 0;  
int counter2 = 0; 

void Servo1TargetReachedHandler(ServoEasing *aServoEasingInstance){
  switch(states){
    case TWITCHING:
    // was muss der Servo1 also Linkes Bein machen bei Twitching, wir gehen nach dem LUT
    // Servo 1 winkel sind in der ersten Spalte des LUT geschrieben 
      if (counter1 <=3){
        Servo1.startEaseTo(Twitching_lut[counter1][0]);
        counter1++; 
        Serial.print("hallo");
        state_commands = CMD_IDLE;
      }
      else {
        states = IDLE; 
      }
      break; 
    }
}
void Servo2TargetReachedHandler(ServoEasing *aServoEasingInstance){
  switch(states) {
  case TWITCHING:
    if(counter2 <= 3){
      Servo2.setEaseTo(Twitching_lut[counter2][1]); 
      counter2++; 
      state_commands = CMD_IDLE; 
  }
  else {
    states = IDLE; 
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
    }
    break; 

    default:
    break; 
  }
}