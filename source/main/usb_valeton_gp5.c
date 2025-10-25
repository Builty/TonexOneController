/*
 Copyright (C) 2025  Greg Smith

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "esp_log.h"
#include "usb/usb_host.h"
#include "usb/cdc_acm_host.h"
#include "usb/midi_host.h"
#include "driver/i2c_master.h"
#include "usb_comms.h"
#include "usb_valeton_gp5.h"
#include "control.h"
#include "display.h"
#include "wifi_config.h"
#include "usb_tonex_common.h"
#include "tonex_params.h"

//***** Valeton GP5 device *****
//-----------------------------
//idVendor = 0x84EF
//idProduct = 0x0184

// Midi Interface: 3
// Interface Descriptor:
// ------------------------------
// 0x09	bLength
// 0x04	bDescriptorType
// 0x03	bInterfaceNumber 
// 0x00	bAlternateSetting
// 0x02	bNumEndPoints
// 0x01	bInterfaceClass   (Audio Device Class)
// 0x03	bInterfaceSubClass   (MIDI Streaming Interface)
// 0x00	bInterfaceProtocol   
// 0x05	iInterface   "GP-5 MIDI"

// MS Interface Header Descriptor:
// ------------------------------
// 0x07	bLength
// 0x24	bDescriptorType
// 0x01	bDescriptorSubtype
// 0x0100	bcdMSC
// 0x0041	wTotalLength   (65 Bytes)

// MS MIDI IN Jack Descriptor:
// ------------------------------
// 0x06	bLength
// 0x24	bDescriptorType
// 0x02	bDescriptorSubtype
// 0x01	bJackType
// 0x01	bJackID
// 0x00	iJack

// MS MIDI IN Jack Descriptor:
// ------------------------------
// 0x06	bLength
// 0x24	bDescriptorType
// 0x02	bDescriptorSubtype
// 0x02	bJackType
// 0x02	bJackID
// 0x00	iJack

// MS MIDI OUT Jack Descriptor:
// ------------------------------
// 0x09	bLength
// 0x24	bDescriptorType
// 0x03	bDescriptorSubtype
// 0x01	bJackType
// 0x03	bJackID
// 0x01	bNrInputPins
// 0x02	baSourceID(1)
// 0x01	baSourcePin(1)
// 0x00	iJack

// MS MIDI OUT Jack Descriptor:
// ------------------------------
// 0x09	bLength
// 0x24	bDescriptorType
// 0x03	bDescriptorSubtype
// 0x02	bJackType
// 0x04	bJackID
// 0x01	bNrInputPins
// 0x01	baSourceID(1)
// 0x01	baSourcePin(1)
// 0x00	iJack


static const char *TAG = "app_Valeton_GP5";


#define VALETON_GP5_MIDI_INTERFACE_INDEX            3
#define MAX_INPUT_BUFFERS                           2
#define MAX_STATE_DATA                              512
#define VALETON_GP5_RX_TEMP_BUFFER_SIZE             8192
#define VALETON_GP5_TX_TEMP_BUFFER_SIZE             1024
#define VALETON_GP5_USB_TX_BUFFER_SIZE              128

// GP5 params, might move to dedicated file
// Credit for this to Facebook user, name to be found
// Pedal	    No_of_Params	Param_0	Param_1	Param_2	Param_3	Param_4	Param_5	Param_6
// Green OD	    3	            Gain	Tone	VOL				
// Yellow OD	2	            Gain	VOL					
// Super OD	    3	            Gain	Tone	VOL				
// SM Dist	    3	            Gain	Tone	VOL				
// Plustortion	2	            Gain	VOL					
// La Charger	3	            Gain	Tone	VOL				
// Darktale	    3	            Gain	Filter	VOL				
// Sora Fuzz	2	            Fuzz	VOL					
// Red Haze	    2	            Fuzz	VOL					
// Bass OD	    5	            Gain	Blend	Vol	    Bass	Treble		
// Guitar EQ 1	6	            125Hz	400Hz	800Hz	1.6kHz	4kHz	VOL	
// Guitar EQ 2	6	            100Hz	500Hz	1kHz	3kHz	6kHz	VOL	
// Bass EQ 1	6	            33Hz	150Hz	600Hz	2kHz	8kHz	VOL	
// Bass EQ 2	6	            50Hz	120Hz	400Hz	800Hz	4.5kHz	VOL	
// Mess EQ	    5	            80Hz	240Hz	750Hz	2.2kHz	6.6kHz		
// Gate	        1	            THRE						
// COMP	        2	            Sustain	VOL					
// COMP4	    4	            Sustain	Attack	VOL	    Clip			
// Boost	    3	            Gain	+3dB	Bright				
// Micro Boost	1	            Gain						
// B-Boost	    4	            Gain	Vol	    Bass	Treble			
// Toucher	    4	            Sense	Range	Q	    Mix			
// Crier	    6	            Depth	Rate	VOL	    Low	Q	High	
// OCTA	        3	            Low	    High	Dry				
// Pitch	    5	            High	Low	Dry	H-VOL	L-VOL		
// Detune	    3	            Detune	Wet	Dry				
// A-Chorus	    3	            Depth	Rate	Tone				
// B-Chorus	    3	            Depth	Rate	VOL				
// Jet	        4	            Depth	Rate	P.Delay	F.Back			
// N-Jet	    4	            Depth	Rate	P.Delay	F.Back			
// O-Phase	    1	            Rate						
// M-Vibe	    2	            Depth	Rate					
// V-Roto	    2	            Depth	Rate					
// Vibrato	    3	            Depth	Rate	VOL				
// O-Trem	    2	            Depth	Rate					
// Sine Trem	3	            Depth	Rate	VOL				
// Bias Trem	4	            Depth	Rate	VOL	    Bias			

enum CommsState
{
    COMMS_STATE_IDLE,
    COMMS_STATE_SYNC,
    COMMS_STATE_READY
};

typedef struct
{
    uint8_t State;
} tValetonGP5Data;

typedef struct
{
    uint8_t Data[VALETON_GP5_RX_TEMP_BUFFER_SIZE];
    uint16_t Length;
    uint8_t ReadyToRead : 1;
    uint8_t ReadyToWrite : 1;
} tInputBufferEntry;

/*
** Static vars
*/
static midi_dev_hdl_t midi_dev;
static tValetonGP5Data* ValetonGP5Data;
static uint8_t* TxBuffer;
static QueueHandle_t input_queue;
static volatile tInputBufferEntry* InputBuffers;
static uint8_t* ProcessingBuffer;
static uint8_t* TransmitBuffer;

/*
** Static function prototypes
*/
static void usb_valeton_gp5_request_preset_sync(void);
static void usb_valeton_gp5_request_current_preset(void);
static void usb_valeton_gp5_request_something_2(void);
static void usb_valeton_gp5_get_preset(uint8_t index);
static void usb_valeton_gp5_set_preset(uint8_t index);
static void usb_valeton_gp5_request_preset_params(void);

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:  credit to https://rvalladares.com/ for parts of this    
*         Expects a buffer starting with 0xF0, 2 bytes skipped for Crc, and ending in 0xF7 
*****************************************************************************/
static uint8_t usb_valeton_gp5_crc8(const uint8_t* sysex_data, uint32_t length) 
{
    uint8_t raw_data[64];
    uint32_t raw_length = 0;
    uint32_t nibble_count;
    uint8_t crc;
    uint8_t high_nibble;
    uint8_t low_nibble;
    uint8_t* sys_ptr = (uint8_t*)sysex_data;

    // skip 0xF0 and 2 byte CRC
    sys_ptr += 3;

    // locate the end of packet
    while (*sys_ptr != 0xF7)
    {
        sys_ptr++;
        raw_length++;
    }

    nibble_count = raw_length / 2;

    // reset pointer to start of data
    sys_ptr = (uint8_t*)sysex_data;
    sys_ptr += 3;

    // Pack low nibbles into raw data bytes
    for (uint8_t i = 0; i < nibble_count; i++) 
    {
        high_nibble = *sys_ptr & 0x0F; // Low nibble of first byte
        sys_ptr++;
        
        low_nibble = *sys_ptr & 0x0F; // Low nibble of second byte
        sys_ptr++;

        raw_data[i] = (high_nibble << 4) | low_nibble;
    }

    // CRC-8 calculation (polynomial 0x07, initial value 0x00)
    crc = 0;
    for (uint8_t i = 0; i < nibble_count; i++) 
    {
        crc ^= raw_data[i];
        
        for (uint8_t j = 0; j < 8; j++) 
        {
            if (crc & 0x80) 
            {
                crc = ((crc << 1) & 0xFF) ^ 0x07;
            } 
            else 
            {
                crc = (crc << 1) & 0xFF;
            }
        }
    }

    return crc;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t usb_valeton_gp5_send_sysex(const uint8_t* buffer, uint8_t len, uint8_t type) 
{
    uint8_t write_index = 0;
    uint8_t read_index = 0;
    uint8_t crc;
    uint8_t packet_size;
    uint8_t triplets;
    uint8_t triplet_count;
    uint8_t usb_transfer[64];

    // packet structure - max 64 bytes to suit USB transfer size
    // 0xF0 start marker
    // CRC (2 bytes)
    // Vendor ID 00 01 00 00
    // Data size (2 byte)
    // 0101 for messages to GP5, 0102 for received? or could be commands and requests?
    // Payload
    // 0xF7 end marker

    memset((void*)usb_transfer, 0, sizeof(usb_transfer));
    memset((void*)TransmitBuffer, 0, VALETON_GP5_TX_TEMP_BUFFER_SIZE);

    // debug
    ESP_LOGI(TAG, "usb_valeton_gp5_send_sysex input:");
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, len, ESP_LOG_INFO);

    // set start marker
    TransmitBuffer[write_index++] = 0xF0;

    // next 2 bytes are Crc, skip for now
    write_index += 2;

    // next 4 bytes are vendor ID??
    TransmitBuffer[write_index++] = 0x00;
    TransmitBuffer[write_index++] = 0x01;
    TransmitBuffer[write_index++] = 0x00;
    TransmitBuffer[write_index++] = 0x00;

    // length bytes. Value sent as e.g. 0x14 as 01 04
    TransmitBuffer[write_index++] = len >> 4;
    TransmitBuffer[write_index++] = len & 0x0F;

    // 0101 for messages sent to GP5, 0102 for receive??
    // or could be 0101 for requests, and 0102 for commands?
    TransmitBuffer[write_index++] = 0x01;
    TransmitBuffer[write_index++] = type;

    // now starts the data payload
    memcpy((void*)&TransmitBuffer[write_index], (void*)buffer, len);
    write_index += len;

    // end packet marker
    TransmitBuffer[write_index++] = 0xF7;

    // save the packet size
    packet_size = write_index;

    // calc Crc before Midi USB encoding
    crc = usb_valeton_gp5_crc8(TransmitBuffer, packet_size);

    // Crc value e.g. 0x63 becomes 06,03
    TransmitBuffer[1] = crc >> 4;
    TransmitBuffer[2] = crc & 0x0F;

    // now insert Midi code index numbers along with the SySEx triplets
    write_index = 0;
    read_index = 0;

    triplet_count = (uint8_t)((float)packet_size / 3.0f);

    // debug
    //ESP_LOGI(TAG, "usb_valeton_gp5_send_sysex usb before midi codes, Triplets:%d", (int)triplet_count);
    //ESP_LOG_BUFFER_HEXDUMP(TAG, TransmitBuffer, packet_size, ESP_LOG_INFO);

    // handle all triplets
    for (uint8_t tc = 0; tc < triplet_count; tc++)
    {
        // Midi start marker 04 "Midi starts or continues"
        usb_transfer[write_index++] = 0x04;

        // payload triplet
        for (triplets = 0; triplets < 3; triplets++)
        {
            usb_transfer[write_index++] = TransmitBuffer[read_index++];
        }
    }

    // see whats left
    if ((packet_size - read_index) == 1)
    {
        //  0x05: "Midi ends with following single byte"
        usb_transfer[write_index++] = 0x05;    

        // end marker
        usb_transfer[write_index++] = TransmitBuffer[read_index++];

        // pad with 0s
        usb_transfer[write_index++] = 0x00;
        usb_transfer[write_index++] = 0x00;
    }
    else if ((packet_size - read_index) == 2)
    {
        //  0x06: "Midi ends with following 2 bytes"
        usb_transfer[write_index++] = 0x06;    

        // last data byte
        usb_transfer[write_index++] = TransmitBuffer[read_index++];    

        // end marker
        usb_transfer[write_index++] = TransmitBuffer[read_index++];    

        // pad with 0s
        usb_transfer[write_index++] = 0x00;
    }
    else if ((packet_size - read_index) == 3)
    {
        //  0x07: "Midi ends with following 3 bytes"
        usb_transfer[write_index++] = 0x07;    

        // last 2 data bytes
        usb_transfer[write_index++] = TransmitBuffer[read_index++];    
        usb_transfer[write_index++] = TransmitBuffer[read_index++];    

        // end marker
        usb_transfer[write_index++] = TransmitBuffer[read_index++];    
    }

    // debug
    ESP_LOGI(TAG, "usb_valeton_gp5_send_sysex usb tx:");
    ESP_LOG_BUFFER_HEXDUMP(TAG, usb_transfer, write_index, ESP_LOG_INFO);

    // send packet
    if (midi_host_data_tx_blocking(midi_dev, (const uint8_t*)usb_transfer, write_index, 50) != ESP_OK)
    {
        ESP_LOGE(TAG, "usb_valeton_gp5_send_sysex failed");
        return ESP_FAIL;
    }

    return ESP_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
// Decode percentage parameters (delay_mix, delay_feedback, reverb_mix, reverb_decay)
float usb_valeton_gp5_decode_percentage(uint8_t byte30, uint8_t byte31, uint8_t byte33) {
    float value;
    if (byte33 == 0x00) { // 0–7% range
        value = (byte30 * 2.0f) + (byte31 / 8.0f);
    } else if (byte33 == 0x01) { // 8–31% range
        value = 8.0f + (byte30 * 2.0f) + (byte31 / 8.0f);
    } else if (byte33 == 0x02) { // 32–100% range
        value = 32.0f + (byte30 * 4.0f) + (byte31 / 4.0f);
    } else if (byte33 == 0x03) { // Extended range (e.g., delay_time 128–255ms)
        value = 128.0f + (byte30 * 4.0f) + (byte31 / 4.0f);
    } else {
        return -1.0f;
    }
    // Apply +2% offset for 32–100% range to match device display
    if (byte33 == 0x02) {
        value += 2.0f;
    }
    return value;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
// Decode delay_time (ms)
float usb_valeton_gp5_decode_time(uint8_t byte30, uint8_t byte31, uint8_t byte33) {
    if (byte33 == 0x01) { // 20–31ms range
        return 20.0f + (byte30 * 2.0f) + (byte31 / 8.0f);
    } else if (byte33 == 0x02) { // 32–127ms range
        return 32.0f + (byte30 * 4.0f) + (byte31 / 4.0f);
    } else if (byte33 == 0x03) { // 128–255ms range
        return 128.0f + (byte30 * 4.0f) + (byte31 / 4.0f);
    }
    return -1.0f;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
// Decode reverb_type or reverb_ok
const char *usb_valeton_gp5_decode_type_or_onoff(uint8_t byte24, uint8_t byte32) {
    if (byte32 == 0x0c) { // reverb_type
        switch (byte24) {
            case 0x0b: return "Air";
            case 0x00: return "Room";
            case 0x01: return "Hall";
            case 0x02: return "Church";
            case 0x0f: return "Plate";
            case 0x04: return "Spring";
            case 0x06: return "N-Star";
            case 0x07: return "Deepsea";
            case 0x05: return "Sweet Space";
            default: return "Unknown";
        }
    } else if (byte32 == 0x0b) { // reverb_ok
        switch (byte24) {
            case 0x01: return "On";
            case 0x00: return "Off";
            default: return "Unknown";
        }
    }
    return "Unknown";
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
// Decode EQ Level (-50 to 50)
float usb_valeton_gp5_decode_level(uint8_t byte30, uint8_t byte31, uint8_t byte32, uint8_t byte33) {
    if (byte32 == 0x0c && byte33 == 0x02) { // Negative range (-50 to -1)
        return -50.0f + (byte30 * 2.0f) + (byte31 / 8.0f);
    } else if (byte32 == 0x04 && byte33 == 0x00) { // Positive range (0 to 50)
        return (byte30 * 2.0f) + (byte31 / 8.0f);
    } else if (byte32 == 0x04 && byte33 == 0x01) { // 8–31 range, possibly mislabeled as Level
        return 8.0f + (byte30 * 2.0f) + (byte31 / 8.0f);
    }
    return -51.0f; // Invalid
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static uint8_t usb_valeton_gp5_parse_sysex(const uint8_t* buffer, uint32_t len) 
{
    uint32_t write_index = 0;
    uint32_t read_index = 0;
    uint32_t bytes_read = 0;
    uint16_t preset_index = 0;
    uint8_t string_index;
    uint16_t ascii_char;
    uint16_t function;
    uint8_t* tmp_ptr = (uint8_t*)buffer;
    uint16_t rx_length = 0;
    uint8_t found_message = 0;
    char name_string[20] = {0};

    // step 1: strip the SysEx markers from the data, to get the message bytes
    while (bytes_read < len)
    {
        if (*tmp_ptr == 0x04)
        {
            //ESP_LOGE(TAG, "Chunk start");

            // skip the 04
            tmp_ptr++;
            bytes_read++;

            if (*tmp_ptr == 0xF0)
            {
                // skip the start marker
                tmp_ptr++;
                bytes_read++;

                // next 2 bytes are Crc, skip
                tmp_ptr += 2;
                bytes_read += 2;

                // skip the 0x04
                tmp_ptr++;
                bytes_read++;

                // next 3 bytes are Vendor ID, skip all
                tmp_ptr += 3;
                bytes_read += 3;

                // skip the 0x04 and last Vendor ID byte
                tmp_ptr += 2;
                bytes_read += 2;

                // grab next 2 bytes (length)
                if (!found_message)
                {
                    rx_length = (*tmp_ptr << 8) | *(tmp_ptr + 1);
                    found_message = 1;
                }
                tmp_ptr += 2;
                bytes_read += 2;

                // skip the 0x04
                tmp_ptr++;
                bytes_read++;

                // next 3 bytes are part of payload, grab them
                ProcessingBuffer[write_index++] = *tmp_ptr++;
                ProcessingBuffer[write_index++] = *tmp_ptr++;
                ProcessingBuffer[write_index++] = *tmp_ptr++;
                bytes_read += 3;

                // skip the next 0x04
                tmp_ptr++;
                bytes_read++;

                // next 3 bytes are part of payload, grab them
                ProcessingBuffer[write_index++] = *tmp_ptr++;
                ProcessingBuffer[write_index++] = *tmp_ptr++;
                ProcessingBuffer[write_index++] = *tmp_ptr++;
                bytes_read += 3;
            }
            else
            {
                // data, grab triplet
                ProcessingBuffer[write_index++] = *tmp_ptr++;
                ProcessingBuffer[write_index++] = *tmp_ptr++;
                ProcessingBuffer[write_index++] = *tmp_ptr++;
                bytes_read += 3;
            }

            // debug
            //ESP_LOG_BUFFER_HEXDUMP(TAG, ProcessingBuffer, write_index, ESP_LOG_INFO);
        }
        else if (*tmp_ptr == 0x05)
        {
            // skip the 05
            tmp_ptr++;
            bytes_read++;

            if (*tmp_ptr == 0xF7)
            {
                // skip end marker
                tmp_ptr++;
                bytes_read++;

                // skip last 2 bytes
                tmp_ptr += 2;
                bytes_read +=2 ;
            }
            else
            {
                ESP_LOGE(TAG, "Processing error 05");
                break;
            }
        }
        else if (*tmp_ptr == 0x06)
        {
            //ESP_LOGE(TAG, "Chunk end");

            // skip the 06
            tmp_ptr++;
            bytes_read++;

            // found end marker, get payload bytes
            ProcessingBuffer[write_index++] = *tmp_ptr++;
            bytes_read++;

            if (*tmp_ptr == 0xF7)
            {
                // skip end marker
                tmp_ptr++;
                bytes_read++;
            }
            else
            {
                ESP_LOGE(TAG, "Processing error 06");
                break;
            }
        }
        else if (*tmp_ptr == 0x07)
        {
            //ESP_LOGE(TAG, "Chunk end");

            // skip the 07
            tmp_ptr++;
            bytes_read++;

            // found end marker, get payload bytes
            ProcessingBuffer[write_index++] = *tmp_ptr++;
            ProcessingBuffer[write_index++] = *tmp_ptr++;
            bytes_read += 2;

            if (*tmp_ptr == 0xF7)
            {
                // skip end marker
                tmp_ptr++;
                bytes_read++;
            }
            else
            {
                ESP_LOGE(TAG, "Processing error 07");
                break;
            }
        }
       
    }

    // debug
    //ESP_LOG_BUFFER_HEXDUMP(TAG, ProcessingBuffer, write_index, ESP_LOG_INFO);
    //vTaskDelay(100);

    // check the message type
    read_index = 0;
   
    // skip the 0102/0101 marker
    read_index += 2;

    // next 2 bytes are function code
    function = (ProcessingBuffer[read_index] << 4) | ProcessingBuffer[read_index + 1];
    read_index += 2;

    switch (function)
    {
        case 0x08:
        {
            // single preset name/index. Don't need as already done full sync
            // get the preset index
            preset_index = (ProcessingBuffer[read_index] * 16) + ProcessingBuffer[read_index + 1];
            read_index += 2;

            // skip 6 bytes of zeros
            read_index += 6;
            string_index = 0;

            // next is 32 bytes/16 characters of preset name, 2 bytes per char
            for (uint32_t character = 0; character < 16; character++)
            {
                // get 4 bit nibbles into byte e.g. 04 07 into 47
                ascii_char = (ProcessingBuffer[read_index] << 4) | ProcessingBuffer[read_index + 1];
                read_index += 2;

                name_string[string_index++] = (char)ascii_char;
            }

            ESP_LOGI(TAG, "Got single Preset details");
        } break;

        case 0x10:
        {
            // unknown
            ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&ProcessingBuffer[read_index], write_index - read_index, ESP_LOG_INFO);

            ESP_LOGI(TAG, "Got unknown 0x10");

            // example response
            // f0 07 0f  00 01 00 00  04 0d  01 02 
            // 01 00 00 01 00 01 00 04 00 00 00 06 00 00 00 01 00 
            // 01 00 02 00 01 00 04 00 00 00 01 00 00 00 00 00 00 
            // 00 02 00 02 00 04 00 00 02 0a 00 00 00 00 00 00 00 
            // 01 00 02 00 01 00 00 00 00 00 03 00 02 00 01 00 00 
            // 06 04 00 01 00 03 00 01 00 00 0f 0e 00 01 00 04 00 
            // 01 00 00 00 00 00 02 00 04 00 01 00 00 00 00 00 03 
            // 00 04 00 01 00 00 00 01 00 04 00 04 00 01 00 00 00
            // 01 00 05 00 04 00 01 00 00 00 00 00 03 00 03 00 01 
            // 00 00 00 00 00 07 00 04 00 02 00 00 00 00 00 00 f7
        } break;

        case 0x1B:
        {
            // preset changed confirmation it seems
            ESP_LOGI(TAG, "Preset changed");

            // update
            usb_valeton_gp5_request_current_preset();
        } break;

        case 0x20:
        {
            // unknown, possibly parameters
            ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&ProcessingBuffer[read_index], write_index - read_index, ESP_LOG_INFO);

            ESP_LOGI(TAG, "Got unknown 0x20");

            // contains the text "User" numerous times

            // example response
            // 0000                                             f0 04
            // 0010   05 00 04 00 00 06 04 01 02 02 00 00 01 00 01 00
            // 0020   01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00
            // 0030   01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00
            // 0040   01 00 01 05 05 07 03 06 05 07 02 02 00 04 09 05
            // 0050   02 02 00 03 01 00 00 00 00 00 00 00 00 00 00 00
            // 0060   00 00 00 05 05 07 03 06 05 07 02 02 00 04 09 05
            // 0070   02 02 00 03 02 00 00 00 00 00 00 00 00 00 00 00
            // 0080   00 00 00 05 05 07 03 06 05 07 02 02 00 04 09 05
            // 0090   02 02 00 03 03 00 00 00 00 00 00 00 00 00 00 00
            // 00a0   00 00 00 05 05 07 03 06 05 07 02 02 00 04 09 05
            // 00b0   02 02 00 03 04 00 00 00 00 00 00 00 00 00 00 00
            // 00c0   00 00 00 05 05 07 03 06 05 07 02 02 00 04 09 05
            // 00d0   02 02 00 03 05 00 00 00 00 00 00 00 00 00 00 f7

            // 0000                                             f0 07
            // 0010   03 00 04 00 01 06 04 00 00 00 00 05 05 07 03 06
            // 0020   05 07 02 02 00 04 09 05 02 02 00 03 06 00 00 00
            // 0030   00 00 00 00 00 00 00 00 00 00 00 05 05 07 03 06
            // 0040   05 07 02 02 00 04 09 05 02 02 00 03 07 00 00 00
            // 0050   00 00 00 00 00 00 00 00 00 00 00 05 05 07 03 06
            // 0060   05 07 02 02 00 04 09 05 02 02 00 03 08 00 00 00
            // 0070   00 00 00 00 00 00 00 00 00 00 00 05 05 07 03 06
            // 0080   05 07 02 02 00 04 09 05 02 02 00 03 09 00 00 00
            // 0090   00 00 00 00 00 00 00 00 00 00 00 05 05 07 03 06
            // 00a0   05 07 02 02 00 04 09 05 02 02 00 03 01 03 00 00
            // 00b0   00 00 00 00 00 00 00 00 00 00 00 05 05 07 03 06
            // 00c0   05 07 02 02 00 04 09 05 02 02 00 03 01 03 01 00
            // 00d0   00 00 00 00 00 00 00 00 00 00 00 05 05 07 03 f7

            // 0000                                             f0 0d
            // 0010   01 00 04 00 02 06 04 06 05 07 02 02 00 04 09 05
            // 0020   02 02 00 03 01 03 02 00 00 00 00 00 00 00 00 00
            // 0030   00 00 00 05 05 07 03 06 05 07 02 02 00 04 09 05
            // 0040   02 02 00 03 01 03 03 00 00 00 00 00 00 00 00 00
            // 0050   00 00 00 05 05 07 03 06 05 07 02 02 00 04 09 05
            // 0060   02 02 00 03 01 03 04 00 00 00 00 00 00 00 00 00
            // 0070   00 00 00 05 05 07 03 06 05 07 02 02 00 04 09 05
            // 0080   02 02 00 03 01 03 05 00 00 00 00 00 00 00 00 00
            // 0090   00 00 00 05 05 07 03 06 05 07 02 02 00 04 09 05
            // 00a0   02 02 00 03 01 03 06 00 00 00 00 00 00 00 00 00
            // 00b0   00 00 00 05 05 07 03 06 05 07 02 02 00 04 09 05
            // 00c0   02 02 00 03 01 03 07 00 00 00 00 00 00 00 00 00
            // 00d0   00 00 00 05 05 07 03 06 05 07 02 02 00 04 09 f7

            // 0000                                             f0 07
            // 0010   01 00 04 00 03 02 0a 05 02 02 00 03 01 03 08 00
            // 0020   00 00 00 00 00 00 00 00 00 00 00 05 05 07 03 06
            // 0030   05 07 02 02 00 04 09 05 02 02 00 03 01 03 09 00
            // 0040   00 00 00 00 00 00 00 00 00 00 00 05 05 07 03 06
            // 0050   05 07 02 02 00 04 09 05 02 02 00 03 02 03 00 00
            // 0060   00 00 00 00 00 00 00 00 00 00 00 f7
        } break;

        case 0x24:
        {
            // unknown, some sort of user slots?
            ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&ProcessingBuffer[read_index], write_index - read_index, ESP_LOG_INFO);

            ESP_LOGI(TAG, "Got unknown 0x24");

            // contains text "OCD", "Reverb", "Sweet Drive", "Empty" etc

            // example response
            // 0000                                             f0 0d
            // 0010   06 00 0e 00 00 06 04 01 02 02 04 00 01 00 01 00
            // 0020   01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00
            // 0030   01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00
            // 0040   01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00
            // 0050   01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00
            // 0060   01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00
            // 0070   01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00
            // 0080   01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00
            // 0090   01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00
            // 00a0   01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00
            // 00b0   01 00 01 00 01 00 01 00 01 00 01 03 01 03 04 02
            // 00c0   00 04 04 05 03 05 04 00 00 00 00 00 00 00 00 00
            // 00d0   00 00 00 00 00 00 00 00 00 00 00 04 06 06 0f f7

            // 0000                                             f0 0a
            // 0010   0a 00 0e 00 01 06 04 07 02 06 03 06 05 02 00 04
            // 0020   0f 04 03 04 04 00 00 00 00 00 00 00 00 00 00 00
            // 0030   00 00 00 05 02 06 05 07 06 06 0f 06 0c 07 04 02
            // 0040   00 04 04 05 03 05 04 00 00 00 00 00 00 00 00 00
            // 0050   00 00 00 05 03 07 07 06 05 06 05 07 04 04 04 07
            // 0060   02 06 09 07 06 06 05 00 00 00 00 00 00 00 00 00
            // 0070   00 00 00 04 06 06 0c 06 01 06 07 06 0d 06 01 06
            // 0080   0e 04 04 05 03 05 04 00 00 00 00 00 00 00 00 00
            // 0090   00 00 00 04 06 06 0f 07 08 07 09 02 00 03 03 03
            // 00a0   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 00b0   00 00 00 05 04 07 07 06 09 06 0e 02 00 05 02 05
            // 00c0   06 04 02 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 00d0   00 00 00 04 0d 06 01 07 04 06 03 06 08 02 00 f7

            // 0000                                             f0 05
            // 0010   08 00 0e 00 02 06 04 03 03 03 00 00 00 00 00 00
            // 0020   00 00 00 00 00 00 00 00 00 00 00 04 0d 06 05 07
            // 0030   03 07 03 05 03 07 04 06 01 07 02 02 00 05 03 00
            // 0040   00 00 00 00 00 00 00 00 00 00 00 04 0d 06 05 07
            // 0050   03 07 03 04 0a 05 00 02 00 04 03 04 08 03 01 00
            // 0060   00 00 00 00 00 00 00 00 00 00 00 05 02 06 0f 06
            // 0070   03 06 0b 03 02 02 00 04 03 04 0c 00 00 00 00 00
            // 0080   00 00 00 00 00 00 00 00 00 00 00 04 0c 06 01 06
            // 0090   0e 07 09 02 00 04 0c 04 08 03 02 03 00 00 00 00
            // 00a0   00 00 00 00 00 00 00 00 00 00 00 05 03 05 05 05
            // 00b0   00 04 04 07 05 06 01 06 0c 00 00 00 00 00 00 00
            // 00c0   00 00 00 00 00 00 00 00 00 00 00 04 02 04 0a 03
            // 00d0   03 02 00 04 03 04 0c 02 00 00 00 00 00 00 00 f7

            // 0000                                             f0 02
            // 0010   0d 00 0e 00 03 06 04 00 00 00 00 00 00 00 00 00
            // 0020   00 00 00 05 05 04 0b 02 00 04 02 04 02 02 00 04
            // 0030   03 04 0c 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0040   00 00 00 04 02 04 0a 03 03 02 00 04 0f 04 04 00
            // 0050   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0060   00 00 00 05 05 04 0b 02 00 04 02 04 02 02 00 04
            // 0070   0f 04 04 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0080   00 00 00 05 05 04 0b 02 00 03 04 03 01 03 00 02
            // 0090   00 04 0f 04 04 00 00 00 00 00 00 00 00 00 00 00
            // 00a0   00 00 00 05 02 06 0f 06 03 06 0b 03 02 02 00 04
            // 00b0   0f 04 04 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 00c0   00 00 00 04 08 05 07 03 01 03 00 03 00 00 00 00
            // 00d0   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 f7

            // 0000                                             f0 0d
            // 0010   06 00 0e 00 04 06 04 00 00 00 00 04 0a 07 05 06
            // 0020   09 06 03 06 05 02 00 04 03 05 02 02 00 04 0e 00
            // 0030   00 00 00 00 00 00 00 00 00 00 00 04 06 06 0f 07
            // 0040   08 07 09 02 00 03 03 03 00 04 0a 04 0d 04 09 00
            // 0050   00 00 00 00 00 00 00 00 00 00 00 04 02 06 0f 06
            // 0060   07 02 00 04 04 05 03 05 04 00 00 00 00 00 00 00
            // 0070   00 00 00 00 00 00 00 00 00 00 00 04 03 05 06 02
            // 0080   00 05 08 05 06 00 00 00 00 00 00 00 00 00 00 00
            // 0090   00 00 00 00 00 00 00 00 00 00 00 04 0d 06 05 07
            // 00a0   03 07 03 04 0a 05 00 02 00 04 03 04 08 03 02 00
            // 00b0   00 00 00 00 00 00 00 00 00 00 00 04 0d 06 05 07
            // 00c0   03 07 03 04 0a 05 00 02 00 04 03 04 08 03 03 00
            // 00d0   00 00 00 00 00 00 00 00 00 00 00 05 05 04 0b f7

            // 0000                                             f0 0f
            // 0010   01 00 0e 00 05 06 04 02 00 03 08 03 00 03 00 00
            // 0020   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0030   00 00 00 05 05 04 0b 02 00 05 03 04 0c 05 00 00
            // 0040   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0050   00 00 00 05 05 04 0b 03 04 03 01 03 00 02 00 04
            // 0060   04 05 03 05 04 03 01 00 00 00 00 00 00 00 00 00
            // 0070   00 00 00 05 05 04 0b 03 04 03 01 03 00 02 00 04
            // 0080   04 05 03 05 04 03 02 00 00 00 00 00 00 00 00 00
            // 0090   00 00 00 05 05 04 0b 02 00 03 09 03 00 03 00 00
            // 00a0   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 00b0   00 00 00 05 05 04 0b 02 00 03 02 03 00 03 00 03
            // 00c0   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 00d0   00 00 00 05 05 04 0b 02 00 04 04 05 03 04 0c f7

            // 0000                                             f0 0d
            // 0010   02 00 0e 00 06 06 04 00 00 00 00 00 00 00 00 00
            // 0020   00 00 00 00 00 00 00 00 00 00 00 04 04 06 09 07
            // 0030   0a 07 0a 02 00 05 06 04 08 00 00 00 00 00 00 00
            // 0040   00 00 00 00 00 00 00 00 00 00 00 04 0d 06 05 07
            // 0050   03 07 03 02 00 05 04 07 02 06 09 05 06 00 00 00
            // 0060   00 00 00 00 00 00 00 00 00 00 00 04 0d 06 05 07
            // 0070   03 07 03 02 00 05 04 07 02 06 09 04 0d 00 00 00
            // 0080   00 00 00 00 00 00 00 00 00 00 00 04 0d 06 05 07
            // 0090   03 07 03 02 00 03 02 04 03 02 0b 00 00 00 00 00
            // 00a0   00 00 00 00 00 00 00 00 00 00 00 04 05 06 01 06
            // 00b0   07 06 0c 06 05 02 00 04 09 07 02 06 0f 06 0e 00
            // 00c0   00 00 00 00 00 00 00 00 00 00 00 04 08 02 06 04
            // 00d0   0b 02 00 04 02 04 0c 04 0b 03 02 03 00 03 00 f7

            // 0000                                             f0 00
            // 0010   0b 00 0e 00 07 06 04 00 00 00 00 00 00 00 00 00
            // 0020   00 00 00 04 0a 07 05 06 09 06 03 06 05 04 03 05
            // 0030   02 04 0d 04 01 05 08 00 00 00 00 00 00 00 00 00
            // 0040   00 00 00 04 01 04 07 04 0c 02 00 04 04 04 02 02
            // 0050   00 04 02 05 03 00 00 00 00 00 00 00 00 00 00 00
            // 0060   00 00 00 04 01 04 0d 05 00 04 07 02 00 03 06 02
            // 0070   00 04 02 05 03 00 00 00 00 00 00 00 00 00 00 00
            // 0080   00 00 00 04 05 04 02 02 00 04 06 06 01 06 06 02
            // 0090   00 04 02 05 03 00 00 00 00 00 00 00 00 00 00 00
            // 00a0   00 00 00 04 08 04 01 04 03 04 0b 02 00 04 02 05
            // 00b0   03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 00c0   00 00 00 05 00 05 06 02 00 04 02 05 03 00 00 00
            // 00d0   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 f7

            // 0000                                             f0 06
            // 0010   00 00 0e 00 08 06 04 00 00 00 00 04 0d 04 01 05
            // 0020   04 05 04 02 00 04 02 05 03 00 00 00 00 00 00 00
            // 0030   00 00 00 00 00 00 00 00 00 00 00 04 08 02 06 04
            // 0040   0b 02 00 04 02 05 03 00 00 00 00 00 00 00 00 00
            // 0050   00 00 00 00 00 00 00 00 00 00 00 04 0a 07 05 06
            // 0060   09 06 03 06 05 02 00 04 0f 04 04 04 02 05 03 00
            // 0070   00 00 00 00 00 00 00 00 00 00 00 04 01 04 03 02
            // 0080   00 05 03 04 09 04 0d 00 00 00 00 00 00 00 00 00
            // 0090   00 00 00 00 00 00 00 00 00 00 00 05 00 06 09 06
            // 00a0   05 07 0a 06 0f 02 00 05 03 04 09 04 0d 00 00 00
            // 00b0   00 00 00 00 00 00 00 00 00 00 00 04 05 06 0d 07
            // 00c0   00 07 04 07 09 00 00 00 00 00 00 00 00 00 00 00
            // 00d0   00 00 00 00 00 00 00 00 00 00 00 04 05 06 0d f7

            // 0000                                             f0 0f
            // 0010   03 00 0e 00 09 06 04 07 00 07 04 07 09 00 00 00
            // 0020   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0030   00 00 00 04 05 06 0d 07 00 07 04 07 09 00 00 00
            // 0040   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0050   00 00 00 04 05 06 0d 07 00 07 04 07 09 00 00 00
            // 0060   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0070   00 00 00 04 05 06 0d 07 00 07 04 07 09 00 00 00
            // 0080   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0090   00 00 00 04 05 06 0d 07 00 07 04 07 09 00 00 00
            // 00a0   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 00b0   00 00 00 04 05 06 0d 07 00 07 04 07 09 00 00 00
            // 00c0   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 00d0   00 00 00 04 05 06 0d 07 00 07 04 07 09 00 00 f7

            // 0000                                             f0 07
            // 0010   0e 00 0e 00 0a 06 04 00 00 00 00 00 00 00 00 00
            // 0020   00 00 00 00 00 00 00 00 00 00 00 04 05 06 0d 07
            // 0030   00 07 04 07 09 00 00 00 00 00 00 00 00 00 00 00
            // 0040   00 00 00 00 00 00 00 00 00 00 00 04 05 06 0d 07
            // 0050   00 07 04 07 09 00 00 00 00 00 00 00 00 00 00 00
            // 0060   00 00 00 00 00 00 00 00 00 00 00 04 05 06 0d 07
            // 0070   00 07 04 07 09 00 00 00 00 00 00 00 00 00 00 00
            // 0080   00 00 00 00 00 00 00 00 00 00 00 04 05 06 0d 07
            // 0090   00 07 04 07 09 00 00 00 00 00 00 00 00 00 00 00
            // 00a0   00 00 00 00 00 00 00 00 00 00 00 04 05 06 0d 07
            // 00b0   00 07 04 07 09 00 00 00 00 00 00 00 00 00 00 00
            // 00c0   00 00 00 00 00 00 00 00 00 00 00 04 05 06 0d 07
            // 00d0   00 07 04 07 09 00 00 00 00 00 00 00 00 00 00 f7

            // 0000                                             f0 0d
            // 0010   09 00 0e 00 0b 06 04 00 00 00 00 00 00 00 00 00
            // 0020   00 00 00 04 05 06 0d 07 00 07 04 07 09 00 00 00
            // 0030   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0040   00 00 00 04 05 06 0d 07 00 07 04 07 09 00 00 00
            // 0050   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0060   00 00 00 04 05 06 0d 07 00 07 04 07 09 00 00 00
            // 0070   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0080   00 00 00 04 05 06 0d 07 00 07 04 07 09 00 00 00
            // 0090   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 00a0   00 00 00 04 05 06 0d 07 00 07 04 07 09 00 00 00
            // 00b0   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 00c0   00 00 00 04 05 06 0d 07 00 07 04 07 09 00 00 00
            // 00d0   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 f7

            // 0000                                             f0 05
            // 0010   09 00 0e 00 0c 06 04 00 00 00 00 04 05 06 0d 07
            // 0020   00 07 04 07 09 00 00 00 00 00 00 00 00 00 00 00
            // 0030   00 00 00 00 00 00 00 00 00 00 00 04 05 06 0d 07
            // 0040   00 07 04 07 09 00 00 00 00 00 00 00 00 00 00 00
            // 0050   00 00 00 00 00 00 00 00 00 00 00 04 05 06 0d 07
            // 0060   00 07 04 07 09 00 00 00 00 00 00 00 00 00 00 00
            // 0070   00 00 00 00 00 00 00 00 00 00 00 04 05 06 0d 07
            // 0080   00 07 04 07 09 00 00 00 00 00 00 00 00 00 00 00
            // 0090   00 00 00 00 00 00 00 00 00 00 00 04 05 06 0d 07
            // 00a0   00 07 04 07 09 00 00 00 00 00 00 00 00 00 00 00
            // 00b0   00 00 00 00 00 00 00 00 00 00 00 04 05 06 0d 07
            // 00c0   00 07 04 07 09 00 00 00 00 00 00 00 00 00 00 00
            // 00d0   00 00 00 00 00 00 00 00 00 00 00 04 05 06 0d f7

            // 0000                                             f0 0a
            // 0010   02 00 0e 00 0d 03 0e 07 00 07 04 07 09 00 00 00
            // 0020   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0030   00 00 00 04 05 06 0d 07 00 07 04 07 09 00 00 00
            // 0040   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0050   00 00 00 04 05 06 0d 07 00 07 04 07 09 00 00 00
            // 0060   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0070   00 00 00 04 05 06 0d 07 00 07 04 07 09 00 00 00
            // 0080   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0090   00 00 00 f7

        } break;

        case 0x40:
        {
            // preset data
            ESP_LOGI(TAG, "Found Preset Data");

            for (uint32_t presets = 0; presets < MAX_PRESETS_VALETON_GP5; presets++)
            {
                // get the preset index
                preset_index = (ProcessingBuffer[read_index] * 16) + ProcessingBuffer[read_index + 1];
                read_index += 2;

                // debug
                //ESP_LOGI(TAG, "Preset Index: %d %d", (int)ProcessingBuffer[read_index - 2], (int)ProcessingBuffer[read_index - 1]);

                // skip 6 bytes of zeros
                read_index += 6;
                string_index = 0;

                // next is 32 bytes/16 characters of preset name, 2 bytes per char
                for (uint32_t character = 0; character < 16; character++)
                {
                    // get 4 bit nibbles into byte e.g. 04 07 into 47
                    ascii_char = (ProcessingBuffer[read_index] << 4) | ProcessingBuffer[read_index + 1];
                    read_index += 2;

                    name_string[string_index++] = (char)ascii_char;
                }
                            
                // save it
                control_sync_preset_name(preset_index, name_string);

                // don't smash the control input queue too hard
                vTaskDelay(10);
            }

            control_set_sync_complete();

            // request the preset
            usb_valeton_gp5_request_current_preset();
            
            ValetonGP5Data->State = COMMS_STATE_READY;
        } break;
        
        case 0x41: 
        {
            // current preset parameters?
            ESP_LOGI(TAG, "Current Preset params?");

            // debug
            //ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&ProcessingBuffer[read_index], write_index - read_index, ESP_LOG_INFO);

            // skip the 0x0F 0x0F bytes
            read_index += 2;

            // suspected 16 byte chunks per parameter. 99 x 16 byte chunks are found. 99 is number of presets, maybe this is not params?
#define MESSAGE_LENGTH 34 

            for (size_t i = read_index; i <= write_index - read_index - MESSAGE_LENGTH; i += MESSAGE_LENGTH) 
            {
                uint8_t byte12 = ProcessingBuffer[i + 12]; // Effect block
                uint8_t byte23 = ProcessingBuffer[i + 23]; // Parameter index or type indicator
                uint8_t byte24 = ProcessingBuffer[i + 24]; // For reverb_type/reverb_ok
                uint8_t byte30 = ProcessingBuffer[i + 30]; // Coarse value
                uint8_t byte31 = ProcessingBuffer[i + 31]; // Fine value
                uint8_t byte32 = ProcessingBuffer[i + 32]; // Parameter type/range
                uint8_t byte33 = ProcessingBuffer[i + 33]; // Range indicator

                //if (byte12 == 0x04) 
                { 
                    // Check for valid message pattern
                    if (byte23 == 0x00 && (byte32 == 0x00 || byte32 == 0x04)) 
                    { 
                        // delay_mix, delay_feedback, reverb_mix, reverb_decay
                        float value = usb_valeton_gp5_decode_percentage(byte30, byte31, byte33);
                        if (value >= 0.0f) 
                        {
                            ESP_LOGI(TAG, "Percentage Parameter (Mix/Feedback/Decay) at index %zu: %.2f%%", i, value);
                        } 
                        else 
                        {
                            ESP_LOGI(TAG, "Invalid Percentage Parameter at index %zu: %02x %02x %02x %02x", i, byte30, byte31, byte32, byte33);
                        }
                    } 
                    else if (byte23 == 0x01 && byte32 == 0x00) 
                    { 
                        // delay_time
                        float value = usb_valeton_gp5_decode_time(byte30, byte31, byte33);
                        if (value >= 0.0f) 
                        {
                            ESP_LOGI(TAG, "Delay Time at index %zu: %.0fms", i, value);
                        } 
                        else 
                        {
                            ESP_LOGI(TAG,  "Invalid Delay Time at index %zu: %02x %02x %02x %02x", i, byte30, byte31, byte32, byte33);
                        }
                    } 
                    else if (byte23 == 0x01 && (byte32 == 0x0c || byte32 == 0x0b)) 
                    { 
                        // reverb_type, reverb_ok
                        const char *value = usb_valeton_gp5_decode_type_or_onoff(byte24, byte32);
                        ESP_LOGI(TAG, "Reverb Type/OnOff at index %zu: %s", i, value);
                    } 
                    else if (byte23 == 0x02 && (byte32 == 0x04 || byte32 == 0x0c || byte32 == 0x04)) 
                    { 
                        // EQ Level
                        float value = usb_valeton_gp5_decode_level(byte30, byte31, byte32, byte33);
                        if (value >= -50.0f && value <= 50.0f) 
                        {
                            ESP_LOGI(TAG, "EQ Level at index %zu: %.2f", i, value);
                        } 
                        else 
                        {
                            ESP_LOGI(TAG, "Invalid EQ Level at index %zu: %02x %02x %02x %02x", i, byte30, byte31, byte32, byte33);
                        }
                    } 
                    else 
                    {
                        ESP_LOGI(TAG, "Unknown Parameter at index %zu: byte12=%02x, byte23=%02x, value=%02x %02x %02x %02x", i, byte12, byte23, byte30, byte31, byte32, byte33);
                    }
                }
            }
    



            // example response
            // I (9019) app_Valeton_GP5: 0x3c3d4730   0f 0f 00 00 01 00 00 00  00 01 00 00 00 04 00 00  |................|
            // I (9026) app_Valeton_GP5: 0x3c3d4740   00 01 00 00 00 00 00 00  00 02 00 00 00 04 00 00  |................|
            // I (9035) app_Valeton_GP5: 0x3c3d4750   00 0a 04 05 04 0d 05 01  00 00 00 00 01 00 00 00  |................|
            // I (9045) app_Valeton_GP5: 0x3c3d4760   00 01 01 00 00 04 00 00  00 0a 00 00 00 00 00 00  |................|
            // I (9054) app_Valeton_GP5: 0x3c3d4770   00 02 01 00 00 04 00 00  00 08 00 00 00 00 00 00  |................|
            // I (9064) app_Valeton_GP5: 0x3c3d4780   00 01 00 00 01 00 00 00  00 01 02 00 00 04 00 00  |................|
            // I (9073) app_Valeton_GP5: 0x3c3d4790   03 02 00 00 00 00 00 00  00 02 02 00 00 04 00 00  |................|
            // I (9083) app_Valeton_GP5: 0x3c3d47a0   07 08 00 00 00 00 00 00  00 02 00 00 08 06 00 01  |................|
            // I (9092) app_Valeton_GP5: 0x3c3d47b0   00 01 03 00 00 04 00 00  01 0d 00 01 00 00 00 00  |................|
            // I (9102) app_Valeton_GP5: 0x3c3d47c0   00 02 03 00 00 0a 00 00  00 00 00 01 00 02 00 09  |................|
            // I (9111) app_Valeton_GP5: 0x3c3d47d0   00 03 00 04 00 05 00 06  00 07 00 08 00 03 03 00  |................|
            // I (9121) app_Valeton_GP5: 0x3c3d47e0   02 08 00 00 01 0b 00 00  00 00 00 00 02 01 00 00  |................|
            // I (9130) app_Valeton_GP5: 0x3c3d47f0   00 00 00 01 00 00 00 00  00 00 00 03 02 07 00 00  |................|
            // I (9139) app_Valeton_GP5: 0x3c3d4800   00 00 00 07 00 08 00 00  00 00 00 0a 03 05 00 00  |................|
            // I (9149) app_Valeton_GP5: 0x3c3d4810   00 00 00 01 01 09 00 00  00 00 00 04 00 00 00 00  |................|
            // I (9158) app_Valeton_GP5: 0x3c3d4820   00 00 00 0b 00 04 00 00  00 00 00 0c 00 00 00 00  |................|
            // I (9168) app_Valeton_GP5: 0x3c3d4830   00 00 00 0f 00 04 03 00  04 00 00 01 00 00 00 00  |................|
            // I (9177) app_Valeton_GP5: 0x3c3d4840   0d 00 04 01 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9187) app_Valeton_GP5: 0x3c3d4850   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9196) app_Valeton_GP5: 0x3c3d4860   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9206) app_Valeton_GP5: 0x3c3d4870   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9215) app_Valeton_GP5: 0x3c3d4880   0a 00 04 01 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9225) app_Valeton_GP5: 0x3c3d4890   07 00 04 02 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9234) app_Valeton_GP5: 0x3c3d48a0   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9244) app_Valeton_GP5: 0x3c3d48b0   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9253) app_Valeton_GP5: 0x3c3d48c0   0a 00 04 01 00 00 00 00  0a 00 04 02 00 00 00 00  |................|
            // I (9263) app_Valeton_GP5: 0x3c3d48d0   05 0c 04 02 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9272) app_Valeton_GP5: 0x3c3d48e0   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9281) app_Valeton_GP5: 0x3c3d48f0   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9291) app_Valeton_GP5: 0x3c3d4900   0a 00 04 01 00 00 00 00  02 00 04 01 00 00 00 00  |................|
            // I (9300) app_Valeton_GP5: 0x3c3d4910   04 08 04 02 00 00 00 00  02 00 04 02 00 00 00 00  |................|
            // I (9310) app_Valeton_GP5: 0x3c3d4920   0b 04 04 02 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9319) app_Valeton_GP5: 0x3c3d4930   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9329) app_Valeton_GP5: 0x3c3d4940   04 08 04 02 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9338) app_Valeton_GP5: 0x3c3d4950   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9348) app_Valeton_GP5: 0x3c3d4960   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9357) app_Valeton_GP5: 0x3c3d4970   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9367) app_Valeton_GP5: 0x3c3d4980   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9376) app_Valeton_GP5: 0x3c3d4990   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9385) app_Valeton_GP5: 0x3c3d49a0   00 00 00 00 00 00 00 00  04 08 04 02 00 00 00 00  |................|
            // I (9395) app_Valeton_GP5: 0x3c3d49b0   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9405) app_Valeton_GP5: 0x3c3d49c0   00 00 03 0f 00 00 00 00  00 00 03 0f 00 00 00 00  |................|
            // I (9414) app_Valeton_GP5: 0x3c3d49d0   04 08 04 02 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9423) app_Valeton_GP5: 0x3c3d49e0   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9433) app_Valeton_GP5: 0x3c3d49f0   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9442) app_Valeton_GP5: 0x3c3d4a00   0a 00 04 01 00 00 00 00  0f 0a 04 03 00 00 00 00  |................|
            // I (9452) app_Valeton_GP5: 0x3c3d4a10   0f 00 04 01 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9461) app_Valeton_GP5: 0x3c3d4a20   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9471) app_Valeton_GP5: 0x3c3d4a30   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9480) app_Valeton_GP5: 0x3c3d4a40   00 0c 04 02 00 00 00 00  0b 08 04 01 00 00 00 00  |................|
            // I (9490) app_Valeton_GP5: 0x3c3d4a50   04 08 04 02 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9499) app_Valeton_GP5: 0x3c3d4a60   04 08 04 02 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9508) app_Valeton_GP5: 0x3c3d4a70   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9518) app_Valeton_GP5: 0x3c3d4a80   04 08 04 02 00 00 00 00  04 08 04 02 00 00 00 00  |................|
            // I (9527) app_Valeton_GP5: 0x3c3d4a90   04 08 04 02 00 00 00 00  04 08 04 02 00 00 00 00  |................|
            // I (9537) app_Valeton_GP5: 0x3c3d4aa0   04 08 04 02 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            // I (9546) app_Valeton_GP5: 0x3c3d4ab0   00 00 00 00 00 00 00 00  00 00 00 00 00 03 00 00  |................|
            // I (9556) app_Valeton_GP5: 0x3c3d4ac0   00 08 00 00 00 04 00 00  00 00 00 00 00 07 00 00  |................|
            // I (9565) app_Valeton_GP5: 0x3c3d4ad0   00 00 00 00                                       |....|
        } break;

        case 0x43:
        {
            // current preset index
            //ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&ProcessingBuffer[read_index], write_index - read_index, ESP_LOG_INFO);
            uint8_t current_preset = (ProcessingBuffer[read_index] << 4) | ProcessingBuffer[read_index + 1];

            ESP_LOGI(TAG, "Got current preset index: %d", current_preset);

            // update UI
            control_sync_preset_details(current_preset, "");

            // request params
            usb_valeton_gp5_request_preset_params();
        } break;
        
        case 0x45:
        {
            // unknown. Seems to be some kind of confirmation response or short data
            ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&ProcessingBuffer[read_index], write_index - read_index, ESP_LOG_INFO);

            ESP_LOGI(TAG, "Got unknown 0x45");

            // example response
            //                                      f0 07
            //  0d 00 01 00 00 00 03 01 02 04 05 00 00 f7
        } break;

        case 0x50:
        {
            // unknown
            ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&ProcessingBuffer[read_index], write_index - read_index, ESP_LOG_INFO);

            ESP_LOGI(TAG, "Got unknown 0x50");

            // example response
            // 0000                                             f0 0b
            // 0010   07 00 01 00 00 03 08 01 02 05 00 00 03 00 00 00
            // 0020   09 00 00 00 03 00 00 00 05 00 00 00 00 00 00 00
            // 0030   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0040   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0050   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0060   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0070   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            // 0080   00 00 00 00 00 00 00 f7
        } break;

        default:
        {
            ESP_LOGI(TAG, "Unknown function type %X", function);
            ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)ProcessingBuffer, 16, ESP_LOG_WARN);
        } break;
    }

    return 1;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_request_preset_sync(void)
{
    uint8_t midi_tx[] = {0x04, 0x00};

    ESP_LOGI(TAG, "Request preset sync");

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x02);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_request_current_preset(void)
{
    uint8_t midi_tx[] = {0x04, 0x03};

    ESP_LOGI(TAG, "Request current preset");

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x02);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_request_preset_params(void)
{
    uint8_t midi_tx[] = {0x04, 0x01};

    ESP_LOGI(TAG, "Request preset params");    

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x02);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_request_unknown(void)
{
    uint8_t midi_tx[] = {0x01, 0x00};

    ESP_LOGI(TAG, "Request unknown");    

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x02);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_request_unknown_2(void)
{
    uint8_t midi_tx[] = {0x05, 0x00};

    ESP_LOGI(TAG, "Request unknown_2");    

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x02);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_request_unknown_3(void)
{
    uint8_t midi_tx[] = {0x04, 0x05};

    ESP_LOGI(TAG, "Request unknown_3");

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x02);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_request_unknown_4(void)
{
    uint8_t midi_tx[] = {0x02, 0x00};

    ESP_LOGI(TAG, "Request unknown_4");

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x02);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_request_unknown_5(void)
{
    uint8_t midi_tx[] = {0x02, 0x04};

    ESP_LOGI(TAG, "Request unknown_5");

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x02);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_get_preset(uint8_t index)
{
    uint8_t midi_tx[] = {0x04, 0x03, 0x00, 0x00, 0x00, 0x00};

    // set the index
    midi_tx[2] = index >> 4;
    midi_tx[3] = index & 0x0F;

    ESP_LOGI(TAG, "Get preset %d", index);

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x01);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_set_preset(uint8_t index)
{
    ESP_LOGI(TAG, "Set preset %d", index);

    // send control change
    uint8_t midi_tx[4];

    midi_tx[0] = 0x0B;
    midi_tx[1] = 0xB0;
    midi_tx[2] = 0x00;
    midi_tx[3] = index;

    midi_host_data_tx_blocking(midi_dev, (const uint8_t*)midi_tx, sizeof(midi_tx), 50);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static bool usb_valeton_gp5_handle_rx(const uint8_t* data, size_t data_len, void* arg)
{
    // debug
    //ESP_LOGI(TAG, "CDC Data received %d", (int)data_len);
    //ESP_LOG_BUFFER_HEXDUMP(TAG, data, data_len, ESP_LOG_INFO);

    if (data_len > VALETON_GP5_RX_TEMP_BUFFER_SIZE)
    {
        ESP_LOGE(TAG, "usb_valeton_gp5_handle_rx data too long! %d", (int)data_len);
        return false;
    }
    else
    {
        // locate a buffer to put it
        for (uint8_t loop = 0; loop < MAX_INPUT_BUFFERS; loop++)
        {
            if (InputBuffers[loop].ReadyToWrite == 1)
            {
                // grab data
                memcpy((void*)InputBuffers[loop].Data, (void*)data, data_len);

                // set buffer as used
                InputBuffers[loop].Length = data_len;
                InputBuffers[loop].ReadyToWrite = 0;
                InputBuffers[loop].ReadyToRead = 1;      
                
                // debug
                //ESP_LOGI(TAG, "CDC Data buffered into %d", (int)loop);

                return true;
            }
        }
    }

    ESP_LOGE(TAG, "usb_valeton_gp5_handle_rx no available buffers!");
    return false;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void usb_valeton_gp5_handle(class_driver_t* driver_obj)
{        
    tUSBMessage message;
    tUSBMessage next_message;

    // check state
    switch (ValetonGP5Data->State)
    {
        case COMMS_STATE_IDLE:
        default:
        {
            usb_valeton_gp5_request_preset_sync();

#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
            // show sync message
            UI_SetPresetLabel(0, "Syncing....");
#endif

            ValetonGP5Data->State = COMMS_STATE_SYNC;
        } break;

        case COMMS_STATE_SYNC:
        {
            // waiting for Sync to complete
        } break;

        case COMMS_STATE_READY:
        {
            // check for any input messages
            if (xQueueReceive(input_queue, (void*)&message, 0) == pdPASS)
            {
                ESP_LOGI(TAG, "Got Input message: %d. Queue: %d", message.Command, uxQueueMessagesWaiting(input_queue));

                // check if the next commands in the input queue would change the same item as this message.
                // if so, no point in processing as it will be overwritten shortly.
                while (uxQueueMessagesWaiting(input_queue) != 0)
                {
                    // get a copy of the next message without removing it from the queue
                    if (xQueuePeek(input_queue, (void*)&next_message, 0) == pdPASS)
                    {
                        // check what it is
                        if (((next_message.Command == USB_COMMAND_MODIFY_PARAMETER) && (next_message.Payload == message.Payload))
                          || (next_message.Command == USB_COMMAND_SET_PRESET)) 
                        {
                            // don't send the current mesage. Instead, receive this next one properly and pull it off the queue
                            // so it can be processed (or overwritten again by another message in the queue still)
                            xQueueReceive(input_queue, (void*)&message, 0);

                            // debug
                            //ESP_LOGW(TAG, "Input message consumed");
                        }
                        else
                        {
                            // this next message is not the same, exit loop and send it
                            break;
                        }
                    }
                    else
                    {
                        // something went wrong with the Peek, don't get stuck in loop
                        break;
                    }
                }

                // process it
                switch (message.Command)
                {
                    case USB_COMMAND_SET_PRESET:
                    {
                        if (message.Payload < MAX_PRESETS_VALETON_GP5)
                        {
                            usb_valeton_gp5_set_preset(message.Payload);
                        }
                    } break;
                    
                    case USB_COMMAND_LOAD_PRESET_TO_SLOT_A:
                    case USB_COMMAND_LOAD_PRESET_TO_SLOT_B:
                    {
                        // unsupported
                    } break;   
                    
                    case USB_COMMAND_MODIFY_PARAMETER:
                    {
                        if (message.Payload < TONEX_PARAM_LAST)
                        {
                        }
                        else if (message.Payload < TONEX_GLOBAL_LAST)
                        {
                            // modify the global
                        }
                        else
                        {
                            ESP_LOGW(TAG, "Attempt to modify unknown param %d", (int)message.Payload);
                        }
                    } break;
                }
            }
        } break;
    }

    // check if we have received anything (via RX interrupt)
    for (uint8_t loop = 0; loop < MAX_INPUT_BUFFERS; loop++)
    {
        if (InputBuffers[loop].ReadyToRead)
        {
            ESP_LOGI(TAG, "Got data via Midi %d", InputBuffers[loop].Length);

            // debug
            //ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)InputBuffers[loop].Data, InputBuffers[loop].Length, ESP_LOG_INFO);

            uint8_t* rx_entry_ptr = (uint8_t*)InputBuffers[loop].Data;
            uint16_t rx_entry_length = InputBuffers[loop].Length;

            usb_valeton_gp5_parse_sysex(rx_entry_ptr, rx_entry_length);

            // set buffer as available       
            InputBuffers[loop].ReadyToRead = 0;
            InputBuffers[loop].ReadyToWrite = 1;   

            vTaskDelay(pdMS_TO_TICKS(2)); 
        } 
    }

    vTaskDelay(pdMS_TO_TICKS(2));
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void usb_valeton_gp5_init(class_driver_t* driver_obj, QueueHandle_t comms_queue)
{
    // save the queue handle
    input_queue = comms_queue;

    // allocate RX buffers in PSRAM
    InputBuffers = heap_caps_malloc(sizeof(tInputBufferEntry) * MAX_INPUT_BUFFERS, MALLOC_CAP_SPIRAM);
    if (InputBuffers == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate input buffers!");
        return;
    }

    ProcessingBuffer = heap_caps_malloc(VALETON_GP5_RX_TEMP_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
    if (ProcessingBuffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate ProcessingBuffer!");
        return;
    }

    
    TransmitBuffer = heap_caps_malloc(VALETON_GP5_TX_TEMP_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
    if (TransmitBuffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate TransmitBuffer!");
        return;
    }

    // set all buffers as ready for writing
    for (uint8_t loop = 0; loop < MAX_INPUT_BUFFERS; loop++)
    {
        memset((void*)InputBuffers[loop].Data, 0, VALETON_GP5_RX_TEMP_BUFFER_SIZE);
        InputBuffers[loop].ReadyToWrite = 1;
        InputBuffers[loop].ReadyToRead = 0;
    }

    // more big buffers in PSRAM
    TxBuffer = heap_caps_malloc(VALETON_GP5_RX_TEMP_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
    if (TxBuffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate TxBuffer buffer!");
        return;
    }
    
    ValetonGP5Data = heap_caps_malloc(sizeof(tValetonGP5Data), MALLOC_CAP_SPIRAM);
    if (ValetonGP5Data == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate ValetonGP5Data buffer!");
        return;
    }

    memset((void*)ValetonGP5Data, 0, sizeof(tValetonGP5Data));
    ValetonGP5Data->State = COMMS_STATE_IDLE;

    // install Midi host driver
    ESP_ERROR_CHECK(midi_host_install(NULL));
    ESP_LOGI(TAG, "Opening Midi device 0x%04X:0x%04X", VALETON_USB_VENDOR, VALETON_GP5_PRODUCT_ID);

    // set the config
    const midi_host_device_config_t dev_config = {
        .connection_timeout_ms = 1000,
        .out_buffer_size = VALETON_GP5_USB_TX_BUFFER_SIZE,
        .in_buffer_size = VALETON_GP5_RX_TEMP_BUFFER_SIZE,
        .user_arg = NULL,
        .event_cb = NULL,
        .data_cb = usb_valeton_gp5_handle_rx
    };

    // release the reserved large buffers space we malloc'd at boot
    tonex_common_release_memory();

    // let the pedal boot up
    vTaskDelay(pdMS_TO_TICKS(1000));

    // open it
    ESP_ERROR_CHECK(midi_host_open(VALETON_USB_VENDOR, VALETON_GP5_PRODUCT_ID, VALETON_GP5_MIDI_INTERFACE_INDEX, &dev_config, &midi_dev));
    assert(midi_dev);
    
    //midi_host_desc_print(midi_dev);

    // update UI
    control_set_usb_status(1);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void usb_valeton_gp5_deinit(void)
{
    //to do here: need to clean up properly if pedal disconnected
    //cdc_acm_host_close();
}
