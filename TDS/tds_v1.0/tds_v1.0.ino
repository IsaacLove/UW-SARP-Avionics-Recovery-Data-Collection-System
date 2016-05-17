////////////////////////////////////////////////////////////////////////////////
// SARP 2015-2016
//  Telemetry Data Systems.
//
// Contributors:
//  Isaac Love
//  TARKAN AL-KAZILY
//
// The Telemetry Data System is the collection and processing system for
//  gathering rocket data.  Attached systems are:
//
//  GPS
//  Barometer / Altimeter
//  Accelerometer
//  DataLogger
//
// Control flow is as follows:
//  1. Data collection period.  During this time, sensros are polled every
//     50ms.  Each reading is averaged together.
//  2. Data processing.  The data packaged into a packet to be sent to the
//     radio.  Each packet has the following form:
//
//   <#general packet form here#><checksum> @TODO fix the packet form.
//
//  3. Flush.  Data is sent to the radio and the data logger.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Imports
////////////////////////////////////////////////////////////////////////////////
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP183.h>
#include <TinyGPS.h>

////////////////////////////////////////////////////////////////////////////////
// Constants and Pin Numbers
////////////////////////////////////////////////////////////////////////////////

#define SAMPLES 100 // Indicates the number of samples that the sensors should take.
#define FILE_NAME "data.txt" // File Name @TODO: make this dynamic (time from data logger perhaps).

// GPS uses Serial1 at 57600
#define GPS_BAUD 57600

// Accelerometer
#define ACCEL_X A2
#define ACCEL_Y A1
#define ACCEL_Z A0

// Nosecone Temperature Sensor (DS18B20)
#define TEMP_DQ 6 // @TODO: finalize pins

////////////////////////////////////////////////////////////////////////////////
// SPI devices:
////////////////////////////////////////////////////////////////////////////////

#define SCK 52
#define MISO 50
#define MOSI 51

// Barometer / Altimeter, pins: @TODO: finalize pins
#define BMP183_SS  2  // Slave Select (CS)

//  DataLogger, pins: @TODO: finalize pins
#define DATALOGGER_SS  3  // Slave Select (CS)

////////////////////////////////////////////////////////////////////////////////
// MicroModem:
////////////////////////////////////////////////////////////////////////////////

//SimpleSerial commands
#define SET_CALLSIGN 'c'
#define SEND_RAW_PACKET '!'
#define SEND_LOCATION '@'
#define SEND_APRS_MSG '#'
#define SET_LATITUDE "lla"
#define SET_LONGITUDE "llo"
#define SHOW_CONFIG 'H'

//MicroModem baud rate
//Serial2 pins reserved for MicroModem: 17 (RX) 16 (TX)
#define MM_BAUD 9600

//Radio Call Sign
static String callsign = "KI7AFR";

////////////////////////////////////////////////////////////////////////////////
// Sensor Class Definitions
////////////////////////////////////////////////////////////////////////////////

// Barometer / Altimeter:
Adafruit_BMP183 bmp = Adafruit_BMP183(BMP183_SS);

// GPS:
TinyGPS gps;

////////////////////////////////////////////////////////////////////////////////
// Setup
//
// Called once upon startup of the Arduino.  Pins are initialized, and a
// confirmation message is sent to both the data logger and the radio.
////////////////////////////////////////////////////////////////////////////////
void setup() {

  //Serial2.begin(MM_BAUD); // @TODO: This should be the micro modem baud rate;

  //while (!Serial2) // Wait for Serial to finish setting up.
  //{}

 //Serial1.begin(GPS_BAUD); // @TODO: Get GPS working

  Serial.begin(9600); // Default serial communication with a computer over USB
  Serial.println("#RocketName: TM1: Serial/Radio Test");

  if (bmp.begin()) // Start the BMP
  {
    Serial.println("#RocketName: TM2: BMP Test");
  }
  else
  {
    Serial.println("#RocketName: ERROR2: NO BMP DETECTED");
  }

  if (SD.begin(DATALOGGER_SS))
  {
    Serial.println("#RocketName: TM3: DataLogger Test");
  }
  else
  {
    Serial.println("#RocketName: ERROR3: NO DATALOGGER DETECTED");
  }

  Serial1.begin(GPS_BAUD); // GPS Baud rate
  if (gpsTest())
  {
    Serial.println("#RocketName: TM4: GPS Test");
  }
  else
  {
    Serial.println("#RocketName: ERROR4: NO GPS DETECTED");
  }

  Serial.println("TDS is Online");
}

void loop() {
  //delay(1000);
  Serial.println(getTemp());
}

////////////////////////////////////////////////////////////////////////////////
// dataFlush
//
// Outputs the String packet to both the data logger and the radio.
////////////////////////////////////////////////////////////////////////////////
void dataFlush(String packet)
{
  Serial.println(packet);

  File dataFile = SD.open(FILE_NAME, FILE_WRITE);

  if (dataFile)
  {
    dataFile.println(packet);
    dataFile.close();
  }
  else
  {
    Serial.println("ERROR101: CANNOT ACCESS FILE: " + (String)FILE_NAME);
  }

}

////////////////////////////////////////////////////////////////////////////////
// getTemp
//
// Returns a float containing the average Temperature in C.
////////////////////////////////////////////////////////////////////////////////
float getTemp ()
{
  float total = 0;
  int measures = 0;

  for (int i = 0; i < SAMPLES; i++)
  {
    measures++;
    total += bmp.getTemperature();
  }

  if (measures != 0)
  {
    return total / (float)measures;
  }

  else // No data collected, return invalid measure.
  {
    return -999.0f;
  }
}

////////////////////////////////////////////////////////////////////////////////
// getRawAccelXYZ
//
// Returns X Y Z data as a string delimeted by ", "
////////////////////////////////////////////////////////////////////////////////
String getRawAccelXYZ ()
{
  return (String)(analogRead(ACCEL_X)) + ", " + (String)(analogRead(ACCEL_Y)) + ", " + (String)(analogRead(ACCEL_Z));
}
////////////////////////////////////////////////////////////////////////////////
// gpsTest
//
// Checks for valid GPS data.
////////////////////////////////////////////////////////////////////////////////
boolean gpsTest ()
{
  return true; // @TODO: test GPS packets for checksum to pass test, or other
}

////////////////////////////////////////////////////////////////////////////////
// getGPSLocation
//
// Returns a String containting three numbers delimited by a comma and a space:
//  Latitude, Longitude, Age
////////////////////////////////////////////////////////////////////////////////
String getGPSLocation ()
{
  float lat;
  float lon;
  unsigned long age;

  gps.f_get_position(&lat, &lon, &age);
  return (String)(lat) + ", " +  (String)(lon) + ", " + (String)(age);
}

String getGPSSpeed()
{
    float speed;

    speed = gps.f_speed_mph();

    return (String) speed;
}

String getGPSDateTime()
{
    unsigned long date;
    unsigned long time;
    unsigned long age;

    gps.get_datetime(&date, &time, &age);

    return (String)(date) + ", " + (String)(time) + ", " + (String)(age);
}
////////////////////////////////////////////////////////////////////////////////
// MicroModem Methods
//
// Uses Serial2 to control the MicroModem
////////////////////////////////////////////////////////////////////////////////

void setCallSign(String call)
{
    String str = "";
    str += SET_CALLSIGN;
    str += call;

    Serial2.print(str);
}

void displaySettings()
{
   String str = "";
   str += SHOW_CONFIG;

   Serial2.print(str);
}

void sendMessage(String message)
{
    String str = "";
    str += SEND_RAW_PACKET;
    //str += SEND_APRS_MSG;
    str += message;

    Serial2.print(str);
}

void setLatitude(String latitude)
{
    String str = "";
    str += SET_LATITUDE;
    str += latitude;

    Serial2.print(str);
}

void setLongitude(String longitude)
{
    String str = "";
    str += SET_LONGITUDE;
    str += longitude;

    Serial2.print(str);
}
