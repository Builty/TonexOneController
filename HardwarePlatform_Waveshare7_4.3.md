# Tonex One Controller: An open-source controller and display interface for the IK Multimedia Tonex One and Tonex Pedal
# Waveshare 7" / 4.3 (not B) Hardware Platform and Wiring Details
# Table of Contents
 1. [Hardware Mod](#mod)
 1. [Connections](#connections)
 2. [Wired Footswitches not supported - onboard](#footswitches)
 3. [Wired Footswitches - external](#footswitches_ext)
 4. [Wired Midi](#midi)

This hardware platform uses this Waveshare 7" or 4.3" (not B) LCD board.
<br><br>
**IMPORTANT NOTE:** these two Waveshare boards can be made to work, but they are less featured than the other platforms and require some more advanced skills to make them work. This info is here for those that insist on using it. It is strongly recommended to use a different platform (like the 4.3B) for the full set of features.
<br>
Feature Limitations:
- Wired footswitched not supported
- Limited number of Amp skins due to 8 MB flash (4.3B has 16 MB flash)
- Wired Midi is input only, not support for Midi output/thru
<br>
https://www.waveshare.com/esp32-s3-touch-lcd-7.htm?sku=27078
<br>
https://www.waveshare.com/esp32-s3-touch-lcd-4.3.htm?sku=25948
<br><br>
<img width="469" height="318" alt="image" src="https://github.com/user-attachments/assets/f2f6ec2e-ada4-4da3-b839-0d234506f14d" />
<br>
<img width="663" height="474" alt="image" src="https://github.com/user-attachments/assets/29c69c15-61eb-4ea6-a276-03f1aee8abe0" />
<br>
<img width="795" height="482" alt="image" src="https://github.com/user-attachments/assets/f72f7fb0-c5f9-4532-9376-900d975bf3fa" />
<br><br>
These modules provide the microcontroller, dual USB ports (one for power input, one for the pedal), LCD display, and a capacitive touch screen.

## Hardware Mod <a name="mod"></a>
USB Host support on these boards requires a small modification. 
- 4.3": Resistor R19 needs to be removed.
<img width="800" height="607" alt="image" src="https://github.com/user-attachments/assets/4de871f8-b3e1-4bba-a078-97a706bc74f0" />
<br>
<br>
- 7": Resistor R115 needs to be removed.
<img width="800" height="544" alt="image" src="https://github.com/user-attachments/assets/92dd23c3-b3c4-4b95-9abc-fd0e651846bf" />
<br>

## Connections <a name="connections"></a>
Note: the controller code relies on the Tonex One pedal being set to Stomp mode. Code is in place to do this automatically.
- Connect the first USB-C port (marked as USB) on the Waveshare board to the ToneX One USB-C port
- Connect the second USB-C port (marked as UART) to a USB-C 5 volt power supply.
- Switch on the power supply
- The Waveshare board USB port will power the Tonex One
- Optional: for the Bluetooth Client version of code, switch on a M-Vave Chocolate Midi pedal (https://www.cuvave.com/productinfo/724103.html). After a few seconds it should connect and the Bluetooth icon should change from gray to blue
- Optional: for the Bluetooth Server version of code, the controller will be available as a peripheral for you to connect to via a Bluetooth Midi device. The Bluetooth icon should change from gray to blue when connected.

<img width="1251" height="930" alt="image" src="https://github.com/user-attachments/assets/319b3f67-869c-48a2-8530-5bf8605cb68b" />
<br><br>

## Wired Footswitches not supported (onboard) <a name="footswitches"></a>
Directly wired footswitches are not supported on these boards due to a lack on available input pins.
<br><br>

## Wired Footswitches (external) <a name="footswitches_ext"></a>
Starting from firmware version 1.0.8.2, with the use of an additional PCB, up to 16 footswitches can be connected.<br> 
The footswitch must be a "momentary" type that is only has its contacts closed when it is pressed.
<br><br>
The additional PCB must use the "SX1509" chip. The recommeded one is the Sparkfun SX1509 breakout board:
https://www.sparkfun.com/sparkfun-16-output-i-o-expander-breakout-sx1509.html
<br>

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
<img width="1286" height="1002" alt="external_waveshare_7" src="https://github.com/user-attachments/assets/1051697c-d963-41a4-9624-2851df45c4a4" />


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
Midi Featherwing:<br>
![midi_featherwing](https://github.com/user-attachments/assets/532d7d81-ae7e-485b-8d59-77ff6056e331)

<img width="1286" height="1002" alt="midi_waveshare_7" src="https://github.com/user-attachments/assets/6ea417e4-ef08-49a3-89c1-48d07a9c9b21" />

