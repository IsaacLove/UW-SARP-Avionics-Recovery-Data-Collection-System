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
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP183.h>
#include <TinyGPS.h>

////////////////////////////////////////////////////////////////////////////////
// Constants and Pin Numbers
////////////////////////////////////////////////////////////////////////////////

#define SAMPLES 100 // Indicates the number of samples that the sensors should take.
#define FILE_NAME "data.txt" // File Name @TODO: make this dynamic (time from data logger perhaps).

// GPS uses Serial1
#define GPS_BAUD 9600 // @TODO: finalize baud

//  Accelerometer, pins: @TODO: finalize pins
//pinMode(#, INPUT);  //  D# (SCK)
//pinMode(#, INPUT);  //  D# (MISO)
//pinMode(#, OUTPUT); //  D# (MOSI)
//pinMode(4, INPUT);  //  D# (SS)

// Nosecone Temperature Sensor (DS18B20)
#define TEMP_DQ 6 // Digital Pin 6

////////////////////////////////////////////////////////////////////////////////
// SPI devices:
////////////////////////////////////////////////////////////////////////////////

#define SCK 52
#define MISO 50
#define MOSI 51

// Barometer / Altimeter, pins: @TODO: finalize pins
#define BMP183_SS  4  // Slave Select (CS)

//  DataLogger, pins: @TODO: finalize pins
#define DATALOGGER_SS  5  // Slave Select (CS)

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
Adafruit_BMP183 bmp = Adafruit_BMP183(BMP183_CLK, BMP183_SDO, BMP183_SDI, BMP183_SS);

// GPS:
TinyGPS gps;

////////////////////////////////////////////////////////////////////////////////
// Setup
//
// Called once upon startup of the Arduino.  Pins are initialized, and a
// confirmation message is sent to both the data logger and the radio.
////////////////////////////////////////////////////////////////////////////////
void setup() {

  //  Accelerometer, pins: @TODO: finalize pins
  //pinMode(#, INPUT);  //  D# (SCK)
  //pinMode(#, INPUT);  //  D# (MOSI)
  //pinMode(#, OUTPUT); //  D# (MISO)
  //pinMode(#, INPUT);  //  D# (SS)

  //Serial2.begin(MM_BAUD); // @TODO: This should be the micro modem baud rate;

  //while (!Serial2) // Wait for Serial to finish setting up.
  //{}

 //Serial1.begin(GPS_BAUD); // @TODO: Get GPS working

  Serial.begin(9600); // Default serial communication with a computer over USB
  Serial.println("#RocketName: TM1: Serial/Radio Test");

  if (!bmp.begin()) // Start the BMP
  {
    Serial.println("#RocketName: ERROR2: NO BMP DETECTED");
  }
  else
  {
    Serial.println("#RocketName: TM2: BMP Test");
  }

  if (!SD.begin(DATALOGGER_SS))
  {
    Serial.println("#RocketName: ERROR3: NO DATALOGGER DETECTED");
  }
  else
  {
    Serial.println("#RocketName: TM3: DataLogger Test");
  }

  gpsSS.begin(57600); // GPS Baud rate
  if (!gpsTest())
  {
    Serial.println("#RocketName: ERROR4: NO GPS DETECTED");
  }
  else
  {
    Serial.println("#RocketName: TM4: GPS Test");
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
// gpsTest
//
// Checks for valid GPS data.
////////////////////////////////////////////////////////////////////////////////
boolean gpsTest ()
{
  return true; // @TODO: test GPS packets for checksum to pass test, or other
}

////////////////////////////////////////////////////////////////////////////////
// getGPSLat
//
// Returns a float containing the average GPS Latitutde in degrees
////////////////////////////////////////////////////////////////////////////////

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
