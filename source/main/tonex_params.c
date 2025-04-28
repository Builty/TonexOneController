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
#include "esp_log.h"
#include "usb/usb_host.h"
#include "usb/cdc_acm_host.h"
#include "driver/i2c.h"
#include "usb_comms.h"
#include "usb_tonex_one.h"
#include "control.h"
#include "tonex_params.h"


#define PARAM_MUTEX_TIMEOUT         2000        // msec

static const char *TAG = "app_ToneParams";


/*
** Static vars
*/
static SemaphoreHandle_t ParamMutex;

// "value" below is just a default, is overridden by the preset on load
static tTonexParameter TonexParameters[TONEX_GLOBAL_LAST] = 
{
    //value, Min,    Max,  Name         Type
    {0,      0,      1,    "NG POST", TONEX_PARAM_TYPE_SWITCH},            // TONEX_PARAM_NOISE_GATE_POST   
    {1,      0,      1,    "NG POWER", TONEX_PARAM_TYPE_SWITCH},           // TONEX_PARAM_NOISE_GATE_ENABLE,
    {-64,    -100,   0,    "NG THRESH", TONEX_PARAM_TYPE_RANGE},          // TONEX_PARAM_NOISE_GATE_THRESHOLD,
    {20,     5,      500,  "NG REL", TONEX_PARAM_TYPE_RANGE},             // TONEX_PARAM_NOISE_GATE_RELEASE,
    {-60,    -100,   -20,  "NG DEPTH", TONEX_PARAM_TYPE_RANGE},           // TONEX_PARAM_NOISE_GATE_DEPTH,

    // Compressor
    {1,      0,      1,    "COMP POST",  TONEX_PARAM_TYPE_SWITCH},           // TONEX_PARAM_COMP_POST,             
    {0,      0,      1,    "COMP POWER", TONEX_PARAM_TYPE_SWITCH},          // TONEX_PARAM_COMP_ENABLE,
    {-14,    -40,    0,    "COMP THRESH", TONEX_PARAM_TYPE_RANGE},         // TONEX_PARAM_COMP_THRESHOLD,
    {-12,    -30,    10,   "COMP GAIN", TONEX_PARAM_TYPE_RANGE},           // TONEX_PARAM_COMP_MAKE_UP,
    {14,     1,      51,   "COMP ATTACK", TONEX_PARAM_TYPE_RANGE},          // TONEX_PARAM_COMP_ATTACK,

    // EQ    
    {0,      0,      1,    "EQ POST",   TONEX_PARAM_TYPE_SWITCH},             // TONEX_PARAM_EQ_POST,                // Pre/Post
    {5,      0,      10,   "EQ BASS", TONEX_PARAM_TYPE_RANGE},             // TONEX_PARAM_EQ_BASS,
    {300,    75,     600,  "EQ BFREQ", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_EQ_BASS_FREQ,
    {5,      0,      10,   "EQ MID", TONEX_PARAM_TYPE_RANGE},              // TONEX_PARAM_EQ_MID,
    {0.7,    0.2,    3.0,  "EQ MIDQ", TONEX_PARAM_TYPE_RANGE},             // TONEX_PARAM_EQ_MIDQ,
    {750,    150,    5000, "EQ MFREQ", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_EQ_MID_FREQ,
    {5,      0,      10,   "EQ TREBLE", TONEX_PARAM_TYPE_RANGE},           // TONEX_PARAM_EQ_TREBLE,
    {1900,   1000,   4000, "EQ TFREQ", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_EQ_TREBLE_FREQ,
    
    // Amplifier Model
    {1,      0,      1,    "MDL AMP", TONEX_PARAM_TYPE_SWITCH},            // TONEX_PARAM_MODEL_AMP_ENABLE,
    {0,      0,      1,    "MDL SW1", TONEX_PARAM_TYPE_SWITCH},            // TONEX_PARAM_MODEL_SW1,
    {5,      0,      10,   "MDL GAIN", TONEX_PARAM_TYPE_RANGE},           // TONEX_PARAM_MODEL_GAIN,
    {5,      0,      10,   "MDL VOL", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_MODEL_VOLUME,
    {100,    0,      100,  "MDL MIX", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_MODEX_MIX,
    {1,      0,      1,    "MDL CABU", TONEX_PARAM_TYPE_SWITCH},           // TONEX_PARAM_MODEL_CABINET_UNKNOWN
    {0,      0,      2,    "MDL CAB", TONEX_PARAM_TYPE_SELECT},            // TONEX_PARAM_CABINET_TYPE,      // 0 = TONEX_CABINET_TONE_MODEL, 1=TONEX_CABINET_VIR, 2=TONEX_CABINET_DISABLED

    // Virtual IR Cabinet
    {5,      0,      10,   "VIR_CMDL", TONEX_PARAM_TYPE_SELECT},           // TONEX_PARAM_VIR_CABINET_MODEL,
    {0,      0,      10,   "VIR_RESO", TONEX_PARAM_TYPE_RANGE},           // TONEX_PARAM_VIR_RESO,
    {0,      0,      2,    "VIR M1", TONEX_PARAM_TYPE_SELECT},             // TONEX_PARAM_VIR_MIC_1,
    {0,      0,      10,   "VIR M1X", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_VIR_MIC_1_X,
    {0,      0,      10,   "VIR M1Z", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_VIR_MIC_1_Z,
    {0,      0,      2,    "VIR M2", TONEX_PARAM_TYPE_SELECT},             // TONEX_PARAM_VIR_MIC_2
    {0,      0,      2,    "VIR M2X", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_VIR_MIC_2_X
    {0,      0,      10,   "VIR M2Z", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_VIR_MIC_2_Z
    {0,      -100,   100,  "VIR BLEND", TONEX_PARAM_TYPE_RANGE},          // TONEX_PARAM_VIR_BLEND

    // More amp params
    {5,      0,      10,   "MDL PRE", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_MODEL_PRESENCE
    {5,      0,      10,   "MDL DEP", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_MODEL_DEPTH
    
    // Reverb
    {0,      0,      1,    "RVB POS", TONEX_PARAM_TYPE_SWITCH},             // TONEX_PARAM_REVERB_POSITION,
    {1,      0,      1,    "RVB POWER", TONEX_PARAM_TYPE_SWITCH},           // TONEX_PARAM_REVERB_ENABLE,
    {0,      0,      5,    "RVB MODEL", TONEX_PARAM_TYPE_SELECT},           // TONEX_PARAM_REVERB_MODEL,
    {5,      0,      10,   "RVB S1 T", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING1_TIME,
    {0,      0,      500,  "RVB S1 P", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING1_PREDELAY,
    {0,      -10,    10,   "RVB S1 C", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING1_COLOR,
    {0,      0,      100,  "RVB S1 M", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING1_MIX,
    {5,      0,      10,   "RVB S2 T", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING2_TIME,
    {0,      0,      500,  "RVB S2 P", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING2_PREDELAY,
    {0,      -10,    10,   "RVB S2 C", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING2_COLOR,
    {0,      0,      100,  "RVB S2 M", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING2_MIX,
    {5,      0,      10,   "RVB S3 T", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING3_TIME,
    {0,      0,      500,  "RVB S3 P", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING3_PREDELAY,
    {0,      -10,    10,   "RVB S3 C", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING3_COLOR,
    {0,      0,      100,  "RVB S3 M", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING3_MIX,
    {5,      0,      10,   "RVB S4 T", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING4_TIME,
    {0,      0,      500,  "RVB S4 P", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING4_PREDELAY,
    {0,      -10,    10,   "RVB S4 C", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING4_COLOR,
    {0,      0,      100,  "RVB S4 M", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_SPRING4_MIX,
    {5,      0,      10,   "RVB RM T", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_ROOM_TIME,
    {0,      0,      500,  "RVB RM P", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_ROOM_PREDELAY,
    {0,      -10,    10,   "RVB RM C", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_ROOM_COLOR,
    {0,      0,      100,  "RVB RM M", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_ROOM_MIX,
    {5,      0,      10,   "RVB PL T", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_PLATE_TIME,
    {0,      0,      500,  "RVB PL P", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_PLATE_PREDELAY,
    {0,      -10,    10,   "RVB PL C", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_PLATE_COLOR,
    {0,      0,      100,  "RVB PL M", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_REVERB_PLATE_MIX,

    // Modulation
    {0,      0,      1,    "MOD POST", TONEX_PARAM_TYPE_SWITCH},            // TONEX_PARAM_MODULATION_POST,
    {0,      0,      1,    "MOD POWER", TONEX_PARAM_TYPE_SWITCH},            // TONEX_PARAM_MODULATION_ENABLE,
    {0,      0,      4,    "MOD MODEL", TONEX_PARAM_TYPE_SELECT},            // TONEX_PARAM_MODULATION_MODEL,
    {0,      0,      1,    "MOD CH S", TONEX_PARAM_TYPE_SWITCH},            // TONEX_PARAM_MODULATION_CHORUS_SYNC,
    {0,      0,      1,    "MOD CH T", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_MODULATION_CHORUS_TS,
    {0.5,    0.1,    10,   "MOD CH R", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_MODULATION_CHORUS_RATE,
    {0,      0,      100,  "MOD CH D", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_MODULATION_CHORUS_DEPTH,
    {0,      0,      10,   "MOD CH L", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_MODULATION_CHORUS_LEVEL,
    {0,      0,      1,    "MOD TR S", TONEX_PARAM_TYPE_SWITCH},            // TONEX_PARAM_MODULATION_TREMOLO_SYNC,
    {0,      0,      1,    "MOD TR T", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_MODULATION_TREMOLO_TS,
    {0.5,    0.1,    10,   "MOD TR R", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_MODULATION_TREMOLO_RATE,
    {0,      0,      10,   "MOD TR P", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_MODULATION_TREMOLO_SHAPE,
    {0,      0,      100,  "MOD TR D", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_MODULATION_TREMOLO_SPREAD,
    {0,      0,      10,   "MOD TR L", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_MODULATION_TREMOLO_LEVEL,
    {0,      0,      1,    "MOD PH S", TONEX_PARAM_TYPE_SWITCH},            // TONEX_PARAM_PHASER_SYNC,
    {0,      0,      1,    "MOD PH T", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_PHASER_TS,
    {0.5,    0.1,    10,   "MOD PH R", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_PHASER_RATE,
    {0,      0,      100,  "MOD PH D", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_PHASER_DEPTH,
    {0,      0,      10,   "MOD PH L", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_PHASER_LEVEL,
    {0,      0,      1,    "MOD FL S", TONEX_PARAM_TYPE_SWITCH},            // TONEX_PARAM_FLANGER_SYNC,
    {0,      0,      1,    "MOD FL T", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_FLANGER_TS,
    {0.5,    0.1,    10,   "MOD FL R", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_FLANGER_RATE,
    {0,      0,      100,  "MOD FL D", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_FLANGER_DEPTH,
    {0,      0,      100,  "MOD FL F", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_FLANGER_FEEDEBACK,
    {0,      0,      10,   "MOD FL L", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_FLANGER_LEVEL,
    {0,      0,      1,    "MOD RO S", TONEX_PARAM_TYPE_SWITCH},            // TONEX_PARAM_ROTARY_SYNC,
    {0,      0,      1,    "MOD RO T", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_ROTARY_TS,
    {0,      0,      400,  "MOD RO S", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_ROTARY_SPEED,
    {0,      0,      300,  "MOD RO R", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_ROTARY_RADIUS,
    {0,      0,      100,  "MOD RO D", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_ROTARY_SPREAD,
    {0,      0,      10,   "MOD RO L", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_ROTARY_LEVEL,
    
    // Delay
    {0,      0,      1,    "DLY POST", TONEX_PARAM_TYPE_SWITCH},            // TONEX_PARAM_DELAY_POST,    
    {0,      0,      1,    "DLY POWER", TONEX_PARAM_TYPE_SWITCH},           // TONEX_PARAM_DELAY_ENABLE,
    {0,      0,      1,    "DLY MODEL", TONEX_PARAM_TYPE_SELECT},           // TONEX_PARAM_DELAY_MODEL,
    {0,      0,      1,    "DLY DG S", TONEX_PARAM_TYPE_SWITCH},            // TONEX_PARAM_DELAY_DIGITAL_SYNC,
    {0,      0,      1,    "DLY DG T", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_DELAY_DIGITAL_TS,
    {0,      0,      1000, "DLY DT M", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_DELAY_DIGITAL_TIME,
    {0,      0,      100,  "DLY DT F", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_DELAY_DIGITAL_FEEDBACK,
    {0,      0,      1,    "DLY DT O", TONEX_PARAM_TYPE_SWITCH},            // TONEX_PARAM_DELAY_DIGITAL_MODE,
    {0,      0,      100,  "DLY DT X", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_DELAY_DIGITAL_MIX,
    {0,      0,      1,    "DLY TA S", TONEX_PARAM_TYPE_SWITCH},            // TONEX_PARAM_DELAY_TAPE_SYNC,
    {0,      0,      1,    "DLY TA T", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_DELAY_TAPE_TS,
    {0,      0,      1000, "DLY TA M", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_DELAY_TAPE_TIME,
    {0,      0,      100,  "DLY TA F", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_DELAY_TAPE_FEEDBACK,
    {0,      0,      1,    "DLY TA O", TONEX_PARAM_TYPE_SWITCH},            // TONEX_PARAM_DELAY_TAPE_MODE,
    {0,      0,      100,  "DLY TA X", TONEX_PARAM_TYPE_RANGE},            // TONEX_PARAM_DELAY_TAPE_MIX,   
    
    // dummy end of params marker
    {0,      0,      0,    "LAST", TONEX_PARAM_TYPE_RANGE},                // TONEX_PARAM_LAST,

    //************* Global params *****************
    {80,     40,     240,  "BPM", TONEX_PARAM_TYPE_RANGE},                 // TONEX_GLOBAL_BPM
    {0,      -15,    15,   "TRIM", TONEX_PARAM_TYPE_RANGE},                // TONEX_GLOBAL_INPUT_TRIM
    {0,      0,      1,    "CABSIM", TONEX_PARAM_TYPE_SWITCH},              // TONEX_GLOBAL_CABSIM_BYPASS
    {0,      0,      1,    "TEMPOS", TONEX_PARAM_TYPE_SWITCH},              // TONEX_GLOBAL_TEMPO_SOURCE
    {440,    415,    465,  "TUNEREF", TONEX_PARAM_TYPE_RANGE},              // TONEX_GLOBAL_TUNING_REFERENCE
};

static tTonexPresetColor TonexPresetColors[MAX_PRESETS] = {
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00}
};

typedef struct {
    uint32_t rawColor;
    uint32_t realColor;
} tTonexPresetColorMapping;

#define COLORS_COUNT 21

const tTonexPresetColorMapping TonexColorMap[COLORS_COUNT] = {
    {0xFF0000, 0xff0619}, // red
    {0xFF3F00, 0xe75116}, // orange
    {0x9FFF00, 0xffe12a}, // yellow
    {0x00FF00, 0x00f642}, // green
    {0x0FFF2F, 0x00fbcd}, // cyan
    {0x00FFFF, 0x009dfc}, // azure
    {0x0000FF, 0x0044fb}, // blue
    {0x2F00FF, 0x6c64fb}, // purple
    {0xFF00FF, 0x845083}, // magenta
    {0xBFBFBF, 0xff8bfc}, // pink
    {0x110000, 0x871218}, // dark red
    {0x111100, 0x7a3616}, // dark orange
    {0x112200, 0x85771c}, // dark yellow
    {0x001100, 0x05802d}, // dark green
    {0x002206, 0x00826e}, // dark cyan
    {0x001919, 0x005882}, // dark azure
    {0x000011, 0x002e82}, // dark blue
    {0x050011, 0x433e82}, // dark purple
    {0x0A000A, 0x851a6b}, // dark magenta
    {0x0B0B0B, 0x845083}, // dark pink
    {0x000000, 0x595959}, // grey
};

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t tonex_params_get_locked_access(tTonexParameter** param_ptr)
{
    // take mutex
    if (xSemaphoreTake(ParamMutex, pdMS_TO_TICKS(PARAM_MUTEX_TIMEOUT)) == pdTRUE)
    {		
        *param_ptr = TonexParameters;
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "tonex_params_get_locked_access Mutex timeout!");   
    }

    return ESP_FAIL;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t tonex_params_release_locked_access(void)
{
    // release mutex
    xSemaphoreGive(ParamMutex);

    return ESP_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t tonex_params_get_min_max(uint16_t param_index, float* min, float* max)
{
    if (param_index >= TONEX_GLOBAL_LAST)
    {
        // invalid
        return ESP_FAIL;
    }

    // take mutex
    if (xSemaphoreTake(ParamMutex, pdMS_TO_TICKS(PARAM_MUTEX_TIMEOUT)) == pdTRUE)
    {		
        *min = TonexParameters[param_index].Min;
        *max = TonexParameters[param_index].Max;

        // release mutex
        xSemaphoreGive(ParamMutex);

        return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "tonex_params_get_min_max Mutex timeout!");   
    }

    return ESP_FAIL;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
float tonex_params_clamp_value(uint16_t param_index, float value)
{
    if (param_index >= TONEX_GLOBAL_LAST)
    {
        // invalid
        return 0;
    }

    // take mutex
    if (xSemaphoreTake(ParamMutex, pdMS_TO_TICKS(PARAM_MUTEX_TIMEOUT)) == pdTRUE)
    {		
        if (value < TonexParameters[param_index].Min)
        {
            value = TonexParameters[param_index].Min;
        }
        else if (value > TonexParameters[param_index].Max)
        {
            value = TonexParameters[param_index].Max;
        }

        // release mutex
        xSemaphoreGive(ParamMutex);

        return value;
    }
    else
    {
        ESP_LOGE(TAG, "tonex_params_clamp_value Mutex timeout!");   
    }

    return 0;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t __attribute__((unused)) tonex_dump_parameters(void)
{
    // take mutex
    if (xSemaphoreTake(ParamMutex, pdMS_TO_TICKS(PARAM_MUTEX_TIMEOUT)) == pdTRUE)
    {	
        // dump all the param values and names
        for (uint32_t loop = 0; loop < TONEX_GLOBAL_LAST; loop++)
        {
            ESP_LOGI(TAG, "Param Dump: %s = %0.2f", TonexParameters[loop].Name, TonexParameters[loop].Value);
        }

        // release mutex
        xSemaphoreGive(ParamMutex);

         return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "tonex_dump_parameters Mutex timeout!");   
    }

    return ESP_FAIL;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t tonex_params_colors_get_locked_access(tTonexPresetColor** color_ptr)
{
    // take mutex
    if (xSemaphoreTake(ParamMutex, pdMS_TO_TICKS(PARAM_MUTEX_TIMEOUT)) == pdTRUE)
    {
        *color_ptr = TonexPresetColors;
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "tonex_params_colors_get_locked_access Mutex timeout!");
    }

    return ESP_FAIL;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t tonex_params_colors_get_color(uint16_t preset_index, uint32_t* preset_color)
{
    // take mutex
    if (xSemaphoreTake(ParamMutex, pdMS_TO_TICKS(PARAM_MUTEX_TIMEOUT)) == pdTRUE)
    {
        tTonexPresetColor color = TonexPresetColors[preset_index];
        *preset_color = (color.red << 16) | (color.green << 8) | color.blue;

        for (uint8_t index = 0; index < COLORS_COUNT; index++) {
            tTonexPresetColorMapping mapping = TonexColorMap[index];
            if (mapping.rawColor == *preset_color) {
                *preset_color = mapping.realColor;
                break;
            }
        }

        // release mutex
        xSemaphoreGive(ParamMutex);

        return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "tonex_params_colors_get_color Mutex timeout!");
    }

    return ESP_FAIL;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t tonex_params_init(void)
{
    // create mutex to protect interprocess issues with memory sharing
    ParamMutex = xSemaphoreCreateMutex();
    if (ParamMutex == NULL)
    {
        ESP_LOGE(TAG, "ParamMutex Mutex create failed!");
        return ESP_FAIL;
    }

    return ESP_OK;
}