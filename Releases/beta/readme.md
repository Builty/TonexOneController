Note: files in this directory are beta versions may contain bugs.
<br><br>
V1.0.8.2 Beta 6.
- fixed bug introduced as part of "State packet is now mapped to a data structure" change in beta 5. 

<br><br>

V1.0.8.2 Beta 5.
REMOVED DUE TO MAJOR BUG
- added Amplifier Presence and Depth controls to web and 4.3B UI
- fixed up issues with cabinet disable via web and 4.3B UI
- added globals page to web and 4.3B UI, with input trim, BPM, tempo source and global cabinet bypass
- added support for Tap Tempo, via Midi and external footswitches
- fixed issue where some web params would be displayed as rounded to the nearest integer
- set Direct Monitoring automatically to the right setting
- New method of preset sync on boot that no longer changes any preset assignments
- State packet is now mapped to a data structure, for easier handling and no more array indexing with magic number offsets
- added more external footswitch assignements: 1x5B, 1x6A, 1x6B, 1x7A, 1x7B

<br><br>
V1.0.8.2 Beta 4.
- Fixed issue where Stomp mode was not set by code, requiring it to be set manually 
- Fixed issue where the new external footswitch effect toggles didn't take into account the current state, resulting in needing multiple presses in some cases

V1.0.8.2 Beta 3.
- Made the new effect icons on main screen of 4.3B pressable, to toggle the effect 
- Changed communications to use the new messages that IK added. Much faster effect switching, and reduced ram usage

V1.0.8.2 Beta 2.
- fixed compatibility issue introduced with Tonex editor release. This version is only for this new pedal firmware
- added icons to main screen of 4.3B to indicate the status of the effects

<br><br>
Files for each platform:
- Espressif DevKit-C with 2 MB PSRAM (Quad SPI):<br>
TonexOneController_V1.0.8.2_beta_4_DevKitC_N8R2.zip
<br><br>
- Espressif DevKit-C with 8 MB PSRAM (Octal SPI):<br>
TonexOneController_V1.0.8.2_beta_4_DevKitC_N16R8.zip
<br><br>
- M5Stack Atom S3R:<br>
TonexOneController_V1.0.8.2_beta_4_M5AtomS3R.zip
<br><br>
- Waveshare 1.69" LCD board (no Touch):<br>
TonexOneController_V1.0.8.2_beta_4_Waveshare_1_69.zip
<br><br>
- Waveshare 1.69" Touch LCD board:<br>
TonexOneController_V1.0.8.2_beta_4_Waveshare_1_69_Touch.zip
<br><br>
- Waveshare 4.3B board, Amplifier skin images:<br>
TonexOneController_V1.0.8.2_beta_4_Waveshare_4_3B_Display_Amp_Skins.zip
<br><br>
- Waveshare 4.3B board, Pedal skin images:<br>
TonexOneController_V1.0.8.2_beta_4_Waveshare_4_3B_Display_Pedal_Skins.zip
<br><br>
- Waveshare Zero board:<br>
TonexOneController_V1.0.8.2_beta_4_Waveshare_Zero.zip


