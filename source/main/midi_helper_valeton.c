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
#include "valeton_params.h"
#include "midi_helper.h"
#include "midi_helper_valeton.h"

static const char *TAG = "app_midi_helper_valeton";

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static uint8_t midi_helper_valeton_boolean_midi_toggle(uint16_t param, uint8_t midi_value, float* value)
{
    tModellerParameter* param_ptr;

    if (midi_value == MIDI_BOOL_TOGGLE)
    {
        // take mutex
        if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
        {		
            // toggle current state
            if (param_ptr[param].Value == 0.0f)
            {
                *value = 1.0f;
            }
            else
            {
                *value = 0.0f;
            }  
                
            // release mutex
            valeton_params_release_locked_access();
        }  
        
        // toggling
        return 1;
    }

    // not toggling
    return 0;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t midi_helper_valeton_adjust_param_via_midi(uint8_t change_num, uint8_t midi_value)
{
    uint16_t param = 0;
    float value = 0.0f;

    switch (change_num)
    {
        case 0:
        {
            param = VALETON_PARAM_NR_ENABLE;  
            
            // check for toggling
            if (!midi_helper_valeton_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = valeton_params_clamp_value(param, value);
            }
        } break;

        case 1:
        {
            param = VALETON_PARAM_PRE_ENABLE;
     
            // check for toggling
            if (!midi_helper_valeton_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = valeton_params_clamp_value(param, value);
            }
        } break;

        case 2:
        {
            param = VALETON_PARAM_DIST_ENABLE;
     
            // check for toggling
            if (!midi_helper_valeton_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = valeton_params_clamp_value(param, value);
            }
        } break;

        case 3:
        {
            param = VALETON_PARAM_AMP_ENABLE;
     
            // check for toggling
            if (!midi_helper_valeton_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = valeton_params_clamp_value(param, value);
            }
        } break;
        
        case 4:
        {
            param = VALETON_PARAM_CAB_ENABLE;
     
            // check for toggling
            if (!midi_helper_valeton_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = valeton_params_clamp_value(param, value);
            }
        } break;
        
        case 5:
        {
            param = VALETON_PARAM_EQ_ENABLE;
     
            // check for toggling
            if (!midi_helper_valeton_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = valeton_params_clamp_value(param, value);
            }
        } break;

        case 6:
        {
            param = VALETON_PARAM_MOD_ENABLE;
     
            // check for toggling
            if (!midi_helper_valeton_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = valeton_params_clamp_value(param, value);
            }
        } break;
         
        case 7:
        {
            param = VALETON_PARAM_DLY_ENABLE;
     
            // check for toggling
            if (!midi_helper_valeton_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = valeton_params_clamp_value(param, value);
            }
        } break;

        case 8:
        {
            param = VALETON_PARAM_RVB_ENABLE;
     
            // check for toggling
            if (!midi_helper_valeton_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = valeton_params_clamp_value(param, value);
            }
        } break;
         
        case 9:
        {
            param = VALETON_PARAM_NS_ENABLE;
     
            // check for toggling
            if (!midi_helper_valeton_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = valeton_params_clamp_value(param, value);
            }
        } break;
    
        case 10:
        {
            param = VALETON_PARAM_NR_TYPE;
            value = (float)midi_value;
            value = valeton_params_clamp_value(param, value);
        } break;

        case 11:
        {
            param = VALETON_PARAM_PRE_TYPE;
            value = (float)midi_value;
            value = valeton_params_clamp_value(param, value);
        } break;

        case 12:
        {
            param = VALETON_PARAM_DIST_TYPE;
            value = (float)midi_value;
            value = valeton_params_clamp_value(param, value);
        } break;

        case 13:
        {
            param = VALETON_PARAM_AMP_TYPE;
            value = (float)midi_value;
            value = valeton_params_clamp_value(param, value);
        } break;

        case 14:
        {
            param = VALETON_PARAM_CAB_TYPE;
            value = (float)midi_value;
            value = valeton_params_clamp_value(param, value);
        } break;

        case 15:
        {
            param = VALETON_PARAM_EQ_TYPE;
            value = (float)midi_value;
            value = valeton_params_clamp_value(param, value);
        } break;

        case 16:
        {
            param = VALETON_PARAM_MOD_TYPE;
            value = (float)midi_value;
            value = valeton_params_clamp_value(param, value);
        } break;

        case 17:
        {
            param = VALETON_PARAM_DLY_TYPE;
            value = (float)midi_value;
            value = valeton_params_clamp_value(param, value);
        } break;

        case 18:
        {
            param = VALETON_PARAM_RVB_TYPE;
            value = (float)midi_value;
            value = valeton_params_clamp_value(param, value);
        } break;

        case 19:
        {
            param = VALETON_PARAM_NS_TYPE;
            value = (float)midi_value;
            value = valeton_params_clamp_value(param, value);
        } break;

        case 20:
        {
            param = VALETON_PARAM_PATCH_VOLUME;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 21:
        {
            param = VALETON_PARAM_NR_PARAM_0;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 23:
        {
            param = VALETON_PARAM_NR_PARAM_1;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 24:
        {
            param = VALETON_PARAM_NR_PARAM_2;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 25:
        {
            param = VALETON_PARAM_NR_PARAM_3;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 26:
        {
            param = VALETON_PARAM_NR_PARAM_4;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 27:
        {
            param = VALETON_PARAM_NR_PARAM_5;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 28:
        {
            param = VALETON_PARAM_NR_PARAM_6;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 29:
        {
            param = VALETON_PARAM_NR_PARAM_7;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 30:
        {
            param = VALETON_PARAM_PRE_PARAM_0;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 31:
        {
            param = VALETON_PARAM_PRE_PARAM_1;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 32:
        {
            param = VALETON_PARAM_PRE_PARAM_2;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 33:
        {
            param = VALETON_PARAM_PRE_PARAM_3;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 34:
        {
            param = VALETON_PARAM_PRE_PARAM_4;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 35:
        {
            param = VALETON_PARAM_PRE_PARAM_5;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 36:
        {
            param = VALETON_PARAM_PRE_PARAM_6;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 37:
        {
            param = VALETON_PARAM_PRE_PARAM_7;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 38:
        {
            param = VALETON_PARAM_DIST_PARAM_0;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;
    
        case 39:
        {
            param = VALETON_PARAM_DIST_PARAM_1;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 40:
        {
            param = VALETON_PARAM_DIST_PARAM_2;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 41:
        {
            param = VALETON_PARAM_DIST_PARAM_3;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 42:
        {
            param = VALETON_PARAM_DIST_PARAM_4;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 43:
        {
            param = VALETON_PARAM_DIST_PARAM_5;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 44:
        {
            param = VALETON_PARAM_DIST_PARAM_6;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 45:
        {
            param = VALETON_PARAM_DIST_PARAM_7;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 46:
        {
            param = VALETON_PARAM_AMP_PARAM_0;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 47:
        {
            param = VALETON_PARAM_AMP_PARAM_1;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 48:
        {
            param = VALETON_PARAM_AMP_PARAM_2;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 49:
        {
            param = VALETON_PARAM_AMP_PARAM_3;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 50:
        {
            param = VALETON_PARAM_AMP_PARAM_4;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 51:
        {
            param = VALETON_PARAM_AMP_PARAM_5;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 52:
        {
            param = VALETON_PARAM_AMP_PARAM_6;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 53:
        {
            param = VALETON_PARAM_AMP_PARAM_7;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 54:
        {
            param = VALETON_PARAM_CAB_PARAM_0;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 55:
        {
            param = VALETON_PARAM_CAB_PARAM_1;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 56:
        {
            param = VALETON_PARAM_CAB_PARAM_2;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 57:
        {
            param = VALETON_PARAM_CAB_PARAM_3;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 58:
        {
            param = VALETON_PARAM_CAB_PARAM_4;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 59:
        {
            param = VALETON_PARAM_CAB_PARAM_5;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 60:
        {
            param = VALETON_PARAM_CAB_PARAM_6;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 61:
        {
            param = VALETON_PARAM_CAB_PARAM_7;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 62:
        {
            param = VALETON_PARAM_EQ_PARAM_0;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 63:
        {
            param = VALETON_PARAM_EQ_PARAM_1;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 64:
        {
            param = VALETON_PARAM_EQ_PARAM_2;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 65:
        {
            param = VALETON_PARAM_EQ_PARAM_3;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 66:
        {
            param = VALETON_PARAM_EQ_PARAM_4;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 67:
        {
            param = VALETON_PARAM_EQ_PARAM_5;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 68:
        {
            param = VALETON_PARAM_EQ_PARAM_6;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 69:
        {
            param = VALETON_PARAM_EQ_PARAM_7;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 70:
        {
            param = VALETON_PARAM_MOD_PARAM_0;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 71:
        {
            param = VALETON_PARAM_MOD_PARAM_1;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 72:
        {
            param = VALETON_PARAM_MOD_PARAM_2;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 73:
        {
            param = VALETON_PARAM_MOD_PARAM_3;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 74:
        {
            param = VALETON_PARAM_MOD_PARAM_4;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 75:
        {
            param = VALETON_PARAM_MOD_PARAM_5;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 76:
        {
            param = VALETON_PARAM_MOD_PARAM_6;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 77:
        {
            param = VALETON_PARAM_MOD_PARAM_7;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 78:
        {
            param = VALETON_PARAM_DLY_PARAM_0;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;
   
        case 79:
        {
            param = VALETON_PARAM_DLY_PARAM_1;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 80:
        {
            param = VALETON_PARAM_DLY_PARAM_2;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 81:
        {
            param = VALETON_PARAM_DLY_PARAM_3;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 82:
        {
            param = VALETON_PARAM_DLY_PARAM_4;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 83:
        {
            param = VALETON_PARAM_DLY_PARAM_5;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 84:
        {
            param = VALETON_PARAM_DLY_PARAM_6;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 85:
        {
            param = VALETON_PARAM_DLY_PARAM_7;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 86:
        {
            param = VALETON_PARAM_RVB_PARAM_0;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;
   
        case 87:
        {
            param = VALETON_PARAM_RVB_PARAM_1;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 88:
        {
            param = VALETON_PARAM_RVB_PARAM_2;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 89:
        {
            param = VALETON_PARAM_RVB_PARAM_3;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 90:
        {
            param = VALETON_PARAM_RVB_PARAM_4;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 91:
        {
            param = VALETON_PARAM_RVB_PARAM_5;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 92:
        {
            param = VALETON_PARAM_RVB_PARAM_6;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 93:
        {
            param = VALETON_PARAM_RVB_PARAM_7;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 94:
        {
            param = VALETON_PARAM_NS_PARAM_0;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 95:
        {
            param = VALETON_PARAM_NS_PARAM_1;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 96:
        {
            param = VALETON_PARAM_NS_PARAM_2;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 97:
        {
            param = VALETON_PARAM_NS_PARAM_3;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 98:
        {
            param = VALETON_PARAM_NS_PARAM_4;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 99:
        {
            param = VALETON_PARAM_NS_PARAM_5;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 100:
        {
            param = VALETON_PARAM_NS_PARAM_6;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 101:
        {
            param = VALETON_PARAM_NS_PARAM_7;

            // scale the value to the range
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        // gap for expansion

        case 116: 
        {
            //preset down
            control_request_preset_down();

            // no param change needed
            return ESP_OK;
        } break;

        case 117:
        {
            //preset up
            control_request_preset_up();

            // no param change needed
            return ESP_OK;
        } break;

        case 118: 
        {
            // bpm
            param = VALETON_GLOBAL_BPM;            
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 119: 
        {
            //tap tempo
            control_trigger_tap_tempo();
            
            // no param change needed
            return ESP_OK;
        } break;
    
        case 120: 
        {
            // input trim
            param = VALETON_GLOBAL_INPUT_TRIM;            
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 121: 
        {
            param = VALETON_GLOBAL_CABSIM_BYPASS;

            // check for toggling
            if (!midi_helper_valeton_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = valeton_params_clamp_value(param, value);
            }
        } break;
        
        case 122:
        {            
            param = VALETON_GLOBAL_MASTER_VOLUME;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 123:
        {            
            param = VALETON_GLOBAL_RECORD_LEVEL;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 124:
        {            
            param = VALETON_GLOBAL_MONITOR_LEVEL;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        case 125:
        {            
            param = VALETON_GLOBAL_BT_LEVEL;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = valeton_params_clamp_value(param, value);
        } break;

        //126 

        case 127: 
        {
            // set preset
            if (midi_value >= (usb_get_max_presets_for_connected_modeller())) 
            {
                ESP_LOGW(TAG, "Unsupported Midi CC 127 value %d", midi_value);
            } 
            else 
            {
                control_request_preset_index(midi_value);
            }

            // no param change needed
            return ESP_OK;
        } break;

        default:
        {
            ESP_LOGW(TAG, "Unsupported Midi change number %d", change_num);
            return ESP_FAIL;
        } break;
    }

    // modify the parameter
    usb_modify_parameter(param, value);

    return ESP_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint16_t midi_helper_valeton_get_param_for_change_num(uint8_t change_num, uint8_t midi_value_1, uint8_t midi_value_2)
{
    uint16_t param = TONEX_UNKNOWN;

    switch (change_num)
    {
        case 0:
        {
            param = VALETON_PARAM_NR_ENABLE;  
        } break;

        case 1:
        {
            param = VALETON_PARAM_PRE_ENABLE;
        } break;

        case 2:
        {
            param = VALETON_PARAM_DIST_ENABLE;
        } break;

        case 3:
        {
            param = VALETON_PARAM_AMP_ENABLE;
        } break;
        
        case 4:
        {
            param = VALETON_PARAM_CAB_ENABLE;
        } break;
        
        case 5:
        {
            param = VALETON_PARAM_EQ_ENABLE;
        } break;

        case 6:
        {
            param = VALETON_PARAM_MOD_ENABLE;
        } break;
         
        case 7:
        {
            param = VALETON_PARAM_DLY_ENABLE;
        } break;

        case 8:
        {
            param = VALETON_PARAM_RVB_ENABLE;
        } break;
         
        case 9:
        {
            param = VALETON_PARAM_NS_ENABLE;
        } break;
    
        case 10:
        {
            param = VALETON_PARAM_NR_TYPE;
        } break;

        case 11:
        {
            param = VALETON_PARAM_PRE_TYPE;
        } break;

        case 12:
        {
            param = VALETON_PARAM_DIST_TYPE;
        } break;

        case 13:
        {
            param = VALETON_PARAM_AMP_TYPE;
        } break;

        case 14:
        {
            param = VALETON_PARAM_CAB_TYPE;
        } break;

        case 15:
        {
            param = VALETON_PARAM_EQ_TYPE;
        } break;

        case 16:
        {
            param = VALETON_PARAM_MOD_TYPE;
        } break;

        case 17:
        {
            param = VALETON_PARAM_DLY_TYPE;
        } break;

        case 18:
        {
            param = VALETON_PARAM_RVB_TYPE;
        } break;

        case 19:
        {
            param = VALETON_PARAM_NS_TYPE;
        } break;

        case 20:
        {
            param = VALETON_PARAM_PATCH_VOLUME;
        } break;

        case 21:
        {
            param = VALETON_PARAM_NR_PARAM_0;
        } break;

        case 23:
        {
            param = VALETON_PARAM_NR_PARAM_1;
        } break;

        case 24:
        {
            param = VALETON_PARAM_NR_PARAM_2;
        } break;

        case 25:
        {
            param = VALETON_PARAM_NR_PARAM_3;
        } break;

        case 26:
        {
            param = VALETON_PARAM_NR_PARAM_4;
        } break;

        case 27:
        {
            param = VALETON_PARAM_NR_PARAM_5;
        } break;

        case 28:
        {
            param = VALETON_PARAM_NR_PARAM_6;
        } break;

        case 29:
        {
            param = VALETON_PARAM_NR_PARAM_7;
        } break;

        case 30:
        {
            param = VALETON_PARAM_PRE_PARAM_0;
        } break;

        case 31:
        {
            param = VALETON_PARAM_PRE_PARAM_1;
        } break;

        case 32:
        {
            param = VALETON_PARAM_PRE_PARAM_2;
        } break;

        case 33:
        {
            param = VALETON_PARAM_PRE_PARAM_3;
        } break;

        case 34:
        {
            param = VALETON_PARAM_PRE_PARAM_4;
        } break;

        case 35:
        {
            param = VALETON_PARAM_PRE_PARAM_5;
        } break;

        case 36:
        {
            param = VALETON_PARAM_PRE_PARAM_6;
        } break;

        case 37:
        {
            param = VALETON_PARAM_PRE_PARAM_7;
        } break;

        case 38:
        {
            param = VALETON_PARAM_DIST_PARAM_0;
        } break;
    
        case 39:
        {
            param = VALETON_PARAM_DIST_PARAM_1;
        } break;

        case 40:
        {
            param = VALETON_PARAM_DIST_PARAM_2;
        } break;

        case 41:
        {
            param = VALETON_PARAM_DIST_PARAM_3;
        } break;

        case 42:
        {
            param = VALETON_PARAM_DIST_PARAM_4;
        } break;

        case 43:
        {
            param = VALETON_PARAM_DIST_PARAM_5;
        } break;

        case 44:
        {
            param = VALETON_PARAM_DIST_PARAM_6;
        } break;

        case 45:
        {
            param = VALETON_PARAM_DIST_PARAM_7;
        } break;

        case 46:
        {
            param = VALETON_PARAM_AMP_PARAM_0;
        } break;

        case 47:
        {
            param = VALETON_PARAM_AMP_PARAM_1;
        } break;

        case 48:
        {
            param = VALETON_PARAM_AMP_PARAM_2;
        } break;

        case 49:
        {
            param = VALETON_PARAM_AMP_PARAM_3;
        } break;

        case 50:
        {
            param = VALETON_PARAM_AMP_PARAM_4;
        } break;

        case 51:
        {
            param = VALETON_PARAM_AMP_PARAM_5;
        } break;

        case 52:
        {
            param = VALETON_PARAM_AMP_PARAM_6;
        } break;

        case 53:
        {
            param = VALETON_PARAM_AMP_PARAM_7;
        } break;

        case 54:
        {
            param = VALETON_PARAM_CAB_PARAM_0;
        } break;

        case 55:
        {
            param = VALETON_PARAM_CAB_PARAM_1;
        } break;

        case 56:
        {
            param = VALETON_PARAM_CAB_PARAM_2;
        } break;

        case 57:
        {
            param = VALETON_PARAM_CAB_PARAM_3;
        } break;

        case 58:
        {
            param = VALETON_PARAM_CAB_PARAM_4;
        } break;

        case 59:
        {
            param = VALETON_PARAM_CAB_PARAM_5;
        } break;

        case 60:
        {
            param = VALETON_PARAM_CAB_PARAM_6;
        } break;

        case 61:
        {
            param = VALETON_PARAM_CAB_PARAM_7;
        } break;

        case 62:
        {
            param = VALETON_PARAM_EQ_PARAM_0;
        } break;

        case 63:
        {
            param = VALETON_PARAM_EQ_PARAM_1;
        } break;

        case 64:
        {
            param = VALETON_PARAM_EQ_PARAM_2;
        } break;

        case 65:
        {
            param = VALETON_PARAM_EQ_PARAM_3;
        } break;

        case 66:
        {
            param = VALETON_PARAM_EQ_PARAM_4;
        } break;

        case 67:
        {
            param = VALETON_PARAM_EQ_PARAM_5;
        } break;

        case 68:
        {
            param = VALETON_PARAM_EQ_PARAM_6;
        } break;

        case 69:
        {
            param = VALETON_PARAM_EQ_PARAM_7;
        } break;

        case 70:
        {
            param = VALETON_PARAM_MOD_PARAM_0;
        } break;

        case 71:
        {
            param = VALETON_PARAM_MOD_PARAM_1;
        } break;

        case 72:
        {
            param = VALETON_PARAM_MOD_PARAM_2;
        } break;

        case 73:
        {
            param = VALETON_PARAM_MOD_PARAM_3;
        } break;

        case 74:
        {
            param = VALETON_PARAM_MOD_PARAM_4;
        } break;

        case 75:
        {
            param = VALETON_PARAM_MOD_PARAM_5;
        } break;

        case 76:
        {
            param = VALETON_PARAM_MOD_PARAM_6;
        } break;

        case 77:
        {
            param = VALETON_PARAM_MOD_PARAM_7;
        } break;

        case 78:
        {
            param = VALETON_PARAM_DLY_PARAM_0;
        } break;
   
        case 79:
        {
            param = VALETON_PARAM_DLY_PARAM_1;
        } break;

        case 80:
        {
            param = VALETON_PARAM_DLY_PARAM_2;
        } break;

        case 81:
        {
            param = VALETON_PARAM_DLY_PARAM_3;
        } break;

        case 82:
        {
            param = VALETON_PARAM_DLY_PARAM_4;
        } break;

        case 83:
        {
            param = VALETON_PARAM_DLY_PARAM_5;
        } break;

        case 84:
        {
            param = VALETON_PARAM_DLY_PARAM_6;
        } break;

        case 85:
        {
            param = VALETON_PARAM_DLY_PARAM_7;
        } break;

        case 86:
        {
            param = VALETON_PARAM_RVB_PARAM_0;
        } break;
   
        case 87:
        {
            param = VALETON_PARAM_RVB_PARAM_1;
        } break;

        case 88:
        {
            param = VALETON_PARAM_RVB_PARAM_2;
        } break;

        case 89:
        {
            param = VALETON_PARAM_RVB_PARAM_3;
        } break;

        case 90:
        {
            param = VALETON_PARAM_RVB_PARAM_4;
        } break;

        case 91:
        {
            param = VALETON_PARAM_RVB_PARAM_5;
        } break;

        case 92:
        {
            param = VALETON_PARAM_RVB_PARAM_6;
        } break;

        case 93:
        {
            param = VALETON_PARAM_RVB_PARAM_7;
        } break;

        case 94:
        {
            param = VALETON_PARAM_NS_PARAM_0;
        } break;

        case 95:
        {
            param = VALETON_PARAM_NS_PARAM_1;
        } break;

        case 96:
        {
            param = VALETON_PARAM_NS_PARAM_2;
        } break;

        case 97:
        {
            param = VALETON_PARAM_NS_PARAM_3;
        } break;

        case 98:
        {
            param = VALETON_PARAM_NS_PARAM_4;
        } break;

        case 99:
        {
            param = VALETON_PARAM_NS_PARAM_5;
        } break;

        case 100:
        {
            param = VALETON_PARAM_NS_PARAM_6;
        } break;

        case 101:
        {
            param = VALETON_PARAM_NS_PARAM_7;
        } break;

        // gap for expansion

        case 116: 
        {
            // preset down
            control_request_preset_down();

            // don't set any return param, as this one is already handled and its not a parameter
        } break;

        case 117:
        {
            // preset up
            control_request_preset_up();

            // don't set any return param, as this one is already handled and its not a parameter
        } break;

        case 118: 
        {
            // bpm
            param = VALETON_GLOBAL_BPM;            
        } break;

        case 119: 
        {
            // BPM
            param = VALETON_GLOBAL_BPM;
        } break;
    
        case 120: 
        {
            // input trim
            param = VALETON_GLOBAL_INPUT_TRIM;            
        } break;

        case 121: 
        {
            param = VALETON_GLOBAL_CABSIM_BYPASS;
        } break;
        
        case 122:
        {            
            param = VALETON_GLOBAL_MASTER_VOLUME;
        } break;

        case 123:
        {            
            param = VALETON_GLOBAL_RECORD_LEVEL;
        } break;

        case 124:
        {            
            param = VALETON_GLOBAL_MONITOR_LEVEL;
        } break;

        case 125:
        {            
            param = VALETON_GLOBAL_BT_LEVEL;
        } break;

        //126

        case 127:
        {
            uint32_t new_preset;
            uint32_t current_preset = control_get_current_preset_index();

            if (current_preset == midi_value_1)
            {
                // select midi value 2
                new_preset = midi_value_2;
            }
            else
            {
                // select midi value 1
                new_preset = midi_value_1;
            }

            if (new_preset >= (usb_get_max_presets_for_connected_modeller())) 
            {
                ESP_LOGW(TAG, "Unsupported Midi CC 127 value %d", new_preset);
            } 
            else 
            {
                control_request_preset_index(new_preset);
            }

            // don't set any return param, as this one is already handled and its not a parameter
        }
    }

    return param;
}
