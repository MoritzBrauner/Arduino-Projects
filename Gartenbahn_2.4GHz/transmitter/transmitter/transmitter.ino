#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Radio - Pins NRF24 
RF24 radio(7, 8); // CE, CSN

//Adresse 
const byte address[6] = "00100";

//Datenpaket zum Senden 
struct Data_Package {
  int lx = 0;
  int ly = 0; 
  bool lz = 0; 

  int rx = 0; 
  int ry = 0; 
  bool rz = 0; 
};

//Datenpacket als "Data" festlegen
Data_Package data; 

//---------------------------------------------------------------
//SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP
//----------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("Setup - Start");

  //Radio Starten 
  bool init_status = radio.begin();
  Serial.print("Radio Initialization: ");
  Serial.println(init_status ? "Success" : "Failed");

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();

  Serial.println("Setup - End");
}

//--------------------------------------------------------------
//LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP
//--------------------------------------------------------------------- 
void loop() {
  //Pins Auslesen 
  data.lx = analogRead(0);
  data.ly = analogRead(1);
  data.lz = digitalRead(A2);
  data.rx = analogRead(3);
  data.ry = analogRead(4);
  data.rz = digitalRead(A5);

  /*Serial.print(" lx: ");
  Serial.print(data.lx);
  Serial.print(" ly: ");
  Serial.print(data.ly);
  Serial.print(" lz: ");
  Serial.print(data.lz);
  Serial.print("rx: ");
  Serial.print(data.rx);
  Serial.print(" ry: ");
  Serial.print(data.ry);
  Serial.print(" rz: ");
  Serial.print(data.rz);
  Serial.println();*/


  //Daten senden 
  radio.write(&data, sizeof(Data_Package));
  //Serial.println("Sende ..");
}