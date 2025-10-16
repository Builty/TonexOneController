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
#define VALETON_GP5_USB_TX_BUFFER_SIZE              128

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
static uint8_t boot_init_needed = 0;
static uint8_t boot_global_request = 0;
static uint8_t boot_preset_request = 0;
static volatile tInputBufferEntry* InputBuffers;
static uint8_t* ProcessingBuffer;

/*
** Static function prototypes
*/
static void usb_valeton_gp5_request_preset_sync(void);
static void usb_valeton_gp5_request_current_preset(void);

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:  credit to https://rvalladares.com/ for this     
*****************************************************************************/
static uint8_t usb_valeton_gp5_crc8(const uint8_t* buffer, uint32_t len) 
{
    uint8_t crc = 0x00; 
    uint8_t cur;
    
    for (uint32_t  i = 0; i < len; i++) 
    {
        cur = buffer[i] & 0xFF;
        crc ^= cur;

        for (uint32_t j = 0; j < 8; j++) 
        {
            if ((crc & 0x80) != 0) 
            {
                crc = ((crc << 1) ^ 0x07) & 0xFF;
            } 
            else 
            {
                crc = (crc << 1) & 0xFF;
            }
        }
    }
    
    return crc & 0xFF; 
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
    uint32_t bytes_processed = 0;
    uint16_t crc;
    uint16_t preset_index = 0;
    uint16_t vendor_id;
    uint32_t packet_length;
    uint8_t string_index;
    uint16_t ascii_char;
    uint8_t* tmp_ptr = (uint8_t*)buffer;
    uint16_t message_type = 0;
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

                // next 3 bytes are unknown, skip all
                tmp_ptr += 3;
                bytes_read += 3;

                // skip the 0x04 and next 00
                tmp_ptr += 2;
                bytes_read += 2;

                // grab next 2 bytes (message type)
                if (!found_message)
                {
                    message_type = (*tmp_ptr << 8) | *(tmp_ptr + 1);
                    found_message = 1;
                }
                tmp_ptr += 2;
                bytes_read += 2;

                //ESP_LOGE(TAG, "Chunk type %X %X", (int)ProcessingBuffer[write_index - 2], (int)ProcessingBuffer[write_index - 1]);

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
                ESP_LOGE(TAG, "Processing error");
                break;
            }
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
                ESP_LOGE(TAG, "Processing error");
                break;
            }
        }

    }

    // debug
    //ESP_LOG_BUFFER_HEXDUMP(TAG, ProcessingBuffer, write_index, ESP_LOG_INFO);
    //vTaskDelay(100);

    // check the message type
    read_index = 0;
   
    switch (message_type)
    {
        case 0x0103:
        {
            // preset data
            // skip the 0102 marker that signifies GP5 -> Host
            read_index += 2;

            uint16_t sub_type = (ProcessingBuffer[read_index] << 8) | ProcessingBuffer[read_index + 1];
            read_index += 2;

            ESP_LOGI(TAG, "Found Preset Data: %d", sub_type);

            if (sub_type == 1024)
            {
                for (uint32_t presets = 0; presets < MAX_PRESETS_VALETON_GP5; presets++)
                {
                    // get the preset index (2 nibbles)
                    preset_index = (ProcessingBuffer[read_index] * 16) + ProcessingBuffer[read_index + 1];
                    read_index += 2;

                    // debug
                    //ESP_LOGI(TAG, "Preset Index: %d %d", (int)ProcessingBuffer[read_index - 2], (int)ProcessingBuffer[read_index - 1]);

                    // debug
                    //if (preset_index == 14)
                    //{
                    //    ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&ProcessingBuffer[read_index - 16], 64, ESP_LOG_INFO);
                    //}

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

                // request the preset details
                usb_valeton_gp5_request_current_preset();

                ValetonGP5Data->State = COMMS_STATE_READY;
            }
            else if (sub_type == 1025) 
            {
                // current preset details
                // skip the 0102 marker that signifies GP5 -> Host
                read_index += 2;

                ESP_LOGI(TAG, "Current Preset details");

                // debug
                ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&ProcessingBuffer[read_index], write_index - read_index, ESP_LOG_INFO);
            }
            else
            {
                ESP_LOGI(TAG, "Unknown Preset details sub-type %X", sub_type);
            }
        } break;

        case 0x0006:
        {
            // skip the 0102 marker that signifies GP5 -> Host
            read_index += 2;

            // debug
            ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&ProcessingBuffer[read_index], write_index - read_index, ESP_LOG_INFO);
        
            ESP_LOGI(TAG, "Preset changed?");

            // request the preset details
            usb_valeton_gp5_request_current_preset();
        } break;

        case 0x0604:
        {
            // skip the 0102 marker that signifies GP5 -> Host
            read_index += 2;

            // debug
            ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&ProcessingBuffer[read_index], write_index - read_index, ESP_LOG_INFO);
        
            ESP_LOGI(TAG, "Preset details?");
        } break;

        case 0x0002:
        {
            // skip the 0102 marker that signifies GP5 -> Host
            read_index += 2;

            // debug
            ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&ProcessingBuffer[read_index], write_index - read_index, ESP_LOG_INFO);
        
            ESP_LOGI(TAG, "Preset request?");
        } break;

        default:
        {
            ESP_LOGW(TAG, "Message Type: %d", (int)message_type);
        } break;
    };

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
    uint8_t midi_tx[] = {0x04, 0xf0, 0x00, 0x0e, 
                         0x04, 0x00, 0x01, 0x00, 
                         0x04, 0x00, 0x00, 0x02, 
                         0x04, 0x01, 0x02, 0x04, 
                         0x06, 0x00, 0xf7, 0x00};

    if (midi_host_data_tx_blocking(midi_dev, (const uint8_t*)midi_tx, sizeof(midi_tx), 50) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send preset sync request");
    }
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
    // BT log f0 00 09 00 01 00 00 00 02 01 02 04 01 f7
    uint8_t midi_tx[] = {0x04, 0xf0, 0x00, 0x09, 
                         0x04, 0x00, 0x01, 0x00, 
                         0x04, 0x00, 0x00, 0x02, 
                         0x04, 0x01, 0x02, 0x04, 
                         0x06, 0x01, 0xf7, 0x00};

    if (midi_host_data_tx_blocking(midi_dev, (const uint8_t*)midi_tx, sizeof(midi_tx), 50) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send usb_valeton_gp5_request_current_preset");
    }
    
    // params??
    // response BT:  f0 03 0d 00 05 00 00 06 04 01 02 04 01 0f 0f 00 00 01
    //  00 00 00 00 01 00 00 00 04 00 00 00 01 00 00 00
    //  00 00 00 00 02 00 00 00 04 00 00 00 0a 04 05 04
    //  0d 05 01 00 00 00 00 01 00 00 00 00 01 01 00 00
    //  04 00 00 00 0a 00 00 00 00 00 00 00 02 01 00 00
    //  04 00 00 00 08 00 00 00 00 00 00 00 01 00 00 01
    //  00 00 00 00 01 02 00 00 04 00 00 03 02 00 00 00
    //  00 00 00 00 02 02 00 00 04 00 00 07 08 00 00 00
    //  00 00 00 00 02 00 00 08 06 00 01 00 01 03 00 00
    //  04 00 00 03 0f 00 01 00 00 00 00 00 02 03 00 00
    //  0a 00 00 00 00 00 01 00 02 00 09 00 03 00 04 00
    //  05 00 06 00 07 00 08 00 03 03 00 02 08 00 00 01
    //  0b 00 00 00 00 00 00 00 01 00 00 00 00 00 00 f7

    // f0 08 06 00 05 00 01 06 04 00 00 00 00 00 00 00 03 00
    // 04 00 00 00 00 00 07 01 09 00 00 00 00 00 0a 03
    // 06 00 00 00 00 00 01 00 00 00 00 00 00 00 04 00
    // 00 00 00 00 00 00 0b 00 06 00 00 00 00 00 0c 00
    // 00 00 00 00 00 00 0f 00 04 03 00 04 00 00 01 00
    // 00 00 00 07 00 04 01 00 00 00 00 00 00 00 00 00
    // 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    // 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    // 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    // 00 00 00 0a 00 04 01 00 00 00 00 03 04 04 02 00
    // 00 00 00 02 00 04 02 00 00 00 00 02 00 04 01 00
    // 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    //  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 f7

    // +more
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_request_something_1(void)
{
    // BT log f0 00 07 00 01 00 00 00 02 01 02 04 03 f7
    uint8_t midi_tx[] = {0x04, 0xf0, 0x00, 0x07, 
                         0x04, 0x00, 0x01, 0x00, 
                         0x04, 0x00, 0x00, 0x02, 
                         0x04, 0x01, 0x02, 0x04, 
                         0x06, 0x03, 0xf7, 0x00};

    if (midi_host_data_tx_blocking(midi_dev, (const uint8_t*)midi_tx, sizeof(midi_tx), 50) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send usb_valeton_gp5_request_something_1");
    }

    // response BT:  f0 00 07 00 01 00 00 00 02 01 02 04 03 f7
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_request_something_2(void)
{
    // BT log f0 03 05 00 01 00 00 00 04 01 02 04 03 00 01 00 00 f7

    uint8_t midi_tx[] = {0x04, 0xf0, 0x03, 0x05, 
                         0x04, 0x00, 0x01, 0x00, 
                         0x04, 0x00, 0x00, 0x04, 
                         0x04, 0x01, 0x02, 0x04, 
                         0x04, 0x03, 0x00, 0x01, 
                         0x06, 0x00, 0x00, 0xf7};

    if (midi_host_data_tx_blocking(midi_dev, (const uint8_t*)midi_tx, sizeof(midi_tx), 50) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send usb_valeton_gp5_request_something_1");
    }

    // response BT: f0 00 09 00 01 00 00 00 02 01 02 04 01 f7
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
                            uint8_t midi_tx[4];

                            midi_tx[0] = 0x0B;
                            midi_tx[1] = 0xB0;
                            midi_tx[2] = 0x00;
                            midi_tx[3] = message.Payload;   // preset index
                            
                            if (midi_host_data_tx_blocking(midi_dev, (const uint8_t*)midi_tx, sizeof(midi_tx), 50) == ESP_OK)
                            {
                                
                            }
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

    // open it
    ESP_ERROR_CHECK(midi_host_open(VALETON_USB_VENDOR, VALETON_GP5_PRODUCT_ID, VALETON_GP5_MIDI_INTERFACE_INDEX, &dev_config, &midi_dev));
    assert(midi_dev);
    
    //midi_host_desc_print(midi_dev);
    // let things finish init and settle
    vTaskDelay(pdMS_TO_TICKS(250));

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
