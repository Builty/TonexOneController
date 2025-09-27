# Tonex Controller: An open-source controller and display interface for the IK Multimedia Tonex One and Tonex pedal
This project uses a low-cost embedded controller (Espressif ESP32-S3) to form a bridge to the IK Multimedia Tonex One guitar pedal (which does not have native Midi capability) and also the Tonex Pedal.

# Development Info
The code is written in C, for the Espressif ESP-IDF development environment version 5.4.1, and using the FreeRTOS operating system.
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

### Partition Table
- All platforms: "partitions.csv"

### SPI RAM config
Mode of SPI RAM chip in use:
- "Octal Mode PSRAM"
- "Quad Mode PSRAM"

### Serial Flasher Config
Flash Spi Mode:
- "QIO" (Quad mode)
- "DIO" (Dual mode)

### Flash Size:
- "8MB"
- "4MB"

# Wireshark USB Capture
- Download and install the latest version of the free app "Wireshark" from https://www.wireshark.org/
- Install it, ensuring the option for USB Pcap is enabled
- Start the Wireshark app
- Connect Tonex pedal to the same PC
- In the Wireshark app, select the Capture interface to USBPcap
<br>
<img width="478" height="339" alt="image" src="https://github.com/user-attachments/assets/4e0f1296-274d-43be-b683-e0cf6bed2085" />
<br><br>
- Press the blue Sharkfin button to begin capturing all USB packets
<br><br>
<img width="438" height="168" alt="image" src="https://github.com/user-attachments/assets/ecbc74c8-8e70-47b5-8516-20a8735f9ed4" />
<br><br>
- Start the Tonex Editor software, and let it complete its sync process<br>
- Press the red square button to stop capturing
<br><br>
<img width="753" height="348" alt="image" src="https://github.com/user-attachments/assets/75d66535-9cc0-48ba-8882-079bdd4fdfb8" />
<br><br>
- Use File -> Save As to save the captured data to disk
 
# Debug Logs
Extensive debug logging is used, in the form of serial printing from the main ESP32-S3 "UART" transmitter pin. To view these logs:
- Obtain a USB serial adaptor, compatible with 3.3 volt TTL signals (low-cost example shown below)<br>
<img width="1327" height="471" alt="image" src="https://github.com/user-attachments/assets/56b48788-f70c-4ccb-bbb5-fcd5f17531af" /><br> 
- Connect the ground pin on the adaptor to a ground pin of the ESP32-S3
- Connect the "RX" pin of the adaptor to TX pin of the ESP32-S3 board (pin locations shown for some platforms below.)
<img width="480" height="308" alt="image" src="https://github.com/user-attachments/assets/7d6a7042-3ece-49e1-ba63-ebefd16ab469" /><br>
<img width="868" height="539" alt="image" src="https://github.com/user-attachments/assets/4118ab86-c613-409a-8d8a-892eb4f12f29" /><br>
<img width="909" height="600" alt="image" src="https://github.com/user-attachments/assets/5403c46d-c7d3-403c-9292-dfbce8568eea" /><br>
<img width="991" height="368" alt="image" src="https://github.com/user-attachments/assets/dd7bcfcb-674a-4d42-b57f-afc069fc3c9f" /><br>
- Download and install the latest version of the free app "Tera Term" from https://github.com/TeraTermProject/teraterm/releases (or use any other serial terminal program)
- Connect the USB serial adaptor to your computer, and let it load any drivers it needs
- Run Tera Term, and select the option for "Serial" and select the comm port corresponding to the USB serial adaptor
<img width="771" height="448" alt="image" src="https://github.com/user-attachments/assets/7c8d4260-68c8-49a5-8e6e-d28ddb29dd8a" /><br>
- Select "Setup" -> "Serial Port", and change the baud rate to 115200<br>
<img width="406" height="324" alt="image" src="https://github.com/user-attachments/assets/45954e1a-027f-4476-af3e-5313747f34e0" /><br>
- Power on the Controller board. Logging should appear on the Tera Term screen
- To log the data to disk, select File -> Log





    
