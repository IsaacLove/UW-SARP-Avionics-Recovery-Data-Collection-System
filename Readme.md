# SARP Avionics and Telemetry Payload Repository

This repo contains the code, notes and documentation for UW's Society of
Advanced Rocket Propulsion's Avionics and Telemetry Tracking payload. Our goal
is to build and launch a rocket that will reach the altitude of 24,000 feet.
The goals of this payload are:  

1. To track and locate the rocket during and after flight.
2. To record inflight data from the location of the nosecone
3. To earn points at our competition at Utah.

This repository covers the basic instrumentation section of the payload, the
K-12 involved payload, and the APRS locators.

---

## Instrumentation

The instrumentation in the payload uses a number of sensors to gather data
during flight.

1. [GPS
LS20030](https://cdn.sparkfun.com/datasheets/GPS/LS20030~3_datasheet_v1.3.pdf) -
gathers latitude, longitude, and altitude of the rocket
2. [Temperature Sensor
DS18B20](https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf) - placed in
the tip of the nosecone, gathers the temperature of the nosecone
3. [Accelerometer
ADXL326](http://www.analog.com/media/en/technical-documentation/data-sheets/ADXL
326.pdf) - gathers three axes of acceleration data
4. [Barometer BMP183](https://cdn-shop.adafruit.com/datasheets/1900_BMP183.pdf)
\- gathers pressure data, altitude, and temperature

These sensors are controlled by a [Seeeduino Mega
2560](http://www.seeedstudio.com/wiki/Seeeduino_Mega), which is an off-brand
Arduino Mega 2560, and the data is stored to an 8 GB microSD card using a
[MicroSD Breakout Board](https://www.adafruit.com/product/254).

A secondary goal is to send the data in flight as basic digital packets for
additional tracking as well as to receive data prior to landing in the event
of a loss of the payload.

#### K-12 payload

The purpose of the K-12 payload is to teach middle schoolers about springs
and acceleration. The payload comprises of a spring with a weight on the
end, enclosed within a graduated cylinder, acting as an accelerometer. The
motion of the weight is tracked by a photocell, using an LED on the mass to
provide light. The raw data taken from the photocell is recorded on the
microSD card to be analyzed using Hooke's Law after landing.

---

## Telemetry

We are using the Amateur Radio bands to transmit the rockets location and
data to a ground station

#### APRS

To locate the rocket efficiently, we are using of Amateur Radio equipment
transmitting on the 2 meter and 70 centimeter bands. We are using the APRS
protocol to send GPS location data on the standard US APRS frequency,
144.390 MHz, since there are a large number of digipeaters to repeat the APRS
signal, as well as add our rocket's location to online APRS maps. The call sign
and SSID of the rocket is KI7AFR-2 and can be tracked online on [aprs.fi
here](http://aprs.fi/#!mt=roadmap&z=11&call=a%2FKI7AFR-2&timerange=3600&tail=360
0).

Our initial tracker is the [Byonics Micro-Trak All In
One](http://www.byonics.com/mt-aio) which has a built in GPS and radio
transmitter programmed to transmit at configurable rates on 144.39 MHz. In
order to be more likely to have a last known location of the rocket in the
before it touches down, as well as to have altitude information during flight,
it is currently configured to transmit every 5 seconds. It is powered by 12
Volts from eight double A batteries. See the above link for documentation and
configuration software for the Micro-Trak.

#### MicroModem

To transmit data in flight, we are using the MicroModem, a custom built
component using an ATmega328p microprocessor. It is running the [MicroAPRS
SimpleSerial 3.3V Firmware](http://unsigned.io/projects/microaprs/) takes
commands over Serial and sends messages using a handheld radio in the rocket.
The handheld radio we are using is the [Baofeng
BF-888s](http://www.miklor.com/BF888/) and transmits on the 70 cm band. The
MicroModem is controlled by the Seeeduino Mega, and sends data collected by the
instrumentation part of the payload, primarily GPS location data and altitude.

#### BigRedBee Tracker

In addition to transmitting location data, we have a small radio beacon
transmitting a tone every five seconds to act as a redundant method of
locating the rocket. The [BigRedBee
Tracker](http://www.bigredbee.com/BeeLine.htm) is configured to transmit
on 433.920 MHz the ground station operator's call sign and its battery voltage
in Morse code, followed by a series of tones spaced five seconds apart for two
and a half minutes before repeating. That way in the event of a loss of APRS
tracking it is possible to locate the rocket using a directional 70 cm antenna
by gauging the signal strength of the beacon in each direction.

---

## Ground Station

Our setup to receive telemetry data includes a VHF/UHF radio to receive the
APRS packets as well as the telemetry data from the MicroModem and to track
the beacon's transmissions. As part of this we are using a high gain ground
plane antenna on the 2 m band to receive APRS packets and a directional
Yagi antenna on the 70 cm band to receive the MicroModem telemetry data as
well as track the beacon's transmissions. We are using a Macbook as a ground
station, connected to a [SignalLink USB by
Tigertronics](http://www.tigertronics.com/slusbmain.htm) configured for a
handheld [Baofeng UVB5](http://www.miklor.com/UVB5/UVB5-FAQ.php) as the
receiving radio. To decode and display the APRS location packets, we are using
the [PocketPacket Mac Software](http://koomasi.com/pocketpacket-mac-os/). In
order to decode the digital packets sent by the MicroModem, we are using the
[QTMM AFSK1200 Decoder Software](https://sourceforge.net/projects/qtmm/) to
display and record the live data.
