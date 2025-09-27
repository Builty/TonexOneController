# Tonex One Controller: An open-source controller and display interface for the IK Multimedia Tonex One guitar pedal
# M5Stack Atom S3R
# Table of Contents
 1. [Connections](#connections)
 2. [Wired Footswitches - onboard](#footswitches)
 3. [Wired Footswitches - external](#footswitches_ext)
 4. [Wired Midi](#midi)
 5. [9 volt Power for 5 volt models](#9v_power)

This hardware platform uses the M5Stack Atom S3R board (8MB flash, 8 MB PSRAM version.) 
<br>
https://shop.m5stack.com/products/atoms3r-dev-kit
<br><br>
<img width="320" height="323" alt="image" src="https://github.com/user-attachments/assets/b02bb831-21ab-4c67-9550-0e0bcce15e7c" />
<br><br>
This module is low cost, supports a tiny LCD display, and comes in a case. It requires a 5 volt DC power supply.
Caution: do not directly connect a pedalboard 9v! If you do, you will probably blow up both the PCB and your Tonex One!<br>
If you do wish to use 9v power, refer to [9 volt Power for 5 volt models](#9v_power)

## Connections <a name="connections"></a>
Note: the controller code relies on the Tonex One pedal being set to Stomp mode. Code is in place to do this automatically.
- Connect a DC jack to the PCB via the 4 pin connector, as shown below. Note the positive and negative polarity must match your power supply
- Connect the OTG USB-C port on the board to the ToneX One USB-C port
- Connect 5 volts DC to the power input jack that you connected in the first step
- Switch on the power supply
- The board USB port will power the Tonex One
- Optional: for the Bluetooth Client version of code, switch on a M-Vave Chocolate Midi pedal (https://www.cuvave.com/productinfo/724103.html). After a few seconds it should connect and the Bluetooth icon should change from gray to blue
- Optional: for the Bluetooth Server version of code, the controller will be available as a peripheral for you to connect to via a Bluetooth Midi device. The Bluetooth icon should change from gray to blue when connected.
![wiring_atom_s3r](https://github.com/user-attachments/assets/0829c254-23bb-4ef6-8695-c5a8d363d817)

<br><br>
## Wired Footswitches (onboard) <a name="footswitches"></a>
Wired footswitches can optionally be used. These "onboard" switches connect directly to the controller with out needing any additional circuitry.<br>
The footswitch must be a "momentary" type that is only has its contacts closed when it is pressed.
The common pin of the footswitch must connect to the Controller ground pin, and the other wires connected as shown.
<br><br>
A maximum of 4 footswitches are supported.
<br>
Various modes are supported, set using the web configuration:
- 2 switches, doing Next/Previous preset
- 4 switches, doing banked switching (just like the M-vave Chocolate pedal does)
- 4 switches, doing direct preset selection via binary (intended for relay control)<br>

![footswitches_atoms3r](https://github.com/user-attachments/assets/e3c227f4-f3ba-480b-a41d-3ab54e3966d5)

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
![external_atom_3sr](https://github.com/user-attachments/assets/2c5db1a0-b66a-4516-96fd-47d03f7526da)


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

M5Stack Atom S3R to Midi Featherwing:<br>
![midi_atoms3r](https://github.com/user-attachments/assets/1663b487-5ce2-44ac-8cf8-6fb71c7622a3)
<br><br>
## 9 volt Power for 5 volt Models<a name="9v_power"></a>
The Atom-S3R board has a maximum of 5 volts input.
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

