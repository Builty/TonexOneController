# Tonex One Controller: An open-source controller and display interface for the IK Multimedia Tonex One and Tonex Pedal
# Waveshare 7B Hardware Platform and Wiring Details
# Table of Contents
 1. [Connections](#connections)
 2. [Wired Footswitches - onboard](#footswitches)
 3. [Wired Footswitches - external](#footswitches_ext)
 4. [Wired Midi](#midi)

This hardware platform uses the Waveshare ESP32-S3-Touch-LCD-7B board (7", 1024x600, capacitive touch).
<br>
https://www.waveshare.com/esp32-s3-touch-lcd-7b.htm?sku=31726
<br><br>
**Important note:** Waveshare have two similar 7" boards:
- ESP32-S3-Touch-LCD-7: 800x480 screen. Supported with some limitations, refer to [Waveshare 7" and 4.3" (not B)](HardwarePlatform_Waveshare7_4.3.md)
- ESP32-S3-Touch-LCD-7**B**: 1024x600 screen. This page. Works without any board modification

This module provides the microcontroller (16 MB flash, 8 MB PSRAM), the 7" LCD screen, a capacitive touch screen, a USB-C port for the Tonex, a USB-C UART port for power and programming, an RS485 port, and I2C and GPIO headers.
<br>
The user interface is the same as the 4.3B, scaled up to fill the 1024x600 screen. The full set of amp and pedal skins is included.

## Connections <a name="connections"></a>
Note: the controller code relies on the Tonex One pedal being set to Stomp mode. Code is in place to do this automatically.
- Connect the USB-C port marked "USB" on the Waveshare board to the ToneX One USB-C port
- Connect the USB-C port marked "UART1" to a 5 volt USB power supply. A good quality supply rated at 1.5 amps or more is recommended, as the board also powers the Tonex One
- The UART selector switch (marked "UART2 UART1") must be set to UART1 when uploading firmware
- Switch on the power supply
- The Waveshare board USB port will power the Tonex One. No resistor removal or other modification is needed
- Optional: for the Bluetooth Client version of code, switch on a M-Vave Chocolate Midi pedal (https://www.cuvave.com/productinfo/724103.html). After a few seconds it should connect and the Bluetooth icon should change from gray to blue
- Optional: for the Bluetooth Server version of code, the controller will be available as a peripheral for you to connect to via a Bluetooth Midi device. The Bluetooth icon should change from gray to blue when connected.

<br><br>
## Wired Footswitches (onboard) <a name="footswitches"></a>
Wired footswitches can optionally be used. These "onboard" switches connect directly to the controller without needing any additional circuitry.<br>
The footswitch must be a "momentary" type that only has its contacts closed when it is pressed.
<br><br>
For the Waveshare 7B, a maximum of 2 footswitches are supported, always in next/previous preset mode.
- Footswitch 1 (previous preset): connect between the "GP6" and "GND" pins of the 3-pin "GPIO" header
- Footswitch 2 (next preset): connect between the "RX" and "GND" pins of the 4-pin "UART2" header. This input is only connected while the UART selector switch is set to UART2. Set the switch back to UART1 to upload firmware
- Never connect the "3V3" pins to ground
- Holding footswitch 1 for about 8 seconds resets the configuration to the defaults

<br><br>
## Wired Footswitches (external) <a name="footswitches_ext"></a>
With the use of an additional PCB, up to 16 footswitches can be connected.<br>
The footswitch must be a "momentary" type that only has its contacts closed when it is pressed.
<br><br>
The additional PCB must use the "SX1509" chip. The recommended one is the Sparkfun SX1509 breakout board:
https://www.sparkfun.com/sparkfun-16-output-i-o-expander-breakout-sx1509.html
<br>
![image](https://github.com/user-attachments/assets/0575f0a0-1eb3-4aef-a7e2-c321876f7ed0)

NOTE: other types of IO expander boards that use different chips are not supported and will not function. It must contain the SX1509 chip.

### Address Setting
The SX1509 PCB has a selectable address system. This must be set correctly in order for the board to function with the controller.
- Using a sharp knife, carefully cut the thin tracks between the pads as shown below
- Use solder to bridge the pads as shown below
- This sets the SX1509 address to "11" which avoids conflicting with other parts on the controller boards
![sx1509_address](https://github.com/user-attachments/assets/84030e88-f6e7-4fc0-8230-3fff6dc1235a)

### Wiring
Connect the SX1509 board to the 4-pin "I2C" header on the Waveshare board: 3V3 to 3V3, GND to GND, SDA to SDA, and SCL to SCL.
<br>
The common pin of each footswitch must connect to the SX1509 ground pins. The labels "0", "1" etc are the individual switch inputs. Footswitch 1 connected to input 0. Footswitch 2 to input 1 etc.
<br><br>
Multiple modes are supported, configured using the web configuration.

## Wired Midi <a name="midi"></a>
Note: Wired Midi is disabled by default. If it is enabled without the proper hardware (detailed below) being fitted, you may get "phantom" preset changes, due to the serial input "floating".
Only enable wired Midi when hardware is connected!<br>
Refer here for details on how to enable it, and set the Midi channel:
https://github.com/Builty/TonexOneController/blob/main/WebConfiguration.md
<br><br>
An extra PCB is required, an "Adafruit Midi FeatherWing kit", for US$7.<br>
https://www.adafruit.com/product/4740 <br>
This Midi board supports both 5-pin DIN sockets (included in kit) and 3.5mm jacks (not included in kit.)<br>
<br>
As with the Waveshare 4.3B, Midi uses the board's RS485 port (terminals "A" and "B"), so another small interface is required. This is a common, low cost "TTL to RS485" adaptor.<br>
Typical examples of this PCB:
https://www.amazon.com/HiLetgo-Reciprocal-Hardware-Automatic-Converter/dp/B082Y19KV9
<br>
https://www.amazon.com.au/dp/B0DDLBYFJB
<br>
![image](https://github.com/user-attachments/assets/bf2ebf51-a250-4fb7-a3b0-ec1d87a9d7db)
<br>
Midi Featherwing:<br>
![midi_featherwing](https://github.com/user-attachments/assets/532d7d81-ae7e-485b-8d59-77ff6056e331)
<br>
Connect the RS485 adaptor's A and B terminals to the Waveshare board's RS485 "A" and "B" terminals, and the adaptor's TTL side to the Midi FeatherWing, the same way as for the Waveshare 4.3B.
<br>
Note: the RS485 port is half-duplex, so Midi thru is not supported on this platform.
