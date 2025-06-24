#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266_LED_Test_MainPage.h>

ESP8266WebServer server(80);

const String WLAN_name = "ESP8266";
const String WLAN_passwort = "67066232197381018236";
const String DNS_servername = "LED";

int PWMSlider_value = 0; 
int istwert = 0; 
int sollwert = 0; 
int schritt = 5; 
String direction;

unsigned long previousmillis = 0; 
const int INTERVAL = 100; 

void setup() {
  // put your setup code here, to run once:
  /*String Website = "";
    Website +=  "<!DOCTYPE html>";
    Website += "<html>";
    Website += "<body>";
    Website += "<h1> Test </h1>";
    Website += "</body";
    Website += "/html>";*/
   //PinModes festlegen
  pinMode(13, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);

  //Serieller Monitor
  Serial.begin(9600);
  Serial.println();
  Serial.println("Setup - Start");
  
  //WLAN Name und Schlüssel erfragen 
  WiFi.begin(WLAN_name, WLAN_passwort);

  //Warten auf Verbindung
  Serial.println("Verbindung wird hergestellt ..");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(13,1);
    delay(500);
    digitalWrite(13, 0);
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
    digitalWrite(13, 1);
  }
  
  //Fehlermeldung
  server.onNotFound([]() {
    server.send(404, "text/plain", "Link wurde nicht gefunden.");
  });

  //Event-Handler

  server.on("/", [PAGE_MAIN] () {
    server.send(200, "text/html", PAGE_MAIN);
  });

  server.on("/BUTTON_0", [] () {
    ProcessLED(5, 255);
  });

  server.on("/BUTTON_1", [] () {
    ProcessLED(5, 0);
  });

  server.on("/UPDATE_PWMSLIDER", [] () {
    PWMSliderupdate();
  });

  server.on("/UPDATE_STEPSLIDER", [] () {
    StepSliderupdate();
  });

  server.on("/UPDATE_DIRECTION", [] () {
    Directionupdate();
  });
  
  /*server.on("/Test", [] () {
    server.send(200, "text/plain", "Test erfolgreich.");
  });*/

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

  unsigned long currentmillis = millis();


  //Timer  (alle 100 millisekunden)
  if (currentmillis - previousmillis >= 100) {
    previousmillis = currentmillis; 
  
    if (sollwert > istwert) {
      istwert += schritt; 
      if (istwert > 255) {
        istwert = 255;
      }
    } else if (sollwert < istwert) {
      istwert -= schritt;
      if (istwert < 0) {
        istwert = 0; 
      } 
    }
    FindDirection();
    ProcessLED(5, istwert);
  }
}



void ProcessLED (int Port, int Wert) {
  analogWrite(Port, Wert); 
  Serial.print("neuer Wert: ");
  Serial.println(String(Wert));
  //server.send(200, "text/plain", "");
}


void PWMSliderupdate() {
  String Status = server.arg("VALUE");
  sollwert = Status.toInt(); 
  PWMSlider_value = sollwert;
  Serial.print("PWMSliderupdate "); 
  Serial.println(sollwert);
  server.send(200, "text/plain", "");
  //Ramp(istwert, sollwert);  
  //istwert = sollwert; 
}


void StepSliderupdate() {
  String Status = server.arg("VALUE");
  schritt = Status.toInt();
  Serial.print("Schrittweitenupdate ");
  Serial.println(schritt);
  server.send(200, "text/plain", "");
}


void Directionupdate() {
  direction = server.arg("VALUE");
  sollwert = 0;
  Serial.print("Richtung: ");
  Serial.println(direction);
  server.send(200, "text/plain", "");
  }

void FindDirection() {
  if (istwert == 0) {
    if (direction == "front") {
      digitalWrite(4, 1);
    } else if (direction == "back") {
      digitalWrite(4, 0);
    }
  sollwert = PWMSlider_value;
  }
}





