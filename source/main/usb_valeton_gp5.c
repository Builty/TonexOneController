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
#define VALETON_GP5_RX_TEMP_BUFFER_SIZE             512
#define VALETON_GP5_USB_TX_BUFFER_SIZE              128

enum CommsState
{
    COMMS_STATE_IDLE,
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

/*
** Static function prototypes
*/

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
            ValetonGP5Data->State = COMMS_STATE_READY;
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
                                control_sync_preset_details(message.Payload, "Unknown");
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
            ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)InputBuffers[loop].Data, InputBuffers[loop].Length, ESP_LOG_INFO);

            uint16_t bytes_consumed = 0;
            uint8_t* rx_entry_ptr = (uint8_t*)InputBuffers[loop].Data;
            uint16_t rx_entry_length = InputBuffers[loop].Length;

            // process all messages received (may be multiple messages appended)
            do
            {    
                // debug
                //ESP_LOG_BUFFER_HEXDUMP(TAG, rx_entry_ptr, end_marker_pos + 1, ESP_LOG_INFO);

                // process it
                //if (usb_valeton_gp5_process_single_message(rx_entry_ptr, end_marker_pos + 1) != ESP_OK)
                //{
                //    break;    
                //}
            
                // skip this message
                //rx_entry_ptr += (end_marker_pos + 1);
                //bytes_consumed += (end_marker_pos + 1);

                //temp
                bytes_consumed = rx_entry_length;

                //ESP_LOGI(TAG, "After message, pos %d cons %d len %d", (int)end_marker_pos, (int)bytes_consumed, (int)rx_entry_length);
            } while (bytes_consumed < rx_entry_length);

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

    // code from ESP support forums, work around start. Refer to https://www.esp32.com/viewtopic.php?t=30601
    // Relates to this:
    // 
    // Endpoint Descriptor:
    // ------------------------------
    // 0x07	bLength
    // 0x05	bDescriptorType
    // 0x87	bEndpointAddress  (IN endpoint 7)
    // 0x02	bmAttributes      (Transfer: Bulk / Synch: None / Usage: Data)
    // 0x0040	wMaxPacketSize    (64 bytes)
    // 0x00	bInterval         
    // *** ERROR: Invalid wMaxPacketSize. Must be 512 bytes in high speed mode.

    //Endpoint Descriptor:
    //------------------------------
    // 0x07	bLength
    // 0x05	bDescriptorType
    // 0x07	bEndpointAddress  (OUT endpoint 7)
    // 0x02	bmAttributes      (Transfer: Bulk / Synch: None / Usage: Data)
    // 0x0200	wMaxPacketSize    (512 bytes)   <= invalid for full speed mode we are using here
    // 0x00	bInterval         
    const usb_config_desc_t* config_desc;
    ESP_ERROR_CHECK(usb_host_get_active_config_descriptor(driver_obj->dev_hdl, &config_desc));

    // fix wMaxPacketSize
    int off = 0;
    uint16_t wTotalLength = config_desc->wTotalLength;
    const usb_standard_desc_t *next_desc = (const usb_standard_desc_t *)config_desc;
    if (next_desc)
    {
        do
        {
            if (next_desc->bDescriptorType == USB_B_DESCRIPTOR_TYPE_ENDPOINT)
            {
                usb_ep_desc_t *mod_desc = (usb_ep_desc_t *)next_desc;
                if (mod_desc->wMaxPacketSize > 64)
                {
                    ESP_LOGW(TAG, "EP 0x%02X with wrong wMaxPacketSize %d - fixed to 64", mod_desc->bEndpointAddress, mod_desc->wMaxPacketSize);
                    mod_desc->wMaxPacketSize = 64;
                }
            }

            next_desc = usb_parse_next_descriptor(next_desc, wTotalLength, &off);
        } while (next_desc != NULL);
    }
    // code from forums, work around end

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
