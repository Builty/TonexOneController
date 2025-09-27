# Tonex One Controller: An open-source controller and display interface for the IK Multimedia Tonex One guitar pedal
This project uses a low-cost embedded controller (Espressif ESP32-S3) to form a bridge to the IK Multimedia Tonex One guitar pedal (which does not have native Midi capability.)

# WiFi/Web Control
New in firmware version V1.0.7.2 is the ability to change all of the pedal parameters, change presets, and change controller options, all from a web page.
<br>
<br>
![image](https://github.com/user-attachments/assets/84ee5aff-d9fe-46bc-8309-11099a25549a)

# Entering Web Control
For the first time, it is necessary to connect to the controller in Access Point mode. Once this has been done, the WiFi mode can be changed if desired.
<br>
To enter Web Control:
- Reboot the controller
- Within 60 seconds, use a phone or PC to connect to the WiFi device "TonexConfig"
- The password for the network is 12345678
- The controller will automatically supply a network address for your device (DHCP is supported)
- Note: some phones may attempt to use this network for Internet access, which will not be be available. Watch out for any messages asking you to confirm the connection
- Open a web browser on your device
- In the address bar of the web browser, enter "tonex.local" (without the quotation marks.) You should see the web config screen
- Once you have saved the settings (or if you don't want to change anything) you can close the web browser
<br>
You can also connect to the controller by scanning the below QR code using your phone camera or QR scanning app:
<br>
<br>
<img width="256" height="256" alt="TonexConfig-qrcode" src="https://github.com/user-attachments/assets/900b717c-2bc6-414c-aabf-d7855ac9cebb" />
<br>

# Changing Presets
The current preset can be changed by clicking on the white box beneath the title. A list will be shown, from 1 to 20. Select a number to select the preset.
<br>
![image](https://github.com/user-attachments/assets/d8edaf8c-3304-4ca8-8087-80c1bc6b600f)

# Changing Pedal Parameters
To change pedal parameters, select a category on the left side menu. All Tonex parameters are avalable to view and edit.
<br>
If a Pedal setting is changed via another method (e.g. using the controller LCD screen) the web page will automatically update and show the changes, without needing to refresh the page.

# Changing Settings
## Bluetooth Settings
The "BT" category on the left menu allows viewing and changing of the bluetooth options.<br>
### Bluetooth Mode
- Disabled: Bluetooth is totally disabled and non-functional
- Central (default): allows the controller to locate and connect to other peripherals, like the M-Vave Chocolate
- Peripheral: allows the controller to be discovered and connected to by other Central devices (like a Phone or a PC.)
### Device Enables
- Enable support for the M-Vave Chocolate and Chocolate Plus bluetooth footswitch controllers (default: on)
- Enable support for the X-Vive MD1 Midi bridge device (default: on)
- Enable support for some other Bluetooth Midi peripheral. Enter its device name, and check the checkbox to enable it (default: off)
### Bluetooth Midi CC
- Enable support for Control Change commands (e.g. enabling effects and changing parameters.)
This is disabled by default, as the MVave Chocolate pedal, when changing banks, sends a conflicting change that modifies the Tonex parameters.<br>
This setting should not be enabled with a Chocolate pedal that has the default configuration loaded!

![image](https://github.com/user-attachments/assets/536cde26-02de-43cb-8556-17f7b74b91c9)

## Midi Settings
Midi settings are available from the Midi category on the left menu.
- Enable support for wired Midi. CAUTION: this should only be enabled when the correct hardware has been connected. If not, random preset switching may occur!
- Midi channel: select the desired Midi channel to use with Wired Midi. 

![image](https://github.com/user-attachments/assets/27785718-9d60-4fa3-b114-345a6887faf2)

## Miscellaneous Settings
Miscellaneous settings are available from the Misc category on the left menu.
### Preset Twice Toggle
- If this setting is disabled (default), then setting the same preset index multiple times will not have any effect. <br>
- If this setting is enabled, then setting the same preset a second time will set the Tonex pedal to bypass mode. Setting it a third time will exit bypass mode.
This setting is most suited to use with Pedal models, where it could for example enable/disable an overdrive pedal
### Footswitch (onboard) Mode
This setting controls how directly wired (onboard) footswitches will function. Note this has nothing to do with Bluetooth footswitch pedals, or the externally connecxted footswitches.
- Dual Next/Previous: 2 footswitches that select preset next and previous
- Quad Banked: 4 footswitches. 5 banks of 4 presets are controlled. 1+2 selects down a bank. 3+4 selects up a bank. Single switch selects the preset of ((bank * 4) + switch number)
- Quad Binary: 4 switch inputs, intended for control by relays. The preset selected depends on the binary combination switch inputs, with switch 1 being the least significant bit, and switch 4 being the most significant bit. E.g. switch inputs 1,0,1,1 = preset 11
### Screen Rotation
- This settings allows the screen to optionally be rotated 180 degrees. This may suit some case designs better.

![image](https://github.com/user-attachments/assets/99d0980a-48a2-4577-ba5f-d60bf652c513)

## Save and Reboot
The Save and Reboot buttons on each configuration page will save all settings and reboot the controller for them to take affect.

<br>

## External Footswitches (Ext)
New in V1.0.8.2. The External Footswitches category allows externally-connected footswitches to be configured.

### Preset Switch Layout
This controls how preset switching will be handled. Options are as follows.
| Name | Total Switches | Arrangement | Bank Up | Bank Down | Notes
|----------- | ------------ | ----------- | ----------- | ------------- | ------------------------- |
| 1x3  | 3   |  1 row of 3 |  2 + 3 | 1 + 2 | |
| 1x4  | 4   |  1 row of 4 |  3 + 4 | 1 + 2 | |
| 1x5A | 5   |  1 row of 5 |  4 + 5 | 1 + 2 | |
| 1x5B | 5   |  1 row of 5 |  5 | 4 | Dedicated bank switches |
| 1x6A | 6   |  1 row of 6 |  5 + 6 | 1 + 2 | |
| 1x6B | 6   |  1 row of 6 |  6 | 5 | Dedicated bank switches |
| 1x7A | 7   |  1 row of 7 |  6 + 7 | 1 + 2 | |
| 1x7B | 7   |  1 row of 7 |  7 | 6 | Dedicated bank switches |
| 2x3  | 6   |  2 rows of 3 |  2 + 3 | 1 + 2 | |
| 2x4  | 8   |  2 rows of 4 |  3 + 4 | 1 + 2 | |
| 2x5A  | 10   |  2 rows of 5 |  4 + 5 | 1 + 2 | |
| 2x5B  | 10   |  2 rows of 5 |  10 | 9 | Dedicated bank switches |
| 2x6A  | 12   |  2 rows of 6 |  5 + 6 | 1 + 2 | |
| 2x6B  | 12   |  2 rows of 6 |  12 | 11 | Dedicated bank switches |

<br>

### Effect Switching
The effect switching configuration allows up to 5 footswitches to be dedicated to functions like enabling/disabling effects. 
<br>
As this is configured using Midi Control Change values, these buttons can also do things like changing the volume of an amplifier between two different levels, or changing the type of modulation.
<br>
Press the "FX" buttons to select which effect change you wish to modify. For each of the 5 FX, the parameters are:
- Foot Switch Number. Footswitch 1 corresponds to the first switch, connected to the SX1509 PCB input 0
- Control Change Number: the CC number of the effect you wish to change. Refer to the Tonex Pedal manual for a list of available numbers
- Midi Value 1: the Midi value that will be sent to the above CC number, when the footswitch is first pressed
- Midi Value 2: the Midi value that will be sent to the above CC number, when the footswitch is pressed a second time
<br>

Example FX values:
- Delay on/off: CC 2. Value 1: 127. Value 2: 0
- Modulation on/off: CC 32. Value 1: 127. Value 2: 0
- Reverb on/off: CC 75. Value 1: 127. Value 2: 0

<br>
Each time the footswitch is pressed, the alternate Midi value will be sent, between value 1 and value 2. This allows for "toggling" of effects with a single switch.

<br>

![image](https://github.com/user-attachments/assets/31b3e7a5-c897-4194-86e9-d8b97e479aad)

<br><br>


## WiFi Settings
The WiFi category allows the WiFi on the controller to be configured.
### WiFi Mode
The WiFi mode can be selected:
- Access Point Timed: this option matches earlier firmware. An Access Point is created when the controller boots, and remains active for 1 minute. If no clients connect, WiFi will be automatically be disabled
- Station Mode: this option is similar to phone, in that it will connect to your home WiFi network, and can then be available to any phone/tablet/PC at any time
- Access Point: Same as option one, except that there is no timer. The Access Point will be enabled permanently

### WiFi Transmit power
New in V1.0.8.2: this setting controls how powerful the WiFi transmission is. Setting this to higher values will allow for a longer distances/range, however some PCBs may experience a lowering of stability when the transmit power is set high.

### SSID and Password
- For Access Point modes, these set the credentials of the Access Point
- For Station Mode, enter the credentials of your WiFi network

### MDNS name
MDNS is a system that allows a WiFi device to be located by a name, instead of its address. This setting controls the name that is used.
<br>
The default name is "tonex" which means you can access the web page via "tonex.local".

![image](https://github.com/user-attachments/assets/89c068a2-71aa-4034-829c-ee2478f5615a)






