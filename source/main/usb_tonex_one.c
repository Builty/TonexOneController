/*
 Copyright (C) 2024  Greg Smith

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

//Tonex One device
//-----------------------------
//idVendor = 0x1963
//idProduct = 0x00D1

//Index  LANGID  String
//0x00   0x0000  0x0409 
//0x01   0x0409  "IK Multimedia"
//0x02   0x0409  "ToneX One"
//0x04   0x0409  "ToneX One Record"
//0x05   0x0409  "ToneX One Playback"
//0x06   0x0409  "ToneX Control VCOM"
//0x09   0x0409  "ToneX One USB Input"
//0x0A   0x0409  "ToneX One USB Output"
//0x0B   0x0409  "ToneX One Internal Clock"
//0x0C   0x0409  "ToneX One In 1"
//0x0D   0x0409  "ToneX One In 2"
//0x10   0x0409  "ToneX One Out 1"
//0x11   0x0409  "ToneX One Out 2"
//0x12   0x0409  "xxxxxxxxxxxxxxxxxxxx"     // Serial Number

//Composite Device.
//- 5 interfaces total
//- Communications Device Class "Tonex Control VCOM"
//- Audio Device Class "Audio Protocol IP version 2.00"

//----------------------------------------------------
//Endpoint 7 is the Control endpoint, using CDC protocol.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "esp_log.h"
#include "usb/usb_host.h"
#include "usb/cdc_acm_host.h"
#include "driver/i2c.h"
#include "usb_comms.h"
#include "usb_tonex_one.h"
#include "control.h"
#include "display.h"
#include "wifi_config.h"
#include "tonex_params.h"

static const char *TAG = "app_TonexOne";

// preset name is proceeded by this byte sequence:
static const uint8_t ToneOnePresetByteMarker[] = {0xB9, 0x04, 0xB9, 0x02, 0xBC, 0x21};

// lengths of preset name and drive character
#define TONEX_ONE_RESP_OFFSET_PRESET_NAME_LEN       32
#define TONEX_ONE_CDC_INTERFACE_INDEX               0

// Tonex One can send quite large data quickly, so make a generous receive buffer
#define RX_TEMP_BUFFER_SIZE                         8192   // even multiple of 64 CDC transfer size
#define MAX_INPUT_BUFFERS                           2
#define USB_TX_BUFFER_SIZE                          8192 

#define MAX_SHORT_PRESET_DATA                       3072
#define MAX_FULL_PRESET_DATA                        RX_TEMP_BUFFER_SIZE
#define MAX_STATE_DATA                              512

// credit to https://github.com/vit3k/tonex_controller for some of the below details and implementation
enum CommsState
{
    COMMS_STATE_IDLE,
    COMMS_STATE_HELLO,
    COMMS_STATE_READY,
    COMMS_STATE_GET_STATE
};

typedef enum Status 
{
    STATUS_OK,
    STATUS_INVALID_FRAME,
    STATUS_INVALID_ESCAPE_SEQUENCE,
    STATUS_CRC_MISMATCH
} Status;

typedef enum Type 
{
    TYPE_UNKNOWN,
    TYPE_STATE_UPDATE,
    TYPE_HELLO,
    TYPE_STATE_PRESET_DETAILS,
    TYPE_STATE_PRESET_DETAILS_FULL,
    TYPE_PARAM_CHANGED
} Type;

typedef enum Slot
{
    A = 0,
    B = 1,
    C = 2
} Slot;


#define TONEX_STATE_OFFSET_START_INPUT_TRIM     15          // 0x000070c1 (-15.0) -> 0x000058c1 (0) -> 0x00007041 (15.0) 
#define TONEX_STATE_OFFSET_START_STOMP_MODE     19          // 0x00 - off, 0x01 - on
#define TONEX_STATE_OFFSET_START_CAB_BYPASS     20          // 0x00 - off, 0x01 - on
#define TONEX_STATE_OFFSET_START_TUNING_MODE    21          // 0x00 - mute, 0x01 - through
#define TONEX_STATE_OFFSET_START_COLORS         22

#define TONEX_STATE_OFFSET_END_BPM              4          
#define TONEX_STATE_OFFSET_END_TEMPO_SOURCE     6           // 00 - GLOBAL, 01 - PRESET 
#define TONEX_STATE_OFFSET_END_DIRECT_MONITOR   7           // 0x00 - off, 0x01 - on 
#define TONEX_STATE_OFFSET_END_TUNING_REF       9           
#define TONEX_STATE_OFFSET_END_CURRENT_SLOT     11           
#define TONEX_STATE_OFFSET_END_BYPASS_MODE      12
#define TONEX_STATE_OFFSET_END_SLOT_C_PRESET    14
#define TONEX_STATE_OFFSET_END_SLOT_B_PRESET    16
#define TONEX_STATE_OFFSET_END_SLOT_A_PRESET    18


typedef struct __attribute__ ((packed)) 
{
    Type type;
    uint16_t size;
    uint16_t unknown;
} tHeader;

typedef struct __attribute__ ((packed)) 
{
    // storage for current pedal state data
    uint8_t StateData[MAX_STATE_DATA]; 
    uint16_t StateDataLength;

    // storage for current preset details data (short version)
    uint8_t PresetData[MAX_SHORT_PRESET_DATA];
    uint16_t PresetDataLength;
    uint16_t PresetParameterStartOffset;
} tPedalData;

typedef struct __attribute__ ((packed)) 
{
    tHeader Header;
    uint8_t SlotAPreset;
    uint8_t SlotBPreset;
    uint8_t SlotCPreset;
    Slot CurrentSlot;
    tPedalData PedalData;
} tTonexMessage;

typedef struct __attribute__ ((packed)) 
{
    tTonexMessage Message;
    uint8_t TonexState;
} tTonexData;

typedef struct
{
    uint8_t Data[RX_TEMP_BUFFER_SIZE];
    uint16_t Length;
    uint8_t ReadyToRead : 1;
    uint8_t ReadyToWrite : 1;
} tInputBufferEntry;



/*
** Static vars
*/
static cdc_acm_dev_hdl_t cdc_dev;
static tTonexData* TonexData;
static char preset_name[TONEX_ONE_RESP_OFFSET_PRESET_NAME_LEN + 1];
static uint8_t* TxBuffer;
static uint8_t* FramedBuffer;
static QueueHandle_t input_queue;
static uint8_t boot_init_needed = 0;
static uint8_t boot_preset_request = 0;
static volatile tInputBufferEntry* InputBuffers;
static uint8_t* PreallocatedMemory;

/*
** Static function prototypes
*/
static uint16_t addFraming(uint8_t* input, uint16_t inlength, uint8_t* output);
static Status removeFraming(uint8_t* input, uint16_t inlength, uint8_t* output, uint16_t* outlength);
static esp_err_t usb_tonex_one_transmit(uint8_t* tx_data, uint16_t tx_len);
static Status usb_tonex_one_parse(uint8_t* message, uint16_t inlength);
static esp_err_t usb_tonex_one_set_active_slot(Slot newSlot);
static esp_err_t usb_tonex_one_set_preset_in_slot(uint16_t preset, Slot newSlot, uint8_t selectSlot);
static uint16_t usb_tonex_one_get_current_active_preset(void);

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static uint16_t calculateCRC(uint8_t* data, uint16_t length) 
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
static uint16_t addByteWithStuffing(uint8_t* output, uint8_t byte) 
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
static uint16_t addFraming(uint8_t* input, uint16_t inlength, uint8_t* output)
{
    uint16_t outlength = 0;

    // Start flag
    output[outlength] = 0x7E;
    outlength++;

    // add input bytes
    for (uint16_t byte = 0; byte < inlength; byte++) 
    {
        outlength += addByteWithStuffing(&output[outlength], input[byte]);
    }

    // add CRC
    uint16_t crc = calculateCRC(input, inlength);
    outlength += addByteWithStuffing(&output[outlength], crc & 0xFF);
    outlength += addByteWithStuffing(&output[outlength], (crc >> 8) & 0xFF);

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
static Status removeFraming(uint8_t* input, uint16_t inlength, uint8_t* output, uint16_t* outlength)
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

    uint16_t calculated_crc = calculateCRC(output, *outlength);

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
static esp_err_t usb_tonex_one_hello(void)
{
    uint16_t outlength;
    
    ESP_LOGI(TAG, "Sending Hello");

    // build message
    uint8_t request[] = {0xb9, 0x03, 0x00, 0x82, 0x04, 0x00, 0x80, 0x0b, 0x01, 0xb9, 0x02, 0x02, 0x0b};

    // add framing
    outlength = addFraming(request, sizeof(request), FramedBuffer);

    // send it
    return usb_tonex_one_transmit(FramedBuffer, outlength);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t usb_tonex_one_request_state(void)
{
    uint16_t outlength;

    // build message
    uint8_t request[] = {0xb9, 0x03, 0x00, 0x82, 0x06, 0x00, 0x80, 0x0b, 0x03, 0xb9, 0x02, 0x81, 0x06, 0x03, 0x0b};

    // add framing
    outlength = addFraming(request, sizeof(request), FramedBuffer);

    // send it
    return usb_tonex_one_transmit(FramedBuffer, outlength);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t __attribute__((unused)) usb_tonex_one_request_preset_details(uint8_t preset_index, uint8_t full_details)
{
    uint16_t outlength;

    ESP_LOGI(TAG, "Requesting full preset details for %d", (int)preset_index);

    // build message                                                                                               Preset Full    
    uint8_t request[] = {0xb9, 0x03, 0x81, 0x00, 0x03, 0x82, 0x06, 0x00, 0x80, 0x0b, 0x03, 0xb9, 0x04, 0x0b, 0x01, 0x00,  0x00};  
    request[15] = preset_index;
    request[16] = full_details;     // 0x00 = approx 2k byte summary. 0x01 = approx 30k byte full preset details

    // add framing
    outlength = addFraming(request, sizeof(request), FramedBuffer);

    // send it
    return usb_tonex_one_transmit(FramedBuffer, outlength);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t usb_tonex_one_send_single_parameter(uint16_t index, float value)
{
    uint16_t framed_length;

    // NOTE: only supported in newer Pedal firmware that came with Editor support!

    // Build message                                         len LSB  len MSB
    uint8_t message[] = {0xb9, 0x03, 0x81, 0x09, 0x03, 0x82, 0x0A,     0x00, 0x80, 0x0B, 0x03};

    //                               0x00 here for global params???
    // payload           unknown       |         param index             4 byte float value
    uint8_t payload[] = {0xB9, 0x04, 0x02, 0x00, 0x00,         0x88, 0x00, 0x00, 0x00, 0x00 };

    // set param index
    payload[4] = index;

    // set param value
    memcpy((void*)&payload[6], (void*)&value, sizeof(value));

    // build total message
    memcpy((void*)TxBuffer, (void*)message, sizeof(message));
    memcpy((void*)&TxBuffer[sizeof(message)], (void*)payload, sizeof(payload));

    // add framing
    framed_length = addFraming(TxBuffer, sizeof(message) + sizeof(payload), FramedBuffer);

    // debug
    //ESP_LOG_BUFFER_HEXDUMP(TAG, FramedBuffer, framed_length, ESP_LOG_INFO);

    // send it
    return usb_tonex_one_transmit(FramedBuffer, framed_length);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void __attribute__((unused)) usb_tonex_one_dump_state(void)
{
    float InputTrim;
    float BPM;
    uint16_t TuningRef;

    memcpy((void*)&BPM, (void*)&TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_BPM], sizeof(float));
    memcpy((void*)&InputTrim, (void*)&TonexData->Message.PedalData.StateData[TONEX_STATE_OFFSET_START_INPUT_TRIM], sizeof(float));    
    memcpy((void*)&TuningRef, (void*)&TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_TUNING_REF], sizeof(uint16_t));

    ESP_LOGI(TAG, "**** Tonex State Data ****");
    ESP_LOGI(TAG, "Input Trim: %3.2f.\t\tStomp Mode: %d", InputTrim, 
                                                          (int)TonexData->Message.PedalData.StateData[TONEX_STATE_OFFSET_START_STOMP_MODE]);

    ESP_LOGI(TAG, "Cab Sim Bypass: %d.\t\tTuning Mode: %d", (int)TonexData->Message.PedalData.StateData[TONEX_STATE_OFFSET_START_CAB_BYPASS], 
                                                            (int)TonexData->Message.PedalData.StateData[TONEX_STATE_OFFSET_START_TUNING_MODE]);

    ESP_LOGI(TAG, "Slot A Preset: %d,\t\tSlot B Preset: %d", (int)TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_SLOT_A_PRESET], 
                                                             (int)TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_SLOT_B_PRESET]);

    ESP_LOGI(TAG, "Slot C Preset: %d.\t\tCurrent Slot: %d", (int)TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_SLOT_C_PRESET], 
                                                            (int)TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_CURRENT_SLOT]);

    ESP_LOGI(TAG, "Tuning Reference: %d.\t\tDirect Monitoring: %d", (int)TuningRef, 
                                                                    (int)TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_DIRECT_MONITOR]);

    ESP_LOGI(TAG, "BPM: %3.2f\t\t\tTempo Source: %d", BPM, (int)TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_TEMPO_SOURCE]);     
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t __attribute__((unused)) usb_tonex_one_set_active_slot(Slot newSlot)
{
    uint16_t framed_length;

    ESP_LOGI(TAG, "Setting slot %d", (int)newSlot);

    // Build message, length to 0 for now                    len LSB  len MSB
    uint8_t message[] = {0xb9, 0x03, 0x81, 0x06, 0x03, 0x82, 0,       0,       0x80, 0x0b, 0x03};
    
    // set length 
    message[6] = TonexData->Message.PedalData.StateDataLength & 0xFF;
    message[7] = (TonexData->Message.PedalData.StateDataLength >> 8) & 0xFF;

    // save the slot
    TonexData->Message.CurrentSlot = newSlot;

    // modify the buffer with the new slot
    TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_CURRENT_SLOT] = (uint8_t)newSlot;

    // build total message
    memcpy((void*)TxBuffer, (void*)message, sizeof(message));
    memcpy((void*)&TxBuffer[sizeof(message)], (void*)TonexData->Message.PedalData.StateData, TonexData->Message.PedalData.StateDataLength);

    // add framing
    framed_length = addFraming(TxBuffer, sizeof(message) + TonexData->Message.PedalData.StateDataLength, FramedBuffer);

    // send it
    return usb_tonex_one_transmit(FramedBuffer, framed_length);    
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t usb_tonex_one_set_preset_in_slot(uint16_t preset, Slot newSlot, uint8_t selectSlot)
{
    uint16_t framed_length;
    
    ESP_LOGI(TAG, "Setting preset %d in slot %d", (int)preset, (int)newSlot);

    // Build message, length to 0 for now                    len LSB  len MSB
    uint8_t message[] = {0xb9, 0x03, 0x81, 0x06, 0x03, 0x82, 0,       0,       0x80, 0x0b, 0x03};
    
    // set length 
    message[6] = TonexData->Message.PedalData.StateDataLength & 0xFF;
    message[7] = (TonexData->Message.PedalData.StateDataLength >> 8) & 0xFF;

    // force pedal to Stomp mode. 0 here = A/B mode, 1 = stomp mode
    TonexData->Message.PedalData.StateData[TONEX_STATE_OFFSET_START_STOMP_MODE] = 1;      // thanks to Riccardo for finding

    // make sure direct monitoring is on so sound not muted from USB connection
    TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_DIRECT_MONITOR] = 1;

    // check if setting same preset twice will set bypass
    if (control_get_config_item_int(CONFIG_ITEM_TOGGLE_BYPASS))
    {
        if (selectSlot && (TonexData->Message.CurrentSlot == newSlot) && (preset == usb_tonex_one_get_current_active_preset()))
        {
            // are we in bypass mode?
            if (TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_BYPASS_MODE] == 1)
            {
                ESP_LOGI(TAG, "Disabling bypass mode");

                // disable bypass mode
                TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_BYPASS_MODE] = 0;
            }
            else
            {
                ESP_LOGI(TAG, "Enabling bypass mode");

                // enable bypass mode
                TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_BYPASS_MODE] = 1;
            }
        }
        else
        {
            // new preset, disable bypass mode to be sure
            TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_BYPASS_MODE] = 0;
        }
    }

    TonexData->Message.CurrentSlot = newSlot;

  
    // set the preset index into the slot position
    switch (newSlot)
    {
        case A:
        {
            TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_SLOT_A_PRESET] = preset;
        } break;

        case B:
        {
            TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_SLOT_B_PRESET] = preset;
        } break;

        case C:
        {
            TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_SLOT_C_PRESET] = preset;
        } break;
    }

    if (selectSlot)
    {
        // modify the buffer with the new slot
        TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_CURRENT_SLOT] = (uint8_t)newSlot;
    }

    // build total message
    memcpy((void*)TxBuffer, (void*)message, sizeof(message));
    memcpy((void*)&TxBuffer[sizeof(message)], (void*)TonexData->Message.PedalData.StateData, TonexData->Message.PedalData.StateDataLength);

    // do framing
    framed_length = addFraming(TxBuffer, sizeof(message) + TonexData->Message.PedalData.StateDataLength, FramedBuffer);

    //ESP_LOGI(TAG, "State Data after changes - framed");
    //ESP_LOG_BUFFER_HEXDUMP(TAG, FramedBuffer, framed_length, ESP_LOG_INFO);

    // send it
    return usb_tonex_one_transmit(FramedBuffer, framed_length);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static bool usb_tonex_one_handle_rx(const uint8_t* data, size_t data_len, void* arg)
{
    // debug
    //ESP_LOGI(TAG, "CDC Data received %d", (int)data_len);
    //ESP_LOG_BUFFER_HEXDUMP(TAG, data, data_len, ESP_LOG_INFO);

    if (data_len > RX_TEMP_BUFFER_SIZE)
    {
        ESP_LOGE(TAG, "usb_tonex_one_handle_rx data too long! %d", (int)data_len);
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

    ESP_LOGE(TAG, "usb_tonex_one_handle_rx no available buffers!");
    return false;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t usb_tonex_one_transmit(uint8_t* tx_data, uint16_t tx_len)
{
    esp_err_t ret = ESP_FAIL;
    uint16_t bytes_this_chunk = 0;
    uint8_t* tx_ptr = tx_data;

    ESP_LOGI(TAG, "Sending %d bytes over CDC", (int)tx_len);

    // send lage packets in chunks
    while (tx_len > 0)
    {
        bytes_this_chunk = tx_len;

        if (bytes_this_chunk > USB_TX_BUFFER_SIZE)
        {
            bytes_this_chunk = USB_TX_BUFFER_SIZE;
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
static esp_err_t usb_tonex_one_modify_parameter(uint16_t index, float value)
{
    tTonexParameter* param_ptr = NULL;
    esp_err_t res = ESP_FAIL;
     
    if (index >= TONEX_PARAM_LAST)
    {
        ESP_LOGE(TAG, "usb_tonex_one_modify_parameters invalid index %d", (int)index);   
        return ESP_FAIL;
    }
        
    if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
    {
        ESP_LOGI(TAG, "usb_tonex_one_modify_parameter index: %d name: %s value: %02f", (int)index, param_ptr[index].Name, value);  

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
static esp_err_t usb_tonex_one_modify_global(uint16_t global_val, float value)
{
    esp_err_t res = ESP_FAIL;

    switch (global_val)
    {
        case TONEX_GLOBAL_BPM:
        {
            // modify the BPM value in state packet
            memcpy((void*)&TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_BPM], (void*)&value, sizeof(float));
            res = ESP_OK;
        } break;

        case TONEX_GLOBAL_INPUT_TRIM:
        {
            // modify the input trim value in state
            memcpy((void*)&TonexData->Message.PedalData.StateData[TONEX_STATE_OFFSET_START_INPUT_TRIM], (void*)&value, sizeof(float));
            res = ESP_OK;
        } break;

        case TONEX_GLOBAL_CABSIM_BYPASS:
        {
            // modify the cabsim bypass in state
            TonexData->Message.PedalData.StateData[TONEX_STATE_OFFSET_START_CAB_BYPASS] = (uint8_t)value;
            res = ESP_OK;
        } break;

        case TONEX_GLOBAL_TEMPO_SOURCE:
        {
            // modify the tempo source value in state
            TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_TEMPO_SOURCE] = (uint8_t)value;
            res = ESP_OK;
        } break;

        case TONEX_GLOBAL_TUNING_REFERENCE:
        {
            // modify the tuning ref value in state packet
            uint16_t freq = (uint16_t)value;
            memcpy((void*)&TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_TUNING_REF], (void*)&freq, sizeof(uint16_t));
            res = ESP_OK;
        } break;
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
uint16_t usb_tonex_one_parse_value(uint8_t* message, uint8_t* index)
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
static Status usb_tonex_one_parse_state(uint8_t* unframed, uint16_t length, uint16_t index)
{
    tTonexParameter* param_ptr;

    TonexData->Message.Header.type = TYPE_STATE_UPDATE;

    TonexData->Message.PedalData.StateDataLength = length - index;
    memcpy((void*)TonexData->Message.PedalData.StateData, (void*)&unframed[index], TonexData->Message.PedalData.StateDataLength);
    ESP_LOGI(TAG, "Saved Pedal StateData: %d", TonexData->Message.PedalData.StateDataLength);
    
    // save preset details
    TonexData->Message.SlotAPreset = TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_SLOT_A_PRESET];
    TonexData->Message.SlotBPreset = TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_SLOT_B_PRESET];
    TonexData->Message.SlotCPreset = TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_SLOT_C_PRESET];
    TonexData->Message.CurrentSlot = TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_CURRENT_SLOT];
 
    // update global params
    if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
    {
        memcpy((void*)&param_ptr[TONEX_GLOBAL_BPM].Value, (void*)&TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_BPM], sizeof(float));
        memcpy((void*)&param_ptr[TONEX_GLOBAL_INPUT_TRIM].Value, (void*)&TonexData->Message.PedalData.StateData[TONEX_STATE_OFFSET_START_INPUT_TRIM], sizeof(float));
        param_ptr[TONEX_GLOBAL_CABSIM_BYPASS].Value = (float)TonexData->Message.PedalData.StateData[TONEX_STATE_OFFSET_START_CAB_BYPASS];
        param_ptr[TONEX_GLOBAL_TEMPO_SOURCE].Value = (float)TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_TEMPO_SOURCE];        
        
        uint16_t freq;
        memcpy((void*)&freq, (void*)&TonexData->Message.PedalData.StateData[TonexData->Message.PedalData.StateDataLength - TONEX_STATE_OFFSET_END_TUNING_REF], sizeof(uint16_t));
        param_ptr[TONEX_GLOBAL_TUNING_REFERENCE].Value = (float)freq;

        tonex_params_release_locked_access();
    }

    tTonexPresetColor* color_ptr;
    // update preset colors
    if (tonex_params_colors_get_locked_access(&color_ptr) == ESP_OK)
    {
        uint8_t data_offset = TONEX_STATE_OFFSET_START_COLORS;
        data_offset += 2; // skip `ba 14` list header
        for (uint16_t preset_index = 0; preset_index < MAX_PRESETS; preset_index++) {
            data_offset += 2; // skip `b9 03` list header

            color_ptr[preset_index].red = usb_tonex_one_parse_value(TonexData->Message.PedalData.StateData, &data_offset);
            color_ptr[preset_index].green = usb_tonex_one_parse_value(TonexData->Message.PedalData.StateData, &data_offset);
            color_ptr[preset_index].blue = usb_tonex_one_parse_value(TonexData->Message.PedalData.StateData, &data_offset);
        }

        tonex_params_release_locked_access();
    }

    // debug
    //ESP_LOG_BUFFER_HEXDUMP(TAG, TonexData->Message.PedalData.StateData, TonexData->Message.PedalData.StateDataLength, ESP_LOG_INFO);
    //tonex_dump_parameters();
    usb_tonex_one_dump_state();

    ESP_LOGI(TAG, "Slot A: %d. Slot B:%d. Slot C:%d. Current slot: %d", (int)TonexData->Message.SlotAPreset, (int)TonexData->Message.SlotBPreset, (int)TonexData->Message.SlotCPreset, (int)TonexData->Message.CurrentSlot);

    return STATUS_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static Status usb_tonex_one_parse_preset_details(uint8_t* unframed, uint16_t length, uint16_t index)
{
    TonexData->Message.Header.type = TYPE_STATE_PRESET_DETAILS;

    TonexData->Message.PedalData.PresetDataLength = length - index;
    memcpy((void*)TonexData->Message.PedalData.PresetData, (void*)&unframed[index], TonexData->Message.PedalData.PresetDataLength);
    ESP_LOGI(TAG, "Saved Preset Details: %d", TonexData->Message.PedalData.PresetDataLength);

    // debug
    //ESP_LOGI(TAG, "Preset Data Rx: %d %d", (int)length, (int)index);
    //ESP_LOG_BUFFER_HEXDUMP(TAG, TonexData->Message.PedalData.PresetData, TonexData->Message.PedalData.PresetDataLength, ESP_LOG_INFO);

    return STATUS_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static uint16_t usb_tonex_one_get_current_active_preset(void)
{
    uint16_t result = 0;

    switch (TonexData->Message.CurrentSlot)
    {
        case A:
        {        
            result = TonexData->Message.SlotAPreset;
        } break;
    
        case B:
        {
            result = TonexData->Message.SlotBPreset;
        } break;
    
        case C:
        default:
        {
            result = TonexData->Message.SlotCPreset;
        } break;
    }
    
    return result;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_tonex_one_parse_preset_parameters(uint8_t* raw_data, uint16_t length)
{
    uint8_t param_start_marker[] = {0xBA, 0x03, 0xBA, 0x6D}; 
    tTonexParameter* param_ptr = NULL;

    ESP_LOGI(TAG, "Parsing Preset parameters");

    // try to locate the start of the first parameter block 
    uint8_t* temp_ptr = memmem((void*)raw_data, length, (void*)param_start_marker, sizeof(param_start_marker));
    if (temp_ptr != NULL)
    {
        // skip the start marker
        temp_ptr += sizeof(param_start_marker);

        // save the offset where the parameters start
        TonexData->Message.PedalData.PresetParameterStartOffset = temp_ptr - raw_data;
        ESP_LOGI(TAG, "Preset parameters offset: %d", (int)TonexData->Message.PedalData.PresetParameterStartOffset);

        if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
        {
            // params here are start marker of 0x88, followed by a 4-byte float
            for (uint32_t loop = 0; loop < TONEX_PARAM_LAST; loop++)
            {
                if (*temp_ptr == 0x88)
                {
                    // skip the marker
                    temp_ptr++;

                    // get the value
                    memcpy((void*)&param_ptr[loop].Value, (void*)temp_ptr, sizeof(float));

                    // skip the float
                    temp_ptr += sizeof(float);
                }
                else
                {
                    ESP_LOGW(TAG, "Unexpected value during Param parse: %d, %d", (int)loop, (int)*temp_ptr);  
                    break;
                }
            }

            tonex_params_release_locked_access();
        }

        ESP_LOGI(TAG, "Parsing Preset parameters complete");
    }
    else
    {
        ESP_LOGW(TAG, "Parsing Preset parameters failed to find start marker");
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static Status usb_tonex_one_parse(uint8_t* message, uint16_t inlength)
{
    uint16_t out_len = 0;

    Status status = removeFraming(message, inlength, FramedBuffer, &out_len);

    if (status != STATUS_OK)
    {
        ESP_LOGE(TAG, "Remove framing failed");
        return STATUS_INVALID_FRAME;
    }
    
    if (out_len < 5)
    {
        ESP_LOGE(TAG, "Message too short");
        return STATUS_INVALID_FRAME;
    }
    
    if ((FramedBuffer[0] != 0xB9) || (FramedBuffer[1] != 0x03))
    {
        ESP_LOGE(TAG, "Invalid header");
        return STATUS_INVALID_FRAME;
    }
    
    tHeader header;
    uint8_t index = 2;
    uint16_t type = usb_tonex_one_parse_value(FramedBuffer, &index);

    switch (type)
    {
        case 0x0306:
        {
            header.type = TYPE_STATE_UPDATE;
        } break;

        case 0x0304:
        {
            // preset details summary
            header.type = TYPE_STATE_PRESET_DETAILS;
        } break;

        case 0x0303:
        {
            // preset details in full
            header.type = TYPE_STATE_PRESET_DETAILS_FULL;
        } break;

        case 0x02:
        {
            header.type = TYPE_HELLO;
        } break;

        case 0x0309:
        {           
            header.type = TYPE_PARAM_CHANGED;
        } break;

        default:
        {
            ESP_LOGI(TAG, "Unknown type %d", (int)type);            
            header.type = TYPE_UNKNOWN;
        } break;
    };
    
    header.size = usb_tonex_one_parse_value(FramedBuffer, &index);
    header.unknown = usb_tonex_one_parse_value(FramedBuffer, &index);

    ESP_LOGI(TAG, "usb_tonex_one_parse: type: %d size: %d", (int)header.type, (int)header.size);

    if ((out_len - index) != header.size)
    {
        ESP_LOGE(TAG, "Invalid message size");
        return STATUS_INVALID_FRAME;
    }

    // make sure we don't trip the task watchdog
    vTaskDelay(pdMS_TO_TICKS(5));

    // check message type
    switch (header.type)
    {
        case TYPE_HELLO:
        {
            ESP_LOGI(TAG, "Hello response");
            memcpy((void*)&TonexData->Message.Header,  (void*)&header, sizeof(header));        
            return STATUS_OK;
        }

        case TYPE_STATE_UPDATE:
        {
            return usb_tonex_one_parse_state(FramedBuffer, out_len, index);
        }
        
        case TYPE_STATE_PRESET_DETAILS:
        {
            return usb_tonex_one_parse_preset_details(FramedBuffer, out_len, index);
        }

        case TYPE_STATE_PRESET_DETAILS_FULL:
        {
            // don't need to process this anymore, thanks to IK new parameter comms :)
            return STATUS_OK;
        }

        case TYPE_PARAM_CHANGED:
        {
            ESP_LOGI(TAG, "Param change confirmation");
            return STATUS_OK;
        } break;

        default:
        {
            ESP_LOGI(TAG, "Unknown structure. Skipping.");            
            memcpy((void*)&TonexData->Message.Header, (void*)&header, sizeof(header));
            return STATUS_OK;
        }
    };
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static uint16_t usb_tonex_one_locate_message_end(uint8_t* data, uint16_t length)
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
static esp_err_t usb_tonex_one_process_single_message(uint8_t* data, uint16_t length)
{
    void* temp_ptr;  
    uint16_t current_preset;

    // check if we got a complete message(s)
    if ((length >= 2) && (data[0] == 0x7E) && (data[length - 1] == 0x7E))
    {
        ESP_LOGI(TAG, "Processing messages len: %d", (int)length);
        Status status = usb_tonex_one_parse(data, length);

        if (status != STATUS_OK)
        {
            ESP_LOGE(TAG, "Error parsing message: %d", (int)status);
        }
        else
        {
            ESP_LOGI(TAG, "Message Header type: %d", (int)TonexData->Message.Header.type);

            // check what we got
            switch (TonexData->Message.Header.type)
            {
                case TYPE_STATE_UPDATE:
                {
                    current_preset = usb_tonex_one_get_current_active_preset();
                    ESP_LOGI(TAG, "Received State Update. Current slot: %d. Preset: %d", (int)TonexData->Message.CurrentSlot, (int)current_preset);
                    
                    // debug
                    //ESP_LOG_BUFFER_HEXDUMP(TAG, data, length, ESP_LOG_INFO);

                    TonexData->TonexState = COMMS_STATE_READY;   

                    if (boot_init_needed)
                    {
                        // start getting preset names
                        usb_tonex_one_request_preset_details(boot_preset_request, 0);
                        boot_init_needed = 0;
                    }
                    else
                    {
                        // signal to refresh param UI with Globals
                        UI_RefreshParameterValues();

                        // update web UI
                        wifi_request_sync(WIFI_SYNC_TYPE_PARAMS, NULL, NULL);
                    }
                } break;

                case TYPE_STATE_PRESET_DETAILS:
                {
                    // locate the ToneOnePresetByteMarker[] to get preset name
                    temp_ptr = memmem((void*)data, length, (void*)ToneOnePresetByteMarker, sizeof(ToneOnePresetByteMarker));
                    if (temp_ptr != NULL)
                    {
                        ESP_LOGI(TAG, "Got preset name");

                        // grab name
                        memcpy((void*)preset_name, (void*)(temp_ptr + sizeof(ToneOnePresetByteMarker)), TONEX_ONE_RESP_OFFSET_PRESET_NAME_LEN);                
                    }

                    current_preset = usb_tonex_one_get_current_active_preset();

                    if (boot_preset_request < MAX_PRESETS) {
                        // save preset name 
                        control_sync_preset_name(boot_preset_request, preset_name);

                        // get next preset name
                        boot_preset_request++;
                        usb_tonex_one_request_preset_details(boot_preset_request, 0);
                    } else if (boot_preset_request == MAX_PRESETS) {
                        // all other preset nammes grabbed, get current preset details
                        boot_preset_request++;
                        usb_tonex_one_request_preset_details(current_preset, 0);
                    } else {
                        ESP_LOGI(TAG, "Received State Update. Current slot: %d. Preset: %d", (int)TonexData->Message.CurrentSlot, (int)current_preset);
                        
                        // make sure we are showing the correct preset as active                
                        control_sync_preset_details(current_preset, preset_name);

                        // read the preset params
                        usb_tonex_one_parse_preset_parameters(data, length);

                        // signal to refresh param UI
                        UI_RefreshParameterValues();

                        // update web UI
                        wifi_request_sync(WIFI_SYNC_TYPE_PARAMS, NULL, NULL);

                        // debug dump parameters
                        //tonex_dump_parameters();
                    }
                } break;

                case TYPE_HELLO:
                {
                    ESP_LOGI(TAG, "Received Hello");

                    // get current state
                    usb_tonex_one_request_state();
                    TonexData->TonexState = COMMS_STATE_GET_STATE;

                    // flag that we need to do the boot init procedure
                    boot_init_needed = 1;
                    boot_preset_request = 0;
                } break;

                case TYPE_STATE_PRESET_DETAILS_FULL:
                {
                    // ignore
                    ESP_LOGI(TAG, "Received Preset details full");
                } break;

                default:
                {
                    ESP_LOGI(TAG, "Message unknown %d", (int)TonexData->Message.Header.type);
                } break;
            }
        }

        return ESP_OK;
    }
    else
    {
        ESP_LOGW(TAG, "Missing start or end bytes. %d, %d, %d", (int)length, (int)data[0], (int)data[length - 1]);
        //ESP_LOG_BUFFER_HEXDUMP(TAG, data, length, ESP_LOG_INFO);
        return ESP_FAIL;
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void usb_tonex_one_handle(class_driver_t* driver_obj)
{        
    tUSBMessage message;

    // check state
    switch (TonexData->TonexState)
    {
        case COMMS_STATE_IDLE:
        default:
        {
            // do the hello 
            if (usb_tonex_one_hello() == ESP_OK)
            {
                TonexData->TonexState = COMMS_STATE_HELLO;
            }
            else
            {
                ESP_LOGI(TAG, "Send Hello failed");
            }
        } break;

        case COMMS_STATE_HELLO:
        {
            // waiting for response to arrive
        } break;

        case COMMS_STATE_READY:
        {
            // check for any input messages
            if (xQueueReceive(input_queue, (void*)&message, 0) == pdPASS)
            {
                ESP_LOGI(TAG, "Got Input message: %d", message.Command);

                // process it
                switch (message.Command)
                {
                    case USB_COMMAND_SET_PRESET:
                    {
                        if (message.Payload < MAX_PRESETS)
                        {
                            // always using Stomp mode C for preset setting
                            if (usb_tonex_one_set_preset_in_slot(message.Payload, C, 1) != ESP_OK)
                            {
                                // failed return to queue?
                            }
                        }
                    } break;   
                    
                    case USB_COMMAND_MODIFY_PARAMETER:
                    {
                        if (message.Payload < TONEX_PARAM_LAST)
                        {
                            // modify the param
                            usb_tonex_one_modify_parameter(message.Payload, message.PayloadFloat);

                            // send it
                            usb_tonex_one_send_single_parameter(message.Payload, message.PayloadFloat);
                        }
                        else if (message.Payload < TONEX_GLOBAL_LAST)
                        {
                            // modify the global
                            usb_tonex_one_modify_global(message.Payload, message.PayloadFloat);

                            // debug
                            //usb_tonex_one_dump_state(&TonexData->Message.PedalData.TonexStateData);

                            // send it by setting the same preset active again, which sends the state data
                            usb_tonex_one_set_preset_in_slot(TonexData->Message.SlotCPreset, C, 1);
                        }
                        else
                        {
                            ESP_LOGW(TAG, "Attempt to modify unknown param %d", (int)message.Payload);
                        }
                    } break;
                }
            }
        } break;

        case COMMS_STATE_GET_STATE:
        {
            // waiting for state data
        } break;
    }

    // check if we have received anything (via RX interrupt)
    for (uint8_t loop = 0; loop < MAX_INPUT_BUFFERS; loop++)
    {
        if (InputBuffers[loop].ReadyToRead)
        {
            ESP_LOGI(TAG, "Got data via CDC %d", InputBuffers[loop].Length);

            // debug
            //ESP_LOG_BUFFER_HEXDUMP(TAG, InputBuffers[loop].Data, InputBuffers[loop].Length, ESP_LOG_INFO);

            uint16_t end_marker_pos;
            uint16_t bytes_consumed = 0;
            uint8_t* rx_entry_ptr = (uint8_t*)InputBuffers[loop].Data;
            uint16_t rx_entry_length = InputBuffers[loop].Length;

            // process all messages received (may be multiple messages appended)
            do
            {    
                // locate the end of the message
                end_marker_pos = usb_tonex_one_locate_message_end(rx_entry_ptr, rx_entry_length);

                if (end_marker_pos == 0)
                {
                    ESP_LOGW(TAG, "Missing end marker!");
                    //ESP_LOG_BUFFER_HEXDUMP(TAG, rx_entry_ptr, rx_entry_length, ESP_LOG_INFO);
                    break;
                }
                else
                {
                    ESP_LOGI(TAG, "Found end marker: %d", end_marker_pos);
                }

                // debug
                //ESP_LOG_BUFFER_HEXDUMP(TAG, rx_entry_ptr, end_marker_pos + 1, ESP_LOG_INFO);

                // process it
                if (usb_tonex_one_process_single_message(rx_entry_ptr, end_marker_pos + 1) != ESP_OK)
                {
                    break;    
                }
            
                // skip this message
                rx_entry_ptr += (end_marker_pos + 1);
                bytes_consumed += (end_marker_pos + 1);

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
void usb_tonex_one_init(class_driver_t* driver_obj, QueueHandle_t comms_queue)
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
        memset((void*)InputBuffers[loop].Data, 0, RX_TEMP_BUFFER_SIZE);
        InputBuffers[loop].ReadyToWrite = 1;
        InputBuffers[loop].ReadyToRead = 0;
    }

    // more big buffers in PSRAM
    TxBuffer = heap_caps_malloc(RX_TEMP_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
    if (TxBuffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate TxBuffer buffer!");
        return;
    }
    
    FramedBuffer = heap_caps_malloc(RX_TEMP_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
    if (FramedBuffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate FramedBuffer buffer!");
        return;
    }

    TonexData = heap_caps_malloc(sizeof(tTonexData), MALLOC_CAP_SPIRAM);
    if (TonexData == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate TonexData buffer!");
        return;
    }

    memset((void*)TonexData, 0, sizeof(tTonexData));
    TonexData->TonexState = COMMS_STATE_IDLE;

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

    // install CDC host driver
    ESP_ERROR_CHECK(cdc_acm_host_install(NULL));

    ESP_LOGI(TAG, "Opening CDC ACM device 0x%04X:0x%04X", IK_MULTIMEDIA_USB_VENDOR, TONEX_ONE_PRODUCT_ID);

    // set the config
    const cdc_acm_host_device_config_t dev_config = {
        .connection_timeout_ms = 1000,
        .out_buffer_size = USB_TX_BUFFER_SIZE,
        .in_buffer_size = RX_TEMP_BUFFER_SIZE,
        .user_arg = NULL,
        .event_cb = NULL,
        .data_cb = usb_tonex_one_handle_rx
    };

    // release the reserved large buffers space we malloc'd at boot
    heap_caps_free(PreallocatedMemory);

    // debug
    //heap_caps_print_heap_info(MALLOC_CAP_DMA);

    // open it
    ESP_ERROR_CHECK(cdc_acm_host_open(IK_MULTIMEDIA_USB_VENDOR, TONEX_ONE_PRODUCT_ID, TONEX_ONE_CDC_INTERFACE_INDEX, &dev_config, &cdc_dev));
    assert(cdc_dev);
    
    //cdc_acm_host_desc_print(cdc_dev);
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG, "Setting up line coding");

    cdc_acm_line_coding_t line_coding;
    ESP_ERROR_CHECK(cdc_acm_host_line_coding_get(cdc_dev, &line_coding));
    ESP_LOGI(TAG, "Line Get: Rate: %d, Stop bits: %d, Parity: %d, Databits: %d", (int)line_coding.dwDTERate, (int)line_coding.bCharFormat, (int)line_coding.bParityType, (int)line_coding.bDataBits);

    // set line coding
    ESP_LOGI(TAG, "Set line coding");
    cdc_acm_line_coding_t new_line_coding = {
        .dwDTERate = 115200,
        .bCharFormat = 0,
        .bParityType = 0,
        .bDataBits = 8
    };

    if (cdc_acm_host_line_coding_set(cdc_dev, &new_line_coding) != ESP_OK)
    {
        ESP_LOGE(TAG, "Set line coding failed");
    }

    // disable flow control
    ESP_LOGI(TAG, "Set line state");
    if (cdc_acm_host_set_control_line_state(cdc_dev, true, true) != ESP_OK)
    {
        ESP_LOGE(TAG, "Set line state failed");
    }

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
void usb_tonex_one_deinit(void)
{
    //to do here: need to clean up properly if pedal disconnected
    //cdc_acm_host_close();
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void usb_tonex_one_preallocate_memory(void)
{
    // note here: the Tonex One requires large CDC buffers. Not many heap areas have enough contiguous space.
    // if we let the system boot and no Tonex is conncted, the other parts of the system allocate mem
    // and leave us with no spaces big enough to hold the buffers.
    // Work around here: preallocate the buffers we need, and then free them just before the CDC allocation,
    // effectively reserving them and then other parts of system can use different heap areas.
    // +256 here just to be sure.
    PreallocatedMemory = heap_caps_malloc(RX_TEMP_BUFFER_SIZE + USB_TX_BUFFER_SIZE + 256, MALLOC_CAP_DMA);
    if (PreallocatedMemory == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate PreallocatedMemory!");
    } 
}