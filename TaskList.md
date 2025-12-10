# Tonex One/Pedal/GP5 Controller: An open-source controller and display interface
# Task List

## Midi Mapping
Add the ability to map all 127 midi program change values to any preset. 

## Expression pedal support. 
Use ADC to read expression pedal pot, and have config to assign what quantity it adjusts and min/max clamps.
Possibility of I2C ADC for platforms that don't have spare pins.

## Support for multiple Bluetooth devices 
Allow at least two simultaneous BT devices to be connected.

## Midi Output support
Add support for Midi output, sending Midi PC and CC messages to indicate to receivers the current state.

## USB disconnect handling
If the pedal is diosconnected after being connected, the ESP32 needs a reboot in order to allow connection again.
Review the de-init and init code to allow it to be handled without needing a reboot.
