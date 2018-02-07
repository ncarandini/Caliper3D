# Caliper3D
This project use a PC with Windows 10, an Arduino Nano with a LCD display and a digital caliper to measure the geometry of the printing bed of a 3D printer.
<p align="center">
  <img src="/ArtWorks/Project%20Architecture.png" width="700" title="Project architecture">
</p>

## Bill of materials
Links refer to Amazon Italy, but you can find it almost everywere:

### Windows 10 PC
Your everyday Windows 10 PC.
Note that because two usb port are required to connect to Arduino and 3D Printer, if your PC has only one then you need aUSB Hub to connect to both devices at the same time, and the USB Hub need to be powered to give power to the Arduino via USB.

### Arduino Nano
Elegoo Nano V3.0 Scheda Elettronica Microcontrollore CH340 ATmega328P con Cavo USB per Arduino
https://www.amazon.it/dp/B071S5K67Z

### Seeedstudio Grove – LCD
https://www.amazon.it/Seeedstudio-Grove-LCD-Retroilluminazione-Rgb/dp/B01AFKPJ6O

### Caliper
Neoteck DTI LCD Compratore Centesimale Digitale
https://www.amazon.it/gp/product/B01H035DRO

## Caliber modding to connect to the serial port
The caliber comes with a serial port (no TTL or RS232 serial port, more on this later) but to take advantage of it, we need to open the caliper and solder four wires to the board:
<p align="center">
  <img src="/ArtWorks/Schema%20saldatura%20calibro.png" width="700" title="Caliper cable soldering">
</p>

## Breadboard circuit
Since the signal coming from the caliper use a 1.5 V logic that is too low to drive the digital input ports of arduino, then two amplifiers are used to convert the caliper 1.5V logic to the Arduino 5V logic. Moreover, we use a simple voltage divider to get +1.5VDC from the +5VDC power source to power the caliper. Beware not to connect the +5VDC to the caliper!
<p align="center">
  <img src="/ArtWorks/Breadboard.png" width="700" title="Breadboard circuit">
</p>

## Mounting the caliper on the printing head of the 3D printer
It all depends on the 3D printer model you have. On my 3D printer has been an easy job, I only needed to change a screw with a longer one and fix the caliper on it with a washer and bolt.
<p align="center">
  <img src="/ArtWorks/Calibro%20montato%20su%20stampante.png" width="512" title="Caliper mounted on the 3D printer">
</p>

## Arduino Sketch
The Sketch loop Read the caliper measure and listen the (USB) serial port waiting for a command from the UWP App. Actually the only command is the R(ead) command, and when the Arduino receive it, it respond sending the last received caliper measure back to the UWP app via the (USB) serial port.

## Windows 10 UWP app
I'm still completing the app, actually
