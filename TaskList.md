# Tonex One Controller: An open-source controller and display interface for the IK Multimedia Tonex One guitar pedal
# Task List

## big Tonex pedal support
Mostly complete.<br>
Issues with handling of global variables. Need to find method to obtain current globals from pedal.

## Effect toggle via CC
Suggestion: Change so that value 64 togles the effect from its current state 

## Bypass Support 
Add support for toggling Bypass mode, via the web UI. 

## Midi Mapping
Add the ability to map all 127 midi program change values to any preset. 

## 4.3B Parameter UI
Add sliders for bass/Mid/Treble frequency, to the 4.3B UI.<br>
Add values to sliders that update as the slider is dragged.
Add units to parameters (db, Hz etc)<br>
Need to port to EEZ Studio due to widget limit in Squareline.

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

## Tonex USB disconect handling
If the pedal is diosconnected after being connected, the ESP32 needs a reboot in order to allow connection again.
Review the de-init and init code to allow it to be handled without needing a reboot.
