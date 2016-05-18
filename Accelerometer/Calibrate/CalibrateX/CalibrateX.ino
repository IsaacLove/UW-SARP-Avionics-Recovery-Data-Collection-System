// Raw output of 350 is about 1g
// Raw output of 325 is about 0g

int X_PIN = 2;
int Y_PIN = 1;
int Z_PIN = 0;

long total = 0;
long num = 0;

void setup() {
    Serial.begin(9600);

    delay(100);

    Serial.println("Analog X Test");
    Serial.println();
}

void loop() {
    for (int i = 0; i < 1000; i++)
    {
        //readX();
        //readY();
        readZ();
        delay(1);
    }
    Serial.println("AVG: " + (String)(total/num));
    total = 0;
    num = 0;
}

void readX()
{
    float xVolts = analogRead(X_PIN);
    //Serial.println(xVolts)

    total += xVolts;
    num++;
}

void readY()
{
    float yVolts = analogRead(Y_PIN);
    //Serial.println(xVolts)

    total += yVolts;
    num++;
}

void readZ()
{
    float zVolts = analogRead(Z_PIN);
    //Serial.println(xVolts)

    total += zVolts;
    num++;
}

void readInputs()
{
    float xVolts = analogRead(X_PIN);
    float yVolts = analogRead(Y_PIN);
    float zVolts = analogRead(Z_PIN);

    Serial.println("x: " + (String)(xVolts)); // / 1023.0 * 16.0));
    Serial.println("y: " + (String)(yVolts)); // / 1023.0 * 16.0));
    Serial.println("z: " + (String)(zVolts)); // / 1023.0 * 16.0));

    Serial.println();
}
