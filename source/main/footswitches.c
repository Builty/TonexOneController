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

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sys/param.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "main.h"
#include "CH422G.h"
#include "control.h"
#include "task_priorities.h"
#include "usb/usb_host.h"
#include "usb_comms.h"
#include "usb_tonex_one.h"
#include "leds.h"
#include "driver/i2c.h"
#include "SX1509.h"
#include "midi_helper.h"
#include "tonex_params.h"

#define FOOTSWITCH_TASK_STACK_SIZE          (3 * 1024)
#define FOOTSWITCH_SAMPLE_COUNT             5       // 20 msec per sample
#define BUTTON_FACTORY_RESET_TIME           400    // * approx 20 msec

enum FootswitchStates
{
    FOOTSWITCH_IDLE,
    FOOTSWITCH_WAIT_RELEASE_1,
    FOOTSWITCH_WAIT_RELEASE_2
};

enum FootswitchHandlers
{
    FOOTSWITCH_HANDLER_ONBOARD_PRESETS,
    FOOTSWITCH_HANDLER_ONBOARD_EFFECTS,
    FOOTSWITCH_HANDLER_EXTERNAL_PRESETS,
    FOOTSWITCH_HANDLER_EXTERNAL_EFFECTS,
    FOOTSWITCH_HANDLER_MAX
};

static const char *TAG = "app_footswitches";

typedef struct
{
    uint8_t state;
    uint32_t sample_counter;
    uint16_t last_binary_val;
    uint16_t current_bank;
    uint16_t index_pending;    
    esp_err_t (*footswitch_single_reader)(uint8_t, uint8_t*);    
    esp_err_t (*footswitch_multiple_reader)(uint16_t*);    
} tFootswitchHandler;

typedef struct
{
    uint8_t toggle;
    tExternalFootswitchEffectConfig config;
} tFootswitchEffectHandler;

typedef struct
{
    tFootswitchHandler Handlers[FOOTSWITCH_HANDLER_MAX];
    uint8_t io_expander_ok;
    uint8_t onboard_switch_mode;   
    uint8_t external_switch_mode;
    tFootswitchEffectHandler ExternalFootswitchEffectHandler[MAX_EXTERNAL_EFFECT_FOOTSWITCHES];
    tFootswitchEffectHandler OnboardFootswitchEffectHandler[MAX_INTERNAL_EFFECT_FOOTSWITCHES];
} tFootswitchControl;

typedef struct
{
    uint8_t total_switches;
    uint8_t presets_per_bank;
    uint16_t bank_down_switch_mask;
    uint16_t bank_up_switch_mask;
} tFootswitchLayoutEntry;

static tFootswitchControl FootswitchControl;
static SemaphoreHandle_t I2CMutexHandle;
static i2c_port_t i2cnum;

static const __attribute__((unused)) tFootswitchLayoutEntry FootswitchLayouts[FOOTSWITCH_LAYOUT_LAST] = 
{
    //tot  ppb  bd mask   bu mask
    {2,    2,   0x0000,   0x0000},            // FOOTSWITCH_LAYOUT_1X2
    {3,    3,   0x0003,   0x0006},            // FOOTSWITCH_LAYOUT_1X3
    {4,    4,   0x0003,   0x000C},            // FOOTSWITCH_LAYOUT_1X4
    {5,    5,   0x0003,   0x0018},            // FOOTSWITCH_LAYOUT_1X5A
    {5,    3,   0x0008,   0x0010},            // FOOTSWITCH_LAYOUT_1X5B
    {6,    6,   0x0003,   0x0030},            // FOOTSWITCH_LAYOUT_1X6A
    {6,    4,   0x0010,   0x0020},            // FOOTSWITCH_LAYOUT_1X6B
    {7,    7,   0x0003,   0x0060},            // FOOTSWITCH_LAYOUT_1X7A
    {7,    5,   0x0020,   0x0040},            // FOOTSWITCH_LAYOUT_1X7B
    {6,    6,   0x0003,   0x0006},            // FOOTSWITCH_LAYOUT_2X3
    {8,    8,   0x0003,   0x000C},            // FOOTSWITCH_LAYOUT_2X4
    {10,   10,  0x0003,   0x0018},            // FOOTSWITCH_LAYOUT_2X5A
    {10,   8,   0x0100,   0x0200},            // FOOTSWITCH_LAYOUT_2X5B
    {12,   12,  0x0003,   0x0030},            // FOOTSWITCH_LAYOUT_2X6A
    {12,   10,  0x0400,   0x0800},            // FOOTSWITCH_LAYOUT_2X6B
    {4,    4,   0x0000,   0x0000},            // FOOTSWITCH_LAYOUT_1X4_BINARY
};

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t footswitch_read_single_onboard(uint8_t number, uint8_t* switch_state)
{
    esp_err_t result = ESP_FAIL;
    int8_t button_index;

    // map abstract switch index to physical IO
    switch (number)
    {
        case 0:
        default:
        {
            button_index = FOOTSWITCH_1;
        } break;

        case 1:
        {
            button_index = FOOTSWITCH_2;
        } break;

        case 2:
        {
            button_index = FOOTSWITCH_3;
        } break;

        case 3:
        {
            button_index = FOOTSWITCH_4;
        } break;
    }

    if (button_index == -1)
    {
        // not configured
        *switch_state = 0;
        return ESP_OK;
    }

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43B || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43DEVONLY
    // display board uses onboard I2C IO expander
    uint8_t value;

    if (CH422G_read_input((uint8_t)button_index, &value) == ESP_OK)
    {
        result = ESP_OK;
        *switch_state = (value == 0);
    }
#else
    // other boards can use direct IO pin
    *switch_state = (gpio_get_level((uint8_t)button_index) == 0);

    result = ESP_OK;
#endif

    return result;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t footswitch_read_multiple_onboard(uint16_t* switch_state)
{
    esp_err_t result = ESP_FAIL;
    *switch_state = 0;

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43B || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43DEVONLY
    // display board uses onboard I2C IO expander
    uint16_t values;

    if (CH422G_read_all_input(&values) == ESP_OK)
    {
        result = ESP_OK;
        *switch_state = values;
    }
#else
    // direct gpio
    if (FOOTSWITCH_1 != -1)
    {
        *switch_state |= (gpio_get_level(FOOTSWITCH_1) == 0);
    }

    if (FOOTSWITCH_2 != -1)
    {
        *switch_state |= ((gpio_get_level(FOOTSWITCH_2) == 0) << 1);
    }

    if (FOOTSWITCH_3 != -1)
    {
        *switch_state |= ((gpio_get_level(FOOTSWITCH_3) == 0) << 2);
    }

    if (FOOTSWITCH_4 != -1)
    {
        *switch_state |= ((gpio_get_level(FOOTSWITCH_4) == 0) << 3);
    }

    result = ESP_OK;
#endif

    return result;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t footswitch_read_single_offboard(uint8_t pin, uint8_t* switch_state)
{
    esp_err_t result = ESP_FAIL;
    uint8_t level;

    if (FootswitchControl.io_expander_ok)
    {       
        if (SX1509_digitalRead(pin, &level) == ESP_OK)
        {            
            // debug
            //ESP_LOGI(TAG, "Footswitch read %d", (int)level_mask);

            result = ESP_OK;
            *switch_state = (level == 0);
        }
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
static esp_err_t footswitch_read_multiple_offboard(uint16_t* switch_states)
{
    esp_err_t result = ESP_FAIL;

    if (FootswitchControl.io_expander_ok)
    {       
        if (SX1509_getPinValues(switch_states) == ESP_OK)
        {            
            // flip so 1 = switch pressed
            *switch_states = ~(*switch_states);

#if 0            
            // debug code to dump footswitch states to log
            char debug_text_1[50] = {0};
            char debug_text_2[4] = {0};
            for (uint8_t loop = 0; loop < 16; loop++)    
            {
                if (((*switch_states) & (1 << (15 - loop))) != 0)
                {
                    sprintf(debug_text_2, "1 ");
                }
                else
                {
                    sprintf(debug_text_2, "0 ");
                }
                strcat(debug_text_1, debug_text_2);
            }
            ESP_LOGI(TAG, "Footswitches read: %s", debug_text_1);
            vTaskDelay(500);    
#endif 

            result = ESP_OK;
        }
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
static void __attribute__((unused)) footswitch_handle_dual_mode(tFootswitchHandler* handler)
{
    uint8_t value;   

    switch (handler->state)
    {
        case FOOTSWITCH_IDLE:
        default:
        {
            // read footswitches
            if (handler->footswitch_single_reader(0, &value) == ESP_OK) 
            {
                if (value == 1)
                {
                    ESP_LOGI(TAG, "Footswitch 1 pressed");

                    // foot switch 1 pressed
                    control_request_preset_down();

                    // wait release	
                    handler->sample_counter = 0;
                    handler->state = FOOTSWITCH_WAIT_RELEASE_1;
                }
            }

            if (handler->state == FOOTSWITCH_IDLE)
            {
                if (handler->footswitch_single_reader(1, &value) == ESP_OK)
                {
                    if (value == 1)
                    {
                        ESP_LOGI(TAG, "Footswitch 2 pressed");

                        // foot switch 2 pressed, send event
                        control_request_preset_up();

                        // wait release	
                        handler->sample_counter = 0;
                        handler->state = FOOTSWITCH_WAIT_RELEASE_2;
                    }
                }
            }
        } break;

        case FOOTSWITCH_WAIT_RELEASE_1:
        {
            // read footswitch 1
            if (handler->footswitch_single_reader(0, &value) == ESP_OK)
            {
                if (value == 0)
                {
                    handler->sample_counter++;
                    if (handler->sample_counter == FOOTSWITCH_SAMPLE_COUNT)
                    {
                        // foot switch released
                        handler->state = FOOTSWITCH_IDLE;		
                    }
                }
                else
                {
                    // reset counter
                    handler->sample_counter = 0;
                }
            }
        } break;

        case FOOTSWITCH_WAIT_RELEASE_2:
        {
            // read footswitch 2
            if (handler->footswitch_single_reader(1, &value) == ESP_OK)
            {
                if (value == 0)
                {
                    handler->sample_counter++;
                    if (handler->sample_counter == FOOTSWITCH_SAMPLE_COUNT)
                    {
                        // foot switch released
                        handler->state = FOOTSWITCH_IDLE;
                    }                 
                }
                else
                {
                    // reset counter
                    handler->sample_counter = 0;
                }
            }
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
static void __attribute__((unused)) footswitch_handle_banked(tFootswitchHandler* handler, tFootswitchLayoutEntry* layout)
{
    uint16_t binary_val = 0;    
    uint16_t loop;
    uint16_t mask = 0;

    // read all footswitches
    if (handler->footswitch_multiple_reader(&binary_val) != ESP_OK)
    {
        // failed
        return;
    }
    
    // check if switch is outside of the used range
    for (loop = 0; loop < layout->total_switches; loop++)
    {
        mask |= (1 << loop);
    }

    // mask off unused switches
    binary_val &= mask;

    // handle state
    switch (handler->state)
    {
        case FOOTSWITCH_IDLE:
        {
            // any buttons pressed?
            if (binary_val != 0) 
            {
                // check if bank down is pressed
                if (binary_val == layout->bank_down_switch_mask)
                {
                    if (handler->current_bank > 0)
                    {
                        // bank down
                        handler->current_bank--;   
                        ESP_LOGI(TAG, "Footswitch banked down %d", handler->current_bank);
                    }

                    handler->state = FOOTSWITCH_WAIT_RELEASE_1;
                }
                // check if bank up is pressed
                else if (binary_val == layout->bank_up_switch_mask)
                {
                    if (handler->current_bank < (MAX_PRESETS / layout->presets_per_bank))
                    {
                        // bank up
                        handler->current_bank++;
                        ESP_LOGI(TAG, "Footswitch banked up %d", handler->current_bank);
                    }

                    handler->state = FOOTSWITCH_WAIT_RELEASE_1;
                }
                else
                {
                    // single button pressed, just store it. Preset select only happens on button release
                    handler->index_pending = binary_val;
                }
            }
            else
            {
                if (handler->index_pending != 0)
                {
                    uint8_t new_preset = handler->current_bank * layout->presets_per_bank;

                    // get the index from the bit set
                    for (uint8_t loop = 1; loop < layout->presets_per_bank; loop++)    
                    {
                        if ((handler->index_pending & (1 << loop)) != 0)    
                        {
                            new_preset += loop;
                            break;
                        }
                    }

                    // set the preset
                    control_request_preset_index(new_preset);
                    handler->index_pending = 0;

                    // give a little debounce time
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
            }
        } break;

        case FOOTSWITCH_WAIT_RELEASE_1:
        {
            // check if all buttons released
            if (binary_val == 0)
            {
                handler->state = FOOTSWITCH_IDLE;
                handler->index_pending = 0;

                // give a little debounce time
                vTaskDelay(pdMS_TO_TICKS(100));
            }
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
static void __attribute__((unused)) footswitch_handle_quad_binary(tFootswitchHandler* handler)
{
    uint8_t value;
    uint8_t binary_val = 0;    

    // read all 4 switches (and swap so 1 is pressed)
    handler->footswitch_single_reader(0, &value);
    if (value == 1)
    {
        binary_val |= 1;
    }

    handler->footswitch_single_reader(1, &value);
    if (value == 1)
    {
        binary_val |= 2;
    }

    handler->footswitch_single_reader(2, &value);
    if (value == 1)
    {
        binary_val |= 4;
    }

    handler->footswitch_single_reader(3, &value);
    if (value == 1)
    {
        binary_val |= 8;
    }

    // has it changed?
    if (binary_val != handler->last_binary_val)
    {
        handler->last_binary_val = binary_val;

        // set preset
        control_request_preset_index(binary_val);

        ESP_LOGI(TAG, "Footswitch binary set preset %d", binary_val);
    }

    // wait a little longer, so we don't jump around presets while inputs are being set
    vTaskDelay(pdMS_TO_TICKS(180));
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void footswitch_handle_effects(tFootswitchHandler* handler, tFootswitchEffectHandler* fx_handler, uint8_t max_configs)
{
    uint8_t loop; 
    uint8_t value;
    uint8_t param_handled;
    uint16_t param;
    float new_value;
    tTonexParameter* param_ptr;

    // handle state
    switch (handler->state)
    {
         case FOOTSWITCH_IDLE:
         default:
         {
            for (loop = 0; loop < max_configs; loop++)    
            {
                // is this switch configured?
                if (fx_handler[loop].config.Switch != SWITCH_NOT_USED)
                {                  
                    // check if switch is pressed
                    if (handler->footswitch_single_reader(fx_handler[loop].config.Switch, &value) == ESP_OK)
                    {
                        if (value == 1)
                        {
                            // get the parameter that corresponds to this Midi control change value
                            param = midi_helper_get_param_for_change_num(fx_handler[loop].config.CC);
                            param_handled = 0;

                            if (param != TONEX_UNKNOWN)
                            {
                                // check if its a param or other special case
                                if (param < TONEX_PARAM_LAST)
                                {
                                    // get the current value of the parameter
                                    if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
                                    {
                                        // is the parameter a boolean type?
                                        if ((param_ptr[param].Min == 0) && (param_ptr[param].Max == 1))
                                        {
                                            // toggle the current value
                                            if (param_ptr[param].Value == 0)
                                            {
                                                new_value = 1;
                                            }
                                            else
                                            {
                                                new_value = 0;
                                            }

                                            ESP_LOGI(TAG, "Footswitch Param change to %d", (int)new_value);

                                            // change the parameter
                                            usb_modify_parameter(param, new_value);   
                                            
                                            param_handled = 1;
                                        }

                                        tonex_params_release_locked_access();
                                    }
                                }
                                     
                                if (!param_handled)
                                {
                                    tonex_params_release_locked_access();

                                    // not a boolean, use local toggle variable
                                    if (fx_handler[loop].toggle == 0)
                                    {
                                        // send first value
                                        midi_helper_adjust_param_via_midi(fx_handler[loop].config.CC, fx_handler[loop].config.Value_1);
    
                                        ESP_LOGI(TAG, "Footswitch Param change (1). Switch: %d. CC:%d. Value:%d", (int)fx_handler[loop].config.Switch, 
                                                                                                                (int)fx_handler[loop].config.CC, 
                                                                                                                (int)fx_handler[loop].config.Value_1);
                                    }
                                    else
                                    {
                                        // send second value
                                        midi_helper_adjust_param_via_midi(fx_handler[loop].config.CC, fx_handler[loop].config.Value_2);
    
                                        ESP_LOGI(TAG, "Footswitch Param change (2). Switch: %d. CC:%d. Value:%d", (int)fx_handler[loop].config.Switch, 
                                                                                                                (int)fx_handler[loop].config.CC, 
                                                                                                                (int)fx_handler[loop].config.Value_2);
                                    }
    
                                    // flip toggle state
                                    fx_handler[loop].toggle = !fx_handler[loop].toggle;
                                }   
                            }

                            // save the switch index
                            handler->index_pending = fx_handler[loop].config.Switch;
                            handler->state = FOOTSWITCH_WAIT_RELEASE_1;
                            break;
                        }
                    }
                }
            }
        } break;

        case FOOTSWITCH_WAIT_RELEASE_1:
        {
            // check if switch is released
            if (handler->footswitch_single_reader(handler->index_pending, &value) == ESP_OK)
            {
                if (value == 0)
                {
                    handler->state = FOOTSWITCH_IDLE;
                    handler->index_pending = 0;

                    // give a little debounce time
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
            }
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
void footswitch_task(void *arg)
{       
    __attribute__((unused)) uint8_t value;
    __attribute__((unused)) uint32_t reset_timer = 0;
    uint8_t configs;

    ESP_LOGI(TAG, "Footswitch task start");

    // let things settle
    vTaskDelay(pdMS_TO_TICKS(1000));

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43B || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43DEVONLY
    // 4.3B doesn't have enough IO, only supports dual mode
    FootswitchControl.onboard_switch_mode = FOOTSWITCH_LAYOUT_1X2;
#else
    // others, get the currently configured mode from web config
    FootswitchControl.onboard_switch_mode = control_get_config_item_int(CONFIG_ITEM_FOOTSWITCH_MODE);
#endif

    // get preset switching layout for external footswitches
    FootswitchControl.external_switch_mode = control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_PRESET_LAYOUT);

    // load config for external effect buttons
    for (configs = 0; configs < MAX_EXTERNAL_EFFECT_FOOTSWITCHES; configs++)
    {
        FootswitchControl.ExternalFootswitchEffectHandler[configs].config.Switch = control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT1_SW + (configs * 4));
        FootswitchControl.ExternalFootswitchEffectHandler[configs].config.CC = control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT1_CC + (configs * 4));
        FootswitchControl.ExternalFootswitchEffectHandler[configs].config.Value_1 = control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT1_VAL1 + (configs * 4));
        FootswitchControl.ExternalFootswitchEffectHandler[configs].config.Value_2 = control_get_config_item_int(CONFIG_ITEM_EXT_FOOTSW_EFFECT1_VAL2 + (configs * 4));
    }

    // load config for internal effect buttons
    for (configs = 0; configs < MAX_INTERNAL_EFFECT_FOOTSWITCHES; configs++)
    {
        FootswitchControl.OnboardFootswitchEffectHandler[configs].config.Switch = control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT1_SW + (configs * 4));
        FootswitchControl.OnboardFootswitchEffectHandler[configs].config.CC = control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT1_CC + (configs * 4));
        FootswitchControl.OnboardFootswitchEffectHandler[configs].config.Value_1 = control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT1_VAL1 + (configs * 4));
        FootswitchControl.OnboardFootswitchEffectHandler[configs].config.Value_2 = control_get_config_item_int(CONFIG_ITEM_INT_FOOTSW_EFFECT1_VAL2 + (configs * 4));

        // debug
        //ESP_LOGI(TAG, "Config Internal Footswitch %d, %d, %d, %d, %d", (int)configs, 
        //                                                            (int)FootswitchControl.OnboardFootswitchEffectHandler[configs].config.Switch,
        //                                                            (int)FootswitchControl.OnboardFootswitchEffectHandler[configs].config.CC,
        //                                                            (int)FootswitchControl.OnboardFootswitchEffectHandler[configs].config.Value_1,
        //                                                            (int)FootswitchControl.OnboardFootswitchEffectHandler[configs].config.Value_2);
    }
    
    // setup handler for onboard IO footswitches
    FootswitchControl.Handlers[FOOTSWITCH_HANDLER_ONBOARD_PRESETS].footswitch_single_reader = &footswitch_read_single_onboard;
    FootswitchControl.Handlers[FOOTSWITCH_HANDLER_ONBOARD_PRESETS].footswitch_multiple_reader = &footswitch_read_multiple_onboard;

    FootswitchControl.Handlers[FOOTSWITCH_HANDLER_ONBOARD_EFFECTS].footswitch_single_reader = &footswitch_read_single_onboard;
    FootswitchControl.Handlers[FOOTSWITCH_HANDLER_ONBOARD_EFFECTS].footswitch_multiple_reader = &footswitch_read_multiple_onboard;

    // setup handler for external IO Expander footswitches
    FootswitchControl.Handlers[FOOTSWITCH_HANDLER_EXTERNAL_PRESETS].footswitch_single_reader = &footswitch_read_single_offboard;
    FootswitchControl.Handlers[FOOTSWITCH_HANDLER_EXTERNAL_PRESETS].footswitch_multiple_reader = &footswitch_read_multiple_offboard;

    FootswitchControl.Handlers[FOOTSWITCH_HANDLER_EXTERNAL_EFFECTS].footswitch_single_reader = &footswitch_read_single_offboard;
    FootswitchControl.Handlers[FOOTSWITCH_HANDLER_EXTERNAL_EFFECTS].footswitch_multiple_reader = &footswitch_read_multiple_offboard;


    while (1)
    {
        // Waveshare 4.3 (not B) development board has different pinout 
#if !CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43DEVONLY        
        // handle onboard IO foot switches (direct GPIO and IO expander on main PCB)
        switch (FootswitchControl.onboard_switch_mode) 
        {
            case FOOTSWITCH_LAYOUT_1X2:
            {
                // run dual mode next/previous
                footswitch_handle_dual_mode(&FootswitchControl.Handlers[FOOTSWITCH_HANDLER_ONBOARD_PRESETS]);
            } break;

            case FOOTSWITCH_LAYOUT_1X3: // fallthrough
            case FOOTSWITCH_LAYOUT_1X4:
            {
                // run bankedswitches
                footswitch_handle_banked(&FootswitchControl.Handlers[FOOTSWITCH_HANDLER_ONBOARD_PRESETS], (tFootswitchLayoutEntry*)&FootswitchLayouts[FootswitchControl.onboard_switch_mode]);
            } break;

            case FOOTSWITCH_LAYOUT_1X4_BINARY:
            {
                // run 4 switch binary mode
                footswitch_handle_quad_binary(&FootswitchControl.Handlers[FOOTSWITCH_HANDLER_ONBOARD_PRESETS]);
            } break;

            case FOOTSWITCH_LAYOUT_DISABLED:
            default:
            {
                // nothing to do
            } break;
        }

        // handle effects switching
        footswitch_handle_effects(&FootswitchControl.Handlers[FOOTSWITCH_HANDLER_ONBOARD_EFFECTS], FootswitchControl.OnboardFootswitchEffectHandler, MAX_INTERNAL_EFFECT_FOOTSWITCHES);
#endif 

        // did we find an IO expander on boot?
        if (FootswitchControl.io_expander_ok)
        {
            switch (FootswitchControl.external_switch_mode) 
            {
                case FOOTSWITCH_LAYOUT_1X2:
                {
                    // run dual mode next/previous
                    footswitch_handle_dual_mode(&FootswitchControl.Handlers[FOOTSWITCH_HANDLER_EXTERNAL_PRESETS]);
                } break;

                case FOOTSWITCH_LAYOUT_1X4_BINARY:
                {
                    // run 4 switch binary mode
                    footswitch_handle_quad_binary(&FootswitchControl.Handlers[FOOTSWITCH_HANDLER_EXTERNAL_PRESETS]);
                } break;

                case FOOTSWITCH_LAYOUT_1X3:   // fallthrough
                case FOOTSWITCH_LAYOUT_1X4:   // fallthrough
                case FOOTSWITCH_LAYOUT_1X5A:  // fallthrough
                case FOOTSWITCH_LAYOUT_1X5B:  // fallthrough
                case FOOTSWITCH_LAYOUT_1X6A:  // fallthrough
                case FOOTSWITCH_LAYOUT_1X6B:  // fallthrough
                case FOOTSWITCH_LAYOUT_1X7A:  // fallthrough
                case FOOTSWITCH_LAYOUT_1X7B:  // fallthrough
                case FOOTSWITCH_LAYOUT_2X3:   // fallthrough
                case FOOTSWITCH_LAYOUT_2X4:   // fallthrough
                case FOOTSWITCH_LAYOUT_2X5A:  // fallthrough
                case FOOTSWITCH_LAYOUT_2X5B:  // fallthrough
                case FOOTSWITCH_LAYOUT_2X6A:  // fallthrough
                case FOOTSWITCH_LAYOUT_2X6B:
                {
                    // handle external footswitches as banked
                    footswitch_handle_banked(&FootswitchControl.Handlers[FOOTSWITCH_HANDLER_EXTERNAL_PRESETS], (tFootswitchLayoutEntry*)&FootswitchLayouts[FootswitchControl.external_switch_mode]);
                } break;
          
                case FOOTSWITCH_LAYOUT_DISABLED:
                default:
                {
                    // nothing to do
                } break;
            }
                    
            // handle effects switching
            footswitch_handle_effects(&FootswitchControl.Handlers[FOOTSWITCH_HANDLER_EXTERNAL_EFFECTS], FootswitchControl.ExternalFootswitchEffectHandler, MAX_EXTERNAL_EFFECT_FOOTSWITCHES);
        }

#if !CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43DEVONLY        
        // check for button held for data reset
        if (FOOTSWITCH_1 != -1)
        {
            if (footswitch_read_single_onboard(0, &value) == ESP_OK)
            {
                if (value == 1)
                {        
                    reset_timer++;

                    // debug
                    //ESP_LOGI(TAG, "Reset timer: %d", (int)reset_timer);  

                    if (reset_timer > BUTTON_FACTORY_RESET_TIME)
                    {
                        ESP_LOGI(TAG, "Config Reset to default");  
                        control_set_default_config(); 

                        // save and reboot
                        control_save_user_data(1);
                    }
                }
                else
                {
                    reset_timer = 0;
                }
            }
        }
#endif 

        // handle leds from this task, to save wasting ram on another task for it
        leds_handle();

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void footswitches_init(i2c_port_t i2c_num, SemaphoreHandle_t I2CMutex)
{	
    memset((void*)&FootswitchControl, 0, sizeof(FootswitchControl));

    // save handles
    I2CMutexHandle = I2CMutex;
	i2cnum = i2c_num;

#if CONFIG_TONEX_CONTROLLER_GPIO_FOOTSWITCHES
    // init GPIO
    gpio_config_t gpio_config_struct;

    gpio_config_struct.pin_bit_mask = (((uint64_t)1 << FOOTSWITCH_1) | ((uint64_t)1 << FOOTSWITCH_2) | ((uint64_t)1 << FOOTSWITCH_3) | ((uint64_t)1 << FOOTSWITCH_4));
    gpio_config_struct.mode = GPIO_MODE_INPUT;
    gpio_config_struct.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config_struct.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&gpio_config_struct);
#endif

    // try to init I2C IO expander
    if (SX1509_Init(i2c_num, I2CMutex) == ESP_OK)
    {
        ESP_LOGI(TAG, "Found External IO Expander");

        // init all pins to inputs
        for (uint8_t pin = 0; pin < 16; pin++)
        {
            SX1509_gpioMode(pin, EXPANDER_INPUT_PULLUP);
        }
        FootswitchControl.io_expander_ok = 1;
    }
    else
    {
        ESP_LOGI(TAG, "External IO Expander not found");
    }

    // init leds
    leds_init();

    // create task
    xTaskCreatePinnedToCore(footswitch_task, "FOOT", FOOTSWITCH_TASK_STACK_SIZE, NULL, FOOTSWITCH_TASK_PRIORITY, NULL, 1);
}
