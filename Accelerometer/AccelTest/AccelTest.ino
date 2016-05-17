int X_PIN = 2;
int Y_PIN = 1;
int Z_PIN = 0;

void setup() {
  Serial.begin(9600);
  
  delay(100);
  
  Serial.println("Analog Test");
  Serial.println();
}

void loop() {
  readInputs();
  delay(1000);
}

void readInputs()
{
  float xVolts = analogRead(X_PIN);
  float yVolts = analogRead(Y_PIN);
  float zVolts = analogRead(Z_PIN);
  
  Serial.println("x: " + (String)(xVolts / 1023.0 * 16.0));
  Serial.println("y: " + (String)(yVolts / 1023.0 * 16.0));
  Serial.println("z: " + (String)(zVolts / 1023.0 * 16.0));
  Serial.println();
}
