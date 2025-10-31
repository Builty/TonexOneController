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
#include "driver/i2c_master.h"
#include "usb_comms.h"
#include "usb_tonex_common.h"
#include "control.h"
#include "display.h"
#include "wifi_config.h"
#include "tonex_params.h"

static const char *TAG = "app_TonexCommon";

static uint8_t* PreallocatedMemory;

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint16_t tonex_common_calculate_CRC(uint8_t* data, uint16_t length) 
{
    uint16_t crc = 0xFFFF;

    for (uint16_t loop = 0; loop < length; loop++) 
    {
        crc ^= data[loop];

        for (uint8_t i = 0; i < 8; ++i) 
        {
            if (crc & 1) 
            {
                crc = (crc >> 1) ^ 0x8408;  // 0x8408 is the reversed polynomial x^16 + x^12 + x^5 + 1
            } 
            else 
            {
                crc = crc >> 1;
            }
        }
    }
    
    return ~crc;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint16_t tonex_common_add_byte_with_stuffing(uint8_t* output, uint8_t byte) 
{
    uint16_t length = 0;

    if (byte == 0x7E || byte == 0x7D) 
    {
        output[length] = 0x7D;
        length++;
        output[length] = byte ^ 0x20;
        length++;
    }
    else 
    {
        output[length] = byte;
        length++;
    }

    return length;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint16_t tonex_common_add_framing(uint8_t* input, uint16_t inlength, uint8_t* output)
{
    uint16_t outlength = 0;

    // Start flag
    output[outlength] = 0x7E;
    outlength++;

    // add input bytes
    for (uint16_t byte = 0; byte < inlength; byte++) 
    {
        outlength += tonex_common_add_byte_with_stuffing(&output[outlength], input[byte]);
    }

    // add CRC
    uint16_t crc = tonex_common_calculate_CRC(input, inlength);
    outlength += tonex_common_add_byte_with_stuffing(&output[outlength], crc & 0xFF);
    outlength += tonex_common_add_byte_with_stuffing(&output[outlength], (crc >> 8) & 0xFF);

    // End flag
    output[outlength] = 0x7E;
    outlength++;

    return outlength;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
TonexStatus tonex_common_remove_framing(uint8_t* input, uint16_t inlength, uint8_t* output, uint16_t* outlength)
{
    *outlength = 0;
    uint8_t* output_ptr = output;

    if ((inlength < 4) || (input[0] != 0x7E) || (input[inlength - 1] != 0x7E))
    {
        ESP_LOGE(TAG, "Invalid Frame (1)");
        return STATUS_INVALID_FRAME;
    }

    for (uint16_t i = 1; i < inlength - 1; ++i) 
    {
        if (input[i] == 0x7D) 
        {
            if ((i + 1) >= (inlength - 1))
            {
                ESP_LOGE(TAG, "Invalid Escape sequence");
                return STATUS_INVALID_ESCAPE_SEQUENCE;
            }

            *output_ptr = input[i + 1] ^ 0x20;
            output_ptr++;
            (*outlength)++;
            ++i;
        } 
        else if (input[i] == 0x7E) 
        {
            break;
        } 
        else 
        {
            *output_ptr = input[i];
            output_ptr++;
            (*outlength)++;
        }
    }

    if (*outlength < 2) 
    {
        ESP_LOGE(TAG, "Invalid Frame (2)");
        return STATUS_INVALID_FRAME;
    }

    //ESP_LOGI(TAG, "In:");
    //ESP_LOG_BUFFER_HEXDUMP(TAG, input, inlength, ESP_LOG_INFO);
    //ESP_LOGI(TAG, "Out:");
    //ESP_LOG_BUFFER_HEXDUMP(TAG, output, *outlength, ESP_LOG_INFO);

    uint16_t received_crc = (output[(*outlength) - 1] << 8) | output[(*outlength) - 2];
    (*outlength) -= 2;

    uint16_t calculated_crc = tonex_common_calculate_CRC(output, *outlength);

    if (received_crc != calculated_crc) 
    {
        ESP_LOGE(TAG, "Crc mismatch: %X, %X", (int)received_crc, (int)calculated_crc);
        return STATUS_CRC_MISMATCH;
    }

    return STATUS_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t tonex_common_transmit(cdc_acm_dev_hdl_t cdc_dev, uint8_t* tx_data, uint16_t tx_len, uint32_t usb_buffer_size)
{
    esp_err_t ret = ESP_FAIL;
    uint16_t bytes_this_chunk = 0;
    uint8_t* tx_ptr = tx_data;

    ESP_LOGI(TAG, "Sending %d bytes over CDC", (int)tx_len);
    
    // debug
    //ESP_LOG_BUFFER_HEX(TAG, tx_data, tx_len);

    // send lage packets in chunks
    while (tx_len > 0)
    {
        bytes_this_chunk = tx_len;

        if (bytes_this_chunk > usb_buffer_size)
        {
            bytes_this_chunk = usb_buffer_size;
        }

        ret = cdc_acm_host_data_tx_blocking(cdc_dev, tx_ptr, bytes_this_chunk, 500);
        
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "cdc_acm_host_data_tx_blocking() failed: %s", esp_err_to_name(ret));   
            break;
        }

        tx_ptr += bytes_this_chunk;
        tx_len -= bytes_this_chunk;
    }

    return ret;
}


/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint16_t tonex_common_parse_value(uint8_t* message, uint8_t* index)
{
    uint16_t value = 0;
    
    if (message[*index] == 0x81 || message[*index] == 0x82)
    {
        value = (message[(*index) + 2] << 8) | message[(*index) + 1];
        (*index) += 3;
    }
    else if (message[*index] == 0x80)
    {
        value = message[(*index) + 1];
        (*index) += 2;
    }
    else
    {
        value = message[*index];
        (*index)++;
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
uint16_t tonex_common_locate_message_end(uint8_t* data, uint16_t length)
{
    // locate the 0x7E end of packet marker
    // starting at index 1 to skip the start marker
    for (uint16_t loop = 1; loop < length; loop++)
    {
        if (data[loop] == 0x7E)
        {
            return loop;
        }
    }

    // not found
    return 0;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t tonex_common_modify_parameter(uint16_t index, float value)
{
    tModellerParameter* param_ptr = NULL;
    esp_err_t res = ESP_FAIL;
     
    if (index >= TONEX_PARAM_LAST)
    {
        ESP_LOGE(TAG, "usb_tonex_modify_parameters invalid index %d", (int)index);   
        return ESP_FAIL;
    }
        
    if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
    {
        ESP_LOGI(TAG, "tonex_common_modify_parameter index: %d name: %s value: %02f", (int)index, param_ptr[index].Name, value);  

        // update the local copy
        memcpy((void*)&param_ptr[index].Value, (void*)&value, sizeof(float));

        tonex_params_release_locked_access();
        res = ESP_OK;
    }

    return res;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void tonex_common_preallocate_memory(void)
{
    // note here: the Tonex One requires large CDC buffers. Not many heap areas have enough contiguous space.
    // if we let the system boot and no Tonex is conncted, the other parts of the system allocate mem
    // and leave us with no spaces big enough to hold the buffers.
    // Work around here: preallocate the buffers we need, and then free them just before the CDC allocation,
    // effectively reserving them and then other parts of system can use different heap areas.
    // +256 here just to be sure.
    PreallocatedMemory = heap_caps_malloc(TONEX_RX_TEMP_BUFFER_SIZE + TONEX_USB_TX_BUFFER_SIZE + 256, MALLOC_CAP_DMA);
    if (PreallocatedMemory == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate PreallocatedMemory!");
    } 
    else
    {
        ESP_LOGI(TAG, "PreallocatedMemory OK");
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void tonex_common_release_memory(void)
{
    ESP_LOGI(TAG, "Releasing preallocated memory");

    if (PreallocatedMemory != NULL)
    {
        heap_caps_free(PreallocatedMemory);
    }

    // debug
    //heap_caps_print_heap_info(MALLOC_CAP_DMA);
}