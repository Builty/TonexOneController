# Tonex One Controller: An open-source controller and display interface for the IK Multimedia Tonex One and Tonex Pedal
# Lilygo T-Display-S3 Hardware Platform and Wiring Details
# Table of Contents
 1. [Connections](#connections)
 2. [Wired Footswitches - onboard](#footswitches)
 3. [Wired Footswitches - external](#footswitches_ext)
 4. [Wired Midi](#midi)
 5. [9 volt Power for 5 volt models](#9v_power)

This hardware platform uses this Lilygo T-Display S3 1.9" Touch/No Touch LCD board.
<br>
https://lilygo.cc/products/t-display-s3
<br><br>
<img width="418" height="367" alt="image" src="https://github.com/user-attachments/assets/3e70fa44-563e-425a-ae01-6c1d3defa87b" />
<br><br>
This module provides the microcontroller, power input suitable for 5v DC pedal board use, and LCD screen with optional capacitive touch.

## Connections <a name="connections"></a>
Note: the controller code relies on the Tonex One pedal being set to Stomp mode. Code is in place to do this automatically.
- Unlike other platforms, this board does not provide power out of its USB-C port. 
- A USB-C splitter device is required, such as https://www.amazon.com.au/dp/B0DCBNGXKG
- The device connects the USB data pins from the Tonex to the Lilygo, and has an input socket. The 5v power from the input socket splits off to power both the Tonex and the Lilygo
<br><br>
<img width="1286" height="1002" alt="wiring_lilygo_tdisps3" src="https://github.com/user-attachments/assets/957cb9bf-6208-453f-ae7a-2554e80a3641" />

## Wired Footswitches (onboard) <a name="footswitches"></a>
Wired footswitches can optionally be used. These "onboard" switches connect directly to the controller with out needing any additional circuitry.<br>
The footswitch must be a "momentary" type that is only has its contacts closed when it is pressed.
The common pin of the footswitch must connect to the Controller ground pin, and the other wires connected as shown.
<br><br>
For the Liygo T-Display S3, a maximum of 4 footswitches are supported.<br>
<img width="1286" height="1002" alt="footswitches_lilygo_tdisps3" src="https://github.com/user-attachments/assets/51862aee-c7c3-46bf-b30e-69b35cd26a4b" />

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

<img width="1286" height="1002" alt="external_wiring_lilygo_tdisps3" src="https://github.com/user-attachments/assets/da21e1bf-ad45-493f-ba6d-dd8802573c84" />

## Wired Midi (firmware version V1.0.4.1 or above required) <a name="midi"></a>
Note: Wired Midi is disabled by default. If it is enabled without the proper hardware (detailed below) being fitted, you may get "phantom" preset changes, due to the serial input "floating".
Only enable wired Midi when hardware is connected!<br>
Refer here for details on how to enable it, and set the Midi channel:
https://github.com/Builty/TonexOneController/blob/main/WebConfiguration.md
<br><br>
An extra PCB is required for all platforms, an "Adafruit Midi FeatherWing kit", for US$7.<br> 
https://www.adafruit.com/product/4740 <br>
This Midi board supports both 5-pin DIN sockets (included in kit) and 3.5mm jacks (not included in kit.)<br>
<br>
![image](https://github.com/user-attachments/assets/bf2ebf51-a250-4fb7-a3b0-ec1d87a9d7db)
<br>
Midi Featherwing:<br>
![midi_featherwing](https://github.com/user-attachments/assets/532d7d81-ae7e-485b-8d59-77ff6056e331)

Lilygo D-Display S3 1.9" to Midi Featherwing:<br>
<img width="1286" height="1002" alt="midi_lilygo_tdisps3" src="https://github.com/user-attachments/assets/8ea58daf-ee60-47f4-aa76-3a1e2f910807" />
