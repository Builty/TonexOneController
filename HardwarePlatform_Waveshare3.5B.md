# Tonex One Controller: An open-source controller and display interface for the IK Multimedia Tonex One and Tonex Pedal
# Waveshare 3.5B Hardware Platform and Wiring Details
# Table of Contents
 1. [Connections](#connections)
 2. [Wired Footswitches - onboard](#footswitches)
 3. [Wired Footswitches - external](#footswitches_ext)
 4. [Wired Midi](#midi)
 5. [9 volt Power for 5 volt models](#9v_power)

This hardware platform uses this Waveshare 3.5B Touch LCD board.
https://www.waveshare.com/product/esp32-s3-touch-lcd-3.5b.htm?sku=31334
<br><br>
<img width="766" height="503" alt="image" src="https://github.com/user-attachments/assets/54648243-ff98-4e42-8070-595b46c5d8ce" />
<br><br>
<img width="653" height="443" alt="image" src="https://github.com/user-attachments/assets/3aff346b-656e-44a4-aad6-2b5c77bd3f6a" />
<br>
<img width="657" height="445" alt="image" src="https://github.com/user-attachments/assets/2c875e18-cdaa-4ca8-b7e3-bf95c980b89c" />
<br><br>
**Important note:** Waveshare have two very similar boards:
- ESP32-S3-3.5: This board is **NOT COMPATIBLE** 
- ESP32-S3-3.5**B**: This is the supported version
<br>
This module provides the microcontroller, power input suitable for 5v DC pedal board use, and LCD screen with capacitive touch.

## Connections <a name="connections"></a>
Note: the controller code relies on the Tonex One pedal being set to Stomp mode. Code is in place to do this automatically.
- Solder a DC jack to the PCB, as shown below. Note the positive and negative polarity must match your power supply
- Connect the USB-C port on the Waveshare board to the ToneX One USB-C port
- Connect 5 volts DC to the power input jack that you soldered in the first step
- Switch on the power supply
- The Waveshare board USB port will power the Tonex One
<img width="1286" height="1002" alt="wiring_waveshare_35b" src="https://github.com/user-attachments/assets/e038a3ef-a979-440c-8bad-d896b43521f4" />
<br><br>

<br><br>
## Wired Footswitches (onboard) <a name="footswitches"></a>
Wired footswitches can optionally be used. These "onboard" switches connect directly to the controller with out needing any additional circuitry.<br>
The footswitch must be a "momentary" type that is only has its contacts closed when it is pressed.
The common pin of the footswitch must connect to the Controller ground pin, and the other wires connected as shown.
<br><br>
For the Waveshare 3.5B, a maximum of 4 footswitches are supported.<br>
<img width="1286" height="1002" alt="footswitches_waveshare_35b" src="https://github.com/user-attachments/assets/2db34327-8f84-4165-8de9-8568eda2d7f4" />
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
<img width="1286" height="1002" alt="external_waveshare_35b" src="https://github.com/user-attachments/assets/b41df9c1-a013-4ab5-a47c-96d3bce64b41" />

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
<img width="1286" height="1002" alt="midi_waveshare_35b" src="https://github.com/user-attachments/assets/0e6da5e6-9c53-4e97-a9a2-620a34e10e9e" />

<br><br>
## 9 volt Power for 5 volt Models<a name="9v_power"></a>
The Waveshare 3.5" board has a maximum of 5 volts input.
<br>It is still possible however to run it from a 9 volt power supply, with the additional of another low cost off-the-shelf PCB.
<br>**Caution:** This section requires some more advanced skills, such as using a multimeter to measure voltage. Incorrect voltage setting or polarity could cause damage to the PCB and/or your Tonex pedal.
<br><br>Various electronic shops, and also suppliers like Amazon, often have low cost "switching regulators." These are a compact circuit that can convert the 9 volt pedalboard power down to the 5 volts required by the Zero and the 1.69. Sample photos are shown below.<br>
Some of these may be a fixed voltage, in which case you must select one with a 5 volt output. Most models however are adjustable, using a small "trimpot."
<br>For the adjustable types, it is necessary to set it to 5 volts output **before** connecting to the controller PCB.
<br>
- Connect the 9 volt input to the input terminals on the voltage regulator PCB. Ensure the positive and negative the right way around. The standard for pedal boards is usually negative to the centre pin of the DC jack, but this should be checked
- Set the multimeter to measure DC voltage, then connect the multimeter probes to the voltage regulator output terminals
- Adjust the trimpot on the voltage regulator PCB to achieve close to 5 volts. I doesn't have to be exactly 5 volts, but should be in the range of 4.95v to 5.05v
- Once this has been achieved, connect the voltage regulator output terminals to the board, in the same locations as shown in the prior wiring diagram
- Keep the Tonex pedal disconnected, and power on the board. Check that it boots up and runs normally
- Once this test has passed, then you can connect the Tonex pedal
![image](https://github.com/user-attachments/assets/e59673c5-f741-4516-b471-5af0eb685d12)
![image](https://github.com/user-attachments/assets/472394d5-a2c9-492d-909c-792480abcb4c)
