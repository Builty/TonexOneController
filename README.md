# Tonex Controller: An open-source controller and display interface for the IK Multimedia Tonex One and Tonex guitar pedals
This project uses a low-cost embedded controller (Espressif ESP32-S3) to form a bridge to the IK Multimedia Tonex One (which does not have native Midi capability) or the bigger Tonex pedal (new in V2.0.0.2.)
<br>It allows selection of the 20 different presets in the pedal, by any or all of touch screen, WiFi, wired footswitches, bluetooth footswitches, bluetooth servers, and midi programs.
<br>A variety of hardware is supported, from a $6 board with no display, up to a $44 board with a 4.3" touch screen LCD and a pretty graphical user interface.

**Note: this project is not endorsed by IK Multimedia. Amplifier skin images copyright is owned by IK Multimedia.**
**TONEX is a registered trademark of IK Multimedia Production Srl**

# Code Use Notice
A number of people have taken significant portions of this project, undertaked minor modifications, and then released commercial products using it. Whilst commercial use of this open source is permitted, it is only permitted if the user provides "attribution", that is, publically states in literature that portions of their product are based on this project, and provide a link to it.<br>
Those who refuse and try to claim this work as their own are violating the license terms and may be reported to Youtube or other entities that may take down your materials.<br>
For example, the "GuerrilhaBox" Controller is clearly based on this project with no attribution, in violation of the license terms. Comments made to their Youtube videos to this effect are being deleted, as its clearly an embarrassment for them.

# Table of Contents
 1. [Key Features](#key_features)
 2. [Demonstration Videos](#demonstration_videos)
 3. [Multiple Hardware Devices](#multiple_devices)
 4. [Hardware Platforms and Wiring Diagrams](#hardware_platforms)
 5. [Pre-Built Controllers from Pirate Midi](#pre-built)
 6. [Uploading/Programming Firmware Releases](#firmware_uploading)
 7. [Configuration and Settings](#config)
 8. [Usage Instructions](#usage_instructions)
 9. [Firmware Development Information](#development_info)
 10. [Acknowledgements](#acknowledgements)
 11. [Firmware Release Notes](#release_notes)
 12. [Companion Projects](#companion_projects)
 13. [License](#license)
 14. [Donations](#donations)

## ⭐ Key Features <a name="key_features"></a>
The supported features vary a little depending on the chosen hardware platform.
- LCD display with capactive touch screen
- Screen displays the name and number of the current preset (all models with displays)
- The User can select an amplifier or pedal skin and also add descriptive text ("4.3B" and "3.5" models)
- Use of simple dual footswitches to select next/previous preset (all platforms)
- Use of four buttons to select a preset via a banked system, or directly via binary inputs (all platforms except for the "4.3B")
- Bluetooth Client support. Use of the "M-Vave Chocolate" bluetooth Midi footswitch device to switch presets (4 buttons, bank up/down)
- Other Bluetooth Midi controllers should be also supported, via the "custom name" option. Refer to [Web Configuration](WebConfiguration.md)
- Bluetooth server support. Pair your phone/tablet with the controller, and send standard Midi program changes, bridged through to the Tonex One pedal (note Server and Client cannot be used simultaneously)
- USB host control of the Tonex pedal
- Wired/Serial Midi support
- New in V1.0.6: full control over all Tonex One parameters and effects, via LCD (4.3B and 3.5 only) and Midi (all platforms.)
- New in V1.0.8: support for up to 16 footswitches, with configurable preset switching layouts, and up to 5 effect/parameter toggle switches
  
## Demonstration Videos <a name="demonstration_videos"></a>
### Release Videos
<a href="https://youtu.be/j0I5G5-CXfg" target="_blank"><img width="455" height="247" alt="image" src="https://github.com/user-attachments/assets/3588465b-8f2e-4e99-a2db-18d2f3ade429" />
<a href="https://www.youtube.com/watch?v=_nemmhjvjcc" target="_blank"><img width="457" height="239" alt="image" src="https://github.com/user-attachments/assets/56fbd134-9518-4b0d-a50d-0e200f71aa5b" />
<a href="https://www.youtube.com/watch?v=ok4EuUgWt44" target="_blank"><img width="456" height="245" alt="image" src="https://github.com/user-attachments/assets/21eb8070-3402-4968-be52-9fb582fb6848" />
<a href="https://www.youtube.com/watch?v=r5wLnxQ4IZo" target="_blank"><img width="458" height="242" alt="image" src="https://github.com/user-attachments/assets/47747b43-f800-4fab-a444-c70403567c80" />
<a href="https://www.youtube.com/watch?v=WF8BB_EIKBo" target="_blank"><img width="452" height="245" alt="image" src="https://github.com/user-attachments/assets/cf955f2e-9ee0-4748-92e1-cd9e1719dc26" />
<a href="https://www.youtube.com/watch?v=hQ5NEcqaVeE" target="_blank"><img width="455" height="245" alt="image" src="https://github.com/user-attachments/assets/887427a8-481d-46b3-a4bf-5472e6e3cca0" />
<br><br>

### Pirate Midi Videos
<a href="https://youtu.be/j0I5G5-CXfg" target="_blank"><img width="458" height="246" alt="image" src="https://github.com/user-attachments/assets/11de8a13-4865-4f64-8b6f-6f6f91a8495f" />
<a href="https://www.youtube.com/watch?v=dkUudtlyilI" target="_blank"><img width="456" height="244" alt="image" src="https://github.com/user-attachments/assets/3293f51a-4178-4156-9dbc-cd26233be540" />
<a href="https://www.youtube.com/watch?v=ygJXO1M7p0s" target="_blank"><img width="455" height="238" alt="image" src="https://github.com/user-attachments/assets/35692c87-0867-47c4-a936-dc5785ffab76" />
<a href="https://www.youtube.com/watch?v=fZ0MSDB0vW4" target="_blank"><img width="456" height="246" alt="image" src="https://github.com/user-attachments/assets/2e9e10d4-9aa1-4f4a-afb4-f66604fd4ae5" />
<br><br>

### Review Videos (seach Youtube for more)
<a href="https://www.youtube.com/watch?v=dL7V-3_e7Gg" target="_blank"><img width="458" height="244" alt="image" src="https://github.com/user-attachments/assets/2a14750e-dee0-4431-a61f-9ea7b8657775" />
<a href="https://www.youtube.com/watch?v=TLoKdBpcDJA" target="_blank"><img width="459" height="248" alt="image" src="https://github.com/user-attachments/assets/84677209-2114-4c6c-875f-bc49a7c3bad4" />
<br><br>

### User Videos (search Youtube for more)
<a href="https://www.youtube.com/watch?v=qkOs5gk3bcQ" target="_blank"><img width="457" height="243" alt="image" src="https://github.com/user-attachments/assets/c5dda5a3-bb22-479d-9498-a1c81d499d80" />
<a href="https://www.youtube.com/watch?v=bNvzW3pwNeY" target="_blank"><img width="459" height="240" alt="image" src="https://github.com/user-attachments/assets/59beea9c-1c10-4e26-a5dd-bf60c0c727e6" />
<a href="https://www.youtube.com/watch?v=pgCZfFsZD_4" target="_blank"><img width="455" height="245" alt="image" src="https://github.com/user-attachments/assets/b91e90f2-6cd1-41f3-a7ae-9ce5d33ad94f" />
<a href="https://www.youtube.com/watch?v=E56C4G4_uKk" target="_blank"><img width="458" height="244" alt="image" src="https://github.com/user-attachments/assets/9c838478-5dcb-4b57-ba2b-74d2d99b77cd" />
<a href="https://www.youtube.com/watch?v=aJPtkqnd2sg" target="_blank"><img width="454" height="242" alt="image" src="https://github.com/user-attachments/assets/c2e260dd-3cb1-4357-a563-76871214ec00" />
<a href="https://www.youtube.com/watch?v=M2mpE5QMi7E" target="_blank"><img width="458" height="244" alt="image" src="https://github.com/user-attachments/assets/0a00d54e-cee2-47d5-800c-4ba8e69c6029" />
<a href="https://www.youtube.com/watch?v=mTD-6dyGXxc" target="_blank"><img width="454" height="241" alt="image" src="https://github.com/user-attachments/assets/07208ba8-67d7-4d6f-9393-5489ab415ccc" />
<br><br>

## Articles/Tutorials written by others
<a href="https://gsus4.com.au/blogs/news-promo/what-if-there-was-a-screen-for-ik-tonex-one-step-by-step-guide" target="_blank"><img width="1338" height="853" alt="image" src="https://github.com/user-attachments/assets/cb3be685-4f83-420f-84f0-761e906978cb" />
<br><br>

## Multiple Hardware Devices <a name="multiple_devices"></a>
This project can run on a variery of different hardware platforms, varying in size and cost. All of them are "off-the-shelf" development boards supplied either by the company "Waveshare", or Espressif.
The code could be adapted to run on other brand ESP32-S3 boards, but to make things easy, pre-built releases are provided for the supported modules.
<br>All platforms support Bluetooth, WiFi, wired footswitches, and wired Midi.
- 4.3" LCD board, supporting touch screen and advanced graphics including customisable amp/pedal skins and text
- 1.69" LCD boards (with or without Touch.) Similar to an Apple Watch, this small board displays the preset name and number
- "Zero" board with no display, is the smallest and cheapest option
- "DevKit-C" board with no display
- "Atom S3R" board with tiny LCD
- 1.9" board (with or without Touch.) Displays the preset name and number
- 3.5" LCD boards, supporting touch screen and advanced graphics including customisable amp/pedal skins and text
<img width="803" height="868" alt="image" src="https://github.com/user-attachments/assets/73d09089-5a49-4c0c-afed-aa8eda7a0b0c" />


## Hardware Platforms and Wiring <a name="hardware_platforms"></a>
For more information about the hardware platforms, refer to [Hardware Platforms](HardwarePlatforms.md)

## Pre-Built Controllers <a name="pre-built"></a>
A collaboration with Pirate Midi has resulted in the availability of fully assembled controllers for sale world-wide.
<br>
This project receives 10% of sales, which is primarily fed back into the purchase of new hardware to develop upon.<br>
https://piratemidi.com/en-au/products/polar

![image](https://github.com/user-attachments/assets/4c5a7a9a-0827-4381-9b4d-265cbf999ce2)

## Uploading/Programming Firmware Releases <a name="firmware_uploading"></a>
For more information about uploading firmware to the boards, refer to [Firmware Uploading](FirmwareUploading.md)

## Configuration and Settings <a name="config"></a>
For more information about changing configuration and settings, for example to change the Midi channel, refer to [Web Configuration](WebConfiguration.md)

## Usage Instructions <a name="usage_instructions"></a>
### Hardware platform with Display
- Connect power
- After a few seconds of boot time, the LCD display should now show the description for your current Preset
- Change presets using one or more of the following methods
  1. Touch screen Next/Previous labels
  2. Dual footswitchs for next/previous preset
  3. Bluetooth Client mode: M-Vave Chocolate footswitches. Bank 1 does presets 1,2,3,4. Bank 2 does presets 5,6,7,8. Etc.
  4. Bluetooth Server mode: Bluetooth Midi controller to send Program change messages 0 to 19
- The Amplifier skin image is not stored in the Tonex One Pedal, hence this needs to be manually selected
- To select an Amp skin and/or change the description text
  1. Press and hold the Preset name for a few seconds
  2. Navigation arrows will appear next to the amp skin image
  3. Use the left/right arrows to navigate through the available amp skins
  4. Press the description text. A keyboard will appear, allowing text to be entered
  5. Press the green tick image to save the changes. Changes will be saved permanently and remembered when next powered on
 
### Hardware platforms without Display
- Connect power
- Change presets using one or more of the following methods
  1. Bluetooth Client mode: M-Vave Chocolate footswitches. Bank 1 does presets 1,2,3,4. Bank 2 does presets 5,6,7,8. Etc.
  2. Bluetooth Server mode: Bluetooth Midi controller to send Program change messages 0 to 19

## Firmware Development Infomation <a name="development_info"></a>
For more information about the firmware development and customisation, refer to [Firmware Development](FirmwareDevelopment.md)

## 🙏 Acknowledgements <a name="acknowledgements"></a>
- [Waveshare board support files]([https://github.com/lifeiteng/vall-e](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3B)) for display and touch screen driver examples
- https://github.com/vit3k/tonex_controller for great work on reverse engineering the Tonex One USB protocol

## Firmware Release Notes <a name="release_notes"></a>
V2.0.0.2 beta 2:
- Caution: due to structural changes, amp/pedal skins and user preset text is not maintained and will need to be entered again. And if rolling back to an older version, all config will be lost
- Upgraded to ESP-IDF 5.5.1 
- Added support for big Tonex pedal 
- Added effect icons to web page, showing order and status of each effect 
- Added new platform LilyGo T-Display S3 1.9" touch (note: delayed release due to issues)
- Added new platform Waveshare ESP32-S3 1.9" touch 
- Added new platform Waveshare ESP32-S3 3.5B touch, with full UI as per 4.3B
- Added new platform JC3248W535C 3.5" touch, with full UI as per 4.3B
- Added new platform Waveshare 1.69" touch in landscape mode 
- Code updates to better isolate hardware platforms from user interface 
- Added left/right swipe gestures to Waveshare 1.69" Touch for next/previous preset 
- AQdded the ability to send value 64 via Midi, to toggle the current state of any on/off parameter 
- Added global volume support (via LCD/Web)
- Added ability to bypass the pedal via web config (Tonex One only)
- Added "toast" dialog message when connected in station mode, with IP address
- Added UDP broadcast to help locate controllers on a network. Windows app available.
- Fixed issue with Lilygo T-Display S3 touch screen
- Added option to increase touch screen sensitivity on 4.3B, to help with screen protectors
- Changed units label of the Reverb time in web config from milliseconds to seconds. Thanks Michael for finding.

V1.0.10.2:
- User contribution from Mateusz: web config now allows the order of the presets to be remapped. E.g. footswitches 1,2,3,4 could now load presets 10,5,11,20 instead of having to be 1,2,3,4.
- User contribution from Mateusz: all 20 presets are now read from the Tonex during boot. Web UI preset selection has all names, instead of only being loaded when the preset was selected
- Platforms with screens show "Syncing" during the above procedure
- User contribution from Mateusz: 4.3B higher quality amp and pedal skin images, and effect icons
- User contribution from Mateusz: the effect icons on the 4.3B main screen arrange themselves to match the pre/post order of the effects
- Added EQ icon to the 4.3B UI, which can be long pressed to jump to the config screen
- Fixed bug where if preset mode was set to quad binary, it could accidently trigger a configuration reset
- User contribution from Yike Lu: added Midi CC 127 which can directly load any preset by number
- Significant performance improvements in the case where multiple serial Midi messages arrive very close together, e.g. when rotating knobs on Midi controllers
- User contribution from Mateusz: added Loop-around footswitch mode, where a single footswitch can navigate 1,2,3...,19,20,1,2,3.., rolling over from 20 back to 1
- User contribution from Jamkid: added ability to click on a parameter value in the web UI, to bring up a dialog box that allows direct entry of a value
- Fixed bug where the 4.3B onboard footswitch disabled mode didn't disable footswitches
- User contribution from Mateusz: when parameters with Sync (syncronise time to the BPM) have the sync enabled, web and 4.3B parameter values change from slider to a drop down with e.g. 1/8, 1/4 etc
- Fixed issue where if parameter Sync was enable (as above) then the Midi control of the values didn't function correctly
- Fixed issue where sending multiple chained Midi CC values over bluetooth, the second value was not processed
- Added landscape LCD screen build for the Waveshare 1.69". Screen rotates 90 degrees so the USB port is on the bottom (or top if 180 degree rotation option is enabled.)
- Added 3 new builds, for collaboration with Pirate Midi, who are selling completed controller products. Serial Midi is enabled by default for all.
- Fixed compatibility issue with the Ampero Control where it would not connect over Bluetooth (thanks to "jjh84" for debugging.)  

V1.0.9.2:
- Added support for Tuning reference (4.3B UI, web config, and Midi)
- Added support for Input trim via Midi
- Improved latency in serial Midi 
- Fixed bug where Access Point mode would disable itself after client disconnect
- User contribution from Mateusz: tap tempo button and indicator, and tempo indicators for modulation and delay, in web config 
- User contribution from Mateusz: web config meta data to allow iOS to save the page as a web app
- User contribution from Jamkid: added bpm indicator and effect status icons to Waveshare 1.69"
- User contribution from Bakatasche: ported bpm and effect status change above to Atom S3R 
- Fixed issue where tempo bpm was limited to 60 bpm instead of 40. Also removed the deadband to make it more accurate
- User contribution from Mateusz: added units to web config range items
- Fixed issue where Midi values 86/87 for preset next/previous didn't work 
- User contribution from Mateusz: added Bank and BPM indicators to 4.3B UI 
- User contribution from Mateusz: Changed footswitch FX CC indexes to use descriptive names, and also to contextually adjust the Value 1 and Value 2 display
- Fixed bug where external footswitch FX toggling, for values that are not boolean, didn't take into account the current value of the parameter when toggling
- Increased maximum external FX switches from 5 to 8
- Added letters to 4.3B effect icons, to show which model is enabled (Modulation, Delay, Reverb)
- Added more footswitch preset layouts to external switches. 1x2, 1x4 binary, disabled etc 
- Added support for switching effects from the onboard footswitches 
- Standardised preset switching layouts for external and internal (onboard) foot switches
- NOTE: due to the preset switching changes, it may be necessary to set your desired configuration again


V1.0.8.2:
- Changes to make the controller work with the latest Tonex editor release
- Fixed memory leak in web config that would eventually lead to malfunction
- Added the ability to configure the WiFi transmit power
- Added support for SX1509 IO expander, that allows up to 16 footswitches on all platforms. Preset switching options, and up to 5 footswitches that can toggle effects/parameters
- Added icons to 4.3B screen, indicating the status of the effects, which can be pressed to toggle the effect 
- Fixed issue where Stomp mode was not set by code, requiring it to be set manually (was broken by latest Ik changes)
- Fixed issue where the new external footswitch effect toggles didn't take into account the current state, resulting in needing multiple presses in some cases
- Changed communications to use the new messages that IK added. Much faster effect switching, and reduced ram usage
- added Amplifier Presence and Depth controls to web and 4.3B UI
- fixed up issues with cabinet disable via web and 4.3B UI
- added globals page to web and 4.3B UI, with input trim, BPM, tempo source and global cabinet bypass
- added support for Tap Tempo, via Midi and external footswitches
- fixed issue where some web params would be displayed as rounded to the nearest integer
- set Direct Monitoring automatically to the right setting
- New method of preset sync on boot that no longer changes any preset assignments
- For the 4.3B, added ability to long-press on the effect icons, to jump straight to the config for that effect 

V1.0.7.2:
- New WiFi options, now allowing connection to other access points.
- Added WiFi status icon for platforms with displays
- New web control feature, allows full remote control of all parameters and presets using WiFi/web. Fully cross platform with no apps needed, just a web browser
- Added support for 180 degree screen rotation (via web config)
- Added ability to reset the configuration to defaults, by holding down wired button 1 for 15 seconds
- Added support for Waveshare ESP32-S3 1.69" Touch
- Fixed some issues with parameter adjustments
- Fixed issue where Bluetooth Midi commands did not check the Midi channel

V1.0.6.1:
- Added support for M5Stack Atom S3R 
- Added support for ESP Devkit-C with 16 MB flash/8MB PSRAM
- added parameter/effect editing user interface and Midi control 
- Fixed issue where pressing the green tick during editing the preset details field would not save the text change

V1.0.5.2:
- Added support for ESP "Devkit-C" board
- Added support for the onboard RGB led on the Zero and the Devkit-C. Three green flashes shown on boot.
- Added support for four wired footswitches. Footswitch mode can be set to one of three different modes, using the web config
- Fixed some build warnings

V1.0.4.2:
- Added support for 1.69" version
- New web configuration system
- Unified the BT client/server versions and renamed the modes to Central/Peripheral
- Added custom name text for connecting to other BT peripherals
- Full support for Wired Midi on all platforms
- Updated build system so all platforms use their own build directory and SDK config files
- Created sub-projects so platform can be selected straight from VS Code
- Added Python script to automate creation of release Zip files
- Memory optimisations and latency reduction/performance tuning

V1.0.3.2:
- NOTE: 1.69" version is not supported in this release! 
- Changed partition table to fix issues with crashing on boot for some users
- new build type with pedal skin images instead of amp skins
- more efficient handling of skin images
- fixed compatibility issue with Midi BT servers that sent time codes
- support for dual wired footswitches on the Zero
- added support for M-Vave Chocolate Plus footswitch

V1.0.2.2:
- WARNING: these files have been problematic for some users! Please use V1.0.3.2 instead
- Updated to be compatible with Tonex 1.8.0 software
- Support for Waveshare Zero low-cost PCB without display
- Support for Bluetooth Server mode

V1.0.1.2:
- Fixed issue with USB comms. Pedal settings are read, modified (only the preset indexes) and then sent back to pedal
- Note: not compatible with Tonex V1.8.0

V1.0.0.2:
- Initial version
- Caution: this version has an issue with USB. It will overwrite the pedal global settings! use with caution and backup your pedal first
- Note: not compatible with Tonex V1.8.0

## Companion Projects <a name="companion_projects"></a>
The MiniMidi project is a small, low cost bluetooth Midi peripheral device, similar to the M-Vave Chocolate pedal.
<br>It can run two or four footswitches, and control the Tonex pedal (or other compatible devices).
Refer to [https://github.com/Builty/TonexOneController/blob/main/minimidi/source](https://github.com/Builty/TonexOneController/blob/main/minimidi/source)

## ©️ License <a name="license"></a>
The Tonex One Controller is under the Apache 2.0 license. It is free for both research and commercial use cases.
<br>However, if you are stealing this work and commercialising it, you are a bad person and you should feel bad.
<br>As per the terms of the Apache license, you are also required to provide "attribution" if you use any parts of the project (link to this project from your project.)

## Donations <a name="donations"></a>
Donations help fund the purchase of new equipment to use in development and testing.<br>
[Donate via Paypal](https://www.paypal.com/donate/?business=RTRMTZA7L7KPC&no_recurring=0&currency_code=AUD)
<br><br>
![QR code](https://github.com/user-attachments/assets/331a7b08-e877-49a4-9d27-2b19a2ff762d)

    
 
