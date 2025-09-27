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


#ifndef _USB_TONEX_COMMON_H
#define _USB_TONEX_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define TONEX_RX_TEMP_BUFFER_SIZE                    8192   // even multiple of 64 CDC transfer size
#define TONEX_USB_TX_BUFFER_SIZE                     512    // even multiple of 64 CDC transfer size
#define TONEX_MAX_SHORT_PRESET_DATA                  3072

typedef enum TonexStatus 
{
    STATUS_OK,
    STATUS_INVALID_FRAME,
    STATUS_INVALID_ESCAPE_SEQUENCE,
    STATUS_CRC_MISMATCH
} TonexStatus;


uint16_t tonex_common_add_framing(uint8_t* input, uint16_t inlength, uint8_t* output);
TonexStatus tonex_common_remove_framing(uint8_t* input, uint16_t inlength, uint8_t* output, uint16_t* outlength);
esp_err_t tonex_common_transmit(cdc_acm_dev_hdl_t cdc_dev, uint8_t* tx_data, uint16_t tx_len, uint32_t usb_buffer_size);
uint16_t tonex_common_calculate_CRC(uint8_t* data, uint16_t length) ;
uint16_t tonex_common_add_byte_with_stuffing(uint8_t* output, uint8_t byte);
uint16_t tonex_common_parse_value(uint8_t* message, uint8_t* index);
uint16_t tonex_common_locate_message_end(uint8_t* data, uint16_t length);
esp_err_t tonex_common_modify_parameter(uint16_t index, float value);
void tonex_common_preallocate_memory(void);
void tonex_common_release_memory(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif