void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Setup - Start");
  pinMode(A2, INPUT);
  pinMode(A5, INPUT);

  Serial.println("Setup - End");
}

void loop() {
  int lx = analogRead(0);
  int ly = analogRead(1);
  int lz = digitalRead(A2);
  int rx = analogRead(3);
  int ry = analogRead(4);
  int rz = digitalRead(A5);

  Serial.print("lx: ");
  Serial.println(lx);
  Serial.print("ly: ");
  Serial.println(ly);
  Serial.print("lz: ");
  Serial.println(lz);
  Serial.print("rx: ");
  Serial.println(rx);
  Serial.print("ry: ");
  Serial.println(ry);
  Serial.print("rz: ");
  Serial.println(rz);
  // put your main code here, to run repeatedly:
  /*for (int i = 0; i < 4; i++) {
    int analogValue = analogRead(i);
    Serial.print("A");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(analogValue);
    Serial.print("  ");
  }*/
  Serial.println(); // Print a new line at the end of each loop

  // Delay to slow down the output for readability
  delay(200);
}
