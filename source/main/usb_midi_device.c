/*
 Copyright (C) 2026  Greg Smith

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
#include "usb_midi_device.h"
#include "control.h"
#include "display.h"
#include "wifi_config.h"
#include "usb_midi_device.h"
#include "midi_control.h"
#include "midi_helper.h"


static const char *TAG = "app_midi_device";

#define MIDI_USB_CHUNK_SIZE         4

/*
** Static vars
*/
static midi_dev_hdl_t midi_dev;
static QueueHandle_t input_queue;
static uint8_t midi_serial_channel = 0;

static const uint8_t k_cin_midi_len[16] = {
    0, /* 0x0 misc / reserved */
    0, /* 0x1 cable event / reserved */
    2, /* 0x2 two-byte system common */
    3, /* 0x3 three-byte system common */
    3, /* 0x4 SysEx start/continue */
    1, /* 0x5 SysEx ends 1 / 1-byte sys common */
    2, /* 0x6 SysEx ends 2 */
    3, /* 0x7 SysEx ends 3 */
    3, /* 0x8 note off */
    3, /* 0x9 note on */
    3, /* 0xA poly aftertouch */
    3, /* 0xB control change */
    2, /* 0xC program change */
    2, /* 0xD channel pressure */
    3, /* 0xE pitch bend */
    1, /* 0xF single byte */
};

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
static uint8_t usb_midi_to_ble_midi(const uint8_t *usb, uint8_t usb_len, uint16_t ts_ms, uint8_t *midi_data, uint8_t midi_data_max)
{
    if (!usb || !midi_data || (usb_len < MIDI_USB_CHUNK_SIZE) || (midi_data_max < 3)) 
    {
        return 0;
    }

    // build header
    const uint8_t header = (uint8_t)(0x80 | ((ts_ms >> 7) & 0x3F));
    const uint8_t tslow  = (uint8_t)(0x80 | (ts_ms & 0x7F));

    uint8_t out = 0;
    int packet_started = 0;

    // Midi USB in 4 byte chunks
    for (size_t i = 0; i + MIDI_USB_CHUNK_SIZE <= usb_len; i += MIDI_USB_CHUNK_SIZE) 
    {
        const uint8_t cin = usb[i] & 0x0F;

        // get the length for this message type
        const uint8_t n = k_cin_midi_len[cin];
        if (n == 0) 
        {
            continue;
        }

        // first MIDI message in this BLE packet gets header + timestamp
        if (!packet_started) 
        {
            if ((out + 2 + n) > midi_data_max) 
            {
                break;
            }
            midi_data[out++] = header;
            midi_data[out++] = tslow;
            packet_started = 1;
        } 
        else 
        {
            // later messages: timestamp low, then MIDI (BLE-MIDI packing)
            if ((out + 1 + n) > midi_data_max) 
            {
                break;
            }
            
            midi_data[out++] = tslow;
        }

        memcpy(&midi_data[out], &usb[i + 1], n);
        out += n;
    }

    return out;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static bool usb_midi_device_handle_rx(const uint8_t* data, size_t data_len, void* arg)
{
    uint8_t midi_buffer[64];
    uint8_t midi_len;

    // debug
    ESP_LOGI(TAG, "CDC Data received %d", (int)data_len);
    ESP_LOG_BUFFER_HEXDUMP(TAG, data, data_len, ESP_LOG_INFO);

    // adapt the USB format into the same format used in serial/bluetooth
    midi_len = usb_midi_to_ble_midi(data, data_len, 0, midi_buffer, sizeof(midi_buffer));

    if (midi_len > 0)
    {
        midi_helper_process_incoming_data(midi_buffer, midi_len, midi_serial_channel, 1);            
    }

    return true;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void usb_midi_device_handle(class_driver_t* driver_obj)
{        
    // nothing needed, all event driven
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void usb_midi_device_init(class_driver_t* driver_obj, uint16_t vendor_id, uint16_t product_id, uint16_t interface_index, QueueHandle_t comms_queue)
{
    // save the queue handle
    input_queue = comms_queue;
    
    // install Midi host driver
    ESP_ERROR_CHECK(midi_host_install(NULL));
    ESP_LOGI(TAG, "Opening USB Midi device");

    // set the config
    const midi_host_device_config_t dev_config = {
        .connection_timeout_ms = 1000,
        .out_buffer_size = 64,
        .in_buffer_size = 64,
        .user_arg = NULL,
        .event_cb = NULL,
        .data_cb = usb_midi_device_handle_rx
    };

    // open it
    if (midi_host_open(vendor_id, product_id, interface_index, &dev_config, &midi_dev) != ESP_OK)
    {
        ESP_LOGE(TAG, "midi_host_open failed");
    }

     // get the channel to use
    midi_serial_channel = control_get_config_item_int(CONFIG_ITEM_MIDI_CHANNEL);

    // adjust to zero based indexing
    if (midi_serial_channel > 0)
    {
        midi_serial_channel--;
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void usb_midi_device_deinit(void)
{
    midi_host_close(midi_dev);
    vTaskDelay(200);
    midi_dev = NULL;
    midi_host_uninstall();
}
