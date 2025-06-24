

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);

const String WLAN_name = "Error404";
const String WLAN_passwort = "67066232197381018236";
const String DNS_servername = "ESP8266";

void setup() {
  //Pinmodes hier einfügen 
  //pinMode(PinNR, OUTPUT/INPUT);

  // Seriellen Monitor Initialisieren
  Serial.begin(9600);
  Serial.println();
  Serial.println("Setup - Start");
  
  //WLAN Name und Schlüssel erfragen 
  WiFi.begin(WLAN_name, WLAN_passwort);

  //Warten auf Verbindung
  Serial.println("Verbindung wird hergestellt ..");
  while (WiFi.status() != WL_CONNECTED) {
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
  }
  
  //Fehlermeldung
  server.onNotFound([]() {
    server.send(404, "text/plain", "Link wurde nicht gefunden.");
  });

  //Event-Handler
  server.on("/", [] () {
    server.send(200, "text/plain", "ESP-Startseite!");
  });

  server.on("/Test", [] () {
    server.send(200, "text/plain", "Test erfolgreich.");
  });
  //Automatischen Standby-Modus deaktivieren (opional)
  wifi_set_sleep_type(NONE_SLEEP_T);

  //Server Starten
  server.begin();

  //Setup Beenden
  Serial.println("Setup - End");
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  MDNS.update();
}
