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


#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_check.h"
#include "esp_log.h"
#include "usb/usb_host.h"
#include "driver/i2c_master.h"
#include "nvs_flash.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_ota_ops.h"
#include "sys/param.h"
#include "main.h"
#include "control.h"
#include "usb_comms.h"
#include "usb/usb_host.h"
#include "usb_tonex_one.h"
#include "footswitches.h"
#include "display.h"
#include "wifi_config.h"
#include "task_priorities.h"
#include "tonex_params.h"

#define CTRL_TASK_STACK_SIZE                (3 * 1024)
#define NVS_USERDATA_NAME                   "userdata"        

#define MAX_TEXT_LENGTH                     128
#define MAX_BT_CUSTOM_NAME                  25                 

enum CommandEvents
{
    EVENT_PRESET_DOWN,
    EVENT_PRESET_UP,
    EVENT_PRESET_INDEX,
    EVENT_BANK_INDEX,
    EVENT_SET_PRESET_NAME,
    EVENT_SET_PRESET_DETAILS,
    EVENT_SET_USB_STATUS,
    EVENT_SET_BT_STATUS,
    EVENT_SET_WIFI_STATUS,
    EVENT_SET_AMP_SKIN,
    EVENT_SAVE_USER_DATA,
    EVENT_SET_USER_TEXT,
    EVENT_SET_CONFIG_ITEM_INT,
    EVENT_SET_CONFIG_ITEM_STRING,
    EVENT_TRIGGER_TAP_TEMPO
};

typedef struct
{
    uint8_t Event;
    char Text[MAX_TEXT_LENGTH];
    uint32_t Value;
    uint32_t Item;
} tControlMessage;

typedef struct __attribute__ ((packed)) 
{
    uint16_t SkinIndex;
    char PresetDescription[MAX_TEXT_LENGTH];
} tUserData;


typedef struct __attribute__ ((packed)) 
{
    // note here: Tonex big pedal support needed this to jump from 20 to 150, which
    // if allocated here, nukes the user's entire configuration.
    // Instead, this part is used for the first 20 (all of the One, and part of the big Tonex), and a new section later
    // is used for the other 130 
    tUserData UserData[MAX_PRESETS_TONEX_ONE];

    uint8_t BTMode;

    // bt client flags
    uint16_t BTClientMvaveChocolateEnable: 1;
    uint16_t BTClientXviveMD1Enable: 1;
    uint16_t BTClientCustomEnable: 1;
    uint16_t BTClientSpares: 13;

    // serial Midi flags
    uint8_t MidiSerialEnable: 1;
    uint8_t EnableBTmidiCC: 1;
    uint8_t MidiSpares: 6;

    uint8_t MidiChannel;

    // general flags
    uint16_t GeneralDoublePressToggleBypass: 1;
    uint16_t GeneralScreenRotation: 2;
    uint16_t GeneralLoopAround: 1;
    uint16_t GeneralSavePresetToSlot: 2;
    uint16_t GeneralSpare: 10;

    uint8_t FootswitchMode;
    char BTClientCustomName[MAX_BT_CUSTOM_NAME];

    // wifi
    uint8_t WiFiMode : 4;
    uint8_t WifiTxPower : 4;
    char WifiSSID[MAX_WIFI_SSID_PW];
    char WifiPassword[MAX_WIFI_SSID_PW];
    char MDNSName[MAX_MDNS_NAME];

    // external footswitches
    uint8_t ExternalFootswitchPresetLayout;
    tExternalFootswitchEffectConfig ExternalFootswitchEffectConfig[MAX_EXTERNAL_EFFECT_FOOTSWITCHES];

    // internal footswitches
    uint8_t InternalFootswitchPresetLayout;
    tExternalFootswitchEffectConfig InternalFootswitchEffectConfig[MAX_INTERNAL_EFFECT_FOOTSWITCHES];

    // preset order mapping
    uint8_t PresetOrder[MAX_SUPPORTED_PRESETS];

    // user data 
    tUserData UserDataExtended[MAX_SUPPORTED_PRESETS - MAX_PRESETS_TONEX_ONE];
} tConfigData;

typedef struct
{
    uint32_t LastTime;
    float BPM;
} tTapTempo;


typedef struct 
{
    uint32_t PresetIndex;                        // 0-based index
    char PresetNames[MAX_SUPPORTED_PRESETS][MAX_TEXT_LENGTH];
    uint32_t USBStatus;
    uint32_t BTStatus;
    uint32_t WiFiStatus;
    tConfigData ConfigData;
    tTapTempo TapTempo;
} tControlData;

static const char *TAG = "app_control";
static QueueHandle_t control_input_queue;
static tControlData ControlData;

#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
static uint8_t PresetIndexForOrderValue(uint8_t value);
#endif
static uint8_t SaveUserData(void);
static uint8_t LoadUserData(void);

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static uint8_t process_control_command(tControlMessage* message)
{
    ESP_LOGI(TAG, "Control command %d", message->Event);

    // check what we got
    switch (message->Event)
    {
        case EVENT_PRESET_DOWN:
        {
            if (ControlData.USBStatus != 0)
            {
                if (control_get_config_item_int(CONFIG_ITEM_LOOP_AROUND))
                {
                    uint8_t newIndex = (ControlData.PresetIndex > 0) ? (ControlData.PresetIndex - 1) : (usb_get_max_presets_for_connected_tonex() - 1);
                    uint8_t preset = ControlData.ConfigData.PresetOrder[newIndex];

                    // send message to USB
                    usb_set_preset(preset);
                }
                else if (ControlData.PresetIndex > 0)
                {
                    uint8_t preset = ControlData.ConfigData.PresetOrder[ControlData.PresetIndex - 1];
                    
                    // send message to USB
                    usb_set_preset(preset);
                }
            }
        } break;

        case EVENT_PRESET_UP:
        {
            if (ControlData.USBStatus != 0)
            {
                if (control_get_config_item_int(CONFIG_ITEM_LOOP_AROUND))
                {
                    uint8_t newIndex = (ControlData.PresetIndex < (usb_get_max_presets_for_connected_tonex() - 1)) ? (ControlData.PresetIndex + 1) : 0;
                    uint8_t preset = ControlData.ConfigData.PresetOrder[newIndex];
                    
                    // send message to USB
                    usb_set_preset(preset);
                }
                else if (ControlData.PresetIndex < (usb_get_max_presets_for_connected_tonex() - 1))
                {
                    uint8_t preset = ControlData.ConfigData.PresetOrder[ControlData.PresetIndex + 1];
                    
                    // send message to USB
                    usb_set_preset(preset);
                }
            }
        } break;

        case EVENT_PRESET_INDEX:
        {
            if (ControlData.USBStatus != 0)
            {
                uint8_t preset = ControlData.ConfigData.PresetOrder[message->Value];

                // send message to USB
                usb_set_preset(preset);
            }
        } break;

        case EVENT_BANK_INDEX:
        {
#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
            // update UI
            UI_SetBankIndex(message->Value);
#endif
        } break;

        case EVENT_SET_PRESET_NAME:
        {
            memcpy((void*)ControlData.PresetNames[message->Value], (void*)message->Text, MAX_TEXT_LENGTH);
            ControlData.PresetNames[message->Value][MAX_TEXT_LENGTH - 1] = 0;

            // update web UI
            wifi_request_sync(WIFI_SYNC_TYPE_PRESET_NAME, (void*)ControlData.PresetNames[message->Value], (void*)&message->Value);
        } break;

        case EVENT_SET_PRESET_DETAILS:
        {
            ControlData.PresetIndex = message->Value;

            memcpy((void*)ControlData.PresetNames[message->Value], (void*)message->Text, MAX_TEXT_LENGTH);
            ControlData.PresetNames[message->Value][MAX_TEXT_LENGTH - 1] = 0;

#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
            // update UI
            UI_SetPresetLabel(PresetIndexForOrderValue(message->Value), ControlData.PresetNames[message->Value]);

            if (ControlData.PresetIndex < MAX_PRESETS_TONEX_ONE)
            {
                // use first bank
                UI_SetAmpSkin(ControlData.ConfigData.UserData[ControlData.PresetIndex].SkinIndex);
                UI_SetPresetDescription(ControlData.ConfigData.UserData[ControlData.PresetIndex].PresetDescription);
            }
            else
            {
                // use second bank            
                UI_SetAmpSkin(ControlData.ConfigData.UserDataExtended[ControlData.PresetIndex - MAX_PRESETS_TONEX_ONE].SkinIndex);
                UI_SetPresetDescription(ControlData.ConfigData.UserDataExtended[ControlData.PresetIndex - MAX_PRESETS_TONEX_ONE].PresetDescription);
            }
#endif

            // update web UI
            wifi_request_sync(WIFI_SYNC_TYPE_PRESET, NULL, (void*)&ControlData.PresetIndex);
        } break;

        case EVENT_SET_USB_STATUS:
        {
            ControlData.USBStatus = message->Value;

#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
            // update UI
            UI_SetUSBStatus(ControlData.USBStatus);
#endif
        } break;

        case EVENT_SET_BT_STATUS:
        {
            ControlData.BTStatus = message->Value;

#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
            // update UI
            UI_SetBTStatus(ControlData.BTStatus);
#endif
        } break;

        case EVENT_SET_WIFI_STATUS:
        {
            ControlData.WiFiStatus = message->Value;

#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
            // update UI
            UI_SetWiFiStatus(ControlData.WiFiStatus);
#endif
        } break;

        case EVENT_SET_AMP_SKIN:
        {            
            if (ControlData.PresetIndex < MAX_PRESETS_TONEX_ONE)
            {
                ControlData.ConfigData.UserData[ControlData.PresetIndex].SkinIndex = message->Value;

#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
                // update UI
                UI_SetAmpSkin(ControlData.ConfigData.UserData[ControlData.PresetIndex].SkinIndex);
#endif                 
            }
            else
            {
                ControlData.ConfigData.UserDataExtended[ControlData.PresetIndex - MAX_PRESETS_TONEX_ONE].SkinIndex = message->Value;

#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
                // update UI
                UI_SetAmpSkin(ControlData.ConfigData.UserDataExtended[ControlData.PresetIndex - MAX_PRESETS_TONEX_ONE].SkinIndex);
#endif                 
            }

        } break;

        case EVENT_SAVE_USER_DATA:
        {
            // save it
            SaveUserData();

            if (message->Value != 0)
            {
                ESP_LOGI(TAG, "Config save rebooting");
                vTaskDelay(10);
                esp_restart();
            }
        } break;

        case EVENT_SET_USER_TEXT:
        {
            if (ControlData.PresetIndex < MAX_PRESETS_TONEX_ONE)
            {
                memcpy((void*)ControlData.ConfigData.UserData[ControlData.PresetIndex].PresetDescription, (void*)message->Text, MAX_TEXT_LENGTH);
                ControlData.ConfigData.UserData[ControlData.PresetIndex].PresetDescription[MAX_TEXT_LENGTH - 1] = 0;
            }
            else
            {
                memcpy((void*)ControlData.ConfigData.UserDataExtended[ControlData.PresetIndex - MAX_PRESETS_TONEX_ONE].PresetDescription, (void*)message->Text, MAX_TEXT_LENGTH);
                ControlData.ConfigData.UserDataExtended[ControlData.PresetIndex - MAX_PRESETS_TONEX_ONE].PresetDescription[MAX_TEXT_LENGTH - 1] = 0;
            }
        } break;

        case EVENT_SET_CONFIG_ITEM_INT:
        {
            switch (message->Item)
            {
                case CONFIG_ITEM_BT_MODE:
                {
                    ESP_LOGI(TAG, "Config set BT mode %d", (int)message->Value);
                    ControlData.ConfigData.BTMode = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_MV_CHOC_ENABLE:
                {
                    ESP_LOGI(TAG, "Config set MV Choc enable %d", (int)message->Value);
                    ControlData.ConfigData.BTClientMvaveChocolateEnable = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_XV_MD1_ENABLE:
                {
                    ESP_LOGI(TAG, "Config set XV MD1 enable %d", (int)message->Value);
                    ControlData.ConfigData.BTClientXviveMD1Enable = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_CUSTOM_BT_ENABLE:
                {
                    ESP_LOGI(TAG, "Config set custom BT enable %d", (int)message->Value);
                    ControlData.ConfigData.BTClientCustomEnable = (uint8_t)message->Value;
                } break;
                
                case CONFIG_ITEM_MIDI_ENABLE:
                {
                    ESP_LOGI(TAG, "Config set Midi enable %d", (int)message->Value);
                    ControlData.ConfigData.MidiSerialEnable = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_MIDI_CHANNEL:
                {
                    ESP_LOGI(TAG, "Config set Midi channel %d", (int)message->Value);
                    ControlData.ConfigData.MidiChannel = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_TOGGLE_BYPASS:
                {
                    ESP_LOGI(TAG, "Config set Toggle Bypass %d", (int)message->Value);
                    ControlData.ConfigData.GeneralDoublePressToggleBypass = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_LOOP_AROUND:
                {
                    ESP_LOGI(TAG, "Config set Loop Around %d", (int)message->Value);
                    ControlData.ConfigData.GeneralLoopAround = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_FOOTSWITCH_MODE:
                {
                    ESP_LOGI(TAG, "Config set Footswitch Mode %d", (int)message->Value);
                    ControlData.ConfigData.FootswitchMode = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_ENABLE_BT_MIDI_CC:
                {
                    ESP_LOGI(TAG, "Config set BT Midi CC enable %d", (int)message->Value);
                    ControlData.ConfigData.EnableBTmidiCC = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_WIFI_MODE:
                {
                    ESP_LOGI(TAG, "Config set WiFi modee %d", (int)message->Value);
                    ControlData.ConfigData.WiFiMode = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_SCREEN_ROTATION:
                {
                    ESP_LOGI(TAG, "Config set screen rotation %d", (int)message->Value);
                    ControlData.ConfigData.GeneralScreenRotation = (uint8_t)message->Value & 0x03;
                } break;

                case CONFIG_ITEM_SAVE_PRESET_TO_SLOT:
                {
                    ESP_LOGI(TAG, "Config set save preset to slot %d", (int)message->Value);
                    ControlData.ConfigData.GeneralSavePresetToSlot = (uint8_t)message->Value & 0x03;
                } break;

                case CONFIG_ITEM_WIFI_TX_POWER:
                {
                    ESP_LOGI(TAG, "Config set wifi tx power %d", (int)message->Value);
                    ControlData.ConfigData.WifiTxPower = (uint8_t)message->Value & 0x0F;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_PRESET_LAYOUT:
                {
                    ESP_LOGI(TAG, "Config set external footsw preset layout %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchPresetLayout = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT1_SW:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect1 sw %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[0].Switch = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT1_CC:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect1 CC %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[0].CC = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT1_VAL1:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect1 Value_1 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[0].Value_1 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT1_VAL2:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect1 Value_2 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[0].Value_2 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT2_SW:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect2 sw %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[1].Switch = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT2_CC:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect2 CC %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[1].CC = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT2_VAL1:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect2 Value_1 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[1].Value_1 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT2_VAL2:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect2 Value_2 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[1].Value_2 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT3_SW:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect3 sw %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[2].Switch = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT3_CC:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect3 CC %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[2].CC = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT3_VAL1:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect3 Value_1 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[2].Value_1 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT3_VAL2:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect3 Value_2 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[2].Value_2 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT4_SW:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect4 sw %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[3].Switch = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT4_CC:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect4 CC %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[3].CC = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT4_VAL1:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect4 Value_1 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[3].Value_1 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT4_VAL2:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect4 Value_2 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[3].Value_2 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT5_SW:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect5 sw %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[4].Switch = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT5_CC:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect5 CC %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[4].CC = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT5_VAL1:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect5 Value_1 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[4].Value_1 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT5_VAL2:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect5 Value_2 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[4].Value_2 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT6_SW:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect6 sw %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[5].Switch = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT6_CC:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect6 CC %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[5].CC = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT6_VAL1:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect6 Value_1 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[5].Value_1 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT6_VAL2:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect6 Value_2 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[5].Value_2 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT7_SW:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect7 sw %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[6].Switch = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT7_CC:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect7 CC %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[6].CC = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT7_VAL1:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect7 Value_1 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[6].Value_1 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT7_VAL2:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect7 Value_2 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[6].Value_2 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT8_SW:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect8 sw %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[7].Switch = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT8_CC:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect8 CC %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[7].CC = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT8_VAL1:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect8 Value_1 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[7].Value_1 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_EXT_FOOTSW_EFFECT8_VAL2:
                {
                    ESP_LOGI(TAG, "Config set external footsw effect8 Value_2 %d", (int)message->Value);
                    ControlData.ConfigData.ExternalFootswitchEffectConfig[7].Value_2 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT1_SW:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect1 sw %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[0].Switch = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT1_CC:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect1 CC %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[0].CC = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT1_VAL1:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect1 Value_1 %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[0].Value_1 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT1_VAL2:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect1 Value_2 %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[0].Value_2 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT2_SW:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect2 sw %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[1].Switch = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT2_CC:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect2 CC %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[1].CC = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT2_VAL1:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect2 Value_1 %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[1].Value_1 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT2_VAL2:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect2 Value_2 %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[1].Value_2 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT3_SW:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect3 sw %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[2].Switch = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT3_CC:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect3 CC %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[2].CC = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT3_VAL1:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect3 Value_1 %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[2].Value_1 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT3_VAL2:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect3 Value_2 %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[2].Value_2 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT4_SW:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect4 sw %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[3].Switch = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT4_CC:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect4 CC %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[3].CC = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT4_VAL1:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect4 Value_1 %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[3].Value_1 = (uint8_t)message->Value;
                } break;

                case CONFIG_ITEM_INT_FOOTSW_EFFECT4_VAL2:
                {
                    ESP_LOGI(TAG, "Config set internal footsw effect4 Value_2 %d", (int)message->Value);
                    ControlData.ConfigData.InternalFootswitchEffectConfig[3].Value_2 = (uint8_t)message->Value;
                } break;
            }
        } break;

        case EVENT_SET_CONFIG_ITEM_STRING:
        {
            switch (message->Item)
            {
                case CONFIG_ITEM_BT_CUSTOM_NAME:
                {
                    ESP_LOGI(TAG, "Config set custom BT name %s", message->Text);
                    strncpy(ControlData.ConfigData.BTClientCustomName, message->Text, MAX_BT_CUSTOM_NAME - 1);
                    ControlData.ConfigData.BTClientCustomName[MAX_BT_CUSTOM_NAME - 1] = 0;
                } break;

                case CONFIG_ITEM_WIFI_SSID:
                {
                    ESP_LOGI(TAG, "Config set WiFi SSID %s", message->Text);
                    strncpy(ControlData.ConfigData.WifiSSID, message->Text, MAX_WIFI_SSID_PW - 1);
                    ControlData.ConfigData.WifiSSID[MAX_WIFI_SSID_PW - 1] = 0;
                } break;

                case CONFIG_ITEM_WIFI_PASSWORD:
                {
                    ESP_LOGI(TAG, "Config set WiFi password <hidden>");
                    strncpy(ControlData.ConfigData.WifiPassword, message->Text, MAX_WIFI_SSID_PW - 1);
                    ControlData.ConfigData.WifiPassword[MAX_WIFI_SSID_PW - 1] = 0;
                } break;

                case CONFIG_ITEM_MDNS_NAME:
                {
                    ESP_LOGI(TAG, "Config set MDNS name %s", message->Text);
                    strncpy(ControlData.ConfigData.MDNSName, message->Text, MAX_MDNS_NAME - 1);
                    ControlData.ConfigData.MDNSName[MAX_MDNS_NAME - 1] = 0;
                } break;
            }
        } break;

        case EVENT_TRIGGER_TAP_TEMPO:
        {
            uint32_t current_time = xTaskGetTickCount(); 
            uint32_t delta = current_time - ControlData.TapTempo.LastTime;
            float bpm;

            // debug
            //ESP_LOGI(TAG, "Tap Tempo %d %d", (int)current_time, (int)delta);

            // BPM can range from 40 to 240 bpm: 1.5 second2 maximum to 250 msec minimum between beats
            
            // check time since last tap
            if (delta > 1500)
            {
                // less than 40 bpm, save time and wait for another trigger
                ControlData.TapTempo.LastTime = current_time;
            }
            else 
            {
                if (delta < 250)
                {
                    // clamp at maximum bpm
                    delta = 250;                       
                }

                // calculate bpm (60,000 is 60 seconds in msec)
                bpm = 60000.0f / (float)delta;

                ControlData.TapTempo.BPM = bpm;

                ESP_LOGI(TAG, "Tap Tempo BPM = %d", (int)bpm);

                // update pedal
                usb_modify_parameter(TONEX_GLOBAL_BPM, ControlData.TapTempo.BPM);

                // save time for next trigger
                ControlData.TapTempo.LastTime = current_time;
            }
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
void control_request_preset_down(void)
{
    tControlMessage message;

    ESP_LOGI(TAG, "control_request_preset_down");            

    message.Event = EVENT_PRESET_DOWN;

    // send to queue
    if (xQueueSend(control_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "control_request_preset_down queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_request_preset_up(void)
{
    tControlMessage message;

    ESP_LOGI(TAG, "control_request_preset_up");

    message.Event = EVENT_PRESET_UP;

    // send to queue
    if (xQueueSend(control_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "control_request_preset_up queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_request_preset_index(uint8_t index)
{
    tControlMessage message;

    ESP_LOGI(TAG, "control_request_preset_index %d", index);

    message.Event = EVENT_PRESET_INDEX;
    message.Value = index;

    // send to queue
    if (xQueueSend(control_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "control_request_preset_index queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_request_bank_index(uint8_t index)
{
    tControlMessage message;

    ESP_LOGI(TAG, "control_request_bank_index %d", index);

    message.Event = EVENT_BANK_INDEX;
    message.Value = index;

    // send to queue
    if (xQueueSend(control_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "control_request_bank_index queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_sync_preset_name(uint16_t index, char* name)
{
    tControlMessage message;

    ESP_LOGI(TAG, "control_sync_preset_name: index: %d, name: %s", (int)index, name);            

    message.Event = EVENT_SET_PRESET_NAME;
    message.Value = index;

    // ensure string is null terminated
    message.Text[0] = 0;
    strncpy(message.Text, name, MAX_TEXT_LENGTH - 1);

    // send to queue
    if (xQueueSend(control_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "control_sync_preset_name queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_sync_preset_details(uint16_t index, char* name)
{
    tControlMessage message;

    ESP_LOGI(TAG, "control_sync_preset_details index: %d, name: %s", (int)index, name);

    message.Event = EVENT_SET_PRESET_DETAILS;
    message.Value = index;

    // ensure string is null terminated
    message.Text[0] = 0;
    strncpy(message.Text, name, MAX_TEXT_LENGTH - 1);

    // send to queue
    if (xQueueSend(control_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "control_sync_preset_details queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_set_user_text(char* text)
{
    tControlMessage message;

    ESP_LOGI(TAG, "control_set_user_text");            

    message.Event = EVENT_SET_USER_TEXT;

    // ensure string is null terminated
    message.Text[0] = 0;
    strncat(message.Text, text, MAX_TEXT_LENGTH - 1);

    // send to queue
    if (xQueueSend(control_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "control_set_user_text queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_set_usb_status(uint32_t status)
{
    tControlMessage message;

    ESP_LOGI(TAG, "control_set_usb_status");

    message.Event = EVENT_SET_USB_STATUS;
    message.Value = status;

    // send to queue
    if (xQueueSend(control_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "control_set_usb_status queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_set_bt_status(uint32_t status)
{
    tControlMessage message;

    ESP_LOGI(TAG, "control_set_bt_status");

    message.Event = EVENT_SET_BT_STATUS;
    message.Value = status;

    // send to queue
    if (xQueueSend(control_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "control_set_usb_status queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_set_wifi_status(uint32_t status)
{
    tControlMessage message;

    ESP_LOGI(TAG, "control_set_wifi_status %d", (int)status);

    message.Event = EVENT_SET_WIFI_STATUS;
    message.Value = status;

    // send to queue
    if (xQueueSend(control_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "control_set_wifi_status queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_save_user_data(uint8_t reboot)
{
    tControlMessage message;

    ESP_LOGI(TAG, "control_save_user_data");

    message.Event = EVENT_SAVE_USER_DATA;
    message.Value = reboot;

    // send to queue
    if (xQueueSend(control_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "control_save_user_data queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_set_amp_skin_index(uint32_t status)
{
    tControlMessage message;

    ESP_LOGI(TAG, "control_set_amp_skin_index");

    message.Event = EVENT_SET_AMP_SKIN;
    message.Value = status;

    // send to queue
    if (xQueueSend(control_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "control_set_amp_skin_index queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
void control_trigger_tap_tempo(void)
{
    tControlMessage message;

    ESP_LOGI(TAG, "control_trigger_tap_tempo");

    message.Event = EVENT_TRIGGER_TAP_TEMPO;

    // send to queue
    if (xQueueSend(control_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "control_trigger_tap_tempo queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_set_config_item_int(uint32_t item, uint32_t status)
{
    tControlMessage message;

    ESP_LOGI(TAG, "control_set_config_item_int: %d %d", (int)item, (int)status);

    message.Event = EVENT_SET_CONFIG_ITEM_INT;
    message.Value = status;
    message.Item = item;

    // send to queue
    if (xQueueSend(control_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "control_set_config_item_int queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_set_config_item_string(uint32_t item, char* name)
{
    tControlMessage message;

    ESP_LOGI(TAG, "control_set_config_item_string: %d", (int)item);

    message.Event = EVENT_SET_CONFIG_ITEM_STRING;

    // ensure string is null terminated
    message.Text[0] = 0;
    strncpy(message.Text, name, MAX_TEXT_LENGTH - 1);
    message.Item = item;

    // send to queue
    if (xQueueSend(control_input_queue, (void*)&message, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "control_set_config_item_string queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint32_t control_get_config_item_int(uint32_t item)
{
    uint32_t value = 0;

    switch (item)
    {
        case CONFIG_ITEM_BT_MODE:
        {
            value = ControlData.ConfigData.BTMode;
        } break;

        case CONFIG_ITEM_MV_CHOC_ENABLE:
        {
            value = ControlData.ConfigData.BTClientMvaveChocolateEnable;
        } break;

        case CONFIG_ITEM_XV_MD1_ENABLE:
        {
            value = ControlData.ConfigData.BTClientXviveMD1Enable;
        } break;

        case CONFIG_ITEM_CUSTOM_BT_ENABLE:
        {
            value = ControlData.ConfigData.BTClientCustomEnable;
        } break;
        
        case CONFIG_ITEM_MIDI_ENABLE:
        {
            value = ControlData.ConfigData.MidiSerialEnable;
        } break;

        case CONFIG_ITEM_MIDI_CHANNEL:
        {
            value = ControlData.ConfigData.MidiChannel;
        } break;

        case CONFIG_ITEM_TOGGLE_BYPASS:
        {
            value = ControlData.ConfigData.GeneralDoublePressToggleBypass;
        } break;

        case CONFIG_ITEM_LOOP_AROUND:
        {
            value = ControlData.ConfigData.GeneralLoopAround;
        } break;

        case CONFIG_ITEM_FOOTSWITCH_MODE:
        {
            value = ControlData.ConfigData.FootswitchMode;
        } break;

        case CONFIG_ITEM_ENABLE_BT_MIDI_CC:
        {
            value = ControlData.ConfigData.EnableBTmidiCC;
        } break;

        case CONFIG_ITEM_WIFI_MODE:
        {
            value = ControlData.ConfigData.WiFiMode;
        } break;

        case CONFIG_ITEM_SCREEN_ROTATION:
        {
            value = ControlData.ConfigData.GeneralScreenRotation;
        } break;

        case CONFIG_ITEM_SAVE_PRESET_TO_SLOT:
        {
            value = ControlData.ConfigData.GeneralSavePresetToSlot;
        } break;

        case CONFIG_ITEM_WIFI_TX_POWER:
        {
            value = ControlData.ConfigData.WifiTxPower;
        } break;
        
        case CONFIG_ITEM_EXT_FOOTSW_PRESET_LAYOUT:
        {
            value = ControlData.ConfigData.ExternalFootswitchPresetLayout;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT1_SW:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[0].Switch;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT1_CC:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[0].CC;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT1_VAL1:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[0].Value_1;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT1_VAL2:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[0].Value_2;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT2_SW:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[1].Switch;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT2_CC:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[1].CC;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT2_VAL1:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[1].Value_1;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT2_VAL2:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[1].Value_2;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT3_SW:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[2].Switch;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT3_CC:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[2].CC;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT3_VAL1:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[2].Value_1;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT3_VAL2:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[2].Value_2;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT4_SW:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[3].Switch;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT4_CC:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[3].CC;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT4_VAL1:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[3].Value_1;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT4_VAL2:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[3].Value_2;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT5_SW:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[4].Switch;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT5_CC:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[4].CC;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT5_VAL1:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[4].Value_1;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT5_VAL2:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[4].Value_2;
        } break;


        //xxxx
        case CONFIG_ITEM_EXT_FOOTSW_EFFECT6_SW:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[5].Switch;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT6_CC:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[5].CC;
        } break;
        
        case CONFIG_ITEM_EXT_FOOTSW_EFFECT6_VAL1:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[5].Value_1;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT6_VAL2:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[5].Value_2;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT7_SW:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[6].Switch;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT7_CC:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[6].CC;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT7_VAL1:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[6].Value_1;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT7_VAL2:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[6].Value_2;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT8_SW:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[7].Switch;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT8_CC:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[7].CC;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT8_VAL1:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[7].Value_1;
        } break;

        case CONFIG_ITEM_EXT_FOOTSW_EFFECT8_VAL2:
        {
            value = ControlData.ConfigData.ExternalFootswitchEffectConfig[7].Value_2;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT1_SW:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[0].Switch;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT1_CC:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[0].CC;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT1_VAL1:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[0].Value_1;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT1_VAL2:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[0].Value_2;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT2_SW:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[1].Switch;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT2_CC:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[1].CC;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT2_VAL1:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[1].Value_1;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT2_VAL2:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[1].Value_2;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT3_SW:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[2].Switch;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT3_CC:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[2].CC;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT3_VAL1:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[2].Value_1;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT3_VAL2:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[2].Value_2;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT4_SW:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[3].Switch;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT4_CC:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[3].CC;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT4_VAL1:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[3].Value_1;
        } break;

        case CONFIG_ITEM_INT_FOOTSW_EFFECT4_VAL2:
        {
            value = ControlData.ConfigData.InternalFootswitchEffectConfig[3].Value_2;
        } break;

        default:
        {
            ESP_LOGE(TAG, "Unknown/Invalid int parameter item %d", (int)item);            
        } break;
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
void control_get_config_item_string(uint32_t item, char* name)
{
    switch (item)
    {
        case CONFIG_ITEM_BT_CUSTOM_NAME:
        {
            strncpy(name, ControlData.ConfigData.BTClientCustomName, MAX_BT_CUSTOM_NAME - 1);
            name[MAX_BT_CUSTOM_NAME - 1] = 0;
        } break;

        case CONFIG_ITEM_WIFI_SSID:
        {
            strncpy(name, ControlData.ConfigData.WifiSSID, MAX_WIFI_SSID_PW - 1);
            name[MAX_WIFI_SSID_PW - 1] = 0;
        } break;

        case CONFIG_ITEM_WIFI_PASSWORD:
        {
            strncpy(name, ControlData.ConfigData.WifiPassword, MAX_WIFI_SSID_PW - 1);
            name[MAX_WIFI_SSID_PW - 1] = 0;            
        } break;

        case CONFIG_ITEM_MDNS_NAME:
        {
            strncpy(name, ControlData.ConfigData.MDNSName, MAX_MDNS_NAME - 1);
            name[MAX_MDNS_NAME - 1] = 0;            
        } break;

        default:
        {
            ESP_LOGE(TAG, "Unknown/Invalid string parameter item %d", (int)item);            
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
void control_set_preset_order(uint8_t* order)
{
    for (uint8_t index = 0; index < usb_get_max_presets_for_connected_tonex(); index++)
    {
        ControlData.ConfigData.PresetOrder[index] = order[index];
    }

#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
    // update UI
    UI_SetPresetLabel(PresetIndexForOrderValue(ControlData.PresetIndex), ControlData.PresetNames[ControlData.PresetIndex]);
#endif
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint8_t* control_get_preset_order(void)
{
    return ControlData.ConfigData.PresetOrder;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_set_skin_next(void)
{
    if (ControlData.PresetIndex < MAX_PRESETS_TONEX_ONE)
    {
        if (ControlData.ConfigData.UserData[ControlData.PresetIndex].SkinIndex < (SKIN_MAX - 1))
        {
            ControlData.ConfigData.UserData[ControlData.PresetIndex].SkinIndex++;
            control_set_amp_skin_index(ControlData.ConfigData.UserData[ControlData.PresetIndex].SkinIndex);
        }
    }
    else
    {
        if (ControlData.ConfigData.UserDataExtended[ControlData.PresetIndex - MAX_PRESETS_TONEX_ONE].SkinIndex < (SKIN_MAX - 1))
        {
            ControlData.ConfigData.UserDataExtended[ControlData.PresetIndex - MAX_PRESETS_TONEX_ONE].SkinIndex++;
            control_set_amp_skin_index(ControlData.ConfigData.UserDataExtended[ControlData.PresetIndex - MAX_PRESETS_TONEX_ONE].SkinIndex);
        }
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_set_skin_previous(void)
{
    if (ControlData.PresetIndex < MAX_PRESETS_TONEX_ONE)
    {
        if (ControlData.ConfigData.UserData[ControlData.PresetIndex].SkinIndex > 0)
        {
            ControlData.ConfigData.UserData[ControlData.PresetIndex].SkinIndex--;
        
            control_set_amp_skin_index(ControlData.ConfigData.UserData[ControlData.PresetIndex].SkinIndex);
        }
    }
    else
    {
        if (ControlData.ConfigData.UserDataExtended[ControlData.PresetIndex - MAX_PRESETS_TONEX_ONE].SkinIndex > 0)
        {
            ControlData.ConfigData.UserDataExtended[ControlData.PresetIndex - MAX_PRESETS_TONEX_ONE].SkinIndex--;
        
            control_set_amp_skin_index(ControlData.ConfigData.UserDataExtended[ControlData.PresetIndex - MAX_PRESETS_TONEX_ONE].SkinIndex);
        }
    }
}

#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static uint8_t PresetIndexForOrderValue(uint8_t value)
{
    for (uint8_t i = 0; i < usb_get_max_presets_for_connected_tonex(); i++)
    {
        if (ControlData.ConfigData.PresetOrder[i] == value)
        {
            return i;
        }
    }
    return -1;
}
#endif

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static uint8_t SaveUserData(void)
{
    esp_err_t err;
    nvs_handle_t my_handle;
    uint8_t result = 0;

    ESP_LOGI(TAG, "Writing User Data");

    // open storage
    err = nvs_open("storage", NVS_READWRITE, &my_handle);

    if (err == ESP_OK) 
    {
        // write value
        size_t required_size = sizeof(ControlData.ConfigData);
        err = nvs_set_blob(my_handle, NVS_USERDATA_NAME, (void*)&ControlData.ConfigData, required_size);

        switch (err) 
        {
            case ESP_OK:
            {
                result = 1;

                ESP_LOGI(TAG, "Wrote User Data OK");
            } break;
            
            default:
            {
                ESP_LOGE(TAG, "Error (%s) writing User Data\n", esp_err_to_name(err));
            } break;
        }

        // commit value
        err = nvs_commit(my_handle);

        // close
        nvs_close(my_handle);
    }
    else
    {
        ESP_LOGE(TAG, "Write User Data failed to open");
    }

    return result;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static uint8_t LoadUserData(void)
{
    esp_err_t err;
    nvs_handle_t my_handle;
    uint8_t result = 0;
    uint8_t save_needed = 0;

    ESP_LOGI(TAG, "Load User Data");

    // open storage
    err = nvs_open("storage", NVS_READWRITE, &my_handle);

    if (err == ESP_OK) 
    {
        // read data
        size_t required_size = sizeof(ControlData.ConfigData);
        err = nvs_get_blob(my_handle, NVS_USERDATA_NAME, (void*)&ControlData.ConfigData, &required_size);

         switch (err) 
         {
            case ESP_OK:
            {
                // close
                nvs_close(my_handle);

                ESP_LOGI(TAG, "Load User Data OK");

                result = 1;
            } break;
            
            case ESP_ERR_NVS_NOT_FOUND:
            default:
            {
                ESP_LOGE(TAG, "Error (%s) reading User Data \n", esp_err_to_name(err));

                // close
                nvs_close(my_handle);

                // write default values
                SaveUserData();
            } break;
        }
    }
    else
    {
        ESP_LOGE(TAG, "Read User Data failed to open");
    }

    // check values
    if (ControlData.ConfigData.BTMode > BT_MODE_PERIPHERAL)
    {
        ESP_LOGW(TAG, "Config BTMode invalid");
        ControlData.ConfigData.BTMode = BT_MODE_CENTRAL;
        save_needed = 1;
    }

    if (ControlData.ConfigData.MidiChannel > 16)
    {
        ESP_LOGW(TAG, "Config MidiChannel invalid");
        ControlData.ConfigData.MidiChannel = 1;
        save_needed = 1;
    }

    if (ControlData.ConfigData.FootswitchMode != FOOTSWITCH_LAYOUT_DISABLED)
    {
        if (ControlData.ConfigData.FootswitchMode >= FOOTSWITCH_LAYOUT_LAST)
        {
            ESP_LOGW(TAG, "Config Footswitch mode invalid");
            ControlData.ConfigData.FootswitchMode = FOOTSWITCH_LAYOUT_1X2;
            save_needed = 1;
        }
    }

    if (ControlData.ConfigData.ExternalFootswitchPresetLayout != FOOTSWITCH_LAYOUT_DISABLED)
    {
        if (ControlData.ConfigData.ExternalFootswitchPresetLayout >= FOOTSWITCH_LAYOUT_LAST) 
        {
            ESP_LOGW(TAG, "Config External Footswitch preset layout invalid");
            ControlData.ConfigData.ExternalFootswitchPresetLayout = FOOTSWITCH_LAYOUT_1X4;
            save_needed = 1;
        }
    }

    if (ControlData.ConfigData.InternalFootswitchPresetLayout != FOOTSWITCH_LAYOUT_DISABLED)
    {
        if (ControlData.ConfigData.InternalFootswitchPresetLayout >= FOOTSWITCH_LAYOUT_LAST) 
        {
            ESP_LOGW(TAG, "Config Internal Footswitch preset layout invalid");
            ControlData.ConfigData.InternalFootswitchPresetLayout = FOOTSWITCH_LAYOUT_1X4;
            save_needed = 1;
        }
    }

    if (save_needed)
    {
        SaveUserData();
    }

    ESP_LOGI(TAG, "Config BT Mode: %d", (int)ControlData.ConfigData.BTMode);
    ESP_LOGI(TAG, "Config BT Mvave Choc: %d", (int)ControlData.ConfigData.BTClientMvaveChocolateEnable);
    ESP_LOGI(TAG, "Config BT Xvive MD1: %d", (int)ControlData.ConfigData.BTClientMvaveChocolateEnable);
    ESP_LOGI(TAG, "Config BT Custom Client Enable: %d", (int)ControlData.ConfigData.BTClientCustomEnable);
    ESP_LOGI(TAG, "Config BT Custom Client Name: %s", ControlData.ConfigData.BTClientCustomName);
    ESP_LOGI(TAG, "Config Midi enable: %d", (int)ControlData.ConfigData.MidiSerialEnable);
    ESP_LOGI(TAG, "Config Midi channel: %d", (int)ControlData.ConfigData.MidiChannel);
    ESP_LOGI(TAG, "Config Toggle bypass: %d", (int)ControlData.ConfigData.GeneralDoublePressToggleBypass);
    ESP_LOGI(TAG, "Config Loop around: %d", (int)ControlData.ConfigData.GeneralLoopAround);
    ESP_LOGI(TAG, "Config Footswitch Mode: %d", (int)ControlData.ConfigData.FootswitchMode);
    ESP_LOGI(TAG, "Config EnableBTmidiCC Mode: %d", (int)ControlData.ConfigData.EnableBTmidiCC);
    ESP_LOGI(TAG, "Config WiFi Mode: %d", (int)ControlData.ConfigData.WiFiMode);
    ESP_LOGI(TAG, "Config WiFi SSID: %s", ControlData.ConfigData.WifiSSID);
    ESP_LOGI(TAG, "Config WiFi Password: <hidden>");
    ESP_LOGI(TAG, "Config MDNS name: %s", ControlData.ConfigData.MDNSName);
    ESP_LOGI(TAG, "Config WiFi TX Power: %d", ControlData.ConfigData.WifiTxPower);
    ESP_LOGI(TAG, "Config Screen Rotation: %d", (int)ControlData.ConfigData.GeneralScreenRotation);
    ESP_LOGI(TAG, "Config Save preset to slot: %d", (int)ControlData.ConfigData.GeneralSavePresetToSlot);
    ESP_LOGI(TAG, "Config Ext Footsw Prst Layout: %d", (int)ControlData.ConfigData.ExternalFootswitchPresetLayout);
    
    for (uint8_t loop = 0; loop < MAX_EXTERNAL_EFFECT_FOOTSWITCHES; loop++)
    {
        ESP_LOGI(TAG, "Config Ext Footsw Effect %d Switch: %d", (int)loop, (int)ControlData.ConfigData.ExternalFootswitchEffectConfig[loop].Switch);
        ESP_LOGI(TAG, "Config Ext Footsw Effect %d CC: %d", (int)loop, (int)ControlData.ConfigData.ExternalFootswitchEffectConfig[loop].CC);
        ESP_LOGI(TAG, "Config Ext Footsw Effect %d Val 1: %d", (int)loop, (int)ControlData.ConfigData.ExternalFootswitchEffectConfig[loop].Value_1);
        ESP_LOGI(TAG, "Config Ext Footsw Effect %d Val 2: %d", (int)loop, (int)ControlData.ConfigData.ExternalFootswitchEffectConfig[loop].Value_2);
    }
    
    for (uint8_t loop = 0; loop < MAX_INTERNAL_EFFECT_FOOTSWITCHES; loop++)
    {
        ESP_LOGI(TAG, "Config Int Footsw Effect %d Switch: %d", (int)loop, (int)ControlData.ConfigData.InternalFootswitchEffectConfig[loop].Switch);
        ESP_LOGI(TAG, "Config Int Footsw Effect %d CC: %d", (int)loop, (int)ControlData.ConfigData.InternalFootswitchEffectConfig[loop].CC);
        ESP_LOGI(TAG, "Config Int Footsw Effect %d Val 1: %d", (int)loop, (int)ControlData.ConfigData.InternalFootswitchEffectConfig[loop].Value_1);
        ESP_LOGI(TAG, "Config Int Footsw Effect %d Val 2: %d", (int)loop, (int)ControlData.ConfigData.InternalFootswitchEffectConfig[loop].Value_2);
    }

    // status    
    return result;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_set_default_config(void)
{
    ControlData.ConfigData.BTMode = BT_MODE_CENTRAL;
    ControlData.ConfigData.BTClientMvaveChocolateEnable = 1;
    ControlData.ConfigData.BTClientXviveMD1Enable = 1;
    ControlData.ConfigData.BTClientCustomEnable = 0;
    ControlData.ConfigData.GeneralDoublePressToggleBypass = 0;
    ControlData.ConfigData.GeneralLoopAround = 0;

#if CONFIG_TONEX_CONTROLLER_DEFAULT_MIDI_ENABLE
    ControlData.ConfigData.MidiSerialEnable = 1;
#else
    ControlData.ConfigData.MidiSerialEnable = 0;    
#endif    

    ControlData.ConfigData.MidiChannel = 1;
    ControlData.ConfigData.FootswitchMode = FOOTSWITCH_LAYOUT_1X2;
    ControlData.ConfigData.EnableBTmidiCC = 0;
    memset((void*)ControlData.ConfigData.BTClientCustomName, 0, sizeof(ControlData.ConfigData.BTClientCustomName));
    ControlData.ConfigData.WiFiMode = WIFI_MODE_ACCESS_POINT_TIMED;
    strcpy(ControlData.ConfigData.WifiSSID, "TonexConfig");
    strcpy(ControlData.ConfigData.WifiPassword, "12345678");   
    strcpy(ControlData.ConfigData.MDNSName, "tonex");   
    ControlData.ConfigData.WifiTxPower = WIFI_TX_POWER_25;

#if CONFIG_TONEX_CONTROLLER_SCREEN_ROTATION_DEFAULT_180    
    ControlData.ConfigData.GeneralScreenRotation = SCREEN_ROTATION_180;
#else
    ControlData.ConfigData.GeneralScreenRotation = SCREEN_ROTATION_0;
#endif    

    ControlData.ConfigData.GeneralSavePresetToSlot = SAVE_PRESET_SLOT_C;
    ControlData.ConfigData.ExternalFootswitchPresetLayout = FOOTSWITCH_LAYOUT_1X4;
    memset((void*)ControlData.ConfigData.ExternalFootswitchEffectConfig, 0, sizeof(ControlData.ConfigData.ExternalFootswitchEffectConfig));
    memset((void*)ControlData.ConfigData.InternalFootswitchEffectConfig, 0, sizeof(ControlData.ConfigData.InternalFootswitchEffectConfig));

    // default to no switches configured
    for (uint8_t loop = 0; loop < MAX_EXTERNAL_EFFECT_FOOTSWITCHES; loop++)
    {
        ControlData.ConfigData.ExternalFootswitchEffectConfig[loop].Switch = SWITCH_NOT_USED;
    }

    // default to no switches configured
    for (uint8_t loop = 0; loop < MAX_INTERNAL_EFFECT_FOOTSWITCHES; loop++)
    {
        ControlData.ConfigData.InternalFootswitchEffectConfig[loop].Switch = SWITCH_NOT_USED;
    }

    for (uint8_t loop = 0; loop < MAX_SUPPORTED_PRESETS; loop++)
    {
        ControlData.ConfigData.PresetOrder[loop] = loop;
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_task(void *arg)
{
    tControlMessage message;

    ESP_LOGI(TAG, "Control task start");

    while (1) 
    {
        // check for any input messages
        if (xQueueReceive(control_input_queue, (void*)&message, pdMS_TO_TICKS(20)) == pdPASS)
        {
            // process it
            process_control_command(&message);
        }

        // don't hog the CPU
        vTaskDelay(pdMS_TO_TICKS(3));
	}
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_load_config(void)
{
    esp_err_t ret;
    uint32_t loop;

    memset((void*)&ControlData, 0, sizeof(ControlData));

    // this will become init from Flash mem
    for (loop = 0; loop < MAX_PRESETS_TONEX_ONE; loop++)
    {
        sprintf(ControlData.ConfigData.UserData[loop].PresetDescription, "Description");
    }

    for (loop = 0; loop < (MAX_SUPPORTED_PRESETS - MAX_PRESETS_TONEX_ONE); loop++)
    {
        sprintf(ControlData.ConfigData.UserDataExtended[loop].PresetDescription, "Description");
    }

    // default config, will be overwritten or used as default
    control_set_default_config();
   
    // Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to init NVS");
    }

    // load the non-volatile user data
    LoadUserData();
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void control_init(void)
{
    // create queue for commands from other threads
    control_input_queue = xQueueCreate(20, sizeof(tControlMessage));
    if (control_input_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create control input queue!");
    }

    xTaskCreatePinnedToCore(control_task, "CTRL", CTRL_TASK_STACK_SIZE, NULL, CTRL_TASK_PRIORITY, NULL, 1);
}
