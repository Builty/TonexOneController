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


#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_check.h"
#include "esp_log.h"
#include "usb/usb_host.h"
#include "driver/i2c.h"
#include "nvs_flash.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_ota_ops.h"
#include "sys/param.h"
#include "control.h"
#include "usb_comms.h"
#include "usb/usb_host.h"
#include "usb_tonex_one.h"
#include "tonex_params.h"
#include "midi_helper.h"
#include "midi_helper_tonex.h"
#include "midi_helper_valeton.h"

static const char *TAG = "app_midi_helper";

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
float midi_helper_scale_midi_to_float(uint16_t param_index, uint8_t midi_value)
{
    float min;
    float max;

    // get this params min/max values
    tonex_params_get_min_max(param_index, &min, &max);

    // scale 0..127 midi value to param
    return min + (((float)midi_value / 127.0f) * (max - min));
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
float midi_helper_boolean_midi_to_float(uint8_t midi_value)
{
    if (midi_value == MIDI_BOOL_ENABLE)
    {
        return 1.0f;
    }
    else
    {
        return 0.0f;
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t midi_helper_adjust_param_via_midi(uint8_t change_num, uint8_t midi_value)
{
    switch (usb_get_connected_modeller_type())
    {
        case AMP_MODELLER_TONEX_ONE:        // fallthrough
        case AMP_MODELLER_TONEX:            // fallthrough
        default:
        {
            return midi_helper_tonex_adjust_param_via_midi(change_num, midi_value);
        } break;

        case AMP_MODELLER_VALETON_GP5:
        {
            return midi_helper_valeton_adjust_param_via_midi(change_num, midi_value);
        } break;
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint16_t midi_helper_get_param_for_change_num(uint8_t change_num, uint8_t midi_value_1, uint8_t midi_value_2)
{
    switch (usb_get_connected_modeller_type())
    {
        case AMP_MODELLER_TONEX_ONE:        // fallthrough
        case AMP_MODELLER_TONEX:            // fallthrough
        default:
        {
            return midi_helper_tonex_get_param_for_change_num(change_num, midi_value_1, midi_value_2);
        } break;

        case AMP_MODELLER_VALETON_GP5:
        {
            return midi_helper_valeton_get_param_for_change_num(change_num, midi_value_1, midi_value_2);
        } break;
    }    
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint8_t midi_helper_process_incoming_data(uint8_t* data, uint8_t length, uint8_t midi_channel, uint8_t enable_CC)
{
    uint8_t bytes_processed = 0;
    uint8_t command;
    uint8_t channel;
    uint8_t* ptr = data;
    uint8_t header_found = 0;

    ESP_LOGI(TAG, "Midi incoming data len: %d: ", (int)length);
    ESP_LOG_BUFFER_HEX(TAG, data, length);

    // make sure we have enough data
    if (length < 2)
    {
        ESP_LOGW(TAG, "Midi incoming wrong length %d", (int)length);
        return 0;
    }

    // an optional 1 byte header could be included, with 0b10xxxxxx
    // in this case, the next byte must also have bit 7 set
    if (((data[0] & 0x80) != 0) && ((data[0] & 0x40) == 0) && ((data[1] & 0x80) != 0))
    {
        // found header, skip it
        bytes_processed++;
        ptr++;

        header_found = 1;
    }

    // loop remaining bytes
    while (bytes_processed < length)
    {
        // if bit 7 is set, it's another timestamp low byte
        if (header_found && ((*ptr & 0x80) != 0))
        {
            // skip timestamp LSB and keep going
            ptr++;
            bytes_processed++;
        }

        // get Midi channel
        channel = *ptr & 0x0F;

        // get the command
        command = *ptr & 0xF0;

        //ESP_LOGW(TAG, "Midi incoming command: %02X, channel: %d", command, (int)channel);

        // this status byte is now processed, skip it
        ptr++;
        bytes_processed++;

        // check the command
        switch (command)
        {
            case 0xC0:
            {
                // Program change Should be 0xC0 XX (XX = preset index, 0-based)
                if (channel == midi_channel)
                {
                    // load the mapped value for this program change value
                    uint8_t map_val = control_get_pc_map()[*ptr];

                    // set preset
                    control_request_preset_index(map_val);
                }

                ptr++;
                bytes_processed++;
            } break;

            case 0xB0:
            {
                // Control Change message
                uint8_t change_num = *ptr++;
                uint8_t value = *ptr++;
                bytes_processed += 2;

                if (enable_CC && (channel == midi_channel)) 
                {
                    midi_helper_adjust_param_via_midi(change_num, value);
                }

                // control change messages can be multiple appended together, 2 bytes each.
                while (bytes_processed < length)
                {
                    // if MSB bit is set, it's a new timestamp, break out to process in main loop
                    if ((bytes_processed >= length) || ((*ptr & 0x80) != 0))
                    {
                        break;
                    }
                    else
                    {
                        // process next CC
                        uint8_t change_num = *ptr++;
                        uint8_t value = *ptr++;
                        bytes_processed += 2;

                        if (enable_CC && (channel == midi_channel)) 
                        {
                            midi_helper_adjust_param_via_midi(change_num, value);
                        }
                    }
                }
            } break;

            default:
            {
                ESP_LOGW(TAG, "Midi incoming unexpected command %02X", (int)command);
            } break;
        }
    }

    return 1;
}
