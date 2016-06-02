## Telemetry

In order to track our rocket, we are using amateur radio equipment to transmit
the location of our rocket on the 2 meter and 70 centimeter wavelengths.

#### APRS

On the 2 meter wavelength, we are using the APRS protocol to track and locate
our rocket. On board our rocket we have a Byonics Micro-Trak All In One which
sends GPS longitude and latitude on the standard APRS frequency 144.390 MHz.
We are using the APRS protocol because amateur operators maintain a large number
of digipeaters which increase the range of the APRS signal, helping us to better
locate our rocket. In addition, some of these digipeaters automatically upload
the APRS data to the internet where it can be
tracked by anyone around the world.

#### MicroModem

On the 70 centimeter wavelength, we are using a MicroModem, an open-source
wireless modem based on an ATmega328p microprocessor. It communicates
with the Arduino data logger, and transmits through a connected handheld radio
over any available frequency. We are using the MicroModem to send redundant GPS
data as well as in-flight telemetry data.
