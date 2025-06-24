# define blinkinterval 400

unsigned long previousmillis = 0; 
unsigned long currentmillis = 0;
 

bool ledstatus = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Setup - Start");

  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  Serial.println("Setup - End");
  // put your setup code here, to run once:
}

void loop() {
  /*currentmillis = millis();
 
  if (currentmillis - previousmillis >= blinkinterval) {
    previousmillis = currentmillis; 

    ledstatus = !ledstatus;
    Serial.println(ledstatus);

    digitalWrite(10, ledstatus);
    digitalWrite(11, ledstatus); 

  }*/
  // put your main code here, to run repeatedly:
  //delay(zeit);
  digitalWrite(10, 1);
  //delay(zeit);
}
