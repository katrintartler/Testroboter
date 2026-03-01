#include <NimBLEDevice.h>
#include <DFRobotDFPlayerMini.h> 
#include <ESP32Servo.h> 


// SERVO SETTINGS 
Servo myServo1; 
Servo myServo2; 
#define SERVO1_PIN 26; // funktioniert stand 2501 nur mit magic numbers NOCHMAL ANSCHAUEN !!!
#define SERVO2_PIN 27; 
#define SERVO3_PIN 14; 
#define SERVO4_PIN 12; 

bool rotate_servo1_flag = false; 
bool rotate_servo2_flag = false; 
bool rotate_servo3_flag = false; 
bool rotate_servo4_flag = false; 

int pos = 0; 
int direction = -1; 
unsigned long lastMovetime = 0;
const int moveInterval = 20; // 20ms movetime 


// SPEAKER SETTINGS 
HardwareSerial FPSerial(2); 
DFRobotDFPlayerMini myPlayer;

bool crying_flag = false; 
bool brabbeln_flag = false; 
bool coughing_flag = false; 
bool sneezing_flag = false; 

// HEATER SETTINGS 
bool heat_flag = false; 