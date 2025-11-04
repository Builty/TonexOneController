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

#include <stdio.h>
#include "sdkconfig.h"
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "lvgl.h"
#include "demos/lv_demos.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_ota_ops.h"
#include "sys/param.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_crc.h"
#include "esp_now.h"
#include "driver/i2c.h"
#include "soc/lldesc.h"
#include "esp_lcd_touch_gt911.h"
#include "esp_lcd_touch_cst816s.h"
#include "esp_lcd_gc9107.h"
#include "esp_lcd_sh8601.h"
#include "esp_intr_alloc.h"
#include "main.h"
#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
    #include "ui.h"
    #include "images.h"
    #include "actions.h"
#endif
#include "usb/usb_host.h"
#include "usb/cdc_acm_host.h"
#include "esp_partition.h"
#include "usb_comms.h"
#include "usb_tonex_common.h"
#include "usb_tonex_one.h"
#include "usb_tonex.h"
#include "display.h"
#include "display_tonex.h"
#include "CH422G.h"
#include "control.h"
#include "task_priorities.h" 
#include "midi_control.h"
#include "LP5562.h"
#include "tonex_params.h"
#include "platform_common.h"

static const char *TAG = "app_display_tonex";

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void tonex_show_settings_tab(lv_event_t * e)
{
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI          
	lv_obj_t* target = lv_event_get_current_target(e);

    if (target == objects.ui_icon_eq)
    {
        // show EQ settings
        lv_tabview_set_act(objects.ui_settings_tab_view, CONFIG_TAB_EQ, LV_ANIM_OFF);
    }
    else if (target == objects.ui_icon_gate)
    {
        // show gate settings
        lv_tabview_set_act(objects.ui_settings_tab_view, CONFIG_TAB_GATE, LV_ANIM_OFF);
    }
    else if ((target == objects.ui_icon_amp) || (target == objects.ui_icon_cab))
    {
        // show amp settings
        lv_tabview_set_act(objects.ui_settings_tab_view, CONFIG_TAB_AMPLIFIER, LV_ANIM_OFF);
    }
    else if (target == objects.ui_icon_comp)
    {
        // show comnpressor settings
        lv_tabview_set_act(objects.ui_settings_tab_view, CONFIG_TAB_COMPRESSOR, LV_ANIM_OFF);
    }
    else if (target == objects.ui_icon_mod)
    {
        // show modulation settings
        lv_tabview_set_act(objects.ui_settings_tab_view, CONFIG_TAB_MODULATION, LV_ANIM_OFF);
    }
    else if (target == objects.ui_icon_delay)
    {
        // show delay settings
        lv_tabview_set_act(objects.ui_settings_tab_view, CONFIG_TAB_DELAY, LV_ANIM_OFF);
    }
    else if (target == objects.ui_icon_reverb)
    {
        // show reverb settings
        lv_tabview_set_act(objects.ui_settings_tab_view, CONFIG_TAB_REVERB, LV_ANIM_OFF);
    }
#endif //CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI          
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void tonex_action_effect_icon_clicked(lv_event_t * e)
{
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI          
    tModellerParameter* param_ptr;
    float value;
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t* event_object = lv_event_get_target(e);

    // called from LVGL 
    ESP_LOGI(TAG, "action_effect_icon_clicked");

    if (event_code == LV_EVENT_SHORT_CLICKED) 
    {
        if (event_object == objects.ui_icon_reverb)
        {
            ESP_LOGI(TAG, "UI Toggle reverb");

            tonex_params_get_locked_access(&param_ptr);
            if (param_ptr[TONEX_PARAM_REVERB_ENABLE].Value == 0.0f)
            {
                value = 1.0f;
            }
            else
            {
                value = 0.0f;
            }
            tonex_params_release_locked_access();

            usb_modify_parameter(TONEX_PARAM_REVERB_ENABLE, value);   
        }
        else if (event_object == objects.ui_icon_delay)
        {
            ESP_LOGI(TAG, "UI Toggle delay");
        
            tonex_params_get_locked_access(&param_ptr);
            if (param_ptr[TONEX_PARAM_DELAY_ENABLE].Value == 0.0f)
            {
                value = 1.0f;
            }
            else
            {
                value = 0.0f;
            }
            tonex_params_release_locked_access();

            usb_modify_parameter(TONEX_PARAM_DELAY_ENABLE, value);   
        }
        else if (event_object == objects.ui_icon_mod)
        {
            ESP_LOGI(TAG, "UI Toggle mod");
            
            tonex_params_get_locked_access(&param_ptr);
            if (param_ptr[TONEX_PARAM_MODULATION_ENABLE].Value == 0.0f)
            {
                value = 1.0f;
            }
            else
            {
                value = 0.0f;
            }
            tonex_params_release_locked_access();

            usb_modify_parameter(TONEX_PARAM_MODULATION_ENABLE, value);  
        } 
        else if (event_object == objects.ui_icon_comp)
        {
            ESP_LOGI(TAG, "UI Toggle comp");
            
            tonex_params_get_locked_access(&param_ptr);
            if (param_ptr[TONEX_PARAM_COMP_ENABLE].Value == 0.0f)
            {
                value = 1.0f;
            }
            else
            {
                value = 0.0f;
            }
            tonex_params_release_locked_access();

            usb_modify_parameter(TONEX_PARAM_COMP_ENABLE, value);  
        }
        else if (event_object == objects.ui_icon_cab)
        {
            ESP_LOGI(TAG, "UI Toggle cab");
            
            tonex_params_get_locked_access(&param_ptr);
            if (param_ptr[TONEX_PARAM_CABINET_TYPE].Value == TONEX_CABINET_DISABLED)
            {
                //todo here: this could have been a VIR cabinet
                value = TONEX_CABINET_TONE_MODEL;
            }
            else
            {
                value = TONEX_CABINET_DISABLED;
            }
            tonex_params_release_locked_access();

            usb_modify_parameter(TONEX_PARAM_CABINET_TYPE, value); 
        }
        else if (event_object == objects.ui_icon_amp)
        {
            ESP_LOGI(TAG, "UI Toggle amp");
            
            tonex_params_get_locked_access(&param_ptr);
            if (param_ptr[TONEX_PARAM_MODEL_AMP_ENABLE].Value == 0.0f)
            {
                value = 1.0f;
            }
            else
            {
                value = 0.0f;
            }
            tonex_params_release_locked_access();

            usb_modify_parameter(TONEX_PARAM_MODEL_AMP_ENABLE, value); 
        }
        else if (event_object == objects.ui_icon_gate)
        {
            ESP_LOGI(TAG, "UI Toggle gate");
            
            tonex_params_get_locked_access(&param_ptr);
            if (param_ptr[TONEX_PARAM_NOISE_GATE_ENABLE].Value == 0.0f)
            {
                value = 1.0f;
            }
            else
            {
                value = 0.0f;
            }
            tonex_params_release_locked_access();

            usb_modify_parameter(TONEX_PARAM_NOISE_GATE_ENABLE, value);   
        }
        else if (event_object == objects.ui_icon_eq)
        {   
            // no short press action
        } 
    }
    else if (event_code == LV_EVENT_LONG_PRESSED) 
    {
        // change to settings page and jump to relevant tab
        action_show_settings_page(e);
        tonex_show_settings_tab(e);      
    }
#endif  //CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI          
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void tonex_action_parameter_changed(lv_event_t * e)
{
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI          
    // get the object that was changed
    lv_obj_t* obj = lv_event_get_current_target(e);

    ESP_LOGI(TAG, "Tonex Parameter changed");

    // see what it was, and update the pedal
    if (obj == objects.ui_noise_gate_switch)
    {
        usb_modify_parameter(TONEX_PARAM_NOISE_GATE_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == objects.ui_noise_gate_post_switch)
    {
        usb_modify_parameter(TONEX_PARAM_NOISE_GATE_POST, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == objects.ui_noise_gate_threshold_slider)
    {
        usb_modify_parameter(TONEX_PARAM_NOISE_GATE_THRESHOLD, lv_slider_get_value(obj));
    }
    else if (obj == objects.ui_noise_gate_release_slider)
    {
        usb_modify_parameter(TONEX_PARAM_NOISE_GATE_RELEASE, lv_slider_get_value(obj));
    }
    else if (obj == objects.ui_noise_gate_depth_slider)
    {
        usb_modify_parameter(TONEX_PARAM_NOISE_GATE_DEPTH, lv_slider_get_value(obj));
    }
    else if (obj == objects.ui_compressor_enable_switch)
    {
        usb_modify_parameter(TONEX_PARAM_COMP_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == objects.ui_compressor_post_switch)
    {
        usb_modify_parameter(TONEX_PARAM_COMP_POST, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == objects.ui_compressor_threshold_slider)
    {
        usb_modify_parameter(TONEX_PARAM_COMP_THRESHOLD, lv_slider_get_value(obj));
    }
    else if (obj == objects.ui_compressor_attack_slider)
    {
        usb_modify_parameter(TONEX_PARAM_COMP_ATTACK, lv_slider_get_value(obj));
    }
    else if (obj == objects.ui_compressor_gain_slider)
    {
        usb_modify_parameter(TONEX_PARAM_COMP_MAKE_UP, lv_slider_get_value(obj));
    }
    else if (obj == objects.ui_eq_post_switch)
    {
        usb_modify_parameter(TONEX_PARAM_EQ_POST, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == objects.ui_eq_bass_slider)
    {
        usb_modify_parameter(TONEX_PARAM_EQ_BASS, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == objects.ui_eq_mid_slider)
    {
        usb_modify_parameter(TONEX_PARAM_EQ_MID, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == objects.ui_eq_mid_qslider)
    {
        usb_modify_parameter(TONEX_PARAM_EQ_MIDQ, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == objects.ui_eq_treble_slider)
    {
        usb_modify_parameter(TONEX_PARAM_EQ_TREBLE, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == objects.ui_reverb_enable_switch)
    {
        usb_modify_parameter(TONEX_PARAM_REVERB_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == objects.ui_reverb_post_switch)
    {
        usb_modify_parameter(TONEX_PARAM_REVERB_POSITION, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == objects.ui_reverb_model_dropdown)
    {
        usb_modify_parameter(TONEX_PARAM_REVERB_MODEL, lv_dropdown_get_selected(obj));
    }
    else if (obj == objects.ui_reverb_mix_slider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(objects.ui_reverb_model_dropdown))
        {
            case TONEX_REVERB_SPRING_1:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING1_MIX, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_SPRING_2:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING2_MIX, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_SPRING_3:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING3_MIX, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_SPRING_4:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING4_MIX, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_ROOM:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_ROOM_MIX, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_PLATE:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_PLATE_MIX, lv_slider_get_value(obj));
            } break;
        }        
    }
    else if (obj == objects.ui_reverb_time_slider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(objects.ui_reverb_model_dropdown))
        {
            case TONEX_REVERB_SPRING_1:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING1_TIME, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_SPRING_2:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING2_TIME, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_SPRING_3:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING3_TIME, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_SPRING_4:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING4_TIME, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_ROOM:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_ROOM_TIME, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_PLATE:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_PLATE_TIME, lv_slider_get_value(obj));
            } break;
        }        
    }
    else if (obj == objects.ui_reverb_predelay_slider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(objects.ui_reverb_model_dropdown))
        {
            case TONEX_REVERB_SPRING_1:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING1_PREDELAY, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_SPRING_2:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING2_PREDELAY, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_SPRING_3:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING3_PREDELAY, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_SPRING_4:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING4_PREDELAY, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_ROOM:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_ROOM_PREDELAY, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_PLATE:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_PLATE_PREDELAY, lv_slider_get_value(obj));
            } break;
        }        
    }
    else if (obj == objects.ui_reverb_color_slider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(objects.ui_reverb_model_dropdown))
        {
            case TONEX_REVERB_SPRING_1:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING1_COLOR, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_SPRING_2:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING2_COLOR, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_SPRING_3:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING3_COLOR, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_SPRING_4:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_SPRING4_COLOR, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_ROOM:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_ROOM_COLOR, lv_slider_get_value(obj));
            } break;

            case TONEX_REVERB_PLATE:
            {
                usb_modify_parameter(TONEX_PARAM_REVERB_PLATE_COLOR, lv_slider_get_value(obj));
            } break;
        }        
    }
    else if (obj == objects.ui_modulation_enable_switch)
    {
        usb_modify_parameter(TONEX_PARAM_MODULATION_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == objects.ui_modulation_post_switch)
    {
        usb_modify_parameter(TONEX_PARAM_MODULATION_POST, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == objects.ui_modulation_model_dropdown)
    {
        usb_modify_parameter(TONEX_PARAM_MODULATION_MODEL, lv_dropdown_get_selected(obj));
    }
    else if (obj == objects.ui_modulation_sync_switch)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(objects.ui_modulation_model_dropdown))
        {
            case TONEX_MODULATION_CHORUS:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_CHORUS_SYNC, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
            } break;

            case TONEX_MODULATION_TREMOLO:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_TREMOLO_SYNC, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
            } break;

            case TONEX_MODULATION_PHASER:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_PHASER_SYNC, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
            } break;

            case TONEX_MODULATION_FLANGER:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_FLANGER_SYNC, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
            } break;

            case TONEX_MODULATION_ROTARY:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_ROTARY_SYNC, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
            } break;
        }
    }
    else if (obj == objects.ui_modulation_ts_dropdown)
    {
        switch (lv_dropdown_get_selected(objects.ui_modulation_model_dropdown))
        {
            case TONEX_MODULATION_CHORUS:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_CHORUS_TS, lv_dropdown_get_selected(obj));
            } break;

            case TONEX_MODULATION_TREMOLO:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_TREMOLO_TS, lv_dropdown_get_selected(obj));
            } break;

            case TONEX_MODULATION_PHASER:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_PHASER_TS, lv_dropdown_get_selected(obj));
            } break;

            case TONEX_MODULATION_FLANGER:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_FLANGER_TS, lv_dropdown_get_selected(obj));
            } break;

            case TONEX_MODULATION_ROTARY:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_ROTARY_TS, lv_dropdown_get_selected(obj));
            } break;
        }
    }
    else if (obj == objects.ui_modulation_param1_slider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(objects.ui_modulation_model_dropdown))
        {
            case TONEX_MODULATION_CHORUS:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_CHORUS_RATE, lv_slider_get_value(obj));
            } break;

            case TONEX_MODULATION_TREMOLO:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_TREMOLO_RATE, lv_slider_get_value(obj));
            } break;

            case TONEX_MODULATION_PHASER:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_PHASER_RATE, lv_slider_get_value(obj));
            } break;

            case TONEX_MODULATION_FLANGER:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_FLANGER_RATE, lv_slider_get_value(obj));
            } break;

            case TONEX_MODULATION_ROTARY:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_ROTARY_SPEED, lv_slider_get_value(obj));
            } break;
        }
    }
    else if (obj == objects.ui_modulation_param2_slider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(objects.ui_modulation_model_dropdown))
        {
            case TONEX_MODULATION_CHORUS:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_CHORUS_DEPTH, lv_slider_get_value(obj));
            } break;

            case TONEX_MODULATION_TREMOLO:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_TREMOLO_SHAPE, lv_slider_get_value(obj));
            } break;

            case TONEX_MODULATION_PHASER:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_PHASER_DEPTH, lv_slider_get_value(obj));
            } break;

            case TONEX_MODULATION_FLANGER:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_FLANGER_DEPTH, lv_slider_get_value(obj));
            } break;

            case TONEX_MODULATION_ROTARY:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_ROTARY_RADIUS, lv_slider_get_value(obj));
            } break;
        }
    }
    else if (obj == objects.ui_modulation_param3_slider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(objects.ui_modulation_model_dropdown))
        {
            case TONEX_MODULATION_CHORUS:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_CHORUS_LEVEL, lv_slider_get_value(obj));
            } break;

            case TONEX_MODULATION_TREMOLO:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_TREMOLO_SPREAD, lv_slider_get_value(obj));
            } break;

            case TONEX_MODULATION_PHASER:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_PHASER_LEVEL, lv_slider_get_value(obj));
            } break;

            case TONEX_MODULATION_FLANGER:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_FLANGER_FEEDBACK, lv_slider_get_value(obj));
            } break;

            case TONEX_MODULATION_ROTARY:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_ROTARY_SPREAD, lv_slider_get_value(obj));
            } break;
        }
    }
    else if (obj == objects.ui_modulation_param4_slider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(objects.ui_modulation_model_dropdown))
        {
            case TONEX_MODULATION_CHORUS:
            {
                // not used
            } break;

            case TONEX_MODULATION_TREMOLO:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_TREMOLO_LEVEL, lv_slider_get_value(obj));
            } break;

            case TONEX_MODULATION_PHASER:
            {
                // not used
            } break;

            case TONEX_MODULATION_FLANGER:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_FLANGER_LEVEL, lv_slider_get_value(obj));
            } break;

            case TONEX_MODULATION_ROTARY:
            {
                usb_modify_parameter(TONEX_PARAM_MODULATION_ROTARY_LEVEL, lv_slider_get_value(obj));
            } break;
        }
    }
    else if (obj == objects.ui_delay_enable_switch)
    {
        usb_modify_parameter(TONEX_PARAM_DELAY_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == objects.ui_delay_post_switch)
    {
        usb_modify_parameter(TONEX_PARAM_DELAY_POST, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == objects.ui_delay_model_dropdown)
    {
        usb_modify_parameter(TONEX_PARAM_DELAY_MODEL, lv_dropdown_get_selected(obj));
    }
    else if (obj == objects.ui_delay_sync_switch)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(objects.ui_delay_model_dropdown))
        {
            case TONEX_DELAY_DIGITAL:
            {
                usb_modify_parameter(TONEX_PARAM_DELAY_DIGITAL_SYNC, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
            } break;

            case TONEX_DELAY_TAPE:
            {
                usb_modify_parameter(TONEX_PARAM_DELAY_TAPE_SYNC, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
            } break;
        }
    }
    else if (obj == objects.ui_delay_ping_pong_switch)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(objects.ui_delay_model_dropdown))
        {
            case TONEX_DELAY_DIGITAL:
            {
                usb_modify_parameter(TONEX_PARAM_DELAY_DIGITAL_MODE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
            } break;

            case TONEX_DELAY_TAPE:
            {
                usb_modify_parameter(TONEX_PARAM_DELAY_TAPE_MODE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
            } break;
        }
    }
    else if (obj == objects.ui_delay_ts_slider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(objects.ui_delay_model_dropdown))
        {
            case TONEX_DELAY_DIGITAL:
            {
                usb_modify_parameter(TONEX_PARAM_DELAY_DIGITAL_TIME, lv_slider_get_value(obj));
            } break;

            case TONEX_DELAY_TAPE:
            {
                usb_modify_parameter(TONEX_PARAM_DELAY_TAPE_TIME, lv_slider_get_value(obj));
            } break;
        }
    }
    else if (obj == objects.ui_delay_ts_dropdown)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(objects.ui_delay_model_dropdown))
        {
            case TONEX_DELAY_DIGITAL:
            {
                usb_modify_parameter(TONEX_PARAM_DELAY_DIGITAL_TS, lv_dropdown_get_selected(obj));
            } break;

            case TONEX_DELAY_TAPE:
            {
                usb_modify_parameter(TONEX_PARAM_DELAY_TAPE_TS, lv_dropdown_get_selected(obj));
            } break;
        }
    }
    else if (obj == objects.ui_delay_feedback_slider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(objects.ui_delay_model_dropdown))
        {
            case TONEX_DELAY_DIGITAL:
            {
                usb_modify_parameter(TONEX_PARAM_DELAY_DIGITAL_FEEDBACK, lv_slider_get_value(obj));
            } break;

            case TONEX_DELAY_TAPE:
            {
                usb_modify_parameter(TONEX_PARAM_DELAY_TAPE_FEEDBACK, lv_slider_get_value(obj));
            } break;
        }        
    }
    else if (obj == objects.ui_delay_mix_slider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(objects.ui_delay_model_dropdown))
        {
            case TONEX_DELAY_DIGITAL:
            {
                usb_modify_parameter(TONEX_PARAM_DELAY_DIGITAL_MIX, lv_slider_get_value(obj));
            } break;

            case TONEX_DELAY_TAPE:
            {
                usb_modify_parameter(TONEX_PARAM_DELAY_TAPE_MIX, lv_slider_get_value(obj));
            } break;
        }     
    }
    else if (obj == objects.ui_amp_enable_switch)
    {
        usb_modify_parameter(TONEX_PARAM_MODEL_AMP_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == objects.ui_cabinet_model_dropdown)
    {
        usb_modify_parameter(TONEX_PARAM_CABINET_TYPE, lv_dropdown_get_selected(obj));
    } 
    else if (obj == objects.ui_amplifier_gain_slider)
    {
        usb_modify_parameter(TONEX_PARAM_MODEL_GAIN, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == objects.ui_amplifier_volume_slider)
    {
        usb_modify_parameter(TONEX_PARAM_MODEL_VOLUME, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == objects.ui_amplifier_presense_slider)
    {
        usb_modify_parameter(TONEX_PARAM_MODEL_PRESENCE, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == objects.ui_amplifier_depth_slider)
    {
        usb_modify_parameter(TONEX_PARAM_MODEL_DEPTH, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == objects.ui_bpm_slider)
    {
        usb_modify_parameter(TONEX_GLOBAL_BPM, lv_slider_get_value(obj));
    }
    else if (obj == objects.ui_input_trim_slider)
    {
        usb_modify_parameter(TONEX_GLOBAL_INPUT_TRIM, lv_slider_get_value(obj));
    }
    else if (obj == objects.ui_cab_bypass_switch)
    {
        usb_modify_parameter(TONEX_GLOBAL_CABSIM_BYPASS, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == objects.ui_tempo_source_switch)
    {
        usb_modify_parameter(TONEX_GLOBAL_TEMPO_SOURCE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == objects.ui_tuning_reference_slider)
    {
        usb_modify_parameter(TONEX_GLOBAL_TUNING_REFERENCE, lv_slider_get_value(obj));
    }
    else if (obj == objects.ui_volume_slider)
    {
        usb_modify_parameter(TONEX_GLOBAL_MASTER_VOLUME, lv_slider_get_value(obj));    
    }
    else
    {
        ESP_LOGW(TAG, "Unknown Parameter changed");    
    }
#endif // CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI          
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void tonex_update_icon_order(void)
{
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI          
    bool gatePost = lv_obj_has_state(objects.ui_noise_gate_post_switch, LV_STATE_CHECKED);
    bool compPost = lv_obj_has_state(objects.ui_compressor_post_switch, LV_STATE_CHECKED);
    bool eqPost = lv_obj_has_state(objects.ui_eq_post_switch, LV_STATE_CHECKED);
    bool modPost = lv_obj_has_state(objects.ui_modulation_post_switch, LV_STATE_CHECKED);
    bool delayPost = lv_obj_has_state(objects.ui_delay_post_switch, LV_STATE_CHECKED);
    bool revPost = lv_obj_has_state(objects.ui_reverb_post_switch, LV_STATE_CHECKED);

    lv_obj_t *icons[8];
    uint8_t index = 0;

    if (!gatePost)
    {
        icons[index++] = objects.ui_icon_gate;
    }
    if (!compPost)
    {
        icons[index++] = objects.ui_icon_comp;
    }
    if (!modPost)
    {
        icons[index++] = objects.ui_icon_mod;
    }
    if (!delayPost)
    {
        icons[index++] = objects.ui_icon_delay;
    }
    if (!eqPost)
    {
        icons[index++] = objects.ui_icon_eq;
    }
    icons[index++] = objects.ui_icon_amp;
    icons[index++] = objects.ui_icon_cab;
    if (eqPost)
    {
        icons[index++] = objects.ui_icon_eq;
    }
    if (gatePost)
    {
        icons[index++] = objects.ui_icon_gate;
    }
    if (compPost)
    {
        icons[index++] = objects.ui_icon_comp;
    }
    if (!revPost)
    {
        icons[index++] = objects.ui_icon_reverb;
    }
    if (modPost)
    {
        icons[index++] = objects.ui_icon_mod;
    }
    if (delayPost)
    {
        icons[index++] = objects.ui_icon_delay;
    }
    if (revPost)
    {
        icons[index] = objects.ui_icon_reverb;
    }
    
    // get the icon coords for this platform
    int16_t offsets[8];
    platform_get_icon_coords(offsets, sizeof(offsets) / sizeof(int16_t));

    for (uint8_t i = 0; i<8; i++)
    {
        lv_obj_t *icon = icons[i];
        int16_t offset = offsets[i];
        lv_obj_set_x(icon, offset);
    }
#endif    
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint8_t tonex_update_ui_parameters(void)
{
    tModellerParameter* param_ptr;
    __attribute__((unused)) char value_string[20];

#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI      
    
    for (uint16_t param = 0; param < TONEX_GLOBAL_LAST; param++)
    {                     
        if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
        {
            tModellerParameter* param_entry = &param_ptr[param];

            // debug
            //ESP_LOGI(TAG, "Param %d: val: %02f, min: %02f, max: %02f", param, param_entry->Value, param_entry->Min, param_entry->Max);

            switch (param)
            {
                case TONEX_PARAM_NOISE_GATE_POST:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_noise_gate_post_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_noise_gate_post_switch, LV_STATE_CHECKED);
                    }
                    tonex_update_icon_order();
                } break;

                case TONEX_PARAM_NOISE_GATE_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_noise_gate_switch, LV_STATE_CHECKED);
                        lv_img_set_src(objects.ui_icon_gate, (lv_obj_t*)&img_effect_icon_gate_on);
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_noise_gate_switch, LV_STATE_CHECKED);
                        lv_img_set_src(objects.ui_icon_gate, (lv_obj_t*)&img_effect_icon_gate_off);
                    }
                } break;

                case TONEX_PARAM_NOISE_GATE_THRESHOLD:
                {                            
                    lv_slider_set_range(objects.ui_noise_gate_threshold_slider, round(param_entry->Min), round(param_entry->Max));
                    lv_slider_set_value(objects.ui_noise_gate_threshold_slider, round(param_entry->Value), LV_ANIM_OFF);

                    // show value and units
                    sprintf(value_string, "%d db", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_noise_gate_threshold_value, value_string);
                } break;

                case TONEX_PARAM_NOISE_GATE_RELEASE:
                {
                    lv_slider_set_range(objects.ui_noise_gate_release_slider, round(param_entry->Min), round(param_entry->Max));
                    lv_slider_set_value(objects.ui_noise_gate_release_slider, round(param_entry->Value), LV_ANIM_OFF); 
                    
                    // show value and units
                    sprintf(value_string, "%d ms", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_noise_gate_release_value, value_string);
                } break;

                case TONEX_PARAM_NOISE_GATE_DEPTH:
                {                            
                    lv_slider_set_range(objects.ui_noise_gate_depth_slider, round(param_entry->Min), round(param_entry->Max));
                    lv_slider_set_value(objects.ui_noise_gate_depth_slider, round(param_entry->Value), LV_ANIM_OFF);

                    // show value and units
                    sprintf(value_string, "%d db", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_noise_gate_depth_value, value_string);
                } break;

                case TONEX_PARAM_COMP_POST:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_compressor_post_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_compressor_post_switch, LV_STATE_CHECKED);
                    }
                    tonex_update_icon_order();
                } break;

                case TONEX_PARAM_COMP_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_compressor_enable_switch, LV_STATE_CHECKED);
                        lv_img_set_src(objects.ui_icon_comp, (lv_obj_t*)&img_effect_icon_comp_on);
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_compressor_enable_switch, LV_STATE_CHECKED);
                        lv_img_set_src(objects.ui_icon_comp, (lv_obj_t*)&img_effect_icon_comp_off);
                    }
                } break;

                case TONEX_PARAM_COMP_THRESHOLD:
                {                            
                    lv_slider_set_range(objects.ui_compressor_threshold_slider, round(param_entry->Min), round(param_entry->Max));
                    lv_slider_set_value(objects.ui_compressor_threshold_slider, round(param_entry->Value), LV_ANIM_OFF);

                    // show value and units
                    sprintf(value_string, "%1.1f db", param_entry->Value);
                    lv_label_set_text(objects.ui_compressor_threshold_value, value_string);                            
                } break;

                case TONEX_PARAM_COMP_MAKE_UP:
                {
                    lv_slider_set_range(objects.ui_compressor_gain_slider, round(param_entry->Min), round(param_entry->Max));
                    lv_slider_set_value(objects.ui_compressor_gain_slider, round(param_entry->Value), LV_ANIM_OFF);    

                    // show value and units
                    sprintf(value_string, "%d db", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_compressor_gain_value, value_string);                            
                } break;

                case TONEX_PARAM_COMP_ATTACK:
                {
                    lv_slider_set_range(objects.ui_compressor_attack_slider, round(param_entry->Min), round(param_entry->Max));
                    lv_slider_set_value(objects.ui_compressor_attack_slider, round(param_entry->Value), LV_ANIM_OFF);                            

                    // show value and units
                    sprintf(value_string, "%d ms", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_compressor_attack_value, value_string);                            
                } break;

                case TONEX_PARAM_EQ_POST:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_eq_post_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_eq_post_switch, LV_STATE_CHECKED);
                    }
                    tonex_update_icon_order();
                } break;

                case TONEX_PARAM_EQ_BASS:
                {
                    lv_slider_set_range(objects.ui_eq_bass_slider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                    lv_slider_set_value(objects.ui_eq_bass_slider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);   

                    // show value and units
                    sprintf(value_string, "%1.1f", param_entry->Value);
                    lv_label_set_text(objects.ui_eq_bass_value, value_string);                            
                } break;

                case TONEX_PARAM_EQ_BASS_FREQ:
                {
                    // not exposed via UI
                } break;

                case TONEX_PARAM_EQ_MID:
                {
                    lv_slider_set_range(objects.ui_eq_mid_slider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                    lv_slider_set_value(objects.ui_eq_mid_slider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);   

                    // show value and units
                    sprintf(value_string, "%1.1f", param_entry->Value);
                    lv_label_set_text(objects.ui_eq_mid_value, value_string);                            
                } break;

                case TONEX_PARAM_EQ_MIDQ:
                {
                    lv_slider_set_range(objects.ui_eq_mid_qslider, round(param_entry->Min * 10.0f), round(param_entry->Max * 10.0f));
                    lv_slider_set_value(objects.ui_eq_mid_qslider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);                            
                    
                    // show value and units
                    sprintf(value_string, "%1.1f", param_entry->Value);
                    lv_label_set_text(objects.ui_eq_mid_qvalue, value_string);                            
                } break;

                case TONEX_PARAM_EQ_MID_FREQ:
                {
                    // not exposed via UI
                } break;

                case TONEX_PARAM_EQ_TREBLE:
                {                            
                    lv_slider_set_range(objects.ui_eq_treble_slider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                    lv_slider_set_value(objects.ui_eq_treble_slider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);
                    
                    // show value and units
                    sprintf(value_string, "%1.1f", param_entry->Value);
                    lv_label_set_text(objects.ui_eq_treble_value, value_string);                            
                } break;

                case TONEX_PARAM_EQ_TREBLE_FREQ:
                {
                    // not exposed via UI    
                } break;
                
                case TONEX_PARAM_MODEL_AMP_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_amp_enable_switch, LV_STATE_CHECKED);
                        lv_img_set_src(objects.ui_icon_amp, (lv_obj_t*)&img_effect_icon_amp_on);
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_amp_enable_switch, LV_STATE_CHECKED);
                        lv_img_set_src(objects.ui_icon_amp, (lv_obj_t*)&img_effect_icon_amp_off);
                    }
                } break;

                case TONEX_PARAM_MODEL_SW1:
                {
                    // not exposed via UI
                } break;

                case TONEX_PARAM_CABINET_TYPE:
                {
                    lv_dropdown_set_selected(objects.ui_cabinet_model_dropdown, param_entry->Value);

                    if (param_entry->Value == TONEX_CABINET_DISABLED)
                    {
                        lv_img_set_src(objects.ui_icon_cab, (lv_obj_t*)&img_effect_icon_cab_off);
                    }
                    else
                    {
                        lv_img_set_src(objects.ui_icon_cab, (lv_obj_t*)&img_effect_icon_cab_on);
                    }
                } break;

                case TONEX_PARAM_MODEL_GAIN:
                {
                    lv_slider_set_range(objects.ui_amplifier_gain_slider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                    lv_slider_set_value(objects.ui_amplifier_gain_slider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);     

                    // show value and units
                    sprintf(value_string, "%1.1f", param_entry->Value);
                    lv_label_set_text(objects.ui_amplifier_gain_value, value_string);                            
                } break;

                case TONEX_PARAM_MODEL_VOLUME:
                {
                    lv_slider_set_range(objects.ui_amplifier_volume_slider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                    lv_slider_set_value(objects.ui_amplifier_volume_slider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);   
                    
                    // show value and units
                    sprintf(value_string, "%1.1f", param_entry->Value);
                    lv_label_set_text(objects.ui_amplifier_volume_value, value_string);                                                        
                } break;

                case TONEX_PARAM_MODEX_MIX:
                {
                    // not exposed via UI
                } break;

                case TONEX_PARAM_MODEL_PRESENCE:
                {                            
                    lv_slider_set_range(objects.ui_amplifier_presense_slider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                    lv_slider_set_value(objects.ui_amplifier_presense_slider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);

                    // show value and units
                    sprintf(value_string, "%1.1f", param_entry->Value);
                    lv_label_set_text(objects.ui_amplifier_presense_value, value_string);                            
                } break;

                //case TONEX_PARAM_CABINET_UNKNOWN:
                //{
                    // not exposed via UI 
                //} break;

                //case TONEX_PARAM_VIR_CABINET:
                //{
                    // not exposed via UI
                //} break;

                case TONEX_PARAM_MODEL_DEPTH:
                {
                    lv_slider_set_range(objects.ui_amplifier_depth_slider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                    lv_slider_set_value(objects.ui_amplifier_depth_slider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);

                    // show value and units
                    sprintf(value_string, "%1.1f", param_entry->Value);
                    lv_label_set_text(objects.ui_amplifier_depth_value, value_string);                            
                } break;

                case TONEX_PARAM_VIR_RESO:
                {
                    // not exposed via UI
                } break;

                case TONEX_PARAM_VIR_MIC_1:
                {
                    // not exposed via UI
                } break;

                case TONEX_PARAM_VIR_MIC_1_X:
                {
                    // not exposed via UI
                } break;

                case TONEX_PARAM_VIR_MIC_1_Z:
                {
                    // not exposed via UI
                } break;

                case TONEX_PARAM_VIR_MIC_2:
                {
                    // not exposed via UI
                } break;

                case TONEX_PARAM_VIR_MIC_2_X:
                {
                    // not exposed via UI
                } break;

                case TONEX_PARAM_VIR_MIC_2_Z:
                {
                    // not exposed via UI
                } break;

                case TONEX_PARAM_VIR_BLEND:
                {
                    // not exposed via UI
                } break;
                
                case TONEX_PARAM_REVERB_POSITION:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_reverb_post_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_reverb_post_switch, LV_STATE_CHECKED);
                    }
                    tonex_update_icon_order();
                } break;

                case TONEX_PARAM_REVERB_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_reverb_enable_switch, LV_STATE_CHECKED);

                        // show enabled icon with letter to indicate the type
                        switch ((int)param_ptr[TONEX_PARAM_REVERB_MODEL].Value)
                        {
                            case TONEX_REVERB_SPRING_1:
                            {
                                lv_img_set_src(objects.ui_icon_reverb, (lv_obj_t*)&img_effect_icon_reverb_on_s1);
                            } break;

                            case TONEX_REVERB_SPRING_2:
                            {
                                lv_img_set_src(objects.ui_icon_reverb, (lv_obj_t*)&img_effect_icon_reverb_on_s2);
                            } break;

                            case TONEX_REVERB_SPRING_3:
                            {
                                lv_img_set_src(objects.ui_icon_reverb, (lv_obj_t*)&img_effect_icon_reverb_on_s3);
                            } break;

                            case TONEX_REVERB_SPRING_4:
                            {
                                lv_img_set_src(objects.ui_icon_reverb, (lv_obj_t*)&img_effect_icon_reverb_on_s4);
                            } break;

                            case TONEX_REVERB_ROOM:
                            {
                                lv_img_set_src(objects.ui_icon_reverb, (lv_obj_t*)&img_effect_icon_reverb_on_r);
                            } break;

                            case TONEX_REVERB_PLATE:
                            default:
                            {
                                lv_img_set_src(objects.ui_icon_reverb, (lv_obj_t*)&img_effect_icon_reverb_on_p);
                            } break;
                        }
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_reverb_enable_switch, LV_STATE_CHECKED);
                        lv_img_set_src(objects.ui_icon_reverb, (lv_obj_t*)&img_effect_icon_reverb_off);
                    }
                } break;

                case TONEX_PARAM_REVERB_MODEL:
                {
                    lv_dropdown_set_selected(objects.ui_reverb_model_dropdown, param_entry->Value);
                } break;

                case TONEX_PARAM_REVERB_SPRING1_TIME:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_1)
                    {                            
                        lv_slider_set_range(objects.ui_reverb_time_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_time_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                        
                        // show value and units
                        sprintf(value_string, "%1.1f", param_entry->Value);
                        lv_label_set_text(objects.ui_reverb_time_value, value_string);                            
                    }
                } break;

                case TONEX_PARAM_REVERB_SPRING1_PREDELAY:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_1)
                    {                          
                        lv_slider_set_range(objects.ui_reverb_predelay_slider, round(param_entry->Min), round(param_entry->Max));  
                        lv_slider_set_value(objects.ui_reverb_predelay_slider, round(param_entry->Value), LV_ANIM_OFF);  
                        
                        // show value and units
                        sprintf(value_string, "%d ms", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_predelay_value, value_string);                            
                    }
                } break;

                case TONEX_PARAM_REVERB_SPRING1_COLOR:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_1)
                    {                            
                        lv_slider_set_range(objects.ui_reverb_color_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_color_slider, round(param_entry->Value), LV_ANIM_OFF);   
                        
                        // show value and units
                        sprintf(value_string, "%d", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_color_value, value_string);                            
                    }
                } break;

                case TONEX_PARAM_REVERB_SPRING1_MIX:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_1)
                    {                           
                        lv_slider_set_range(objects.ui_reverb_mix_slider, round(param_entry->Min), round(param_entry->Max)); 
                        lv_slider_set_value(objects.ui_reverb_mix_slider, round(param_entry->Value), LV_ANIM_OFF);   

                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_mix_value, value_string);                            
                    }
                } break;

                case TONEX_PARAM_REVERB_SPRING2_TIME:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_2)
                    {                                                            
                        lv_slider_set_range(objects.ui_reverb_time_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_time_slider, round(param_entry->Value), LV_ANIM_OFF);

                        // show value and units
                        sprintf(value_string, "%1.1f", param_entry->Value);
                        lv_label_set_text(objects.ui_reverb_time_value, value_string);                            
                    }
                } break;

                case TONEX_PARAM_REVERB_SPRING2_PREDELAY:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_2)
                    {                            
                        lv_slider_set_range(objects.ui_reverb_predelay_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_predelay_slider, round(param_entry->Value), LV_ANIM_OFF);  
                        
                        // show value and units
                        sprintf(value_string, "%d ms", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_predelay_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_SPRING2_COLOR:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_2)
                    {                                                            
                        lv_slider_set_range(objects.ui_reverb_color_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_color_slider, round(param_entry->Value), LV_ANIM_OFF);

                        // show value and units
                        sprintf(value_string, "%d", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_color_value, value_string);                            
                    }
                } break;

                case TONEX_PARAM_REVERB_SPRING2_MIX:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_2)
                    {                            
                        lv_slider_set_range(objects.ui_reverb_mix_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_mix_slider, round(param_entry->Value), LV_ANIM_OFF);       
                        
                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_mix_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_SPRING3_TIME:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_3)
                    {             
                        lv_slider_set_range(objects.ui_reverb_time_slider, round(param_entry->Min), round(param_entry->Max));               
                        lv_slider_set_value(objects.ui_reverb_time_slider, round(param_entry->Value), LV_ANIM_OFF);     

                        // show value and units
                        sprintf(value_string, "%1.1f", param_entry->Value);
                        lv_label_set_text(objects.ui_reverb_time_value, value_string);                            
                    }
                } break;

                case TONEX_PARAM_REVERB_SPRING3_PREDELAY:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_3)
                    {              
                        lv_slider_set_range(objects.ui_reverb_predelay_slider, round(param_entry->Min), round(param_entry->Max));              
                        lv_slider_set_value(objects.ui_reverb_predelay_slider, round(param_entry->Value), LV_ANIM_OFF);    
                        
                        // show value and units
                        sprintf(value_string, "%d ms", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_predelay_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_SPRING3_COLOR:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_3)
                    {                            
                        lv_slider_set_range(objects.ui_reverb_color_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_color_slider, round(param_entry->Value), LV_ANIM_OFF);    
                        
                        // show value and units
                        sprintf(value_string, "%d", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_color_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_SPRING3_MIX:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_3)
                    {                            
                        lv_slider_set_range(objects.ui_reverb_mix_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_mix_slider, round(param_entry->Value), LV_ANIM_OFF);    
                        
                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_mix_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_SPRING4_TIME:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_4)
                    {                    
                        lv_slider_set_range(objects.ui_reverb_time_slider, round(param_entry->Min), round(param_entry->Max));        
                        lv_slider_set_value(objects.ui_reverb_time_slider, round(param_entry->Value), LV_ANIM_OFF);    
                        
                        // show value and units
                        sprintf(value_string, "%1.1f", param_entry->Value);
                        lv_label_set_text(objects.ui_reverb_time_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_SPRING4_PREDELAY:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_4)
                    {                            
                        lv_slider_set_range(objects.ui_reverb_predelay_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_predelay_slider, round(param_entry->Value), LV_ANIM_OFF);    
                        
                        // show value and units
                        sprintf(value_string, "%d ms", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_predelay_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_SPRING4_COLOR:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_4)
                    {                            
                        lv_slider_set_range(objects.ui_reverb_color_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_color_slider, round(param_entry->Value), LV_ANIM_OFF);   
                        
                        // show value and units
                        sprintf(value_string, "%d", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_color_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_SPRING4_MIX:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_4)
                    {                            
                        lv_slider_set_range(objects.ui_reverb_mix_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_mix_slider, round(param_entry->Value), LV_ANIM_OFF);   
                        
                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_mix_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_ROOM_TIME:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_ROOM)
                    {                        
                        lv_slider_set_range(objects.ui_reverb_time_slider, round(param_entry->Min), round(param_entry->Max));    
                        lv_slider_set_value(objects.ui_reverb_time_slider, round(param_entry->Value), LV_ANIM_OFF);   
                        
                        // show value and units
                        sprintf(value_string, "%1.1f", param_entry->Value);
                        lv_label_set_text(objects.ui_reverb_time_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_ROOM_PREDELAY:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_ROOM)
                    {                            
                        lv_slider_set_range(objects.ui_reverb_predelay_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_predelay_slider, round(param_entry->Value), LV_ANIM_OFF);   
                        
                        // show value and units
                        sprintf(value_string, "%d ms", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_predelay_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_ROOM_COLOR:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_ROOM)
                    {                            
                        lv_slider_set_range(objects.ui_reverb_color_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_color_slider, round(param_entry->Value), LV_ANIM_OFF);    
                        
                        // show value and units
                        sprintf(value_string, "%d", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_color_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_ROOM_MIX:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_ROOM)
                    {                            
                        lv_slider_set_range(objects.ui_reverb_mix_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_mix_slider, round(param_entry->Value), LV_ANIM_OFF);    
                        
                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_mix_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_PLATE_TIME:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_PLATE)
                    {                            
                        lv_slider_set_range(objects.ui_reverb_time_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_time_slider, round(param_entry->Value), LV_ANIM_OFF);  
                        
                        // show value and units
                        sprintf(value_string, "%1.1f", param_entry->Value);
                        lv_label_set_text(objects.ui_reverb_time_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_PLATE_PREDELAY:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_PLATE)
                    {                    
                        lv_slider_set_range(objects.ui_reverb_predelay_slider, round(param_entry->Min), round(param_entry->Max));        
                        lv_slider_set_value(objects.ui_reverb_predelay_slider, round(param_entry->Value), LV_ANIM_OFF);     
                        
                        // show value and units
                        sprintf(value_string, "%d ms", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_predelay_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_PLATE_COLOR:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_PLATE)
                    {                            
                        lv_slider_set_range(objects.ui_reverb_color_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_color_slider, round(param_entry->Value), LV_ANIM_OFF);   
                        
                        // show value and units
                        sprintf(value_string, "%d", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_color_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_REVERB_PLATE_MIX:
                {
                    if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_PLATE)
                    {                            
                        lv_slider_set_range(objects.ui_reverb_mix_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_reverb_mix_slider, round(param_entry->Value), LV_ANIM_OFF);  
                        
                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_reverb_mix_value, value_string);                                                            
                    }
                } break;

                case TONEX_PARAM_MODULATION_POST:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_modulation_post_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_modulation_post_switch, LV_STATE_CHECKED);
                    }
                    tonex_update_icon_order();
                } break;

                case TONEX_PARAM_MODULATION_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_modulation_enable_switch, LV_STATE_CHECKED);

                        // show enabled icon with letter to indicate the type
                        switch ((int)param_ptr[TONEX_PARAM_MODULATION_MODEL].Value)
                        {
                            case TONEX_MODULATION_CHORUS:
                            {
                                lv_img_set_src(objects.ui_icon_mod, (lv_obj_t*)&img_effect_icon_mod_on_chorus);
                            } break;

                            case TONEX_MODULATION_TREMOLO:
                            {
                                lv_img_set_src(objects.ui_icon_mod, (lv_obj_t*)&img_effect_icon_mod_on_tremolo);
                            } break;

                            case TONEX_MODULATION_PHASER:
                            {
                                lv_img_set_src(objects.ui_icon_mod, (lv_obj_t*)&img_effect_icon_mod_on_phaser);
                            } break;

                            case TONEX_MODULATION_FLANGER:
                            {
                                lv_img_set_src(objects.ui_icon_mod, (lv_obj_t*)&img_effect_icon_mod_on_flanger);
                            } break;

                            case TONEX_MODULATION_ROTARY:
                            default:
                            {
                                lv_img_set_src(objects.ui_icon_mod, (lv_obj_t*)&img_effect_icon_mod_on_rotary);
                            } break;
                        }
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_modulation_enable_switch, LV_STATE_CHECKED);
                        lv_img_set_src(objects.ui_icon_mod, (lv_obj_t*)&img_effect_icon_mod_off);
                    }
                } break;

                case TONEX_PARAM_MODULATION_MODEL:
                {
                    lv_dropdown_set_selected(objects.ui_modulation_model_dropdown, param_entry->Value);

                    // configure the variable UI items
                    switch ((int)param_entry->Value)
                    {
                        case TONEX_MODULATION_CHORUS:
                        {
                            lv_label_set_text(objects.ui_modulation_param1_label, "Rate");
                            lv_label_set_text(objects.ui_modulation_param2_label, "Depth");
                            lv_label_set_text(objects.ui_modulation_param3_label, "Level");
                            lv_obj_add_flag(objects.ui_modulation_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_modulation_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_modulation_param4_value, LV_OBJ_FLAG_HIDDEN);
                        } break;

                        case TONEX_MODULATION_TREMOLO:
                        {
                            lv_label_set_text(objects.ui_modulation_param1_label, "Rate");
                            lv_label_set_text(objects.ui_modulation_param2_label, "Shape");
                            lv_label_set_text(objects.ui_modulation_param3_label, "Spread");
                            lv_label_set_text(objects.ui_modulation_param4_label, "Level");
                            lv_obj_clear_flag(objects.ui_modulation_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_param4_value, LV_OBJ_FLAG_HIDDEN);
                        } break;

                        case TONEX_MODULATION_PHASER:
                        {
                            lv_label_set_text(objects.ui_modulation_param1_label, "Rate");
                            lv_label_set_text(objects.ui_modulation_param2_label, "Depth");
                            lv_label_set_text(objects.ui_modulation_param3_label, "Level");
                            lv_obj_add_flag(objects.ui_modulation_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_modulation_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_modulation_param4_value, LV_OBJ_FLAG_HIDDEN);
                        } break;

                        case TONEX_MODULATION_FLANGER:
                        {
                            lv_label_set_text(objects.ui_modulation_param1_label, "Rate");
                            lv_label_set_text(objects.ui_modulation_param2_label, "Depth");
                            lv_label_set_text(objects.ui_modulation_param3_label, "Feedback");
                            lv_label_set_text(objects.ui_modulation_param4_label, "Level");
                            lv_obj_clear_flag(objects.ui_modulation_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_param4_value, LV_OBJ_FLAG_HIDDEN);
                        } break;

                        case TONEX_MODULATION_ROTARY:
                        {
                            lv_label_set_text(objects.ui_modulation_param1_label, "Speed");
                            lv_label_set_text(objects.ui_modulation_param2_label, "Radius");
                            lv_label_set_text(objects.ui_modulation_param3_label, "Spread");
                            lv_label_set_text(objects.ui_modulation_param4_label, "Level");
                            lv_obj_clear_flag(objects.ui_modulation_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_param4_value, LV_OBJ_FLAG_HIDDEN);
                        } break;

                        default:
                        {
                            ESP_LOGW(TAG, "Unknown modulation model: %d", (int)param_entry->Value);
                        } break;
                    }
                } break;

                case TONEX_PARAM_MODULATION_CHORUS_SYNC:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_CHORUS)
                    {      
                        if (param_entry->Value)
                        {
                            lv_obj_add_state(objects.ui_modulation_sync_switch, LV_STATE_CHECKED);
                            lv_obj_add_flag(objects.ui_modulation_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_modulation_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                        }
                        else
                        {
                            lv_obj_clear_state(objects.ui_modulation_sync_switch, LV_STATE_CHECKED);
                            lv_obj_clear_flag(objects.ui_modulation_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_modulation_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                        }                        
                    }
                } break;

                case TONEX_PARAM_MODULATION_CHORUS_TS:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_CHORUS)
                    {
                        lv_dropdown_set_selected(objects.ui_modulation_ts_dropdown, param_entry->Value);              
                    }
                } break;

                case TONEX_PARAM_MODULATION_CHORUS_RATE:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_CHORUS)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param1_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param1_slider, round(param_entry->Value), LV_ANIM_OFF);   
                                                        
                        // show value and units
                        sprintf(value_string, "%1.1f Hz", param_entry->Value);
                        lv_label_set_text(objects.ui_modulation_param1_value, value_string);              
                    }
                } break;

                case TONEX_PARAM_MODULATION_CHORUS_DEPTH:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_CHORUS)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param2_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param2_slider, round(param_entry->Value), LV_ANIM_OFF);     
                                                                                
                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_modulation_param2_value, value_string);                                     
                    }
                } break;

                case TONEX_PARAM_MODULATION_CHORUS_LEVEL:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_CHORUS)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param3_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param3_slider, round(param_entry->Value), LV_ANIM_OFF);  
                                                                                
                        // show value and units
                        sprintf(value_string, "%1.1f", param_entry->Value);
                        lv_label_set_text(objects.ui_modulation_param3_value, value_string);                                     
                    }
                } break;

                case TONEX_PARAM_MODULATION_TREMOLO_SYNC:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_TREMOLO)
                    {      
                        if (param_entry->Value)
                        {
                            lv_obj_add_state(objects.ui_modulation_sync_switch, LV_STATE_CHECKED);
                            lv_obj_add_flag(objects.ui_modulation_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_modulation_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                        }
                        else
                        {
                            lv_obj_clear_state(objects.ui_modulation_sync_switch, LV_STATE_CHECKED);
                            lv_obj_clear_flag(objects.ui_modulation_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_modulation_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                        }                        
                    }
                } break;

                case TONEX_PARAM_MODULATION_TREMOLO_TS:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_TREMOLO)
                    {
                        lv_dropdown_set_selected(objects.ui_modulation_ts_dropdown, param_entry->Value);
                    }
                } break;

                case TONEX_PARAM_MODULATION_TREMOLO_RATE:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_TREMOLO)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param1_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param1_slider, round(param_entry->Value), LV_ANIM_OFF);   
                                                                                                                    
                        // show value and units
                        sprintf(value_string, "%1.1f Hz", param_entry->Value);
                        lv_label_set_text(objects.ui_modulation_param1_value, value_string);     
                    }
                } break;

                case TONEX_PARAM_MODULATION_TREMOLO_SHAPE:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_TREMOLO)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param2_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param2_slider, round(param_entry->Value), LV_ANIM_OFF);    
                                                                                                                                      
                        // show value and units
                        sprintf(value_string, "%1.1f", param_entry->Value);
                        lv_label_set_text(objects.ui_modulation_param2_value, value_string);                                     
                    }
                } break;

                case TONEX_PARAM_MODULATION_TREMOLO_SPREAD:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_TREMOLO)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param3_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param3_slider, round(param_entry->Value), LV_ANIM_OFF);    
                                                                                                                                      
                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_modulation_param3_value, value_string);                                 
                    }
                } break;

                case TONEX_PARAM_MODULATION_TREMOLO_LEVEL:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_TREMOLO)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param4_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param4_slider, round(param_entry->Value), LV_ANIM_OFF); 
                                                                                                      
                        // show value and units
                        sprintf(value_string, "%1.1f", param_entry->Value);
                        lv_label_set_text(objects.ui_modulation_param4_value, value_string);                                                                    
                    }
                } break;

                case TONEX_PARAM_MODULATION_PHASER_SYNC:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_PHASER)
                    {      
                        if (param_entry->Value)
                        {
                            lv_obj_add_state(objects.ui_modulation_sync_switch, LV_STATE_CHECKED);
                            lv_obj_add_flag(objects.ui_modulation_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_modulation_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                        }
                        else
                        {
                            lv_obj_clear_state(objects.ui_modulation_sync_switch, LV_STATE_CHECKED);
                            lv_obj_clear_flag(objects.ui_modulation_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_modulation_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                        }                        
                    }
                } break;

                case TONEX_PARAM_MODULATION_PHASER_TS:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_PHASER)
                    {
                        lv_dropdown_set_selected(objects.ui_modulation_ts_dropdown, param_entry->Value);
                    }
                } break;

                case TONEX_PARAM_MODULATION_PHASER_RATE:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_PHASER)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param1_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param1_slider, round(param_entry->Value), LV_ANIM_OFF);   
                                                                                                      
                        // show value and units
                        sprintf(value_string, "%1.1f Hz", param_entry->Value);
                        lv_label_set_text(objects.ui_modulation_param1_value, value_string);                                                                  
                    }
                } break;

                case TONEX_PARAM_MODULATION_PHASER_DEPTH:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_PHASER)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param2_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param2_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                                                                                                   
                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_modulation_param2_value, value_string);                                          
                    }
                } break;

                case TONEX_PARAM_MODULATION_PHASER_LEVEL:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_PHASER)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param3_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param3_slider, round(param_entry->Value), LV_ANIM_OFF);   
                                                                                                   
                        // show value and units
                        sprintf(value_string, "%1.1f", param_entry->Value);
                        lv_label_set_text(objects.ui_modulation_param3_value, value_string);                                                                       
                    }
                } break;

                case TONEX_PARAM_MODULATION_FLANGER_SYNC:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_FLANGER)
                    {      
                        if (param_entry->Value)
                        {
                            lv_obj_add_state(objects.ui_modulation_sync_switch, LV_STATE_CHECKED);
                            lv_obj_add_flag(objects.ui_modulation_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_modulation_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                        }
                        else
                        {
                            lv_obj_clear_state(objects.ui_modulation_sync_switch, LV_STATE_CHECKED);
                            lv_obj_clear_flag(objects.ui_modulation_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_modulation_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                        }                        
                    }
                } break;

                case TONEX_PARAM_MODULATION_FLANGER_TS:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_FLANGER)
                    {
                        lv_dropdown_set_selected(objects.ui_modulation_ts_dropdown, param_entry->Value);
                    }
                } break;

                case TONEX_PARAM_MODULATION_FLANGER_RATE:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_FLANGER)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param1_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param1_slider, round(param_entry->Value), LV_ANIM_OFF);   
                                                                                                   
                        // show value and units
                        sprintf(value_string, "%1.1f Hz", param_entry->Value);
                        lv_label_set_text(objects.ui_modulation_param1_value, value_string);                                                                       
                    }
                } break;

                case TONEX_PARAM_MODULATION_FLANGER_DEPTH:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_FLANGER)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param2_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param2_slider, round(param_entry->Value), LV_ANIM_OFF);      
                                                                                                  
                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_modulation_param2_value, value_string);                                                                    
                    }
                } break;

                case TONEX_PARAM_MODULATION_FLANGER_FEEDBACK:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_FLANGER)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param3_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param3_slider, round(param_entry->Value), LV_ANIM_OFF);    
                                                                                                  
                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_modulation_param3_value, value_string);                                                                      
                    }
                } break;

                case TONEX_PARAM_MODULATION_FLANGER_LEVEL:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_FLANGER)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param4_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param4_slider, round(param_entry->Value), LV_ANIM_OFF);      
                                                                                                  
                        // show value and units
                        sprintf(value_string, "%1.1f", param_entry->Value);
                        lv_label_set_text(objects.ui_modulation_param4_value, value_string);                                                                    
                    }
                } break;

                case TONEX_PARAM_MODULATION_ROTARY_SYNC:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_ROTARY)
                    {      
                        if (param_entry->Value)
                        {
                            lv_obj_add_state(objects.ui_modulation_sync_switch, LV_STATE_CHECKED);
                            lv_obj_add_flag(objects.ui_modulation_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_modulation_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                        }
                        else
                        {
                            lv_obj_clear_state(objects.ui_modulation_sync_switch, LV_STATE_CHECKED);
                            lv_obj_clear_flag(objects.ui_modulation_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_modulation_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_modulation_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                        }                        
                    }
                } break;

                case TONEX_PARAM_MODULATION_ROTARY_TS:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_ROTARY)
                    {
                        lv_dropdown_set_selected(objects.ui_modulation_ts_dropdown, param_entry->Value);
                    }
                } break;

                case TONEX_PARAM_MODULATION_ROTARY_SPEED:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_ROTARY)
                    {                                 
                        lv_slider_set_range(objects.ui_modulation_param1_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                                                                                                  
                        // show value and units
                        sprintf(value_string, "%d RPM", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_modulation_param1_value, value_string);                                          
                    }
                } break;

                case TONEX_PARAM_MODULATION_ROTARY_RADIUS:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_ROTARY)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param2_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param2_slider, round(param_entry->Value), LV_ANIM_OFF);    
                                                                                                 
                        // show value and units
                        sprintf(value_string, "%d mm", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_modulation_param2_value, value_string);                                                                        
                    }
                } break;

                case TONEX_PARAM_MODULATION_ROTARY_SPREAD:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_ROTARY)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param3_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param3_slider, round(param_entry->Value), LV_ANIM_OFF);     
                                                                                                 
                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_modulation_param3_value, value_string);                                                                       
                    }
                } break;

                case TONEX_PARAM_MODULATION_ROTARY_LEVEL:
                {
                    if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_ROTARY)
                    { 
                        lv_slider_set_range(objects.ui_modulation_param4_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_modulation_param4_slider, round(param_entry->Value), LV_ANIM_OFF);    
                                                                                                 
                        // show value and units
                        sprintf(value_string, "%1.1f", param_entry->Value);
                        lv_label_set_text(objects.ui_modulation_param4_value, value_string);                                                                        
                    }
                } break;
                
                case TONEX_PARAM_DELAY_POST:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_delay_post_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_delay_post_switch, LV_STATE_CHECKED);
                    }
                    tonex_update_icon_order();
                } break;

                case TONEX_PARAM_DELAY_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_delay_enable_switch, LV_STATE_CHECKED);

                         // show enabled icon with letter to indicate the type
                         switch ((int)param_ptr[TONEX_PARAM_DELAY_MODEL].Value)
                         {
                             case TONEX_DELAY_DIGITAL:
                             {
                                 lv_img_set_src(objects.ui_icon_delay, (lv_obj_t*)&img_effect_icon_delay_on_d);
                             } break;

                             case TONEX_DELAY_TAPE:
                             default:
                             {
                                 lv_img_set_src(objects.ui_icon_delay, (lv_obj_t*)&img_effect_icon_delay_on_t);
                             } break;
                         }
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_delay_enable_switch, LV_STATE_CHECKED);
                        lv_img_set_src(objects.ui_icon_delay, (lv_obj_t*)&img_effect_icon_delay_off);
                    }
                } break;

                case TONEX_PARAM_DELAY_MODEL:
                {
                    lv_dropdown_set_selected(objects.ui_delay_model_dropdown, param_entry->Value);
                } break;

                case TONEX_PARAM_DELAY_DIGITAL_SYNC:
                {
                    if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_DIGITAL)
                    {
                        if (param_entry->Value)
                        {
                            lv_obj_add_state(objects.ui_delay_sync_switch, LV_STATE_CHECKED);
                            lv_obj_add_flag(objects.ui_delay_ts_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_delay_ts_value, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_delay_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                        }
                        else
                        {
                            lv_obj_clear_state(objects.ui_delay_sync_switch, LV_STATE_CHECKED);
                            lv_obj_clear_flag(objects.ui_delay_ts_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_delay_ts_value, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_delay_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                        }
                    }
                } break;

                case TONEX_PARAM_DELAY_DIGITAL_TS:
                {
                    if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_DIGITAL)
                    {
                        lv_dropdown_set_selected(objects.ui_delay_ts_dropdown, param_entry->Value);
                    }
                } break;

                case TONEX_PARAM_DELAY_DIGITAL_TIME:
                {
                    if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_DIGITAL)
                    { 
                        lv_slider_set_range(objects.ui_delay_ts_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_delay_ts_slider, round(param_entry->Value), LV_ANIM_OFF);   
                                                                                                 
                        // show value and units
                        sprintf(value_string, "%d ms", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_delay_ts_value, value_string);                                                                         
                    }
                } break;

                case TONEX_PARAM_DELAY_DIGITAL_FEEDBACK:
                {
                    if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_DIGITAL)
                    { 
                        lv_slider_set_range(objects.ui_delay_feedback_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_delay_feedback_slider, round(param_entry->Value), LV_ANIM_OFF);    
                                                                                                 
                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_delay_feedback_value, value_string);                                                                         
                    }
                } break;

                case TONEX_PARAM_DELAY_DIGITAL_MODE:
                {
                    if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_DIGITAL)
                    {
                        if (param_entry->Value)
                        {
                            lv_obj_add_state(objects.ui_delay_ping_pong_switch, LV_STATE_CHECKED);
                        }
                        else
                        {
                            lv_obj_clear_state(objects.ui_delay_ping_pong_switch, LV_STATE_CHECKED);
                        }
                    }
                } break;

                case TONEX_PARAM_DELAY_DIGITAL_MIX:
                {
                    if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_DIGITAL)
                    { 
                        lv_slider_set_range(objects.ui_delay_mix_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_delay_mix_slider, round(param_entry->Value), LV_ANIM_OFF);    
                                                                                                                                
                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_delay_mix_value, value_string);                                                                                                         
                    }
                } break;

                case TONEX_PARAM_DELAY_TAPE_SYNC:
                {
                    if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_TAPE)
                    {
                        if (param_entry->Value)
                        {
                            lv_obj_add_state(objects.ui_delay_sync_switch, LV_STATE_CHECKED);
                            lv_obj_add_flag(objects.ui_delay_ts_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_delay_ts_value, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_delay_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                        }
                        else
                        {
                            lv_obj_clear_state(objects.ui_delay_sync_switch, LV_STATE_CHECKED);
                            lv_obj_clear_flag(objects.ui_delay_ts_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_delay_ts_value, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_delay_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                        }
                    }
                } break;

                case TONEX_PARAM_DELAY_TAPE_TS:
                {
                    if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_TAPE)
                    {
                        lv_dropdown_set_selected(objects.ui_delay_ts_dropdown, param_entry->Value);
                    }
                } break;

                case TONEX_PARAM_DELAY_TAPE_TIME:
                {
                    if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_TAPE)
                    { 
                        lv_slider_set_range(objects.ui_delay_ts_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_delay_ts_slider, round(param_entry->Value), LV_ANIM_OFF);    
                                                                                                                                 
                        // show value and units
                        sprintf(value_string, "%d ms", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_delay_ts_value, value_string);                                                                                                         
                    }
                } break;

                case TONEX_PARAM_DELAY_TAPE_FEEDBACK:
                {
                    if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_TAPE)
                    { 
                        lv_slider_set_range(objects.ui_delay_feedback_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_delay_feedback_slider, round(param_entry->Value), LV_ANIM_OFF);    
                                                                                                                                 
                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_delay_feedback_value, value_string);                                                                                                         
                    }
                } break;

                case TONEX_PARAM_DELAY_TAPE_MODE:
                {
                    if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_TAPE)
                    {
                        if (param_entry->Value)
                        {
                            lv_obj_add_state(objects.ui_delay_ping_pong_switch, LV_STATE_CHECKED);
                        }
                        else
                        {
                            lv_obj_clear_state(objects.ui_delay_ping_pong_switch, LV_STATE_CHECKED);
                        }
                    }
                } break;
                
                case TONEX_PARAM_DELAY_TAPE_MIX:
                {
                    if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_TAPE)
                    { 
                        lv_slider_set_range(objects.ui_delay_mix_slider, round(param_entry->Min), round(param_entry->Max));
                        lv_slider_set_value(objects.ui_delay_mix_slider, round(param_entry->Value), LV_ANIM_OFF);   
                                                                                                                                 
                        // show value and units
                        sprintf(value_string, "%d%%", (int)round(param_entry->Value));
                        lv_label_set_text(objects.ui_delay_mix_value, value_string);                                                                                                         
                    }
                } break;

                case TONEX_GLOBAL_CABSIM_BYPASS:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_cab_bypass_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_cab_bypass_switch, LV_STATE_CHECKED);
                    }
                } break;

                case TONEX_GLOBAL_TEMPO_SOURCE:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_tempo_source_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_tempo_source_switch, LV_STATE_CHECKED);
                    }
                } break;

                case TONEX_GLOBAL_BPM:
                {
                    lv_slider_set_range(objects.ui_bpm_slider, round(param_entry->Min), round(param_entry->Max));
                    lv_slider_set_value(objects.ui_bpm_slider, round(param_entry->Value), LV_ANIM_OFF); 
                                                                                                                                                             
                    // show value and units
                    sprintf(value_string, "%1.1f", param_entry->Value);
                    lv_label_set_text(objects.ui_bpm_value, value_string);                                                                                                         

                    char buf[128];
                    sprintf(buf, "%.1f", param_entry->Value);
                    lv_label_set_text(objects.ui_bpm_value_label, buf);
                    
#if CONFIG_TONEX_CONTROLLER_SHOW_BPM_INDICATOR                            
                    ui_BPMAnimate(objects.ui_bpm_indicator, 1000 * 60 / param_entry->Value);
#endif                            
                } break;

                case TONEX_GLOBAL_INPUT_TRIM:
                {
                    lv_slider_set_range(objects.ui_input_trim_slider, round(param_entry->Min), round(param_entry->Max));
                    lv_slider_set_value(objects.ui_input_trim_slider, round(param_entry->Value), LV_ANIM_OFF);   
                                                                                                                                                             
                    // show value and units
                    sprintf(value_string, "%1.1f db", param_entry->Value);
                    lv_label_set_text(objects.ui_input_trim_value, value_string);                                                                                                         
                } break;
                
                case TONEX_GLOBAL_TUNING_REFERENCE:
                {                            
                    lv_slider_set_range(objects.ui_tuning_reference_slider, round(param_entry->Min), round(param_entry->Max));
                    lv_slider_set_value(objects.ui_tuning_reference_slider, round(param_entry->Value), LV_ANIM_OFF);  
                                                                                                                                                             
                    // show value and units
                    sprintf(value_string, "%d Hz", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_tuning_reference_value, value_string);                                                                                                         
                } break;

                case TONEX_GLOBAL_MASTER_VOLUME:
                {                            
                    lv_slider_set_range(objects.ui_volume_slider, round(param_entry->Min), round(param_entry->Max));
                    lv_slider_set_value(objects.ui_volume_slider, round(param_entry->Value), LV_ANIM_OFF);   
                                                                                                                                                             
                    // show value and units
                    sprintf(value_string, "%1.1f db", param_entry->Value);
                    lv_label_set_text(objects.ui_volume_value, value_string);                                                                                                         
                } break;
            } 

            tonex_params_release_locked_access();
        }               
    }
#endif 

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169 \
    || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169TOUCH \
    || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_M5ATOMS3R \
    || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_LILYGO_TDISPLAY_S3 \
    || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_19TOUCH

    for (uint16_t param = 0; param < TONEX_GLOBAL_LAST; param++)
    {                     
        if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
        {
            tModellerParameter* param_entry = &param_ptr[param];

            // debug
            //ESP_LOGI(TAG, "Param %d: val: %02f, min: %02f, max: %02f", param, param_entry->Value, param_entry->Min, param_entry->Max);

            switch (param)
            {
                case TONEX_GLOBAL_BPM:
                {
                    char buf[128];
                    sprintf(buf, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_bpm, buf);  
                } break;

                case TONEX_PARAM_COMP_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_set_style_border_color(objects.ui_cstatus, lv_color_hex(0xDDDD00), LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                    else
                    {
                        lv_obj_set_style_border_color(objects.ui_cstatus, lv_color_hex(0x563F2A), LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                } break;

                case TONEX_PARAM_MODULATION_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_set_style_border_color(objects.ui_mstatus, lv_color_hex(0xEEAA00), LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                    else
                    {
                        lv_obj_set_style_border_color(objects.ui_mstatus, lv_color_hex(0x563F2A), LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                } break;

                case TONEX_PARAM_DELAY_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_set_style_border_color(objects.ui_dstatus, lv_color_hex(0x00CC00), LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                    else
                    {
                        lv_obj_set_style_border_color(objects.ui_dstatus, lv_color_hex(0x563F2A), LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                } break;

                case TONEX_PARAM_REVERB_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_set_style_border_color(objects.ui_rstatus, lv_color_hex(0x33FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                    else
                    {
                        lv_obj_set_style_border_color(objects.ui_rstatus, lv_color_hex(0x563F2A), LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                } break;

            }

            tonex_params_release_locked_access();
        }               
    }
#endif

    return 1;
}
