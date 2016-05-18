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
#define BASE_NAME "ORDATA" // File Name @TODO: make this dynamic (time from data logger perhaps).
#define EXTENSION ".txt"
#define SEA_LEVEL_PRESSURE 1013.25
#define TICKS_PER_G 25.0f // @TODO: Make these calibrated somehow
#define ZERO_G_OFFSET 325.0f
#define ONE_SECOND 1000 // How many milliseconds per second

// GPS uses Serial1
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

// Barometer / Altimeter, pins:
#define BMP183_SS  2  // Slave Select (CS)

//  DataLogger, pins:
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

// Filename variable
String file = "null";

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
        int i = 1;
        do
        {
            file = BASE_NAME + (String) i + EXTENSION;
            i++;
        } while (SD.exists(file));
        Serial.println("#RocketName: Filename: " + file);
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
    unsigned long milli = millis();
    smartdelay(1000);
    String accelerometer_data = getAccelData();
    String barometer_data = getBaromData();
    String gps_data = getGPSLocation() + ", " + getGPSSpeed();
    //Serial.println("Millis: " + milli);
    //Serial.println(accelerometer_data);
    logData(accelerometer_data);
    //Serial.println(barometer_data);
    logData(barometer_data);
    //Serial.println(gps_data);
    logData(gps_data);
}

////////////////////////////////////////////////////////////////////////////////
// logData
//
// Outputs the String packet to both the data logger and the radio.
////////////////////////////////////////////////////////////////////////////////
void logData(String packet)
{
    String time_string = (String) (millis()/ONE_SECOND) + "s: ";
    packet = time_string + packet;
    Serial.println(packet);

    File dataFile = SD.open(file, FILE_WRITE);

    if (dataFile)
    {
        dataFile.println(packet);
        dataFile.close();
    }
    else
    {
        Serial.println("ERROR101: CANNOT ACCESS FILE: " + file);
    }
}

////////////////////////////////////////////////////////////////////////////////
// getTemperature
//
// Returns a float containing Temperature in C.
////////////////////////////////////////////////////////////////////////////////
float getTemperature()
{
    return bmp.getTemperature();
}

////////////////////////////////////////////////////////////////////////////////
// getPressure
//
// Returns a float containing pressure in Pascals
////////////////////////////////////////////////////////////////////////////////
float getPressure()
{
    return bmp.getPressure();
}

////////////////////////////////////////////////////////////////////////////////
// getAltitude
//
// Returns a float containing altitude in meters
////////////////////////////////////////////////////////////////////////////////
float getAltitude()
{
    return bmp.getAltitude(SEA_LEVEL_PRESSURE);
}

String getBaromData()
{
    return "Temp: " + (String)(getTemperature()) + ", " + "Pascal: " + (String)(getPressure())
        + ", " + "Alt: " + (String)(getAltitude());
}

////////////////////////////////////////////////////////////////////////////////
// getRawAccelX
//
// Returns X value as an int
////////////////////////////////////////////////////////////////////////////////
int getRawAccelX ()
{
    return analogRead(ACCEL_X);
}

////////////////////////////////////////////////////////////////////////////////
// getRawAccelY
//
// Returns Y value as an int
////////////////////////////////////////////////////////////////////////////////
int getRawAccelY ()
{
    return analogRead(ACCEL_Y);
}

////////////////////////////////////////////////////////////////////////////////
// getRawAccelZ
//
// Returns Z value as an int
////////////////////////////////////////////////////////////////////////////////
int getRawAccelZ ()
{
    return analogRead(ACCEL_Z);
}

////////////////////////////////////////////////////////////////////////////////
// getAccelGs
//
// Takes raw Accelerometer data and outputs Accelerometer Gs as a float.
////////////////////////////////////////////////////////////////////////////////
float getAccelGs (int raw)
{
    // @TODO make these constants
    return (((float)raw) - ZERO_G_OFFSET) / TICKS_PER_G; // Sanity Check please
}

String getAccelData()
{
    float x_accel = getAccelGs(getRawAccelX());
    float y_accel = getAccelGs(getRawAccelY());
    float z_accel = getAccelGs(getRawAccelZ());

    return "X_Accel: " + (String)(x_accel) + ", " + "Y_Accel: " + (String)(y_accel)
        + ", " + "Z_Accel: " + (String)(z_accel);
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
    //Serial.println("----" + (String)lat);
    //Serial.println("----" + (String)lon);
    //Serial.println("----" + (String)age);
    return "Lat: " + (String)(lat) + ", " + "Long: " + (String)(lon) + ", " +
        "Age: " + (String)(age);
}

String getGPSSpeed()
{
    float speed;

    speed = gps.f_speed_mph();

    return "MPH: " + (String)(speed);
}

String getGPSDateTime()
{
    unsigned long date;
    unsigned long time;
    unsigned long age;

    gps.get_datetime(&date, &time, &age);

    return (String)(date) + ", " + (String)(time) + ", " + (String)(age);
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
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
