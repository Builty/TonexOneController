# Tonex One Controller: An open-source controller and display interface for the IK Multimedia Tonex One guitar pedal
# Task List

## 4.3B Parameter UI
Add sliders for bass/Mid/Treble frequency, to the 4.3B UI.<br>
Due to the widget limit of 150 in free Squareline Studio, consider migrating to EEZ Studio.
Add values to sliders that update as the slider is dragged.
Add units to parameters (db, Hz etc)

## Waveshare 1.69" touch support. 
Swipe left/right to select next/previous preset.<br>
Status: complete, ready to release with next version

## Global volume support. 
Need to find how to read the current value. 

## Expression pedal support. 
Use ADC to read expression pedal pot, and have config to assign what quantity it adjusts and min/max clamps.

## Direct Preset load from footswitch 
Add ability to load preset X from a footswitch press.<br> 

## Support for multiple Bluetooth devices 
Allow at least two simultaneous BT devices to be connected.

## Amp and Pedal skins in same build
Look at ways of adding amp and pedal skins to the same build.<br>
Could be done by adding a new platform type, with the Cmake file including some of each.

## Midi Output support
Add support for Midi output, sending Midi PC and CC messages to indicate to receivers the current state.

## Secure Bluetooth pairing. 
Add support for pairing with devices that use security.<br>
Add to web UI a display of scanned devices, and allow one to be selected and paired with.<br>
Also ability to delete a paired device.

## USB Hub support
Migrate to newer version of ESP IDF, to gain support for USB hubs.<br>
Look at adding USB Midi support once this is complete.
Status: migration to ESP IDF 5.4.1 is almost complete.

## Tonex USB disconect handling
If the pedal is diosconnected after being connected, the ESP32 needs a reboot in order to allow connection again.
Review the de-init and init code to allow it to be handled without needing a reboot.

## Bypass Support 
Add support for toggling Bypass mode, via the web UI. 

## Midi Mapping
Add the ability to map all 127 midi program change values to any preset. 

