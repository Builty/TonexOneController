# Tonex One Controller: An open-source controller and display interface for the IK Multimedia Tonex One and Tonex pedal
# Waveshare 1.69/1.69 Touch
# Table of Contents
 1. [Connections](#connections)
 2. [Wired Footswitches - onboard](#footswitches)
 3. [Wired Footswitches - external](#footswitches_ext)
 4. [Wired Midi](#midi)
 5. [Cases](#cases)
 6. [9 volt Power for 5 volt models](#9v_power)

This hardware platform uses this Waveshare ESP32-S3 1.69" LCD board.
[https://www.waveshare.com/esp32-s3-lcd-1.69.htm](https://www.waveshare.com/esp32-s3-lcd-1.69.htm)
<br><br>
<img width="329" height="385" alt="image" src="https://github.com/user-attachments/assets/07c819e9-1271-429c-8c21-cab7750ffff4" />
<br><br>
**Important note**: Waveshare have a V1 and a V2 PCB. The V2 has a small sticker on the USB-C port.
<br>The V2 works very well, but on the V1, due to an error in Waveshare's design, the onboard Buzzer will make some noise.
<br>It is recommended to check with your supplier if the board has the V2 sticker, and try to only purchase this V2 version.
![image](https://github.com/user-attachments/assets/12c0c7ca-7d92-4596-969f-53fc22a1ddf0)
<br>
If you have already purchased and received a V1 (no V2 sticker on the USB-C port) then there are two options to address the buzzer noise:
- Cover the small hole on the buzzer. It is square plastic component next to the USB-C port. Cover the hole with tape, or a small amount of glue or similar. This should reduce the noise, but may still be audible
- Using the below diagram, remove the resistor shown using a soldering iron. Note that this will void the PCB warranty, but being so cheap the return postage cost for a warranty claim would probably cost more than the PCB anyway. This modification will completely disable the buzzer permanently and eliminate all noise from it
![Waveshare_169_V1_mod](https://github.com/user-attachments/assets/bd9aac38-cc4c-44a8-8f61-732791c53abc)

This module is low cost (around US$16) and supports an LCD display, about the same size as an Apple Watch. It requires a 5 volt DC power supply.
Caution: do not directly connect a pedalboard 9v! If you do, you will probably blow up both the PCB and your Tonex One!<br>
If you do wish to use 9v power, refer to [9 volt Power for 5 volt models](#9v_power)
<br>
Waveshare makes this board both with and without a touch screen. They are almost the same, except that the Touch version doesn't support footswitch four.
<br>
## Connections<a name="connections"></a>
Note: the controller code relies on the Tonex One pedal being set to Stomp mode. Code is in place to do this automatically.
- Solder a DC jack to the PCB, as shown below. Note the positive and negative polarity must match your power supply
- Connect the USB-C port on the Waveshare board to the ToneX One USB-C port
- Connect 5 volts DC to the power input jack that you soldered in the first step
- Switch on the power supply
- The Waveshare board USB port will power the Tonex One
![wiring_waveshare_169](https://github.com/user-attachments/assets/104c7423-a844-4c03-878e-3543ede0bc2d)

<br><br>
## Wired Footswitches (onboard) <a name="footswitches"></a>
Wired footswitches can optionally be used. These "onboard" switches connect directly to the controller with out needing any additional circuitry.<br>
The footswitch must be a "momentary" type that is only has its contacts closed when it is pressed.
The common pin of the footswitch must connect to the Controller ground pin, and the other wires connected as shown.
<br><br>
For the Waveshare 1.69" Touch, a maximum of 3 footswitches are supported. For the 1.69" (no touch) 4 switches are supported.
<br>
Various modes are supported, set using the web configuration:
- 2 switches, doing Next/Previous preset
- 4 switches, doing banked switching (just like the M-vave Chocolate pedal does)
- 4 switches, doing direct preset selection via binary (intended for relay control)<br>

![footswitches_waveshare_169](https://github.com/user-attachments/assets/93c0014e-db42-483d-9508-44a4478d2f75)

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
![external_waveshare_169](https://github.com/user-attachments/assets/3feb5ce3-fa6e-42c6-ac09-b5c4db784d1f)


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

Waveshare 1.69" to Midi Featherwing:<br>
![midi_waveshare_169](https://github.com/user-attachments/assets/f9c58088-8730-4ef9-908b-d7cda44d5c9c)

## Cases <a name="cases"></a>
With the Zero 1.69" boards being a bare PCB, a case of some type is useful to protect it. Here are some links to 3D printed options.
<br>
### Waveshare 1.69"
Community member "AlmaMaterFL" designed this one:
https://www.printables.com/model/1114384-esp32-s3-169inch-case
![image](https://github.com/user-attachments/assets/b09e51fb-da3f-41dd-a42f-305c141e3812)
![image](https://github.com/user-attachments/assets/0cd594f5-fb4c-4e16-bc39-dd08e65308cc)
<br><br>
Community member "bauerbyter" designed this one:
https://www.tinkercad.com/things/28eY73uKDzx-copy-of-tonex-one-controller?sharecode=IBZcHWdWU0IU3KJacFaUorztHQgdARKkLArXdZe-xvc
<br>
![image](https://github.com/user-attachments/assets/3a248b85-a766-4aba-8da9-e19d037890da)
<br><br>
It fits together with this part:
https://www.thingiverse.com/thing:6715828
<br><br>
![image](https://github.com/user-attachments/assets/f2271541-03e6-440f-91ce-e01776bbc3b7)

<br><br>
## 9 volt Power for 5 volt Models<a name="9v_power"></a>
The Zero boards has a maximum of 5 volts input.
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
