/*ReadDS18B20
ver: 6 Jly 2010
THIS IS A FIRST DRAFT.... WORKS, but scheduled for overhaul.


Simple, simple test of reading DS18B20
connected to nuelectronics.com datalogging shield.

See...

http://sheepdogguides.com/arduino/ar3ne1tt.htm

... for explanation of this code.

Code lightly adapted from code from nuelectronics.com*/

#include <OneWire.h>

#define TEMP_PIN  14 //See Note 1, sheepdogguides..ar3ne1tt.htm

void OneWireReset(int Pin);//See Note 2
void OneWireOutByte(int Pin, byte d);
byte OneWireInByte(int Pin);

void setup() {
    digitalWrite(TEMP_PIN, LOW);
    pinMode(TEMP_PIN, INPUT);      // sets the digital pin as input (logic 1)
Serial.begin(9600);
//9600 to match the data rate being used by the
//serial monitor on my system, which is set to
//the Arduino default. (Sample code published
//by nuelectronics used a faster baud rate.)
    delay(100);
    Serial.print("temperature measurement:\n");
}

void loop(){
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;

  OneWireReset(TEMP_PIN);
  OneWireOutByte(TEMP_PIN, 0xcc);
  OneWireOutByte(TEMP_PIN, 0x44); // perform temperature conversion, strong pullup for one sec

  OneWireReset(TEMP_PIN);
  OneWireOutByte(TEMP_PIN, 0xcc);
  OneWireOutByte(TEMP_PIN, 0xbe);

  LowByte = OneWireInByte(TEMP_PIN);
  HighByte = OneWireInByte(TEMP_PIN);
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25

  Whole = Tc_100 / 100;  // separate off the whole and fractional portions
  Fract = Tc_100 % 100;


  if (SignBit) // If its negative
  {
     Serial.print("-");
  }
  Serial.print(Whole);
  Serial.print(".");
  if (Fract < 10)
  {
     Serial.print("0");
  }

  Serial.print(Fract);

      Serial.print("\n");
  delay(5000);      // 5 second delay.  Adjust as necessary
}

void OneWireReset(int Pin) // reset.  Should improve to act as a presence pulse
{
     digitalWrite(Pin, LOW);
     pinMode(Pin, OUTPUT); // bring low for 500 us
     delayMicroseconds(500);
     pinMode(Pin, INPUT);
     delayMicroseconds(500);
}

void OneWireOutByte(int Pin, byte d) // output byte d (least sig bit first).
{
   byte n;

   for(n=8; n!=0; n--)
   {
      if ((d & 0x01) == 1)  // test least sig bit
      {
         digitalWrite(Pin, LOW);
         pinMode(Pin, OUTPUT);
         delayMicroseconds(5);
         pinMode(Pin, INPUT);
         delayMicroseconds(60);
      }
      else
      {
         digitalWrite(Pin, LOW);
         pinMode(Pin, OUTPUT);
         delayMicroseconds(60);
         pinMode(Pin, INPUT);
      }

      d=d>>1; // now the next bit is in the least sig bit position.
   }

}

byte OneWireInByte(int Pin) // read byte, least sig byte first
{
    byte d, n, b;

    for (n=0; n<8; n++)
    {
        digitalWrite(Pin, LOW);
        pinMode(Pin, OUTPUT);
        delayMicroseconds(5);
        pinMode(Pin, INPUT);
        delayMicroseconds(5);
        b = digitalRead(Pin);
        delayMicroseconds(50);
        d = (d >> 1) | (b<<7); // shift d to right and insert b in most sig bit position
    }
    return(d);
}
