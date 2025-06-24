#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266_LED_Test_MainPage4.h>

#include <Wire.h>           //Bibliothek I2C 
#include <INA219_WE.h>      //Ina219 

#define I2C_ADDRESS 0x40    //I2C - Adresse INA
INA219_WE ina219 = INA219_WE(I2C_ADDRESS);  //Ina219 Objekt erstellen 

ESP8266WebServer server(80); //Webserver aufsetzen 

//Pinbelegung 
#define PIN_WLAN 0      //Verbindungsstatus
#define PIN_PWM 12      //PWM - Signal
#define PIN_VOR 13      //Rückwärts
#define PIN_ZURUECK 14  //Vorwärts
#define PIN_BUS_SCL 4   //an INA219 SCL PIN
#define PIN_BUS_SDA 5   //an INA219 SDA PIN 

//Konstanten für WLAN - Verbindung
#define WLAN_name "ESP8266"
#define WLAN_passwort "67066232197381018236"
#define DNS_servername "GARTENBAHN"

//Variablen für PWM-Werte und Richtung 
int PWMSlider_wert = 0;

int sollwert = 0;
int istwert = 0; 
int interval = 100;
String richtung = ""; 
int stromstaerke = 0; 

int timeout = 0; 
bool nothalt = false; 

String richtungalt = ""; 

//Variablen für Timer 
unsigned long previousmillis = 0; 

//Variablen für Rücksendung von Daten 
String buf = "";


void setup() {
  //PinModes festlegen 
  pinMode(PIN_VOR, OUTPUT);
  pinMode(PIN_ZURUECK, OUTPUT);
  pinMode(PIN_PWM, OUTPUT);
  pinMode(PIN_WLAN, OUTPUT);

  //Serieller Monitor
  Serial.begin(9600);
  Serial.println();
  Serial.println("Setup - Start");
  
  //WLAN Name und Schlüssel erfragen 
  WiFi.begin(WLAN_name, WLAN_passwort);

  //Bus - Kommunikation starten
  Wire.begin();
  if(!ina219.init()){   
    Serial.println("INA219 not connected!");  //Fehlermeldung bei Kommunikationsfehler
  }         
  ina219.setADCMode(SAMPLE_MODE_128); //Zeitinterval der Messung 
  Serial.println("INA219 connected");

  //Warten auf WLAN - Verbindung
  Serial.println("Verbindung wird hergestellt ..");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(PIN_WLAN, 1);
    delay(500);
    digitalWrite(PIN_WLAN, 0);
    delay(500);
    Serial.print(".");
  }

  //Verbunden 
  Serial.println();
  Serial.print("Verbunden! IP-Adresse: ");
  Serial.println(WiFi.localIP());

  //MDNS
  if (MDNS.begin(DNS_servername)) {
    Serial.print("DNS gestartet! Server-Domain: ");
    Serial.print(DNS_servername);
    Serial.println(".local");
    digitalWrite(PIN_WLAN, 1);
  }
  
  //Fehlermeldung
  server.onNotFound([]() {
    server.send(404, "text/plain", "Link wurde nicht gefunden.");
  });

  //Event-Handler

  server.on("/", [PAGE_MAIN] () {
    server.send(200, "text/html", PAGE_MAIN);
  });

  server.on("/UPDATE_DATA", [] () { 
    Updatedata();
  });

  server.on("/NOTHALT", [] () {  
    if (nothalt == false) {   
      istwert = 0; 
      nothalt = true; 
      server.send(200, "text/plain", "nothalt");
    } else {
      nothalt = false; 
      server.send(200, "text/plain", "");
    }
  });

  //Standby - Modus deaktivieren 
  wifi_set_sleep_type(NONE_SLEEP_T);

  //Server Starten
  server.begin();

  //Setup Beenden
  Serial.println("Setup - End");
}

void loop() {

  //Server konstant aktuell halten
  server.handleClient();
  MDNS.update();
  yield();
  //Serial.println(analogRead(A0));

  //Timer  (alle 100 millisekunden)
  unsigned long currentmillis = millis();

  if (currentmillis - previousmillis >= interval) {
    previousmillis = currentmillis;
    //timeout hochzählen  
    timeout += interval; 
    //Serial.println(timeout);
    //Abbruchbedingung entweder nothalt oder über 5 sekunden keine antwort
    if (timeout < 5000 && nothalt == false) {
      if (sollwert > istwert) {
        istwert ++; 
      //Istwert vermindern
      } else if (sollwert < istwert) {
        istwert --; 
      }
      //Richtung ändern, falls gewünscht
      FindDirection();
      analogWrite(PIN_PWM, istwert);
    } else {
      analogWrite(PIN_PWM , 0);
    }

  }
}

void FindDirection() {
  if (istwert == 0) {
    if (richtung == "vor") {
      digitalWrite(PIN_VOR, 1);
      digitalWrite(PIN_ZURUECK, 0);
    } else if (richtung == "zurueck") {
      digitalWrite(PIN_VOR, 0);
      digitalWrite(PIN_ZURUECK, 1);
    }
  richtungalt = richtung;
  }
}

void Updatedata() {
  
  int trenn1;
  int trenn2;  
   
  //rohen Datenstring empfangen 
  //Format: 
  //PWM Wert-interval-richtung
  String rawdata = server.arg("DATA");
  //Serial.print("rawdata: ");
  //Serial.println(rawdata);

  //Datenstring zerteilen
  trenn1 = rawdata.indexOf("-");
  trenn2 = rawdata.lastIndexOf("-");

  PWMSlider_wert = rawdata.substring(0, trenn1).toInt();
  //Serial.print("PWM Wert: ");
  //Serial.println(PWMSlider_wert);

  interval = rawdata.substring(trenn1 + 1, trenn2).toInt();
  //Serial.print("interval: ");
  //Serial.println(interval); 
  richtung = rawdata.substring(trenn2 + 1);
  //Serial.print("richtung: ");
  //Serial.println(richtung);
  
  //Bei Richtungsänderung Fahrtrichtung ändern
  if (richtungalt != richtung) {
    sollwert = 0;    
  } else {
    sollwert = PWMSlider_wert;  
  }
   
  //Server antworten
  stromstaerke = ina219.getCurrent_mA();
  //Serial.print("Stromstarke:");
  //Serial.println(stromstaerke);

  buf = String(sollwert) + "-" + String(istwert) + "-" + String(interval) + "-" + richtung + "-" + String(stromstaerke); 
  server.send(200, "text/plain", buf);
  timeout = 0; 
}
