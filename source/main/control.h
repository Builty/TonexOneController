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

#pragma once

void control_init(void);
void control_load_config(void);

enum Skins
{
#if CONFIG_TONEX_CONTROLLER_SKINS_AMP    
    // Amps
    AMP_SKIN_JCM,
    AMP_SKIN_SILVERFACE,
    AMP_SKIN_TONEXAMPBLACK,
    AMP_SKIN_5150,
    AMP_SKIN_AMPEGCHROME,
    AMP_SKIN_FENDERTWEEDBIG,
    AMP_SKIN_FENDERHOTROD,
    AMP_SKIN_MESABOOGIEDUAL,
    AMP_SKIN_ELEGANTBLUE,
    AMP_SKIN_MODERNWHITEPLEXI,
    AMP_SKIN_ROLANDJAZZ,
    AMP_SKIN_ORANGEOR120,
    AMP_SKIN_MODERNBLACKPLEXI,
    AMP_SKIN_FENDERTWIN,
    AMP_SKIN_BA500,
    AMP_SKIN_MESAMARKWOOD,
    AMP_SKIN_MESAMARKV,
    AMP_SKIN_JTM,
    AMP_SKIN_JBDUMBLE1,
    AMP_SKIN_JETCITY,
    AMP_SKIN_AC30,
    AMP_SKIN_EVH,
    AMP_SKIN_TONEXAMPRED,
    AMP_SKIN_FRIEDMANN,
    AMP_SKIN_SUPRO,
    AMP_SKIN_DIEZEL,
    AMP_SKIN_WHITEMODERN,
    AMP_SKIN_WOODAMP,
#endif

#if CONFIG_TONEX_CONTROLLER_SKINS_PEDAL
    // Pedals
    PEDAL_SKIN_BIGMUFF,
    PEDAL_SKIN_BOSSBLACK,
    PEDAL_SKIN_BOSSSILVER,
    PEDAL_SKIN_BOSSYELLOW,
    PEDAL_SKIN_FUZZRED,
    PEDAL_SKIN_FUZZSILVER,
    PEDAL_SKIN_IBANEZBLUE,
    PEDAL_SKIN_IBANEZDARKBLUE,
    PEDAL_SKIN_IBANEZGREEN,
    PEDAL_SKIN_IBANEZRED,
    PEDAL_SKIN_KLONGOLD,
    PEDAL_SKIN_LIFEPEDAL,
    PEDAL_SKIN_MORNINGGLORY,
    PEDAL_SKIN_MXRDOUBLEBLACK,
    PEDAL_SKIN_MXRDOUBLERED,
    PEDAL_SKIN_MXRSINGLEBLACK,
    PEDAL_SKIN_MXRSINGLEGOLD,
    PEDAL_SKIN_MXRSINGLEGREEN,
    PEDAL_SKIN_MXRSINGLEORANGE,
    PEDAL_SKIN_MXRSINGLEWHITE,
    PEDAL_SKIN_MXRSINGLEYELLOW,
    PEDAL_SKIN_RATYELLOW,
#endif 

    SKIN_MAX        // must be last
};

enum ConfigItems
{
    CONFIG_ITEM_BT_MODE,
    CONFIG_ITEM_MV_CHOC_ENABLE,
    CONFIG_ITEM_XV_MD1_ENABLE,
    CONFIG_ITEM_CUSTOM_BT_ENABLE,
    CONFIG_ITEM_BT_CUSTOM_NAME,
    CONFIG_ITEM_MIDI_ENABLE,
    CONFIG_ITEM_MIDI_CHANNEL,
    CONFIG_ITEM_TOGGLE_BYPASS,
    CONFIG_ITEM_LOOP_AROUND,
    CONFIG_ITEM_FOOTSWITCH_MODE,
    CONFIG_ITEM_ENABLE_BT_MIDI_CC,
    CONFIG_ITEM_WIFI_MODE,
    CONFIG_ITEM_WIFI_SSID,
    CONFIG_ITEM_WIFI_PASSWORD,
    CONFIG_ITEM_SCREEN_ROTATION,
    CONFIG_ITEM_SAVE_PRESET_TO_SLOT,
    CONFIG_ITEM_WIFI_TX_POWER,
    CONFIG_ITEM_MDNS_NAME,
    CONFIG_ITEM_EXT_FOOTSW_PRESET_LAYOUT,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT1_SW,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT1_CC,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT1_VAL1,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT1_VAL2,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT2_SW,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT2_CC,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT2_VAL1,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT2_VAL2,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT3_SW,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT3_CC,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT3_VAL1,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT3_VAL2,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT4_SW,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT4_CC,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT4_VAL1,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT4_VAL2,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT5_SW,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT5_CC,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT5_VAL1,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT5_VAL2,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT6_SW,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT6_CC,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT6_VAL1,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT6_VAL2,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT7_SW,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT7_CC,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT7_VAL1,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT7_VAL2,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT8_SW,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT8_CC,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT8_VAL1,
    CONFIG_ITEM_EXT_FOOTSW_EFFECT8_VAL2,
    CONFIG_ITEM_INT_FOOTSW_EFFECT1_SW,
    CONFIG_ITEM_INT_FOOTSW_EFFECT1_CC,
    CONFIG_ITEM_INT_FOOTSW_EFFECT1_VAL1,
    CONFIG_ITEM_INT_FOOTSW_EFFECT1_VAL2,
    CONFIG_ITEM_INT_FOOTSW_EFFECT2_SW,
    CONFIG_ITEM_INT_FOOTSW_EFFECT2_CC,
    CONFIG_ITEM_INT_FOOTSW_EFFECT2_VAL1,
    CONFIG_ITEM_INT_FOOTSW_EFFECT2_VAL2,
    CONFIG_ITEM_INT_FOOTSW_EFFECT3_SW,
    CONFIG_ITEM_INT_FOOTSW_EFFECT3_CC,
    CONFIG_ITEM_INT_FOOTSW_EFFECT3_VAL1,
    CONFIG_ITEM_INT_FOOTSW_EFFECT3_VAL2,
    CONFIG_ITEM_INT_FOOTSW_EFFECT4_SW,
    CONFIG_ITEM_INT_FOOTSW_EFFECT4_CC,
    CONFIG_ITEM_INT_FOOTSW_EFFECT4_VAL1,
    CONFIG_ITEM_INT_FOOTSW_EFFECT4_VAL2,
};

enum BluetoothModes
{
    BT_MODE_DISABLED,
    BT_MODE_CENTRAL,
    BT_MODE_PERIPHERAL,
};

enum WiFiModes
{
    WIFI_MODE_ACCESS_POINT_TIMED,       // access point for 1 minute on boot
    WIFI_MODE_STATION,                  // station mode
    WIFI_MODE_ACCESS_POINT              // access point, no timeout
};

enum ScreenRotation
{
    SCREEN_ROTATION_0,
    SCREEN_ROTATION_180,
    // 90 and 270 one day maybe but needs big UI changes
    //SCREEN_ROTATION_90,
    //SCREEN_ROTATION_270,
    SCREEN_ROTATION_MAX,
};

enum SavePresetSlot
{
    SAVE_PRESET_SLOT_CURRENT,
    SAVE_PRESET_SLOT_A,
    SAVE_PRESET_SLOT_B,
    SAVE_PRESET_SLOT_C
};

enum WiFiTxPower
{
    WIFI_TX_POWER_25,
    WIFI_TX_POWER_50,
    WIFI_TX_POWER_75,    
    WIFI_TX_POWER_100
};

enum ConfigTabs43B
{
    CONFIG_TAB_GATE,
    CONFIG_TAB_COMPRESSOR,
    CONFIG_TAB_AMPLIFIER,
    CONFIG_TAB_EQ,
    CONFIG_TAB_MODULATION,
    CONFIG_TAB_DELAY,
    CONFIG_TAB_REVERB,
    CONFIG_TAB_GLOBAL,
};

enum FootswitchLayouts
{
    FOOTSWITCH_LAYOUT_1X2,                // next/previous
    FOOTSWITCH_LAYOUT_1X3,                // 1 row of 3 switches, bank via 1+2 and 2+3
    FOOTSWITCH_LAYOUT_1X4,                // 1 row of 4 switches, bank via 1+2 and 3+4
    FOOTSWITCH_LAYOUT_1X5A,               // 1 row of 5 switches, bank via 1+2 and 4+5
    FOOTSWITCH_LAYOUT_1X5B,               // 1 row of 5 switches, bank via last 2
    FOOTSWITCH_LAYOUT_1X6A,               // 1 row of 6 switches, bank via 1+2 and 5+6  
    FOOTSWITCH_LAYOUT_1X6B,               // 1 row of 6 switches, bank via last 2
    FOOTSWITCH_LAYOUT_1X7A,               // 1 row of 7 switches, bank via 1+2 and 6+7  
    FOOTSWITCH_LAYOUT_1X7B,               // 1 row of 7 switches, bank via last 2
    FOOTSWITCH_LAYOUT_2X3,                // 2 row2 of 3 switches, bank via 1+2 and 2+3
    FOOTSWITCH_LAYOUT_2X4,                // 2 rows of 4 switches, bank via 1+2 and 3+4
    FOOTSWITCH_LAYOUT_2X5A,               // 2 rows of 5 switches, bank via 1+2 and 4+5
    FOOTSWITCH_LAYOUT_2X5B,               // 2 rows of 5 switches, bank via last 2
    FOOTSWITCH_LAYOUT_2X6A,               // 2 rows of 6 switches, bank via 1+2 and 5+6
    FOOTSWITCH_LAYOUT_2X6B,               // 2 rows of 6 switches, bank via last 2
    FOOTSWITCH_LAYOUT_1X4_BINARY,         // 4 bit binary 
    FOOTSWITCH_LAYOUT_LAST,
    FOOTSWITCH_LAYOUT_DISABLED = 0xFF
};

enum IOExpanderPins
{
    IO_EXPANDER_PIN_1,
    IO_EXPANDER_PIN_2,
    IO_EXPANDER_PIN_3,
    IO_EXPANDER_PIN_4,
    IO_EXPANDER_PIN_5,
    IO_EXPANDER_PIN_6,
    IO_EXPANDER_PIN_7,
    IO_EXPANDER_PIN_8,
    IO_EXPANDER_PIN_9,
    IO_EXPANDER_PIN_10,
    IO_EXPANDER_PIN_11,
    IO_EXPANDER_PIN_12,
    IO_EXPANDER_PIN_13,
    IO_EXPANDER_PIN_14,
    IO_EXPANDER_PIN_15,
    IO_EXPANDER_PIN_16
};

typedef struct __attribute__ ((packed)) 
{
    uint8_t Switch;
    uint8_t CC;
    uint8_t Value_1;
    uint8_t Value_2;
} tExternalFootswitchEffectConfig;

#define MAX_WIFI_SSID_PW                        65   
#define MAX_MDNS_NAME                           32
#define MAX_EXTERNAL_EFFECT_FOOTSWITCHES        8
#define MAX_INTERNAL_EFFECT_FOOTSWITCHES        4
#define SWITCH_NOT_USED                         0xFF
#define MAX_SUPPORTED_PRESETS                   150
#define MAX_PRESET_NAME_LENGTH                  34

// thread safe public API
void control_request_preset_up(void);
void control_request_preset_down(void);
void control_request_preset_index(uint8_t index);
void control_request_bank_index(uint8_t index);
void control_set_usb_status(uint32_t status);
void control_set_bt_status(uint32_t status);
void control_set_wifi_status(uint32_t status);
void control_set_amp_skin_index(uint32_t status);
void control_set_skin_next(void);
void control_set_skin_previous(void);
void control_save_user_data(uint8_t reboot);
void control_sync_preset_name(uint16_t index, char* name);
void control_sync_preset_details(uint16_t index, char* name);
void control_set_user_text(char* text);
void control_trigger_tap_tempo(void);
void control_set_preset_order(uint8_t* order);
uint8_t* control_get_preset_order(void);

// config API
void control_set_default_config(void);
void control_set_config_item_int(uint32_t item, uint32_t status);
void control_set_config_item_string(uint32_t item, char* name);
void control_set_config_item_object(uint32_t item, void* object);

uint32_t control_get_config_item_int(uint32_t item);
void control_get_config_item_string(uint32_t item, char* name);
void control_get_config_item_object(uint32_t item, void* object);