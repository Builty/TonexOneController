Note: files in this directory are beta versions may contain bugs.
<br><br>

Release of V1.0.7.2 beta 1.
* first release 

Release of V1.0.7.2 beta 2.
* fixed issue with web control reverb enable not working
* removed Amp presence control from LCD UI and Web UI, as it was mis-identified and really changed the cabinet
* added amp enable to web config
* added cab enable to web config
* fixed bug where 4th param on web Modulation page could be shown when it should have been hidden
* fixed bug with EQ mid freq, where it had its slider max set to 500 instead of 5000

Release of V1.0.7.2 beta 3.
* fixed issue with Bluetooth mode not saving via web config 

Release of V1.0.7.2 beta 4.
* special build for user to assist debugging the Zero

Release of V1.0.7.2 beta 5.
* renamed 2 unknown amp parameters to ?? in web control. Let me know if you can figure out what they do!
* reduced the brightness of the boot flash led on the Zero and Devkit-C, and changed it to blue (as some leds have red and green swapped)
* added EQ MidQ to 4.3B UI 
* added Amp and Cab enable switches to 4.3B UI, but note the cab switch does not appear to work properly, related to the above 2 unknown amp parameters 

Release of V1.0.7.2 beta 6.
* added official support for the Waveshare 1.69" Touch
* fixed issue where Midi over Bluetooth did not check the midi channel
* added new sdkconfig items to simplify the core code (e.g. HAS_LCD instead of 4.3 or 1.69 or 1.69T or Atom S3R etc)

