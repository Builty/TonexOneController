# Tonex One Controller: An open-source controller and display interface for the IK Multimedia Tonex One guitar pedal
This project uses a low-cost embedded controller (Espressif ESP32-S3) to form a bridge to the IK Multimedia Tonex One guitar pedal (which does not have native Midi capability.)

# Development Info
The code is written in C, for the Espressif ESP-IDF development environment version 5.0.2, and using the FreeRTOS operating system.
The LVGL library is used as the graphics engine.

## Task overview:
- Control task is the co-ordinator for all other tasks
- Display task handles the LCD display and touch screen
- Midi Control task handles Bleutooth link to Midi pedals
- USB comms task handles the USB host
- USB Tonex One handles the comms to the Tonex One pedal

## Building Custom sources
Building the application requires some skill and patience.
- Follow the instructions at https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3B#ESP-IDF to install VS Code and ESP-IDF V5.02
- Open the project Source folder using VS Code
- Follow the instructions at https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3B#Modify_COM_Port to select the correct comm port for your Waveshare board, and compile the app

## Menu Config options
Use the Menu Config system to select which components of the Controller you wish to enable.
![image](https://github.com/user-attachments/assets/593d48fb-aeea-4b20-87c7-dc9212952213)

There are also a few settings that need changing between the Waveshare board with display and the Zero without a display, mainly due to the headless board having 4 MB flash versus 8MB on the display board:
### Partition Table
- Waveshare Zero: "partitions_zero.csv"
- All others: "partitions.csv"

### SPI RAM config
Mode of SPI RAM chip in use:
- Display board: "Octal Mode PSRAM"
- No Display board: "Quad Mode PSRAM"

### Serial Flasher Config
Flash Spi Mode:
- Display board: "QIO" (Quad mode)
- No Display board: "DIO" (Dual mode)

### Flash Size:
- Display board: "8MB"
- No Display board: "4MB"

## Adding a new Platform
To add support for a new PCB platform, the following steps are required:
- Create sdkconfig.<boardname> in source folder, where <boardname> is a short descriptive name of the board. E.g. Waveshare 4.3B is "ws43B"
- In this file, referring to the existing examples, configure the partition table, CPU speed, flash mode, and SPIRAM. If the board has a display, create a new descriptive Config for it. E.g. CONFIG_TONEX_CONTROLLER_DISPLAY_WAVESHARE_800_480
- Edit the file "esp_idf_project_configuration.json" in the source folder. This file defines all supported platforms, and links a shot name to a build directory, and the sdkconfig file (created earlier)
- If the board does not have a display, you should now be able to select the board from VS Code, and build it
- If the board has a display, then more work is required, as follows
- Ceate a new folder in the TonexOneController folder, called "ui_design_<descriptivename> where descriptive name is something uniqiue about the LCD, possibly its size or resolution
- In this folder create a UI project using Squareline Studio, and design the UI. Refer to the other ui designs, and try to match the names of the screens and components as much as possible (this will minimise other code changes)
- Set the Squareline Studio to export its files into "source/main/ui_generated_<descriptivename>"
- Edit the file "source/main/CMakeLists.txt" and enter the unique Config you created before, and use that to set the include directory to the new ui_generated folder, and add the source files from it
- Edit display.c to add your new Config, and inside that define, perform the display initialisation
- Update the function update_ui_element() to handle the events needed. These events are sent from other parts of the system, for things like enabling/disabling Bluetooth status icon, setting the name of the preset etc
  
