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
#include "valeton_params.h"

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

typedef struct __attribute__ ((packed))  
{
    uint8_t effect_index;
    uint32_t byte_sequence;
} EffectMapEntry_t;

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
static uint8_t boot_sync_done = 0;

static const EffectMapEntry_t effect_map_nr[] = {
                                            {VALETON_EFFECT_NR_GATE, 0x1B000000},
                                          };

static const EffectMapEntry_t effect_map_pre[] = {
                                            {VALETON_EFFECT_PRE_COMP, 0x00000000},
                                            {VALETON_EFFECT_PRE_COMP4, 0x01000000},
                                            {VALETON_EFFECT_PRE_BOOST, 0x1A000000},
                                            {VALETON_EFFECT_PRE_MICRO_BOOST, 0x14000000},
                                            {VALETON_EFFECT_PRE_B_BOOST, 0x0B000000},
                                            {VALETON_EFFECT_PRE_TOUCHER, 0x0F000001},
                                            {VALETON_EFFECT_PRE_CRIER, 0x15000001},
                                            {VALETON_EFFECT_PRE_OCTA, 0x21000001},
                                            {VALETON_EFFECT_PRE_PITCH, 0x23000001},
                                            {VALETON_EFFECT_PRE_DETUNE, 0x29000001},
                                            { 0xFF, 0xFFFFFFFF}, // end of table
                                          };

static const EffectMapEntry_t effect_map_dst[] = {
                                            { VALETON_EFFECT_DIST_GREEN_OD, 0x00000003},
                                            { VALETON_EFFECT_DIST_YELLOW_OD, 0x02000003},
                                            { VALETON_EFFECT_DIST_SUPER_OD, 0x06000003},
                                            { VALETON_EFFECT_DIST_SM_DIST, 0x2A000003},
                                            { VALETON_EFFECT_DIST_PLUSTORTION, 0x29000003},
                                            { VALETON_EFFECT_DIST_LA_CHARGER, 0x30000003},
                                            { VALETON_EFFECT_DIST_DARKTALE, 0x2B000003},
                                            { VALETON_EFFECT_DIST_SORA_FUZZ, 0x22000003},
                                            { VALETON_EFFECT_DIST_RED_HAZE, 0x24000003},
                                            { VALETON_EFFECT_DIST_BASS_OD, 0x40000003}, 
                                            { 0xFF, 0xFFFFFFFF}, // end of table
                                          };

                                          
static const EffectMapEntry_t effect_map_amp[] = {
                                            { VALETON_EFFECT_AMP_TWEEDY, 0x01000007},
                                            { VALETON_EFFECT_AMP_BELLMAN_59N, 0x03000007},
                                            { VALETON_EFFECT_AMP_DARK_TWIN, 0x04000007},
                                            { VALETON_EFFECT_AMP_FOXY_30N, 0x11000007},
                                            { VALETON_EFFECT_AMP_J_120_CL, 0x14000007},
                                            { VALETON_EFFECT_AMP_MATCH_CL, 0x15000007},
                                            { VALETON_EFFECT_AMP_L_STAR_CL, 0x19000007},
                                            { VALETON_EFFECT_AMP_UK_45, 0x2A000007},
                                            { VALETON_EFFECT_AMP_UK_50JP, 0x2F000007},
                                            { VALETON_EFFECT_AMP_UK_800, 0x35000007},
                                            { VALETON_EFFECT_AMP_BELLMAN_59B, 0x24000007},
                                            { VALETON_EFFECT_AMP_FOXY_30TB, 0x27000007},
                                            { VALETON_EFFECT_AMP_SUPDUAL_OD, 0x28000007},
                                            { VALETON_EFFECT_AMP_SOLO100_OD, 0x47000007},
                                            { VALETON_EFFECT_AMP_Z38_OD, 0x49000007},
                                            { VALETON_EFFECT_AMP_BAD_KT_OD, 0x4B000007},
                                            { VALETON_EFFECT_AMP_JUICE_R100, 0x53000007},
                                            { VALETON_EFFECT_AMP_DIZZ_VH, 0x65000007},
                                            { VALETON_EFFECT_AMP_DIZZ_VH_PLUS, 0x6A000007},
                                            { VALETON_EFFECT_AMP_EAGLE_120, 0x5F000007},
                                            { VALETON_EFFECT_AMP_EV_51, 0x5A000007},
                                            { VALETON_EFFECT_AMP_SOLO100_LD, 0x59000007},
                                            { VALETON_EFFECT_AMP_MESS_DUALV, 0x68000007},
                                            { VALETON_EFFECT_AMP_MESS_DUALM, 0x69000007},
                                            { VALETON_EFFECT_AMP_POWER_LD, 0x63000007},
                                            { VALETON_EFFECT_AMP_FLAGMAN_PLUS, 0x5D000007},
                                            { VALETON_EFFECT_AMP_BOG_REDV, 0x6D000007},
                                            { VALETON_EFFECT_AMP_CLASSIC_BASS, 0x73000007},
                                            { VALETON_EFFECT_AMP_FOXY_BASS, 0x75000007},
                                            { VALETON_EFFECT_AMP_MESS_BASS, 0x77000007},
                                            { VALETON_EFFECT_AMP_AC_PRE1, 0x7A000008},
                                            { VALETON_EFFECT_AMP_AC_PRE2, 0x7B000008},
                                            { 0xFF, 0xFFFFFFFF}, // end of table
                                          };

static const EffectMapEntry_t effect_map_cab[] = {
                                            { VALETON_EFFECT_CAB_TWD_CP, 0x0100000A},
                                            { VALETON_EFFECT_CAB_DARK_VIT, 0x0400000A},
                                            { VALETON_EFFECT_CAB_FOXY_1X12, 0x0800000A},
                                            { VALETON_EFFECT_CAB_L_STAR_1X12, 0x0900000A},
                                            { VALETON_EFFECT_CAB_DARK_CS_2X12, 0x1B00000A},
                                            { VALETON_EFFECT_CAB_DARK_TWIN_2X12, 0x1200000A},
                                            { VALETON_EFFECT_CAB_SUP_STAR_2X12, 0x1900000A},
                                            { VALETON_EFFECT_CAB_J_120_2X12, 0x1100000A},
                                            { VALETON_EFFECT_CAB_FOXY_2X12, 0x0F00000A},
                                            { VALETON_EFFECT_CAB_UK_GRN_2X12, 0x1300000A},
                                            { VALETON_EFFECT_CAB_UK_GRN_4X12, 0x2200000A},
                                            { VALETON_EFFECT_CAB_BOG_4X12, 0x2500000A},
                                            { VALETON_EFFECT_CAB_DIZZ_4X12, 0x2E00000A},
                                            { VALETON_EFFECT_CAB_EV_4X12, 0x2000000A},
                                            { VALETON_EFFECT_CAB_SOLO_4X12, 0x2800000A},
                                            { VALETON_EFFECT_CAB_MESS_4X12, 0x2400000A},
                                            { VALETON_EFFECT_CAB_EAGLE_4X12, 0x2600000A},
                                            { VALETON_EFFECT_CAB_JUICE_4X12, 0x2900000A},
                                            { VALETON_EFFECT_CAB_BELLMAN_2X12, 0x1600000A},
                                            { VALETON_EFFECT_CAB_AMPG_4X10, 0x3800000A},
                                            { 0xFF, 0xFFFFFFFF}, // end of table
                                          };

static const EffectMapEntry_t effect_map_eq[] = {
                                            { VALETON_EFFECT_EQ_GUITAR_EQ1, 0x35000001},
                                            { VALETON_EFFECT_EQ_GUITAR_EQ2, 0x36000001},
                                            { VALETON_EFFECT_EQ_BASS_EQ1, 0x39000001},
                                            { VALETON_EFFECT_EQ_BASS_EQ2, 0x3A000001},
                                            { VALETON_EFFECT_EQ_MESS_EQ, 0x3C000001},
                                            { 0xFF, 0xFFFFFFFF}, // end of table
                                          };

static const EffectMapEntry_t effect_map_mod[] = {
                                            { VALETON_EFFECT_MOD_A_CHORUS, 0x00000004},
                                            { VALETON_EFFECT_MOD_B_CHORUS, 0x08000004},
                                            { VALETON_EFFECT_MOD_JET, 0x11000004},
                                            { VALETON_EFFECT_MOD_N_JET, 0x13000004},
                                            { VALETON_EFFECT_MOD_O_PHASE, 0x19000004},
                                            { VALETON_EFFECT_MOD_M_VIBE, 0x1F000004},
                                            { VALETON_EFFECT_MOD_V_ROTO, 0x15000004},
                                            { VALETON_EFFECT_MOD_VIBRATO, 0x17000004},
                                            { VALETON_EFFECT_MOD_O_TREM, 0x21000004},
                                            { VALETON_EFFECT_MOD_SINE_TREM, 0x26000004},
                                            { VALETON_EFFECT_MOD_BIAS_TREM, 0x28000004},
                                            { 0xFF, 0xFFFFFFFF}, // end of table
                                          };

static const EffectMapEntry_t effect_map_dly[] = {
                                            { VALETON_EFFECT_DLY_PURE, 0x0000000B},
                                            { VALETON_EFFECT_DLY_ANALOG, 0x0100000B},
                                            { VALETON_EFFECT_DLY_SLAPBACK, 0x0500000B},
                                            { VALETON_EFFECT_DLY_SWEET_ECHO, 0x0D00000B},
                                            { VALETON_EFFECT_DLY_TAPE, 0x0200000B},
                                            { VALETON_EFFECT_DLY_TUBE, 0x0B00000B},
                                            { VALETON_EFFECT_DLY_REV_ECHO, 0x1300000B},
                                            { VALETON_EFFECT_DLY_RING_ECHO, 0x0900000B},
                                            { VALETON_EFFECT_DLY_SWEEP_ECHO, 0x0600000B},
                                            { VALETON_EFFECT_DLY_PING_PONG, 0x0400000B},
                                            { 0xFF, 0xFFFFFFFF}, // end of table
                                          };

static const EffectMapEntry_t effect_map_rvb[] = {
                                            { VALETON_EFFECT_RVB_AIR, 0xB00000C},
                                            { VALETON_EFFECT_RVB_ROOM, 0x00000C},
                                            { VALETON_EFFECT_RVB_HALL, 0x100000C},
                                            { VALETON_EFFECT_RVB_CHURCH, 0x200000C},
                                            { VALETON_EFFECT_RVB_PLATE_L, 0x1000000C},
                                            { VALETON_EFFECT_RVB_PLATE, 0x0F00000C},
                                            { VALETON_EFFECT_RVB_SPRING, 0x400000C},
                                            { VALETON_EFFECT_RVB_N_STAR, 0x600000C},
                                            { VALETON_EFFECT_RVB_DEEPSEA, 0x700000C},
                                            { VALETON_EFFECT_RVB_SWEET_SPACE, 0x1500000C},
                                            { 0xFF, 0xFFFFFFFF}, // end of table
                                          };

static const EffectMapEntry_t effect_map_ns[] = {
                                            { 0, 0x0000000F},
                                            { 1, 0x0100000F},
                                            { 2, 0x0200000F},
                                            { 3, 0x0300000F},
                                            { 4, 0x0400000F},
                                            { 5, 0x0500000F},
                                            { 6, 0x0600000F},
                                            { 7, 0x0700000F},
                                            { 8, 0x0800000F},
                                            { 9, 0x0900000F},
                                            { 10, 0x0A00000F},
                                            { 11, 0x0B00000F},
                                            { 12, 0x0C00000F},
                                            { 13, 0x0D00000F},
                                            { 14, 0x0E00000F},
                                            { 15, 0x0F00000F},
                                            { 16, 0x1000000F},
                                            { 17, 0x1100000F},
                                            { 18, 0x1200000F},
                                            { 19, 0x1300000F},
                                            { 20, 0x1400000F},
                                            { 21, 0x1500000F},
                                            { 22, 0x1600000F},
                                            { 23, 0x1700000F},
                                            { 24, 0x1800000F},
                                            { 25, 0x1900000F},
                                            { 26, 0x1A00000F},
                                            { 27, 0x1B00000F},
                                            { 28, 0x1C00000F},
                                            { 29, 0x1D00000F},
                                            { 30, 0x1E00000F},
                                            { 31, 0x1F00000F},
                                            { 32, 0x2000000F},
                                            { 33, 0x2100000F},
                                            { 34, 0x2200000F},
                                            { 35, 0x2300000F},
                                            { 36, 0x2400000F},
                                            { 37, 0x2500000F},
                                            { 38, 0x2600000F},
                                            { 39, 0x2700000F},
                                            { 40, 0x2800000F},
                                            { 41, 0x2900000F},
                                            { 42, 0x2A00000F},
                                            { 43, 0x2B00000F},
                                            { 44, 0x2C00000F},
                                            { 45, 0x2D00000F},
                                            { 46, 0x2E00000F},
                                            { 47, 0x2F00000F},
                                            { 48, 0x3000000F},
                                            { 49, 0x3100000F},
                                            { 50, 0x3200000F},
                                            { 51, 0x3300000F},
                                            { 52, 0x3400000F},
                                            { 53, 0x3500000F},
                                            { 54, 0x3600000F},
                                            { 55, 0x3700000F},
                                            { 56, 0x3800000F},
                                            { 57, 0x3900000F},
                                            { 58, 0x3A00000F},
                                            { 59, 0x3B00000F},
                                            { 60, 0x3C00000F},
                                            { 61, 0x3D00000F},
                                            { 62, 0x3E00000F},
                                            { 63, 0x3F00000F},
                                            { 64, 0x4000000F},
                                            { 65, 0x4100000F},
                                            { 66, 0x4200000F},
                                            { 67, 0x4300000F},
                                            { 68, 0x4400000F},
                                            { 69, 0x4500000F},
                                            { 70, 0x4600000F},
                                            { 71, 0x4700000F},
                                            { 72, 0x4800000F},
                                            { 73, 0x4900000F},
                                            { 74, 0x4A00000F},
                                            { 75, 0x4B00000F},
                                            { 76, 0x4C00000F},
                                            { 77, 0x4D00000F},
                                            { 78, 0x4E00000F},
                                            { 79, 0x4F00000F},
                                            { 0xFF, 0xFFFFFFFF}, // end of table
                                          };

static const EffectMapEntry_t* effect_maps[] = {
                                                effect_map_nr,
                                                effect_map_pre,
                                                effect_map_dst,
                                                effect_map_amp,
                                                effect_map_cab,
                                                effect_map_eq,
                                                effect_map_mod,
                                                effect_map_dly,
                                                effect_map_rvb,
                                                effect_map_ns
                                            };

/*
** Static function prototypes
*/
static void usb_valeton_gp5_request_preset_sync(void);
static void usb_valeton_gp5_request_current_preset(void);
static void usb_valeton_gp5_get_preset(uint8_t index);
static void usb_valeton_gp5_set_preset(uint8_t index);
static void usb_valeton_gp5_request_preset_params(void);
static void usb_valeton_gp5_set_effect_block_state(uint8_t block_index, uint8_t state);
static uint8_t usb_valeton_gp5_process_single_sysex(const uint8_t* buffer, uint32_t len) ;
static void usb_valeton_gp5_request_ir(void);
static void usb_valeton_gp5_request_nams(void);
static void usb_valeton_gp5_request_globals(void);

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
    // Total chunks e.g. 00 01 
    // Chunk index e.g. 00 00
    // Data size (2 byte)
    // 0101 for messages to GP5, 0102 for received? or could be commands and requests?
    // Payload
    // 0xF7 end marker

    memset((void*)usb_transfer, 0, sizeof(usb_transfer));
    memset((void*)TransmitBuffer, 0, VALETON_GP5_TX_TEMP_BUFFER_SIZE);

    // debug
    //ESP_LOGI(TAG, "usb_valeton_gp5_send_sysex input:");
    //ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, len, ESP_LOG_INFO);

    // set start marker
    TransmitBuffer[write_index++] = 0xF0;

    // next 2 bytes are Crc, skip for now
    write_index += 2;

    // Total chunks
    TransmitBuffer[write_index++] = 0x00;
    TransmitBuffer[write_index++] = 0x01;

    // Chunk index
    TransmitBuffer[write_index++] = 0x00;
    TransmitBuffer[write_index++] = 0x00;

    // length bytes. Value sent as e.g. 0x14 as 01 04. Sent as number of byte pairs hence divide by 2
    // +2 here for the 0102/0101 bytes that come next
    TransmitBuffer[write_index++] = ((len + 2) / 2) >> 4;
    TransmitBuffer[write_index++] = ((len + 2) / 2) & 0x0F;

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
    //ESP_LOGI(TAG, "usb_valeton_gp5_send_sysex usb tx:");
    //ESP_LOG_BUFFER_HEXDUMP(TAG, usb_transfer, write_index, ESP_LOG_INFO);

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
static uint8_t usb_valeton_gp5_parse_sysex(const uint8_t* buffer, uint32_t len) 
{
    uint32_t write_index = 0;
    uint32_t bytes_read = 0;
    uint16_t __attribute__((unused))  rx_length;
    uint8_t* tmp_ptr = (uint8_t*)buffer;
    uint16_t chunk_count = 0;
    uint16_t this_chunk = 0;
    uint8_t found_chunks = 0;

    // debug
    //ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, len, ESP_LOG_INFO);

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

                // next 2 bytes are total chunks in this message
                chunk_count = (*tmp_ptr << 4) | *(tmp_ptr + 1);
                tmp_ptr += 2;
                bytes_read += 2;

                // next byte is first byte of current chunk index
                this_chunk = (*tmp_ptr << 4);
                tmp_ptr++;
                bytes_read++;

                // skip the 0x04 
                tmp_ptr++;
                bytes_read++;

                // get last chunk byte
                this_chunk |= *tmp_ptr; 
                tmp_ptr++;
                bytes_read++;

                found_chunks = 1;

                // debug
                //ESP_LOGI(TAG, "Total chunks: %d. This chunk: %d", (int)chunk_count, (int)this_chunk);

                // grab next 2 bytes (length)
                // length is number of byte pairs/nibbles (half of byte length) to the next 0xF7
                rx_length = (*tmp_ptr << 4) | *(tmp_ptr + 1);
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

        if (found_chunks)
        {
            // see if we have all the chunks. +1 here as the total eg 10 is chunks 0 to 9
            if ((this_chunk + 1) == chunk_count)
            {
                // process this messsage
                usb_valeton_gp5_process_single_sysex(ProcessingBuffer, write_index);

                // reset vars for possible next chunk
                write_index = 0;
                found_chunks = 0;
                this_chunk = 0;
                chunk_count = 0;
            }
        }
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
static uint8_t usb_valeton_gp5_get_effect_block_index(uint32_t in, uint8_t block)
{
    if (block < VALETON_EFFECT_BLOCK_LAST)
    {
        const EffectMapEntry_t* map = effect_maps[block];

        //ESP_LOGI(TAG, "Effect map:%d %X ", (int)map->effect_index, (int)map->byte_sequence);

        // search for the in marker
        while (map->effect_index != 0xFF)
        {
            // debug
            //ESP_LOGI(TAG, "Effect map %X ", (int)map->byte_sequence);

            if (map->byte_sequence == in)
            {
                // found match
                return map->effect_index;
            }

            map++;
        }
    }

    // unknown
    ESP_LOGW(TAG, "Effect map in %X not found", (int)in);
    return 0;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static int16_t usb_valeton_gp5_convert_hex_byte_pair_s16(uint8_t byte1, uint8_t byte2) 
{
    int16_t value= (byte1 * 16) + byte2;

    if (value & 0x80) 
    {
        value = value - 0x100;
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
static uint8_t usb_valeton_gp5_process_single_sysex(const uint8_t* buffer, uint32_t len) 
{
    uint32_t read_index = 0;
    uint16_t preset_index = 0;
    uint8_t string_index;
    uint16_t ascii_char;
    uint16_t function;
    char name_string[20] = {0};

    ESP_LOGI(TAG, "usb_valeton_gp5_process_single_sysex len: %d", (int)len);
    // debug
    //ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, len, ESP_LOG_INFO);

    // check the message type
    read_index = 0;
   
    // skip the 0102/0101 marker
    read_index += 2;

    // next 2 bytes are function code
    function = (buffer[read_index] << 4) | buffer[read_index + 1];
    read_index += 2;

    switch (function)
    {
        case 0x08:
        {
            // this seems to be a confirmation message on param change
            ESP_LOGI(TAG, "Got 0x08");            
        } break;

        case 0x10:
        {
            tModellerParameter* param_ptr;

            ESP_LOGI(TAG, "Got Globals");
            
            // debug
            //ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&buffer[read_index], len - read_index, ESP_LOG_INFO);
           
            // skip to master volume
            read_index += 40;

            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {                
                // master volume
                param_ptr[VALETON_GLOBAL_MASTER_VOLUME].Value = (float)usb_valeton_gp5_convert_hex_byte_pair_s16(buffer[read_index], buffer[read_index + 1]);
                read_index += 36;

                // get input trim
                param_ptr[VALETON_GLOBAL_INPUT_TRIM].Value = (float)usb_valeton_gp5_convert_hex_byte_pair_s16(buffer[read_index], buffer[read_index + 1]);
                read_index += 10;

                // rec level
                param_ptr[VALETON_GLOBAL_RECORD_LEVEL].Value = (float)usb_valeton_gp5_convert_hex_byte_pair_s16(buffer[read_index], buffer[read_index + 1]);
                read_index += 10;

                // mon level
                param_ptr[VALETON_GLOBAL_MONITOR_LEVEL].Value = (float)usb_valeton_gp5_convert_hex_byte_pair_s16(buffer[read_index], buffer[read_index + 1]);
                read_index += 10;

                // show BT level
                param_ptr[VALETON_GLOBAL_BT_LEVEL].Value = (float)usb_valeton_gp5_convert_hex_byte_pair_s16(buffer[read_index], buffer[read_index + 1]);
                read_index += 10;
            
                read_index += 21;
                param_ptr[VALETON_GLOBAL_CABSIM_BYPASS].Value = (float)buffer[read_index];
                read_index += 10;

                // footswitch mode is next byte

                valeton_params_release_locked_access();
            }

            // debug
            valeton_dump_parameters();
            
            // signal to refresh param UI
            UI_RefreshParameterValues();

            // update web UI
            wifi_request_sync(WIFI_SYNC_TYPE_PARAMS, NULL, NULL);

            control_set_sync_complete();

            // request nams
            usb_valeton_gp5_request_nams();
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
            // IRs
            //ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&buffer[read_index], len - read_index, ESP_LOG_INFO);

            ESP_LOGI(TAG, "Got IRs");
        } break;

        case 0x24:
        {
            // NAM models/Snaptones
            //ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&buffer[read_index], len - read_index, ESP_LOG_INFO);

            ESP_LOGI(TAG, "Got Snaptones");

            // request ITs
            usb_valeton_gp5_request_ir();
        } break;

        case 0x40:
        {
            // preset data
            ESP_LOGI(TAG, "Found Preset Data");

            for (uint32_t presets = 0; presets < MAX_PRESETS_VALETON_GP5; presets++)
            {
                // get the preset index
                preset_index = (buffer[read_index] * 16) + buffer[read_index + 1];
                read_index += 2;

                // debug
                //ESP_LOGI(TAG, "Preset Index: %d %d", (int)buffer[read_index - 2], (int)buffer[read_index - 1]);

                // skip 6 bytes of zeros
                read_index += 6;
                string_index = 0;

                // next is 32 bytes/16 characters of preset name, 2 bytes per char
                for (uint32_t character = 0; character < 16; character++)
                {
                    // get 4 bit nibbles into byte e.g. 04 07 into 47
                    ascii_char = (buffer[read_index] << 4) | buffer[read_index + 1];
                    read_index += 2;

                    name_string[string_index++] = (char)ascii_char;
                }
                            
                // save it
                control_sync_preset_name(preset_index, name_string);

                // don't smash the control input queue too hard
                vTaskDelay(10);
            }

            // request the preset
            usb_valeton_gp5_request_current_preset();
            
            ValetonGP5Data->State = COMMS_STATE_READY;
        } break;
        
        case 0x41: 
        {
            // current preset parameters
            ESP_LOGI(TAG, "Current Preset params");

            // debug
            //ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&buffer[read_index], len - read_index, ESP_LOG_INFO);
            uint8_t temp_buf[4];
            uint32_t param_index = 0;
            float temp_val;
            tModellerParameter* param_ptr;
            uint8_t effect_states_1;
            uint8_t effect_states_2;
            uint8_t loop;

            // skip to patch vol
            read_index += 96; 

            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                // get patch volume
                param_ptr[VALETON_PARAM_PATCH_VOLUME].Value = (float)usb_valeton_gp5_convert_hex_byte_pair_s16(buffer[read_index], buffer[read_index + 1]);
                valeton_params_release_locked_access();
            }
            
            // skip to effect block bit flags
            read_index += 40;

            effect_states_1 = (buffer[read_index] << 4) | (buffer[read_index + 1] & 0x0F);  
            read_index += 2;
            effect_states_2 = (buffer[read_index] << 4) | (buffer[read_index + 1] & 0x0F);  
            read_index += 2;

            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                // set effect block states
                param_ptr[VALETON_PARAM_NR_ENABLE].Value = (float)((effect_states_1 & 1) != 0);
                param_ptr[VALETON_PARAM_PRE_ENABLE].Value = (float)((effect_states_1 & 2) != 0);
                param_ptr[VALETON_PARAM_DIST_ENABLE].Value = (float)((effect_states_1 & 4) != 0);
                param_ptr[VALETON_PARAM_AMP_ENABLE].Value = (float)((effect_states_1 & 8) != 0);
                param_ptr[VALETON_PARAM_CAB_ENABLE].Value = (float)((effect_states_1 & 16) != 0);
                param_ptr[VALETON_PARAM_EQ_ENABLE].Value = (float)((effect_states_1 & 32) != 0);
                param_ptr[VALETON_PARAM_MOD_ENABLE].Value = (float)((effect_states_1 & 64) != 0);
                param_ptr[VALETON_PARAM_DLY_ENABLE].Value = (float)((effect_states_1 & 128) != 0);
                param_ptr[VALETON_PARAM_RVB_ENABLE].Value = (float)((effect_states_2 & 1) != 0);
                param_ptr[VALETON_PARAM_NS_ENABLE].Value = (float)((effect_states_2 & 2) != 0);
                
                valeton_params_release_locked_access();
            }
           
            // skip to effect order. these are 2 byte values, with block index in order from slot 0 to 9
            read_index += 12;

            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                // set effect block order
                param_ptr[VALETON_PARAM_EFFECT_SLOT_0].Value = (float)((buffer[read_index + 0] << 4) | (buffer[read_index + 1] & 0x0F));
                param_ptr[VALETON_PARAM_EFFECT_SLOT_1].Value = (float)((buffer[read_index + 2] << 4) | (buffer[read_index + 3] & 0x0F));
                param_ptr[VALETON_PARAM_EFFECT_SLOT_2].Value = (float)((buffer[read_index + 4] << 4) | (buffer[read_index + 5] & 0x0F));
                param_ptr[VALETON_PARAM_EFFECT_SLOT_3].Value = (float)((buffer[read_index + 6] << 4) | (buffer[read_index + 7] & 0x0F));
                param_ptr[VALETON_PARAM_EFFECT_SLOT_4].Value = (float)((buffer[read_index + 8] << 4) | (buffer[read_index + 9] & 0x0F));
                param_ptr[VALETON_PARAM_EFFECT_SLOT_5].Value = (float)((buffer[read_index + 10] << 4) | (buffer[read_index + 11] & 0x0F));
                param_ptr[VALETON_PARAM_EFFECT_SLOT_6].Value = (float)((buffer[read_index + 12] << 4) | (buffer[read_index + 13] & 0x0F));
                param_ptr[VALETON_PARAM_EFFECT_SLOT_7].Value = (float)((buffer[read_index + 14] << 4) | (buffer[read_index + 15] & 0x0F));
                param_ptr[VALETON_PARAM_EFFECT_SLOT_8].Value = (float)((buffer[read_index + 16] << 4) | (buffer[read_index + 17] & 0x0F));
                param_ptr[VALETON_PARAM_EFFECT_SLOT_9].Value = (float)((buffer[read_index + 18] << 4) | (buffer[read_index + 19] & 0x0F));
                
                valeton_params_release_locked_access();
            }

            // debug
            //ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&buffer[read_index], 32, ESP_LOG_INFO);

            // skip to effect block models
            read_index += 28;
            
            // debug
            //ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&buffer[read_index], 80, ESP_LOG_INFO);

            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                for (loop = VALETON_EFFECT_BLOCK_NR; loop < VALETON_EFFECT_BLOCK_NS; loop++)
                {
                    // get 4 byte effect code from 8 nibbles
                    uint32_t effect_code = (((buffer[read_index] << 4) | (buffer[read_index + 1] & 0x0F)) << 24);
                    effect_code |= (((buffer[read_index + 2] << 4) | (buffer[read_index + 3] & 0x0F)) << 16);
                    effect_code |= (((buffer[read_index + 4] << 4) | (buffer[read_index + 5] & 0x0F)) << 8);
                    effect_code |= (((buffer[read_index + 6] << 4) | (buffer[read_index + 7] & 0x0F)));
                      
                    //ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&buffer[read_index], 8, ESP_LOG_INFO);
                    
                    param_ptr[VALETON_PARAM_NR_TYPE + loop].Value = (float)usb_valeton_gp5_get_effect_block_index(effect_code, loop);
                    read_index += 8;

                    //ESP_LOGI(TAG, "Effect code %X for effect %d, model:%d", (int)effect_code, (int)loop, (int)param_ptr[VALETON_PARAM_NR_TYPE + loop].Value);
                } 

                valeton_params_release_locked_access();
            }

            // skip to start of params, back to back 32 bit big-endian floats
            read_index += 16; 
            
            //ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&buffer[read_index], 64, ESP_LOG_INFO);
            //ESP_LOGI(TAG, "Param values read_index: %d", (int)read_index);

            // params following match the order defined in Valeton params from VALETON_PARAM_NR_PARAM_0 down
            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                while (read_index < len)
                {
                    // join 8 nibbles to form a 4 byte value
                    for (loop = 0; loop < 4; loop++)
                    {
                        temp_buf[loop] = buffer[read_index++] << 4;
                        temp_buf[loop] |= buffer[read_index++] & 0x0F;
                    }

                    // get value as 32 bit big-endian float
                    memcpy((void*)&temp_val, (void*)temp_buf, sizeof(float));

                    if ((VALETON_PARAM_NR_PARAM_0 + param_index) < VALETON_PARAM_LAST)
                    {
                        // update local copy                
                        param_ptr[VALETON_PARAM_NR_PARAM_0 + param_index].Value = temp_val;
                    }
                    else
                    {
                        ESP_LOGW(TAG, "Invalid Param count %d %f!", (int)(VALETON_PARAM_NR_PARAM_0 + param_index), temp_val);  
                    }
                    
                    //ESP_LOGI(TAG, "Param val %d: %3.2f", (int)param_index, temp_val);
                    param_index++;
                }

                valeton_params_release_locked_access();
            }
            else
            {
                ESP_LOGW(TAG, "Param mutex failed!");  
            }
         
            if (!boot_sync_done)
            {
                // request globals once only
                usb_valeton_gp5_request_globals();
                boot_sync_done = 1;
            }
            else
            {
                // if we have messages waiting in the queue, it will trigger another
                // change that will overwrite this one. Skip the UI refresh to save time
                if (uxQueueMessagesWaiting(input_queue) == 0)
                {
                    // signal to refresh param UI
                    UI_RefreshParameterValues();

                    // update web UI
                    wifi_request_sync(WIFI_SYNC_TYPE_PARAMS, NULL, NULL);
                }
            }
        } break;

        case 0x43:
        {
            // current preset index
            //ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&buffer[read_index], len - read_index, ESP_LOG_INFO);
            uint8_t current_preset = (buffer[read_index] << 4) | buffer[read_index + 1];

            ESP_LOGI(TAG, "Got current preset index: %d", current_preset);

            // update UI
            control_sync_preset_details(current_preset, "");

            // request params
            usb_valeton_gp5_request_preset_params();
        } break;
        
        case 0x45:
        {
            // unknown. Seems to be some kind of confirmation response or short data
            ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&buffer[read_index], len - read_index, ESP_LOG_INFO);

            ESP_LOGI(TAG, "Got unknown 0x45");

            // example response
            //                                      f0 07
            //  0d 00 01 00 00 00 03 01 02 04 05 00 00 f7
        } break;

        case 0x47:
        {
            // effect selection confirmation.
            ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&buffer[read_index], len - read_index, ESP_LOG_INFO);

            ESP_LOGI(TAG, "Got 0x47");

            // example response
            //  f0 0c 05 00 01 00 00 00 0e 01 01 04 07 00 01 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 f7
        } break;

        case 0x48:
        {
            // parameter change confirmation
            ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&buffer[read_index], len - read_index, ESP_LOG_INFO);

            uint8_t effect_block = (buffer[read_index] << 4) | buffer[read_index + 1];
            read_index += 8;

            uint8_t param_index = (buffer[read_index] << 4) | buffer[read_index + 1];
            read_index += 6;

            uint8_t value[4];
            value[0] =  (buffer[read_index] << 4) | buffer[read_index + 1];   
            read_index += 2;

            value[1] =  (buffer[read_index] << 4) | buffer[read_index + 1];   
            read_index += 2;

            value[2] =  (buffer[read_index] << 4) | buffer[read_index + 1];   
            read_index += 2;

            value[3] =  (buffer[read_index] << 4) | buffer[read_index + 1];   
            read_index += 2;

            float value_f;
            memcpy((void*)&value_f, (void*)value, sizeof(value_f));
            ESP_LOGI(TAG, "Got 0x48 param changed. Block:%d. Param Index:%d. Value:%f", (int)effect_block, (int)param_index, value_f);

            // locate the parameter and update local copy
            tModellerParameter* param_ptr = NULL;
            uint8_t block;
            uint8_t __attribute__((unused)) pedal;
            uint8_t param;

            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                for (uint32_t param_index = 0; param_index < VALETON_PARAM_LAST; param_index++)
                {
                    block = param_ptr[param_index].Data1;
                    pedal = param_ptr[param_index].Data2;
                    param = param_ptr[param_index].Data3;

                    if ((effect_block == block) && (param_index == param))
                    {
                        // found it, update local copy
                        param_ptr[param_index].Value = value_f;
                        break;
                    }
                }

                valeton_params_release_locked_access();
            }
        } break;

        case 0x49:
        {
            // effect block state
            //ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&buffer[read_index], len - read_index, ESP_LOG_INFO);
            uint8_t effect_block = (buffer[read_index] << 4) | buffer[read_index + 1];
            read_index += 2;

            // skip to status byte
            read_index += 7;
            uint8_t state = buffer[read_index];

            ESP_LOGI(TAG, "Got effect block state %d %d", effect_block, state);         
        } break;

        case 0x50:
        {
            // unknown
            ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)&buffer[read_index], len - read_index, ESP_LOG_INFO);

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
            ESP_LOG_BUFFER_HEXDUMP(TAG, (uint8_t*)buffer, 16, ESP_LOG_WARN);
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
static void __attribute__((unused)) usb_valeton_gp5_request_globals(void)
{
    uint8_t midi_tx[] = {0x01, 0x00};

    ESP_LOGI(TAG, "Request globals");    

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x02);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void __attribute__((unused)) usb_valeton_gp5_request_unknown_2(void)
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
static void __attribute__((unused)) usb_valeton_gp5_request_unknown_3(void)
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
static void __attribute__((unused)) usb_valeton_gp5_request_ir(void)
{
    uint8_t midi_tx[] = {0x02, 0x00};

    ESP_LOGI(TAG, "Request IR");

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x02);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void __attribute__((unused)) usb_valeton_gp5_request_nams(void)
{
    uint8_t midi_tx[] = {0x02, 0x04};

    ESP_LOGI(TAG, "Request NAMs");

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x02);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void __attribute__((unused)) usb_valeton_gp5_get_preset(uint8_t index)
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

    uint8_t midi_tx[] = {0x04, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // set the preset index
    midi_tx[2] = index >> 4;
    midi_tx[3] = index & 0x0F;

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x01);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_set_effect_block_state(uint8_t block_index, uint8_t state)
{
    uint8_t midi_tx[] = {0x04, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // set the block index
    midi_tx[2] = block_index >> 4;
    midi_tx[3] = block_index & 0x0F;

    // set state
    midi_tx[11] = state;

    ESP_LOGI(TAG, "Set Effect Block state %d %d", block_index, state);

    // debug
    //ESP_LOG_BUFFER_HEXDUMP(TAG, midi_tx, sizeof(midi_tx), ESP_LOG_INFO);

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x01);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_set_effect_block_model(uint8_t block_index, uint8_t model)
{
    //                               | block  |                                      | block  |                                      |          effect code                       |
    uint8_t midi_tx[] = {0x04, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint32_t effect_code = 0;

    // set the block index (twice)
    midi_tx[2] = block_index >> 4;
    midi_tx[3] = block_index & 0x0F;

    midi_tx[10] = block_index >> 4;
    midi_tx[11] = block_index & 0x0F;

    if (block_index < VALETON_EFFECT_BLOCK_LAST)
    {
        const EffectMapEntry_t* map = effect_maps[block_index];

        // search for the matching effect code for the model
        while (map->effect_index != 0xFF)
        {
            // debug
            //ESP_LOGI(TAG, "Effect map %X ", (int)map->byte_sequence);

            if (map->effect_index == model)
            {
                // found match
                effect_code = map->byte_sequence;
                break;
            }

            map++;
        }
    }

    if (effect_code != 0)
    {
        ESP_LOGI(TAG, "Set Effect Block model %d %d %X", (int)block_index, (int)model, (int)effect_code);

        // set the effect code
        uint8_t* ptr = (uint8_t*)&effect_code;
        // 18 to 25
        midi_tx[18] = ptr[3] >> 4;
        midi_tx[19] = ptr[3] & 0x0F;
        midi_tx[20] = ptr[2] >> 4;
        midi_tx[11] = ptr[2] & 0x0F;
        midi_tx[22] = ptr[1] >> 4;
        midi_tx[23] = ptr[1] & 0x0F;
        midi_tx[24] = ptr[0] >> 4;
        midi_tx[25] = ptr[0] & 0x0F;

        // debug
        //ESP_LOG_BUFFER_HEXDUMP(TAG, midi_tx, sizeof(midi_tx), ESP_LOG_INFO);

        usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x01);
    }
    else
    {
        ESP_LOGI(TAG, "Set Effect Block model code not found for %d %d", block_index, model);
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_set_effect_block_model_parameter(uint8_t block_index, uint8_t parameter_index, float value)
{
    uint8_t midi_tx[] = {0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // set the block index
    midi_tx[2] = block_index >> 4;
    midi_tx[3] = block_index & 0x0F;

    // set parameter index
    midi_tx[10] = parameter_index >> 4;
    midi_tx[11] = parameter_index & 0x0F;

    // set parameter value, big endian IEEE float value in nibbles
    uint8_t float_bytes[4];
    memcpy((void*)float_bytes, (void*)&value, sizeof(value));

    // break into 4 bit nibbles.
    uint8_t index = 18;
    for (uint8_t loop = 0; loop < 4; loop++)
    {
        midi_tx[index++] = float_bytes[loop] >> 4;
        midi_tx[index++] = float_bytes[loop] & 0x0F;
    }

    ESP_LOGI(TAG, "Set Effect Block model parameter %d %d %3.2f", block_index, parameter_index, value);

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x01);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_set_global(uint16_t param_index, float value)
{
    uint16_t temp_val;
    uint8_t midi_tx[] = {0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    switch (param_index)
    {
        case VALETON_GLOBAL_INPUT_TRIM:
        {
            midi_tx[3] = 1;
            midi_tx[5] = 3;
        } break;

        case VALETON_GLOBAL_MASTER_VOLUME:
        {
            midi_tx[3] = 2;
            midi_tx[5] = 2;
        } break;

        case VALETON_GLOBAL_CABSIM_BYPASS:
        {
            midi_tx[3] = 3;
            midi_tx[5] = 3;
        } break;

        case VALETON_GLOBAL_RECORD_LEVEL:
        {
            midi_tx[3] = 1;
            midi_tx[5] = 4;
        } break;

        case VALETON_GLOBAL_MONITOR_LEVEL:
        {
            midi_tx[3] = 2;
            midi_tx[5] = 4;
        } break;

        case VALETON_GLOBAL_BT_LEVEL:
        {
            midi_tx[3] = 5;
            midi_tx[5] = 4;
        } break;

        case VALETON_GLOBAL_BPM:            // fallthrough
        default:
        {
            ESP_LOGW(TAG, "Invalid set global %d", param_index);
            return;
        } break;
    }

    // set the value
    if (value < 0)
    {
        temp_val = (uint16_t)(0x100 + value);
    }
    else
    {
        temp_val = (uint16_t)value;
    }

    midi_tx[10] = temp_val >> 8;
    midi_tx[11] = temp_val & 0xFF;

    ESP_LOGI(TAG, "Set Global %d: %f", param_index, value);

    // debug
    //ESP_LOG_BUFFER_HEXDUMP(TAG, midi_tx, sizeof(midi_tx), ESP_LOG_INFO);

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x01);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void usb_valeton_gp5_set_patch_volume(float value)
{
    uint16_t temp_val;
    uint8_t midi_tx[] = {0x04, 0x02, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // set the value
    if (value < 0)
    {
        temp_val = (uint16_t)(0x100 + value);
    }
    else
    {
        temp_val = (uint16_t)value;
    }

    midi_tx[10] = temp_val >> 8;
    midi_tx[11] = temp_val & 0xFF;

    ESP_LOGI(TAG, "Set Patch volume %f", value);

    // debug
    //ESP_LOG_BUFFER_HEXDUMP(TAG, midi_tx, sizeof(midi_tx), ESP_LOG_INFO);

    usb_valeton_gp5_send_sysex((const uint8_t*)midi_tx, sizeof(midi_tx), 0x01);
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
static esp_err_t usb_valeton_gp5_send_single_parameter(uint16_t index, float value)
{
    esp_err_t res = ESP_FAIL;
    tModellerParameter* param_ptr = NULL;

    if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
    {
        uint8_t block = param_ptr[index].Data1;
        uint8_t pedal = param_ptr[index].Data2;
        uint8_t param = param_ptr[index].Data3;

        valeton_params_release_locked_access();

        if (pedal == VALETON_BLOCK_SELECTION)
        {
            // pedal selection within block
            usb_valeton_gp5_set_effect_block_model(block, (uint8_t)value);
        }
        else if (pedal == VALETON_BLOCK_ENABLE)
        {
            // block enable
            usb_valeton_gp5_set_effect_block_state(block, (uint8_t)value);
        }
        else
        {
            // regular param, send it
            usb_valeton_gp5_set_effect_block_model_parameter(block, param, value);
        }

        // if we have messages waiting in the queue, it will trigger another
        // change that will overwrite this one. Skip the UI refresh to save time
        if (uxQueueMessagesWaiting(input_queue) == 0)
        {
            usb_valeton_gp5_request_preset_params();

            // signal to refresh param UI
            //UI_RefreshParameterValues();

            // update web UI
            //wifi_request_sync(WIFI_SYNC_TYPE_PARAMS, NULL, NULL);
        }

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
esp_err_t usb_valeton_gp5_modify_parameter(uint16_t index, float value)
{
    tModellerParameter* param_ptr = NULL;
    esp_err_t res = ESP_FAIL;
     
    if (index >= VALETON_PARAM_LAST)
    {
        ESP_LOGE(TAG, "usb_valeton_gp5_modify_parameter invalid index %d", (int)index);   
        return ESP_FAIL;
    }
        
    if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
    {
        ESP_LOGI(TAG, "usb_valeton_gp5_modify_parameter index: %d name: %s value: %02f", (int)index, param_ptr[index].Name, value);  

        // update the local copy
        memcpy((void*)&param_ptr[index].Value, (void*)&value, sizeof(float));

        valeton_params_release_locked_access();
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
                        if (message.Payload < VALETON_PARAM_LAST)
                        {
                            if (message.Payload == VALETON_PARAM_PATCH_VOLUME)
                            {
                                // special case for patch vol
                                usb_valeton_gp5_set_patch_volume(message.PayloadFloat);
                            }
                            else
                            {
                                // modify param
                                usb_valeton_gp5_modify_parameter(message.Payload, message.PayloadFloat);

                                // send it
                                usb_valeton_gp5_send_single_parameter(message.Payload, message.PayloadFloat);
                            }
                        }
                        else if (message.Payload < VALETON_GLOBAL_LAST)
                        {
                            // modify the global
                            usb_valeton_gp5_set_global(message.Payload, message.PayloadFloat);
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
    vTaskDelay(pdMS_TO_TICKS(200));

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
