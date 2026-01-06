# Tonex Controller: An open-source controller and display interface for the IK Multimedia Tonex One/Tonex guitar pedal and Valeton GP5

# Programming a pre-built firmware release
## New: Web Tool
- Starting from version 2.0.2.2, a new web tool has been created. This is similar to the below web tool, but allows the selection of the hardware platform, and avoids the need to download and extract, as it automatically fetches it from the releases area and fills in the required settings
- visit the programming web page here:  https://builty.github.io/TonexOneController/
- Press and hold the "Boot" button on the board
- Connect a USB-C cable to the board and a PC or Mac
- After the USB connection, release the Boot button
- Select your hardware platform, and then press the Connect button
- If you wish to erase all settings back to default, then select the option to Erase. If you wish to keep your current settings, do not select Erase
- Follow the on screen instructions until completed
- Unplug the board and connect power to test it

<img width="1205" height="925" alt="image" src="https://github.com/user-attachments/assets/f263bdb2-381c-4d39-a47e-81ae3c547785" />


## Older method 1: Windows (using ESP Flash Tool)
- Download the release zip file from the Releases folder and unzip it
- Open the Windows "Control Centre", "Device Manager", and expand the "Ports" section
- Press and hold the "Boot" button on the board
- Connect a USB-C cable to the board and a PC
- After the USB connection, release the Boot button
- A new USB serial port should appear. Note down the "COM" number
 
![image](https://github.com/user-attachments/assets/9e7511eb-041e-4aaa-8d95-a3f4d841d678)

- Run the programmer exe on a Windows PC (note: this is provided as a binary package by Espressif Systems, refer to https://www.espressif.com/en/support/download/other-tools)
- Set the Chip Type as "ESP32-S3"
- Set the Work Mode as "Factory"
- Set the Load Mode as "USB"

![image](https://github.com/user-attachments/assets/0c16f2bd-18be-4011-906d-448e4f1dd384)

- In Download Panel 1, select the Comm port corresponding to your ESP32-S3, determined in the prior steps
- Press the Start button to flash the image into the Waveshare module
- When finished, close the app and disconnect the USB cable (the screen will be blank until the board has been power cycled)
- All done!

### Note!
- If Tonex doesn't want to react to a fairly simple actions after the firmware update, e.g. changing presets, you most probably need to "Erase" the memory from the hardware (using ESP Flash Tool) and installing the firmware after that. Keep in mind, this action is going to delete **everything** from the device memory!

![image](https://github.com/user-attachments/assets/e2e21f46-1d3a-4eec-aee9-25de87c072c7)

## Older Method 2: All OS (using Web Browser)
Thanks to user DrD85 for this method.
- Download the release zip file from the Releases folder and unzip it
- Press and hold the "Boot" button on the board
- Connect a USB-C cable to the board and a PC or Mac
- After the USB connection, release the Boot button
- Open a web browser (use Chrome or Edge) and go to the address "https://esp.huhn.me/"
- Press the Connect button
- A list of serial ports is shown. Look for the port that your ESP32-S3 is using. It may be called "USB JTAG/serial debug unit", or something similar (it may vary with operating system.) Select the port
![image](https://github.com/user-attachments/assets/59729ba8-67f1-4716-9685-93286365d6a3)
- Download the release zip file like above, but extract the contents of it to a temporary directory
- Navigate in the temporary directory to the "bin" directory. These are the files that need to be entered onto the web page
- Navigate in the temporary directory to the "configure/esp32s3" folder. The file "multi_download.conf" contains a list of addresses that need to be entered
- A summary table of the required settings and files is shown below
- Once this information has been entered, press the Program button
- The Output windows should show the progress. Once completed, unplug the USB cable, and then power the board normally
![image](https://github.com/user-attachments/assets/aada9df3-826b-450f-a06c-2f5bab24da5b)
<br>
Settings for the all platforms<br>

| Platform  | File 1      | File 2       | File 3     | File 4      | File 5     |
| --------- | ------------| ------------ | ---------- | ----------- | ---------- |
| All except below 4 | bootloader.bin 0x0 | partition_table.bin 0x8000 | ota_data_initial.bin 0xd000 | TonexController.bin 0x10000 | N/A  |
| Waveshare 3.5B | bootloader.bin 0x0 | partition_table.bin 0x8000 | ota_data_initial.bin 0xd000 | TonexController.bin 0x10000 | skins.bin 0x4f2000 |
| JC3248W 3.5" | bootloader.bin 0x0 | partition_table.bin 0x8000 | ota_data_initial.bin 0xd000 | TonexController.bin 0x10000 | skins.bin 0x4f2000 |
| Waveshare 4.3B | bootloader.bin 0x0 | partition_table.bin 0x8000 | ota_data_initial.bin 0xd000 | TonexController.bin 0x10000 | skins.bin 0x4f2000 |
| Pirate Midi Polar Max | bootloader.bin 0x0 | partition_table.bin 0x8000 | ota_data_initial.bin 0xd000 | TonexController.bin 0x10000 | skins.bin 0x4f2000 |










