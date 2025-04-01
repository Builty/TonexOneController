# Tonex One Controller: An open-source controller and display interface for the IK Multimedia Tonex One guitar pedal
# Hardware Platforms and Wiring Details
# Table of Contents
 1. [Hardware Platform 1 Waveshare 4.3B](#waveshare_43B)
 2. [Hardware Platform 2 Waveshare Zero](#waveshare_zero)
 3. [Hardware Platform 3 Waveshare 1.69](#waveshare_169)
 4. [Hardware Platform 4 Espressif DevKit-C](#esp_devkitc)
 5. [Hardware Platform 5 M5Stack Atom S3R](#m5stack_atoms3r)
 6. [Wired Footswitches - onboard](#footswitches)
 7. [Wired Footswitches - external](#footswitches_ext)
 8. [Wired Midi](#midi)
 9. [Cases](#cases)
 10. [9 volt Power for 5 volt models](#9v_power)

Four hardware platforms are supported. Other ESP32-S3 platforms could be supported but would require code changes. 
<br>
They would need to meet the minimum requirements:
- Minimum SPI Flash size: 8 MB with display, 4 MB without
- Minimum PSRAM size: 2 MB
- USB OTG port

## Hardware Platform 1: Waveshare 4.3" B LCD board (Display supported) <a name="waveshare_43B"></a>
This hardware platform uses this Waveshare 4.3" B LCD board.
https://www.waveshare.com/product/esp32-s3-touch-lcd-4.3b.htm?sku=28141

**Important note:** Waveshare have two very similar boards:
- ESP32-S3-4.3: 2 USB-C ports. 5 volt power input. This board is **NOT RECOMMENDED** but can be made to work with a board modification (remove R19)
- ESP32-S3-4.3**B**: 1 USB-C port and a terminal block for 9v power input. This is the recommended board and works without modification

This module provides the microcontroller, power input suitable for 9v DC pedal board use, LCD screen, capacitive touch screen, and dual isolated inputs suitable for momentary foot switches.

### Connections
Note: the controller code relies on the Tonex One pedal being set to Stomp mode. Code is in place to do this automatically.
- Connect the USB-C port on the Waveshare board to the ToneX One USB-C port
- Optional: connect dual footswitches to the isolated inputs on the Waveshare board. GND to ground. DI0 for footswitch 1. DI1 for footwitch 2. Exact wiring depends on the footswitch but is usually a 6.5mm stereo jack
- Connect 9V DC power supply to the terminals on the Waveshare board. The terminals are screw terminals, so most likely a DC jack to wires will be needed.
- Switch on the power supply
- The Waveshare board USB port will power the Tonex One. Some users have tried powering the Tonex from both USB and 9 volts at the same time, however I do not know if this is recommended by IK Muliemedia
- Optional: for the Bluetooth Client version of code, switch on a M-Vave Chocolate Midi pedal (https://www.cuvave.com/productinfo/724103.html). After a few seconds it should connect and the Bluetooth icon should change from gray to blue
- Optional: for the Bluetooth Server version of code, the controller will be available as a peripheral for you to connect to via a Bluetooth Midi device. The Bluetooth icon should change from gray to blue when connected.

![wiring_waveshare_display](https://github.com/user-attachments/assets/f5edb5a8-f0d5-4b56-b29a-38f75dfab98c)
<br><br>

# Hardware Platform 2: Waveshare ESP32-S3 Zero (no Display support) <a name="waveshare_zero"></a>
This hardware platform uses this Waveshare Zero board.
[https://www.waveshare.com/product/esp32-s3-zero.htm](https://www.waveshare.com/product/esp32-s3-zero.htm)

**Ensure its the ESP32-S3FH4R2 with USB-C port. There are some similar boards with no USB-C or slightly different processor**
This module is very low cost (around US$6) and does not support an LCD display. It requires a 5 volt DC power supply.
Caution: do not directly connect a pedalboard 9v! If you do, you will probably blow up both the PCB and your Tonex One!<br>
If you do wish to use 9v power, refer to [9 volt Power for Zero and 1.69](#9v_power)

### Connections
Note: the controller code relies on the Tonex One pedal being set to Stomp mode. Code is in place to do this automatically.
- Solder a DC jack to the PCB, as shown below. Note the positive and negative polarity must match your power supply
- Connect the USB-C port on the Waveshare board to the ToneX One USB-C port
- Connect 5 volts DC to the power input jack that you soldered in the first step
- Switch on the power supply
- The Waveshare board USB port will power the Tonex One. Do not connect 9 volts to it!
- Optional: for the Bluetooth Client version of code, switch on a M-Vave Chocolate Midi pedal (https://www.cuvave.com/productinfo/724103.html). After a few seconds it should connect and the Bluetooth icon should change from gray to blue
- Optional: for the Bluetooth Server version of code, the controller will be available as a peripheral for you to connect to via a Bluetooth Midi device. The Bluetooth icon should change from gray to blue when connected.

![wiring_waveshare_zero](https://github.com/user-attachments/assets/3e439e78-9e7d-4b95-9563-c71eceb17eb9)
<br><br>


# Hardware Platform 3: Waveshare ESP32-S3 1.69" Display board (no touch) and Touch<a name="waveshare_169"></a>
This hardware platform uses this Waveshare ESP32-S3 1.69" LCD board.
[https://www.waveshare.com/esp32-s3-lcd-1.69.htm](https://www.waveshare.com/esp32-s3-lcd-1.69.htm)

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
If you do wish to use 9v power, refer to [9 volt Power for Zero and 1.69](#9v_power)
<br>
Waveshare makes this board both with and without a touch screen. They are almost the same, except that the Touch version doesn't support footswitch four.
### Connections
Note: the controller code relies on the Tonex One pedal being set to Stomp mode. Code is in place to do this automatically.
- Solder a DC jack to the PCB, as shown below. Note the positive and negative polarity must match your power supply
- Connect the USB-C port on the Waveshare board to the ToneX One USB-C port
- Connect 5 volts DC to the power input jack that you soldered in the first step
- Switch on the power supply
- The Waveshare board USB port will power the Tonex One. Some users have tried powering the Tonex from both USB and 9 volts at the same time, however I do not know if this is recommended by IK Muliemedia
![wiring_waveshare_169](https://github.com/user-attachments/assets/104c7423-a844-4c03-878e-3543ede0bc2d)

# Hardware Platform 4: Espressif ESP32-S3 DevKit-C (no Display support) <a name="esp_devkitc"></a>
This hardware platform uses the Espressif ESP32-S3 Devkit-C board (8MB flash, 2 MB PSRAM version.) 

This module is low cost and does not support an LCD display. It requires a 5 volt DC power supply.
Caution: do not directly connect a pedalboard 9v! If you do, you will probably blow up both the PCB and your Tonex One!<br>
If you do wish to use 9v power, refer to [9 volt Power for Zero, 1.69 and Devkit-C](#9v_power)

### Connections
Note: the controller code relies on the Tonex One pedal being set to Stomp mode. Code is in place to do this automatically.
- Solder a DC jack to the PCB, as shown below. Note the positive and negative polarity must match your power supply
- Connect the OTG USB-C port on the board to the ToneX One USB-C port
- Connect 5 volts DC to the power input jack that you soldered in the first step
- Switch on the power supply
- The board USB port will power the Tonex One. Do not connect 9 volts to it!
- Optional: for the Bluetooth Client version of code, switch on a M-Vave Chocolate Midi pedal (https://www.cuvave.com/productinfo/724103.html). After a few seconds it should connect and the Bluetooth icon should change from gray to blue
- Optional: for the Bluetooth Server version of code, the controller will be available as a peripheral for you to connect to via a Bluetooth Midi device. The Bluetooth icon should change from gray to blue when connected.
![wiring_devkitc](https://github.com/user-attachments/assets/0cf7c7f0-d597-41b4-967e-2e2ea810b26d)
<br><br>

# Hardware Platform 5: M5Stack Atom S3R <a name="m5stack_atoms3r"></a>
This hardware platform uses the M5Stack Atom S3R board (8MB flash, 8 MB PSRAM version.) 

This module is low cost, supports a tiny LCD display, and comes in a case. It requires a 5 volt DC power supply.
Caution: do not directly connect a pedalboard 9v! If you do, you will probably blow up both the PCB and your Tonex One!<br>
If you do wish to use 9v power, refer to [9 volt Power for Zero, 1.69 and Devkit-C](#9v_power)

### Connections
Note: the controller code relies on the Tonex One pedal being set to Stomp mode. Code is in place to do this automatically.
- Connect a DC jack to the PCB via the 4 pin connector, as shown below. Note the positive and negative polarity must match your power supply
- Connect the OTG USB-C port on the board to the ToneX One USB-C port
- Connect 5 volts DC to the power input jack that you connected in the first step
- Switch on the power supply
- The board USB port will power the Tonex One. Some users have tried powering the Tonex from both USB and 9 volts at the same time, however I do not know if this is recommended by IK Muliemedia
- Optional: for the Bluetooth Client version of code, switch on a M-Vave Chocolate Midi pedal (https://www.cuvave.com/productinfo/724103.html). After a few seconds it should connect and the Bluetooth icon should change from gray to blue
- Optional: for the Bluetooth Server version of code, the controller will be available as a peripheral for you to connect to via a Bluetooth Midi device. The Bluetooth icon should change from gray to blue when connected.
![wiring_atom_s3r](https://github.com/user-attachments/assets/0829c254-23bb-4ef6-8695-c5a8d363d817)

<br><br>

<br><br>
## Wired Footswitches (onboard) <a name="footswitches"></a>
Wired footswitches can optionally be used. These "onboard" switches connect directly to the controller with out needing any additional circuitry.<br>
The footswitch must be a "momentary" type that is only has its contacts closed when it is pressed.
The common pin of the footswitch must connect to the Controller ground pin, and the other wires connected as shown.
<br><br>
For the Waveshare 4.3B, a maximum of 2 footswitches are supported, always in next/previous preset mode.<br>
For the Waveshare 1.69" Touch, a maximum of 3 footswitches are supported.
<br>
For the other platforms, with firmware version 1.0.5.2 or above, three modes are supported, set using the web configuration, to one of:
- 2 switches, doing Next/Previous preset
- 4 switches, doing banked switching (just like the M-vave Chocolate pedal does)
- 4 switches, doing direct preset selection via binary (intended for relay control)<br>

![footswitches_waveshare_display](https://github.com/user-attachments/assets/0016e19f-eb87-4d31-ba1c-2b210b559e99)
![footswitches_waveshare_zero](https://github.com/user-attachments/assets/7f1110cc-6b27-4317-af04-880c098b839e)
![footswitches_waveshare_169](https://github.com/user-attachments/assets/93c0014e-db42-483d-9508-44a4478d2f75)
![footswitches_devkitc](https://github.com/user-attachments/assets/bbeb5898-8cb4-49ca-80a5-eb6f4dedb8fb)
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
![external_waveshare_display](https://github.com/user-attachments/assets/0cfaa971-6afc-41d9-9ee7-74fc7572e22d)
![external_waveshare_zero](https://github.com/user-attachments/assets/dfcdd4a2-199f-4045-a01f-b32459fd3f50)
![external_waveshare_169](https://github.com/user-attachments/assets/3feb5ce3-fa6e-42c6-ac09-b5c4db784d1f)
![external_devkitc](https://github.com/user-attachments/assets/d05c903a-fd8a-4bae-86be-006db2a4abfc)
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
The Waveshare Zero, 1.69" LCD, and Atom S3R boards can directly connect to the Midi Featherwing PCB.<br>
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

Waveshare Zero to Midi Featherwing:<br>
![midi_waveshare_zero](https://github.com/user-attachments/assets/8b49dc76-28e3-4bfd-9a68-63ca2e453aa0)

Waveshare 1.69" to Midi Featherwing:<br>
![midi_waveshare_169](https://github.com/user-attachments/assets/f9c58088-8730-4ef9-908b-d7cda44d5c9c)

Espressif Devkit-C to Midi Featherwing:<br>
![midi_devkitc](https://github.com/user-attachments/assets/8dde8924-ea4e-435f-b1ed-c2cb99568b2e)

Waveshare 4.3B to Midi Featherwing via the TTL to RS485 adaptor:<br>
![midi_waveshare_43b](https://github.com/user-attachments/assets/61f27686-6097-4534-b7a8-9f42f3c1282c)

M5Stack Atom S3R to Midi Featherwing:<br>
![midi_atoms3r](https://github.com/user-attachments/assets/1663b487-5ce2-44ac-8cf8-6fb71c7622a3)

## Cases <a name="cases"></a>
With the Zero and 1.69" boards being bare PCBs, a case of some type is useful to protect it. Here are some links to 3D printed options.
<br>
### Waveshare Zero
Community member "AlmaMaterFL" designed this one:
https://www.printables.com/model/1110479-esp32-s3-zero-m-case-pin-version
![image](https://github.com/user-attachments/assets/57f8d28c-1519-48c2-a8f1-efc36a1a4ada)
![image](https://github.com/user-attachments/assets/5079b8be-012a-4d2d-86a6-2c4f35423682)
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
The 4.3B model can accept the standard 9 volt pedalboard power, however the Zero, the 1.69", the Devkit-C, and the Atom S3R boards are a maximum of 5 volts input.
<br>It is still possible however to run them from a 9 volt power supply, with the additional of another low cost off-the-shelf PCB.
<br>**Caution:** This section requires some more advanced skills, such as using a multimeter to measure voltage. Incorrect voltage setting or polarity could cause damage to the PCB and/or your Tonex pedal.
<br><br>Various electronic shops, and also suppliers like Amazon, often have low cost "switching regulators." These are a compact circuit that can convert the 9 volt pedalboard power down to the 5 volts required by the Zero and the 1.69. Sample photos are shown below.<br>
Some of these may be a fixed voltage, in which case you must select one with a 5 volt output. Most models however are adjustable, using a small "trimpot."
<br>For the adjustable types, it is necessary to set it to 5 volts output **before** connecting to the controller PCB.
<br>
- Connect the 9 volt input to the input terminals on the voltage regulator PCB. Ensure the positive and negative the right way around. The standard for pedal boards is usually negative to the centre pin of the DC jack, but this should be checked
- Set the multimeter to measure DC voltage, then connect the multimeter probes to the voltage regulator output terminals
- Adjust the trimpot on the voltage regulator PCB to achieve close to 5 volts. I doesn't have to be exactly 5 volts, but should be in the range of 4.95v to 5.05v
- Once this has been achieved, connect the voltage regulator output terminals to the Waveshare Zero, 1.69", Devkit-C, or Atom S3R board, in the same locations as shown in the prior wiring diagrams
- Keep the Tonex pedal disconnected, and power on the board. Check that it boots up and runs normally
- Once this test has passed, then you can connect the Tonex pedal
![image](https://github.com/user-attachments/assets/e59673c5-f741-4516-b471-5af0eb685d12)
![image](https://github.com/user-attachments/assets/472394d5-a2c9-492d-909c-792480abcb4c)
