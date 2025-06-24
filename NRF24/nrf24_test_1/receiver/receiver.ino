#include <AFMotor.h>

#include <Servo.h>
#include <nRF24L01.h>
#include <RF24.h>


//Stick LX    Servomotor
#define PIN_LX 10  //Rotation 
#define DEFAULT_ANGLE 45
#define MIN_ANGLE 10
#define MAX_ANGLE 75



Servo servo; 
int pos = DEFAULT_ANGLE; 

//Stick LY
//#define PIN_LY 6
#define PIN_LIGHT_F 44
#define PIN_LIGHT_B 45  
bool lichtstatus = false; 
bool lichtstatuslock = false; 

//Schalter LZ 
AF_DCMotor motor(4);
#define PIN_HORN 30 

//Stick RX    Blinker
//Schalter RZ Warnblinker
#define PIN_BLINKER_FL 38
#define PIN_BLINKER_FR 39
#define PIN_BLINKER_RL 40
#define PIN_BLINKER_RR 41

#define BLINKINTERVAL 400 
unsigned long currentblink = 0; 
unsigned long previuosblink = 0; 
bool blinkerstatus = false;
String blinkerseite = ""; 

//Stick RY    Fahrmotor
#define PIN_RY 11            //PWM Signal   Moto-Shield Pin 5 
#define PIN_MOTOR_FORWARD 22  //Vorwärts    Moto-Shield Pin 7
#define PIN_MOTOR_BACKWARD 23 //Rückwärts   Moto-Shield Pin 8
//                                5V Pin an Moto-Shield Pin A0
//                                Motor Plus an A1
//                                Motor Minus an B1 
int PWM_RY = 0; 


//Radio - Pins
#define PIN_CE 48 
#define PIN_CSN 49
RF24 radio(PIN_CE, PIN_CSN); // CE, CSN

//Radio Adresse 
const byte address[6] = "00100";

//Datenpaket 
struct Data_Package {
  int lx = 0;
  int ly = 0; 
  bool lz = 0; 

  int rx = 0; 
  int ry = 0; 
  bool rz = 0; 
};
Data_Package data;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP 
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() {
  //Serieller Monitor
  Serial.begin(9600);
  Serial.println();
  Serial.println("Setup - Start");

  //Radio Starten
  bool init_status = radio.begin();
  Serial.print("Radio Initialization: ");
  Serial.println(init_status ? "Success" : "Failed");

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();


  pinMode(PIN_LX, OUTPUT);

  pinMode(PIN_RY, OUTPUT);
  pinMode(PIN_MOTOR_FORWARD, OUTPUT);
  pinMode(PIN_MOTOR_BACKWARD, OUTPUT); 

  pinMode(PIN_LIGHT_F, OUTPUT);
  pinMode(PIN_LIGHT_B, OUTPUT);

  pinMode(PIN_BLINKER_FL, OUTPUT);
  pinMode(PIN_BLINKER_FR, OUTPUT);
  pinMode(PIN_BLINKER_RL, OUTPUT);
  pinMode(PIN_BLINKER_RR, OUTPUT);

  //Servo Pin festlegen 
  servo.attach(PIN_LX);

  //Servo Standardposition festlegen 
  servo.write(pos);

  Serial.println("Setup - End");
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP 
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {

  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package));
  } else {
    data.lx = 500; 
    data.ly = 500; 
    data.lz = 0; 
    data.rx = 500; 
    data.ry = 500; 
    data.rz = 0; 
  }

  //LX      Servo
  pos = map(data.lx, 0, 1024, MIN_ANGLE, MAX_ANGLE);
  servo.write(pos);
  Serial.println(pos);

  //LY      Licht 
  if (data.ly > 1000 && !lichtstatuslock) {
    lichtstatuslock = true; 
    lichtstatus = !lichtstatus;
  } else if (data.ly < 80) {
    lichtstatuslock = false; 
  }
  digitalWrite(PIN_LIGHT_F, lichtstatus);
  digitalWrite(PIN_LIGHT_B, lichtstatus);



  //LZ      Hupe 
  if (data.lz == 1) {
    motor.run(FORWARD);
    motor.setSpeed(255);
  } else {
    motor.run(RELEASE);
    motor.setSpeed(0);
  } 
  
  //RX, RZ  Blinker, Warnblinker
  if (data.rz == 1) {
    blinkerseite = "w"; 
  } else if (data.rx > 800) {
    blinkerseite = "l"; 
  } else if (data.rx < 200) {
    blinkerseite = "r";
  } else {
    blinkerseite = ""; 
  }

  currentblink = millis();
  //Timer-Funktion Blinker / Warnblinker
  if (currentblink - previuosblink >= BLINKINTERVAL) {
    previuosblink = currentblink; 
    blinkerstatus = !blinkerstatus; 
    
    if (blinkerseite == "l") {
      digitalWrite(PIN_BLINKER_FL, blinkerstatus);
      digitalWrite(PIN_BLINKER_FR, 0);
      digitalWrite(PIN_BLINKER_RL, blinkerstatus); 
      digitalWrite(PIN_BLINKER_RR, 0); 
    } else if (blinkerseite == "r") {
      digitalWrite(PIN_BLINKER_FL, 0);
      digitalWrite(PIN_BLINKER_FR, blinkerstatus);
      digitalWrite(PIN_BLINKER_RL, 0); 
      digitalWrite(PIN_BLINKER_RR, blinkerstatus); 
    }  else if (blinkerseite == "w") {
      digitalWrite(PIN_BLINKER_FL, blinkerstatus);
      digitalWrite(PIN_BLINKER_FR, blinkerstatus);
      digitalWrite(PIN_BLINKER_RL, blinkerstatus); 
      digitalWrite(PIN_BLINKER_RR, blinkerstatus);
    } else {
      digitalWrite(PIN_BLINKER_FL, 0);
      digitalWrite(PIN_BLINKER_FR, 0);
      digitalWrite(PIN_BLINKER_RL, 0); 
      digitalWrite(PIN_BLINKER_RR, 0); 
    }
  }

  //RY      Fahrmotor
  data.ry -= 500; 
  if (data.ry > 0) {
    digitalWrite(PIN_MOTOR_FORWARD, 1);
    digitalWrite(PIN_MOTOR_BACKWARD, 0);  
  } else {
    data.ry = data.ry * -1; 
    digitalWrite(PIN_MOTOR_FORWARD, 0);
    digitalWrite(PIN_MOTOR_BACKWARD, 1);
  }  
  
  PWM_RY = map(data.ry, 0 , 500, 0, 255);

  if (PWM_RY < 20) {
    PWM_RY = 0; 
  }
  analogWrite(PIN_RY, PWM_RY);
  //RZ siehe RX  
}


