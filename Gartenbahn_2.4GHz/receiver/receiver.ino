#include <nRF24L01.h>
#include <RF24.h>

bool debug = false; 

#define PIN_CE 7
#define PIN_CSN 8

//asdasd

#define PIN_FORWARD 2
#define PIN_BACKWARD 4
#define PIN_PWM 3

int pwm = 0; 
bool direction; 
bool lock = true; 

RF24 radio(PIN_CE, PIN_CSN); // CE, CSN

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

void setup() {
  Serial.begin(9600);
  Serial.println("Setup - Start");

  //Initialize Radio Communication
  bool init_status = radio.begin();
  Serial.print("Radio Initialization: ");
  Serial.println(init_status ? "Success" : "Failed");

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();

  pinMode(PIN_FORWARD, OUTPUT);
  pinMode(PIN_BACKWARD, OUTPUT);
  pinMode(PIN_PWM, OUTPUT);

  Serial.println("Setup - End");
}

void loop() {


  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package));
  }
  

  //LY
  data.ly = constrain(data.ly, 40, 980);
  pwm = map(data.ly, 40, 980, 0, 255); 
  if (pwm == 0) {
    lock = false;
  } else {
    lock = true; 
  }
  analogWrite(PIN_PWM, pwm);

  //RY
  if (!lock) {
    if (data.ry > 800) {
      direction = true; 
      digitalWrite(PIN_BACKWARD, !direction);
      digitalWrite(PIN_FORWARD, direction);
    } else if (data.ry < 100) {
      direction = false; 
      digitalWrite(PIN_FORWARD, direction);
      digitalWrite(PIN_BACKWARD, !direction);
    }
  }



  
 

  Serial.print(data.ly);
  Serial.print("    ");
  Serial.print(pwm);
  Serial.print("    ");
  Serial.print(lock);
  Serial.print("    I    ");
  Serial.print(data.ry);
  Serial.println(direction);

  if (debug == true) {
    Serial.print(" lx:");
    Serial.print(data.lx);
    Serial.print(" ly:");
    Serial.print(data.ly);
    Serial.print(" lz:");
    Serial.print(data.lz);
    Serial.print(" rx:");
    Serial.print(data.rx);
    Serial.print(" ry:");
    Serial.print(data.ry);
    Serial.print(" rz:");
    Serial.print(data.rz);
    Serial.println();
  }

  // digitalWrite(PIN_BACKWARD, 1);
  // delay(500);
  // digitalWrite(PIN_BACKWARD, 0);
  // delay(500);
  // digitalWrite(PIN_FORWARD, 1);
  // delay(500);
  // digitalWrite(PIN_FORWARD, 0);
  // delay(500);

  // for(int i = 0; i <= 255; i++) {
  //   
  //   Serial.println(i);
  //   delay(10);
  // }

  //   for(int i = 255; i >= 0; i--) {
  //   analogWrite(PIN_PWM, i);
  //   Serial.println(i);
  //   delay(10);
  // }
}
