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

static const char *TAG = "app_midi_helper_tonex";

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static uint8_t midi_helper_tonex_boolean_midi_toggle(uint16_t param, uint8_t midi_value, float* value)
{
    tModellerParameter* param_ptr;

    if (midi_value == MIDI_BOOL_TOGGLE)
    {
        // take mutex
        if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
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
            tonex_params_release_locked_access();
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
esp_err_t midi_helper_tonex_adjust_param_via_midi(uint8_t change_num, uint8_t midi_value)
{
    uint16_t param;
    float value;
    tModellerParameter* param_ptr;

    // Midi mapping done to match the big Tonex pedal
    switch (change_num)
    {
        // 0: midi patch bank on big tonex

        case 1:
        {
            param = TONEX_PARAM_DELAY_POST;  
            
            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 2:
        {
            param = TONEX_PARAM_DELAY_ENABLE;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 3:
        {
            param = TONEX_PARAM_DELAY_MODEL;
            value = (float)midi_value;
            value = tonex_params_clamp_value(param, value);
        } break;        

        case 4:
        {
            param = TONEX_PARAM_DELAY_DIGITAL_SYNC;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 5:
        {
            // take mutex
            if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
            {		
                if (param_ptr[TONEX_PARAM_DELAY_DIGITAL_SYNC].Value == 0.00f)
                {
                    ESP_LOGW(TAG, "TONEX_PARAM_DELAY_DIGITAL_TIME");

                    param = TONEX_PARAM_DELAY_DIGITAL_TIME;

                    // release mutex as next functions need it
                    tonex_params_release_locked_access();

                    // scale the value to the time range
                    value = midi_helper_scale_midi_to_float(param, midi_value);
                    value = tonex_params_clamp_value(param, value);
                }
                else
                {
                    ESP_LOGW(TAG, "TONEX_PARAM_DELAY_DIGITAL_TS");

                    param = TONEX_PARAM_DELAY_DIGITAL_TS;

                    // release mutex as next functions need it
                    tonex_params_release_locked_access();

                    // set the time sign
                    value = (float)midi_value;
                    value = tonex_params_clamp_value(param, value);        
                }                                                   
            }
            else
            {
                // failed to get access to params
                ESP_LOGW(TAG, "TONEX_PARAM_DELAY_DIGITAL_TIME param access failed");
                return ESP_FAIL;
            }
        } break;

        case 6:
        {
            param = TONEX_PARAM_DELAY_DIGITAL_FEEDBACK;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 7:
        { 
            param = TONEX_PARAM_DELAY_DIGITAL_MODE;
            if (midi_value == 64)
            {
                value = 1.0f;
            }
            else
            {
                value = 0.0f;
            }
            value = tonex_params_clamp_value(param, value);
        } break;        
        
        case 8:
        {
            param = TONEX_PARAM_DELAY_DIGITAL_MIX;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        // 9 tuner
        
        case 10: 
        {
            //tap tempo
            control_trigger_tap_tempo();
            
            // no param change needed
            return ESP_OK;
        } break;

        // 11: expression pedal
        
        case 12: 
        {
            //preset on/off
            param = TONEX_GLOBAL_BYPASS;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;
        
        case 13:
        {
            param = TONEX_PARAM_NOISE_GATE_POST;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 14:
        {
            param = TONEX_PARAM_NOISE_GATE_ENABLE;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 15:
        {
            param = TONEX_PARAM_NOISE_GATE_THRESHOLD;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 16:
        {
            param = TONEX_PARAM_NOISE_GATE_RELEASE;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 17:
        {
            param = TONEX_PARAM_NOISE_GATE_DEPTH;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 18:
        {
            param = TONEX_PARAM_COMP_ENABLE;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 19:             
        { 
            param = TONEX_PARAM_COMP_THRESHOLD;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 20:
        {
            param = TONEX_PARAM_COMP_MAKE_UP;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 21:
        {
            param = TONEX_PARAM_COMP_ATTACK;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 22:
        {
            param = TONEX_PARAM_COMP_POST;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 23:
        {
            param = TONEX_PARAM_EQ_BASS;
            value = midi_helper_scale_midi_to_float(param, midi_value);
        } break;

        case 24:
        {
            param = TONEX_PARAM_EQ_BASS_FREQ;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 25:
        {
            param = TONEX_PARAM_EQ_MID;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 26:
        {
            param = TONEX_PARAM_EQ_MIDQ;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 27:
        {
            param = TONEX_PARAM_EQ_MID_FREQ;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 28:
        {
            param = TONEX_PARAM_EQ_TREBLE;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 29:
        {
            param = TONEX_PARAM_EQ_TREBLE_FREQ;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 30:
        {
            param = TONEX_PARAM_EQ_POST;
            value = midi_helper_boolean_midi_to_float(midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 31:
        {
            param = TONEX_PARAM_MODULATION_POST;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 32:
        {       
            param = TONEX_PARAM_MODULATION_ENABLE;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 33:
        {
            param = TONEX_PARAM_MODULATION_MODEL;
            value = (float)midi_value;
            value = tonex_params_clamp_value(param, value);
        } break;

        case 34:
        {
            param = TONEX_PARAM_MODULATION_CHORUS_SYNC;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 35:
        {
            // take mutex
            if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
            {		
                if (param_ptr[TONEX_PARAM_MODULATION_CHORUS_SYNC].Value == 0.00f)
                {
                    param = TONEX_PARAM_MODULATION_CHORUS_RATE;

                    // release mutex as next functions need it
                    tonex_params_release_locked_access();

                    // scale the value to the time range
                    value = midi_helper_scale_midi_to_float(param, midi_value);
                    value = tonex_params_clamp_value(param, value);
                }
                else
                {
                    param = TONEX_PARAM_MODULATION_CHORUS_TS;

                    // release mutex as next functions need it
                    tonex_params_release_locked_access();

                    // set the time sign
                    value = (float)midi_value;
                    value = tonex_params_clamp_value(param, value);        
                }                                                   
            }
            else
            {
                // failed to get access to params
                return ESP_FAIL;
            }            
        } break;

        case 36:
        {
            param = TONEX_PARAM_MODULATION_CHORUS_DEPTH;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 37:
        {
            param = TONEX_PARAM_MODULATION_CHORUS_LEVEL;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 38:
        {
            param = TONEX_PARAM_MODULATION_TREMOLO_SYNC;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 39:
        {
            // take mutex
            if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
            {		
                if (param_ptr[TONEX_PARAM_MODULATION_TREMOLO_SYNC].Value == 0.00f)
                {
                    param = TONEX_PARAM_MODULATION_TREMOLO_RATE;

                    // release mutex as next functions need it
                    tonex_params_release_locked_access();

                    // scale the value to the time range
                    value = midi_helper_scale_midi_to_float(param, midi_value);
                    value = tonex_params_clamp_value(param, value);
                }
                else
                {
                    param = TONEX_PARAM_MODULATION_TREMOLO_TS;

                    // release mutex as next functions need it
                    tonex_params_release_locked_access();

                    // set the time sign
                    value = (float)midi_value;
                    value = tonex_params_clamp_value(param, value);        
                }                                                   
            }
            else
            {
                // failed to get access to params
                return ESP_FAIL;
            }            
        } break;

        case 40:
        {
            param = TONEX_PARAM_MODULATION_TREMOLO_SHAPE;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 41:
        {
            param = TONEX_PARAM_MODULATION_TREMOLO_SPREAD;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 42:
        {
            param = TONEX_PARAM_MODULATION_TREMOLO_LEVEL;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 43:
        {
            param = TONEX_PARAM_MODULATION_PHASER_SYNC;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 44:
        {
            // take mutex
            if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
            {		
                if (param_ptr[TONEX_PARAM_MODULATION_PHASER_SYNC].Value == 0.00f)
                {
                    param = TONEX_PARAM_MODULATION_PHASER_RATE;

                    // release mutex as next functions need it
                    tonex_params_release_locked_access();

                    // scale the value to the time range
                    value = midi_helper_scale_midi_to_float(param, midi_value);
                    value = tonex_params_clamp_value(param, value);
                }
                else
                {
                    param = TONEX_PARAM_MODULATION_PHASER_TS;

                    // release mutex as next functions need it
                    tonex_params_release_locked_access();

                    // set the time sign
                    value = (float)midi_value;
                    value = tonex_params_clamp_value(param, value);        
                }                                                   
            }
            else
            {
                // failed to get access to params
                return ESP_FAIL;
            }            
        } break;

        case 45:
        {
            param = TONEX_PARAM_MODULATION_PHASER_DEPTH;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 46:
        {
            param = TONEX_PARAM_MODULATION_PHASER_LEVEL;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 47:
        {
            param = TONEX_PARAM_MODULATION_FLANGER_SYNC;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 48:
        {
            // take mutex
            if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
            {		
                if (param_ptr[TONEX_PARAM_MODULATION_FLANGER_SYNC].Value == 0.00f)
                {
                    param = TONEX_PARAM_MODULATION_FLANGER_RATE;

                    // release mutex as next functions need it
                    tonex_params_release_locked_access();

                    // scale the value to the time range
                    value = midi_helper_scale_midi_to_float(param, midi_value);
                    value = tonex_params_clamp_value(param, value);
                }
                else
                {
                    param = TONEX_PARAM_MODULATION_FLANGER_TS;

                    // release mutex as next functions need it
                    tonex_params_release_locked_access();

                    // set the time sign
                    value = (float)midi_value;
                    value = tonex_params_clamp_value(param, value);        
                }                                                   
            }
            else
            {
                // failed to get access to params
                return ESP_FAIL;
            }            
        } break;

        case 49:
        {
            param = TONEX_PARAM_MODULATION_FLANGER_DEPTH;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 50:
        {
            param = TONEX_PARAM_MODULATION_FLANGER_FEEDBACK;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 51:
        {
            param = TONEX_PARAM_MODULATION_FLANGER_LEVEL;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 52:
        {
            param = TONEX_PARAM_MODULATION_ROTARY_SYNC;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 53:
        {
            // take mutex
            if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
            {		
                if (param_ptr[TONEX_PARAM_MODULATION_ROTARY_SYNC].Value == 0.00f)
                {
                    param = TONEX_PARAM_MODULATION_ROTARY_SPEED;

                    // release mutex as next functions need it
                    tonex_params_release_locked_access();

                    // scale the value to the time range
                    value = midi_helper_scale_midi_to_float(param, midi_value);
                    value = tonex_params_clamp_value(param, value);
                }
                else
                {
                    param = TONEX_PARAM_MODULATION_ROTARY_TS;

                    // release mutex as next functions need it
                    tonex_params_release_locked_access();

                    // set the time sign
                    value = (float)midi_value;
                    value = tonex_params_clamp_value(param, value);        
                }                                                   
            }
            else
            {
                // failed to get access to params
                return ESP_FAIL;
            }
        } break;
        
        case 54:
        {
            param = TONEX_PARAM_MODULATION_ROTARY_RADIUS;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 55:
        {
            param = TONEX_PARAM_MODULATION_ROTARY_SPREAD;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 56:
        {
            param = TONEX_PARAM_MODULATION_ROTARY_LEVEL;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        // 57 - 58 not used

        case 59: 
        {
            param = TONEX_PARAM_REVERB_SPRING1_TIME;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 60:
        {
            param = TONEX_PARAM_REVERB_SPRING1_PREDELAY;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 61:
        {
            param = TONEX_PARAM_REVERB_SPRING1_COLOR;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 62:
        {
            param = TONEX_PARAM_REVERB_SPRING1_MIX;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 63:
        {
            param = TONEX_PARAM_REVERB_SPRING2_TIME;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 64:
        {
            param = TONEX_PARAM_REVERB_SPRING2_PREDELAY;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 65:
        {
            param = TONEX_PARAM_REVERB_SPRING2_COLOR;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 66:
        {
            param = TONEX_PARAM_REVERB_SPRING2_MIX;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 67:
        {
            param = TONEX_PARAM_REVERB_SPRING3_TIME;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 68:
        {
            param = TONEX_PARAM_REVERB_SPRING3_PREDELAY;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 69:
        {
            param = TONEX_PARAM_REVERB_SPRING3_COLOR;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 70:
        {
            param = TONEX_PARAM_REVERB_SPRING3_MIX;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 71:
        {
            param = TONEX_PARAM_REVERB_ROOM_TIME;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 72:
        {
            param = TONEX_PARAM_REVERB_ROOM_PREDELAY;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 73:
        {
            param = TONEX_PARAM_REVERB_ROOM_COLOR;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 74:
        {
            param = TONEX_PARAM_REVERB_ROOM_MIX;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 75:
        {
            param = TONEX_PARAM_REVERB_ENABLE;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 76:
        {
            param = TONEX_PARAM_REVERB_PLATE_TIME;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 77:
        {
            param = TONEX_PARAM_REVERB_PLATE_PREDELAY;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 78:
        {
            param = TONEX_PARAM_REVERB_PLATE_COLOR;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 79: 
        {
            param = TONEX_PARAM_REVERB_PLATE_MIX;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 80:
        {
            param = TONEX_PARAM_REVERB_SPRING4_TIME;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 81:
        {
            param = TONEX_PARAM_REVERB_SPRING4_PREDELAY;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 82:
        {
            param = TONEX_PARAM_REVERB_SPRING4_COLOR;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 83:
        {
            param = TONEX_PARAM_REVERB_SPRING4_MIX;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 84:
        {
            param = TONEX_PARAM_REVERB_POSITION;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 85:
        {
            param = TONEX_PARAM_REVERB_MODEL;
            value = (float)midi_value;
            value = tonex_params_clamp_value(param, value);
        } break;

        case 86: 
        {
            //preset down
            control_request_preset_down();

            // no param change needed
            return ESP_OK;
        } break;

        case 87:
        {
            //preset up
            control_request_preset_up();

            // no param change needed
            return ESP_OK;
        } break;

        case 88: 
        {
            // bpm
            param = TONEX_GLOBAL_BPM;            
            value = midi_helper_scale_midi_to_float(param, midi_value);
        } break;
        
        // 89: bank down
        // 90: bank up    

        case 91:
        {
            param = TONEX_PARAM_DELAY_TAPE_SYNC;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 92:
        {
            // take mutex
            if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
            {		
                if (param_ptr[TONEX_PARAM_DELAY_TAPE_SYNC].Value == 0.00f)
                {
                    param = TONEX_PARAM_DELAY_TAPE_TIME;

                    // release mutex as next functions need it
                    tonex_params_release_locked_access();

                    // scale the value to the time range
                    value = midi_helper_scale_midi_to_float(param, midi_value);
                    value = tonex_params_clamp_value(param, value);
                }
                else
                {
                    param = TONEX_PARAM_DELAY_TAPE_TS;

                    // release mutex as next functions need it
                    tonex_params_release_locked_access();

                    // set the time sign
                    value = (float)midi_value;
                    value = tonex_params_clamp_value(param, value);        
                }                                                   
            }
            else
            {
                // failed to get access to params
                return ESP_FAIL;
            }
        } break;

        case 93:
        {    
            param = TONEX_PARAM_DELAY_TAPE_FEEDBACK;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 94:
        {
            param = TONEX_PARAM_DELAY_TAPE_MODE;
            if (midi_value == 64)
            {
                value = 1.0f;
            }
            else
            {
                value = 0.0f;
            }
            value = tonex_params_clamp_value(param, value);
        } break;

        case 95:
        {
            param = TONEX_PARAM_DELAY_TAPE_MIX;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        // 96 to 101 not used       

        case 99: 
        {
            // bpm
            param = TONEX_GLOBAL_BPM; 
            value = (midi_value < 40) ? 40 : midi_value;
        } break;
        
        case 100: 
        {
            // bpm
            param = TONEX_GLOBAL_BPM;            
            value = midi_value+100;
        } break;

        case 102:
        {
            param = TONEX_PARAM_MODEL_GAIN;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 103:
        {
            param = TONEX_PARAM_MODEL_VOLUME;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;
        
        case 104:
        {
            param = TONEX_PARAM_MODEX_MIX;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        // 105 not used

        case 106:
        {
            param = TONEX_PARAM_MODEL_PRESENCE;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 107:
        { 
            param = TONEX_PARAM_MODEL_DEPTH;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 108:
        {
            param = TONEX_PARAM_VIR_RESO;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 109:
        {
            param = TONEX_PARAM_VIR_MIC_1;
            value = (float)midi_value;
            value = tonex_params_clamp_value(param, value);
        } break;

        case 110:
        {
            param = TONEX_PARAM_VIR_MIC_1_X;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 111:
        {
            param = TONEX_PARAM_VIR_MIC_1_Z;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 112:
        {
            param = TONEX_PARAM_VIR_MIC_2;
            value = (float)midi_value;
            value = tonex_params_clamp_value(param, value);
        } break;

        case 113:
        {
            param = TONEX_PARAM_VIR_MIC_2_X;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 114:
        {
            param = TONEX_PARAM_VIR_MIC_2_Z;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 115:
        {
            param = TONEX_PARAM_VIR_BLEND;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;
     
        // below items not supported on bigger Tonex pedal, custom for this controller
        case 116: 
        {
            // input trim
            param = TONEX_GLOBAL_INPUT_TRIM;            
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 117: 
        {
            param = TONEX_GLOBAL_CABSIM_BYPASS;

            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;
        
        case 118:
        {
            param = TONEX_GLOBAL_TEMPO_SOURCE;
            
            // check for toggling
            if (!midi_helper_tonex_boolean_midi_toggle(param, midi_value, &value))
            {
                value = midi_helper_boolean_midi_to_float(midi_value);
                value = tonex_params_clamp_value(param, value);
            }
        } break;

        case 119:
        {
            param = TONEX_GLOBAL_TUNING_REFERENCE;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 120:
        {
            // Load preset to Slot A without switching to it
            if (midi_value >= (usb_get_max_presets_for_connected_modeller()))
            {
                ESP_LOGW(TAG, "Unsupported Midi CC 120 value %d (max %d)", midi_value, usb_get_max_presets_for_connected_modeller() - 1);
            }
            else
            {
                usb_load_preset_to_slot_a(midi_value);
            }

            // no param change needed
            return ESP_OK;
        } break;

        case 121:
        {
            // Load preset to Slot B without switching to it
            if (midi_value >= (usb_get_max_presets_for_connected_modeller()))
            {
                ESP_LOGW(TAG, "Unsupported Midi CC 121 value %d (max %d)", midi_value, usb_get_max_presets_for_connected_modeller() - 1);
            }
            else
            {
                usb_load_preset_to_slot_b(midi_value);
            }

            // no param change needed
            return ESP_OK;
        } break;

        case 122:
        {            
            param = TONEX_GLOBAL_MASTER_VOLUME;
            value = midi_helper_scale_midi_to_float(param, midi_value);
            value = tonex_params_clamp_value(param, value);
        } break;

        case 127: 
        {
            // Custom case: use CC to change params.
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
uint16_t midi_helper_tonex_get_param_for_change_num(uint8_t change_num, uint8_t midi_value_1, uint8_t midi_value_2)
{
    uint16_t param = TONEX_UNKNOWN;
    tModellerParameter* param_ptr;

    // Midi mapping done to match the big Tonex pedal
    switch (change_num)
    {
        // 0: midi patch bank on big tonex

        case 1:
        {
            param = TONEX_PARAM_DELAY_POST;       
        } break;

        case 2:
        {
            param = TONEX_PARAM_DELAY_ENABLE;
        } break;

        case 3:
        {
            param = TONEX_PARAM_DELAY_MODEL;
        } break;        

        case 4:
        {
            param = TONEX_PARAM_DELAY_DIGITAL_SYNC;
        } break;

        case 5:
        {
            // take mutex
            if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
            {		
                if (param_ptr[TONEX_PARAM_DELAY_DIGITAL_SYNC].Value == 0.00f)
                {
                    param = TONEX_PARAM_DELAY_DIGITAL_TIME;
                }
                else
                {
                    param = TONEX_PARAM_DELAY_DIGITAL_TS;
                }  
                 
                // release mutex
                tonex_params_release_locked_access();
            }
        } break;

        case 6:
        {
            param = TONEX_PARAM_DELAY_DIGITAL_FEEDBACK;
        } break;

        case 7:
        { 
            param = TONEX_PARAM_DELAY_DIGITAL_MODE;
        } break;        
        
        case 8:
        {
            param = TONEX_PARAM_DELAY_DIGITAL_MIX;
        } break;

        // 9 tuner
        
        case 10: 
        {
            // BPM
            param = TONEX_GLOBAL_BPM;
        } break;

        // 11: expression pedal
        
        case 12: 
        {
            // preset on/off
            param = TONEX_GLOBAL_BYPASS;
        } break;

        case 13:
        {
            param = TONEX_PARAM_NOISE_GATE_POST;
        } break;

        case 14:
        {
            param = TONEX_PARAM_NOISE_GATE_ENABLE;
        } break;

        case 15:
        {
            param = TONEX_PARAM_NOISE_GATE_THRESHOLD;
        } break;

        case 16:
        {
            param = TONEX_PARAM_NOISE_GATE_RELEASE;
        } break;

        case 17:
        {
            param = TONEX_PARAM_NOISE_GATE_DEPTH;
        } break;

        case 18:
        {
            param = TONEX_PARAM_COMP_ENABLE;
        } break;

        case 19:             
        { 
            param = TONEX_PARAM_COMP_THRESHOLD;
        } break;

        case 20:
        {
            param = TONEX_PARAM_COMP_MAKE_UP;
        } break;

        case 21:
        {
            param = TONEX_PARAM_COMP_ATTACK;
        } break;

        case 22:
        {
            param = TONEX_PARAM_COMP_POST;
        } break;

        case 23:
        {
            param = TONEX_PARAM_EQ_BASS;
        } break;

        case 24:
        {
            param = TONEX_PARAM_EQ_BASS_FREQ;
        } break;

        case 25:
        {
            param = TONEX_PARAM_EQ_MID;
        } break;

        case 26:
        {
            param = TONEX_PARAM_EQ_MIDQ;
        } break;

        case 27:
        {
            param = TONEX_PARAM_EQ_MID_FREQ;
        } break;

        case 28:
        {
            param = TONEX_PARAM_EQ_TREBLE;
        } break;

        case 29:
        {
            param = TONEX_PARAM_EQ_TREBLE_FREQ;
        } break;

        case 30:
        {
            param = TONEX_PARAM_EQ_POST;
        } break;

        case 31:
        {
            param = TONEX_PARAM_MODULATION_POST;
        } break;

        case 32:
        {       
            param = TONEX_PARAM_MODULATION_ENABLE;
        } break;

        case 33:
        {
            param = TONEX_PARAM_MODULATION_MODEL;
        } break;

        case 34:
        {
            param = TONEX_PARAM_MODULATION_CHORUS_SYNC;
        } break;

        case 35:
        {
            // take mutex
            if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
            {		
                if (param_ptr[TONEX_PARAM_MODULATION_CHORUS_SYNC].Value == 0.00f)
                {
                    param = TONEX_PARAM_MODULATION_CHORUS_RATE;
                }
                else
                {
                    param = TONEX_PARAM_MODULATION_CHORUS_TS;
                }  
                    
                // release mutex
                tonex_params_release_locked_access();
            }            
        } break;

        case 36:
        {
            param = TONEX_PARAM_MODULATION_CHORUS_DEPTH;
        } break;

        case 37:
        {
            param = TONEX_PARAM_MODULATION_CHORUS_LEVEL;
        } break;

        case 38:
        {
            param = TONEX_PARAM_MODULATION_TREMOLO_SYNC;
        } break;

        case 39:
        {
            // take mutex
            if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
            {		
                if (param_ptr[TONEX_PARAM_MODULATION_TREMOLO_SYNC].Value == 0.00f)
                {
                    param = TONEX_PARAM_MODULATION_TREMOLO_RATE;
                }
                else
                {
                    param = TONEX_PARAM_MODULATION_TREMOLO_TS;
                }  
                    
                // release mutex
                tonex_params_release_locked_access();
            }            
        } break;

        case 40:
        {
            param = TONEX_PARAM_MODULATION_TREMOLO_SHAPE;
        } break;

        case 41:
        {
            param = TONEX_PARAM_MODULATION_TREMOLO_SPREAD;
        } break;

        case 42:
        {
            param = TONEX_PARAM_MODULATION_TREMOLO_LEVEL;
        } break;

        case 43:
        {
            param = TONEX_PARAM_MODULATION_PHASER_SYNC;
        } break;

        case 44:
        {
            // take mutex
            if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
            {		
                if (param_ptr[TONEX_PARAM_MODULATION_PHASER_SYNC].Value == 0.00f)
                {
                    param = TONEX_PARAM_MODULATION_PHASER_RATE;
                }
                else
                {
                    param = TONEX_PARAM_MODULATION_PHASER_TS;
                }  
                    
                // release mutex
                tonex_params_release_locked_access();
            }            
        } break;

        case 45:
        {
            param = TONEX_PARAM_MODULATION_PHASER_DEPTH;
        } break;

        case 46:
        {
            param = TONEX_PARAM_MODULATION_PHASER_LEVEL;
        } break;

        case 47:
        {
            param = TONEX_PARAM_MODULATION_FLANGER_SYNC;
        } break;

        case 48:
        {
            // take mutex
            if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
            {		
                if (param_ptr[TONEX_PARAM_MODULATION_FLANGER_SYNC].Value == 0.00f)
                {
                    param = TONEX_PARAM_MODULATION_FLANGER_RATE;
                }
                else
                {
                    param = TONEX_PARAM_MODULATION_FLANGER_TS;
                }  
                    
                // release mutex
                tonex_params_release_locked_access();
            }            
        } break;

        case 49:
        {
            param = TONEX_PARAM_MODULATION_FLANGER_DEPTH;
        } break;

        case 50:
        {
            param = TONEX_PARAM_MODULATION_FLANGER_FEEDBACK;
        } break;

        case 51:
        {
            param = TONEX_PARAM_MODULATION_FLANGER_LEVEL;
        } break;

        case 52:
        {
            param = TONEX_PARAM_MODULATION_ROTARY_SYNC;
        } break;

        case 53:
        {
            // take mutex
            if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
            {		
                if (param_ptr[TONEX_PARAM_MODULATION_ROTARY_SYNC].Value == 0.00f)
                {
                    param = TONEX_PARAM_MODULATION_ROTARY_SPEED;
                }
                else
                {
                    param = TONEX_PARAM_MODULATION_ROTARY_TS;
                }  
                    
                // release mutex
                tonex_params_release_locked_access();
            }            
        } break;
        
        case 54:
        {
            param = TONEX_PARAM_MODULATION_ROTARY_RADIUS;
        } break;

        case 55:
        {
            param = TONEX_PARAM_MODULATION_ROTARY_SPREAD;
        } break;

        case 56:
        {
            param = TONEX_PARAM_MODULATION_ROTARY_LEVEL;
        } break;

        // 57 - 58 not used

        case 59: 
        {
            param = TONEX_PARAM_REVERB_SPRING1_TIME;
        } break;

        case 60:
        {
            param = TONEX_PARAM_REVERB_SPRING1_PREDELAY;
        } break;

        case 61:
        {
            param = TONEX_PARAM_REVERB_SPRING1_COLOR;
        } break;

        case 62:
        {
            param = TONEX_PARAM_REVERB_SPRING1_MIX;
        } break;

        case 63:
        {
            param = TONEX_PARAM_REVERB_SPRING2_TIME;
        } break;

        case 64:
        {
            param = TONEX_PARAM_REVERB_SPRING2_PREDELAY;
        } break;

        case 65:
        {
            param = TONEX_PARAM_REVERB_SPRING2_COLOR;
        } break;

        case 66:
        {
            param = TONEX_PARAM_REVERB_SPRING2_MIX;
        } break;

        case 67:
        {
            param = TONEX_PARAM_REVERB_SPRING3_TIME;
        } break;

        case 68:
        {
            param = TONEX_PARAM_REVERB_SPRING3_PREDELAY;
        } break;

        case 69:
        {
            param = TONEX_PARAM_REVERB_SPRING3_COLOR;
        } break;

        case 70:
        {
            param = TONEX_PARAM_REVERB_SPRING3_MIX;
        } break;

        case 71:
        {
            param = TONEX_PARAM_REVERB_ROOM_TIME;
        } break;

        case 72:
        {
            param = TONEX_PARAM_REVERB_ROOM_PREDELAY;
        } break;

        case 73:
        {
            param = TONEX_PARAM_REVERB_ROOM_COLOR;
        } break;

        case 74:
        {
            param = TONEX_PARAM_REVERB_ROOM_MIX;
        } break;

        case 75:
        {
            param = TONEX_PARAM_REVERB_ENABLE;
        } break;

        case 76:
        {
            param = TONEX_PARAM_REVERB_PLATE_TIME;
        } break;

        case 77:
        {
            param = TONEX_PARAM_REVERB_PLATE_PREDELAY;
        } break;

        case 78:
        {
            param = TONEX_PARAM_REVERB_PLATE_COLOR;
        } break;

        case 79: 
        {
            param = TONEX_PARAM_REVERB_PLATE_MIX;
        } break;

        case 80:
        {
            param = TONEX_PARAM_REVERB_SPRING4_TIME;
        } break;

        case 81:
        {
            param = TONEX_PARAM_REVERB_SPRING4_PREDELAY;
        } break;

        case 82:
        {
            param = TONEX_PARAM_REVERB_SPRING4_COLOR;
        } break;

        case 83:
        {
            param = TONEX_PARAM_REVERB_SPRING4_MIX;
        } break;

        case 84:
        {
            param = TONEX_PARAM_REVERB_POSITION;
        } break;

        case 85:
        {
            param = TONEX_PARAM_REVERB_MODEL;
        } break;

        case 86: 
        {
            // preset down
            control_request_preset_down();

            // don't set any return param, as this one is already handled and its not a parameter
        } break;

        case 87:
        {
            //preset up
            control_request_preset_up();

            // don't set any return param, as this one is already handled and its not a parameter
        } break;

        case 88: 
        {
            //bpm
            param = TONEX_GLOBAL_BPM;
        } break;

        // 89: bank down
        // 90: bank up    

        case 91:
        {
            param = TONEX_PARAM_DELAY_TAPE_SYNC;
        } break;

        case 92:
        {
            // take mutex
            if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
            {		
                if (param_ptr[TONEX_PARAM_DELAY_TAPE_SYNC].Value == 0.00f)
                {
                    param = TONEX_PARAM_DELAY_TAPE_TIME;
                }
                else
                {
                    param = TONEX_PARAM_DELAY_TAPE_TS;
                }  
                    
                // release mutex
                tonex_params_release_locked_access();
            }            
        } break;

        case 93:
        {    
            param = TONEX_PARAM_DELAY_TAPE_FEEDBACK;
        } break;

        case 94:
        {
            param = TONEX_PARAM_DELAY_TAPE_MODE;
        } break;

        case 95:
        {
            param = TONEX_PARAM_DELAY_TAPE_MIX;
        } break;

        // 96 to 101 not used       

        case 102:
        {
            param = TONEX_PARAM_MODEL_GAIN;
        } break;

        case 103:
        {
            param = TONEX_PARAM_MODEL_VOLUME;
        } break;
        
        case 104:
        {
            param = TONEX_PARAM_MODEX_MIX;
        } break;

        // 105 not used

        case 106:
        {        
            param = TONEX_PARAM_MODEL_PRESENCE;
        } break;

        case 107:
        { 
            param = TONEX_PARAM_MODEL_DEPTH;
        } break;

        case 108:
        {
            param = TONEX_PARAM_VIR_RESO;
        } break;

        case 109:
        {
            param = TONEX_PARAM_VIR_MIC_1;
        } break;

        case 110:
        {
            param = TONEX_PARAM_VIR_MIC_1_X;
        } break;

        case 111:
        {
            param = TONEX_PARAM_VIR_MIC_1_Z;
        } break;

        case 112:
        {
            param = TONEX_PARAM_VIR_MIC_2;
        } break;

        case 113:
        {
            param = TONEX_PARAM_VIR_MIC_2_X;
        } break;

        case 114:
        {
            param = TONEX_PARAM_VIR_MIC_2_Z;
        } break;

        case 115:
        {
            param = TONEX_PARAM_VIR_BLEND;
        } break;

        // below items not supported on bigger Tonex pedal, custom for this controller
        case 116: 
        {
            param = TONEX_GLOBAL_INPUT_TRIM;            
        } break;

        case 117: 
        {
            param = TONEX_GLOBAL_CABSIM_BYPASS;
        } break;
         
        case 118:
        {
            param = TONEX_GLOBAL_TEMPO_SOURCE;
        } break;
 
        case 119:
        {
            param = TONEX_GLOBAL_TUNING_REFERENCE;
        } break;

        case 120:
        {
            // reserved for setting preset in slot A
        } break;

        case 121:
        {
            // reserved for setting preset in slot B
        } break;

        case 122:
        {            
            param = TONEX_GLOBAL_MASTER_VOLUME;
        } break;

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
