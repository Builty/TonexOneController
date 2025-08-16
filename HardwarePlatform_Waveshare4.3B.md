# Tonex One Controller: An open-source controller and display interface for the IK Multimedia Tonex One and Tonex Pedal
# Waveshare 4.3B Hardware Platform and Wiring Details
# Table of Contents
 1. [Connections](#connections)
 2. [Wired Footswitches - onboard](#footswitches)
 3. [Wired Footswitches - external](#footswitches_ext)
 4. [Wired Midi](#midi)

This hardware platform uses this Waveshare 4.3" B LCD board.
https://www.waveshare.com/product/esp32-s3-touch-lcd-4.3b.htm?sku=28141

**Important note:** Waveshare have two very similar boards:
- ESP32-S3-4.3: 2 USB-C ports. 5 volt power input. This board is **NOT RECOMMENDED** but can be made to work with a board modification (remove R19)
- ESP32-S3-4.3**B**: 1 USB-C port and a terminal block for 9v power input. This is the recommended board and works without modification

This module provides the microcontroller, power input suitable for 9v DC pedal board use, LCD screen, capacitive touch screen, and dual isolated inputs suitable for momentary foot switches.

## Connections <a name="connections"></a>
Note: the controller code relies on the Tonex One pedal being set to Stomp mode. Code is in place to do this automatically.
- Connect the USB-C port on the Waveshare board to the ToneX One USB-C port
- Optional: connect dual footswitches to the isolated inputs on the Waveshare board. GND to ground. DI0 for footswitch 1. DI1 for footwitch 2. Exact wiring depends on the footswitch but is usually a 6.5mm stereo jack
- Connect 9V DC power supply to the terminals on the Waveshare board. The terminals are screw terminals, so most likely a DC jack to wires will be needed.
- Switch on the power supply
- The Waveshare board USB port will power the Tonex One
- Optional: for the Bluetooth Client version of code, switch on a M-Vave Chocolate Midi pedal (https://www.cuvave.com/productinfo/724103.html). After a few seconds it should connect and the Bluetooth icon should change from gray to blue
- Optional: for the Bluetooth Server version of code, the controller will be available as a peripheral for you to connect to via a Bluetooth Midi device. The Bluetooth icon should change from gray to blue when connected.

![wiring_waveshare_display](https://github.com/user-attachments/assets/f5edb5a8-f0d5-4b56-b29a-38f75dfab98c)
<br><br>

<br><br>
## Wired Footswitches (onboard) <a name="footswitches"></a>
Wired footswitches can optionally be used. These "onboard" switches connect directly to the controller with out needing any additional circuitry.<br>
The footswitch must be a "momentary" type that is only has its contacts closed when it is pressed.
The common pin of the footswitch must connect to the Controller ground pin, and the other wires connected as shown.
<br><br>
For the Waveshare 4.3B, a maximum of 2 footswitches are supported, always in next/previous preset mode.<br>

![footswitches_waveshare_display](https://github.com/user-attachments/assets/0016e19f-eb87-4d31-ba1c-2b210b559e99)

<br><br>
## Wired Footswitches (external) <a name="footswitches_ext"></a>
Starting from firmware version 1.0.8.2, with the use of an additional PCB, up to 16 footswitches can be connected.<br> 
The footswitch must be a "momentary" type that is only has its contacts closed when it is pressed.
<br><br>
The additional PCB must use the "SX1509" chip. The recommeded one is the Sparkfun SX1509 breakout board:
https://www.sparkfun.com/sparkfun-16-output-i-o-expander-breakout-sx1509.html
<br>
![image](https://github.com/user-attachments/assets/0575f0a0-1eb3-4aef-a7e2-c321876f7ed0)

NOTE: other types of IO expander boards that use different chips are not supported and will not function. It must contain the SX1509 chip.

### Address Setting 
The SX1509 PCB has a selectable address system. This must be set correctly in order for the board to function with the controller.
- Using a sharp knife, carefully cut the thin tracks between the pads as shown below
- Use solder to bridge the pads as shown below
- This sets the SX1509 adress to "11" which avoids conflicting with other parts on the controller boards
![sx1509_address](https://github.com/user-attachments/assets/84030e88-f6e7-4fc0-8230-3fff6dc1235a)


The common pin of each footswitch must connect to the SX1509 ground pins. The labels "0", "1" etc are the individual switch inputs. Footswitch 1 connected to input 0. Footswitch 2 to input 1 etc.
<br><br>
Multiple modes are supported, configured using the web configuration.
![external_waveshare_display](https://github.com/user-attachments/assets/0cfaa971-6afc-41d9-9ee7-74fc7572e22d)


## Wired Midi (firmware version V1.0.4.1 or above required) <a name="midi"></a>
Note: Wired Midi is disabled by default. If it is enabled without the proper hardware (detailed below) being fitted, you may get "phantom" preset changes, due to the serial input "floating".
Only enable wired Midi when hardware is connected!<br>
Refer here for details on how to enable it, and set the Midi channel:
https://github.com/Builty/TonexOneController/blob/main/WebConfiguration.md
<br><br>
Wired Midi is supported on all platforms. A extra PCB is required for all platforms, an "Adafruit Midi FeatherWing kit", for US$7.<br> 
https://www.adafruit.com/product/4740 <br>
This Midi board supports both 5-pin DIN sockets (included in kit) and 3.5mm jacks (not included in kit.)<br>
<br>
The Waveshare 4.3B, due to hardware limitations, requires another small interface. This a common, low cost "TTL to RS485" adaptor.<br>
Typical examples of this PCB:
https://www.amazon.com/HiLetgo-Reciprocal-Hardware-Automatic-Converter/dp/B082Y19KV9 
<br>
https://www.amazon.com.au/dp/B0DDLBYFJB
<br>
![image](https://github.com/user-attachments/assets/bf2ebf51-a250-4fb7-a3b0-ec1d87a9d7db)
<br>
Midi Featherwing:<br>
![midi_featherwing](https://github.com/user-attachments/assets/532d7d81-ae7e-485b-8d59-77ff6056e331)

Waveshare 4.3B to Midi Featherwing via the TTL to RS485 adaptor:<br>
![midi_waveshare_43b](https://github.com/user-attachments/assets/61f27686-6097-4534-b7a8-9f42f3c1282c)
