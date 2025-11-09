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
#include "usb_valeton_gp5.h"
#include "usb_tonex.h"
#include "display.h"
#include "display_valeton.h"
#include "CH422G.h"
#include "control.h"
#include "task_priorities.h" 
#include "midi_control.h"
#include "LP5562.h"
#include "valeton_params.h"
#include "platform_common.h"

static const char *TAG = "app_display_valeton";

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void valeton_show_settings_tab(lv_event_t * e)
{
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI          
	lv_obj_t* target = lv_event_get_current_target(e);

    if (target == objects.ui_icon_val_pre)
    {
        lv_tabview_set_act(objects.ui_val_settings_tab_view, CONFIG_TAB_VAL_PRE, LV_ANIM_OFF);
    }
    else if (target == objects.ui_icon_val_nr)
    {
        lv_tabview_set_act(objects.ui_val_settings_tab_view, CONFIG_TAB_VAL_NR, LV_ANIM_OFF);
    }
    else if (target == objects.ui_icon_val_rvb)
    {
        lv_tabview_set_act(objects.ui_val_settings_tab_view, CONFIG_TAB_VAL_RVB, LV_ANIM_OFF);
    }
    else if (target == objects.ui_icon_val_amp) 
    {
        lv_tabview_set_act(objects.ui_val_settings_tab_view, CONFIG_TAB_VAL_AMP, LV_ANIM_OFF);
    }
    else if (target == objects.ui_icon_val_cab)
    {
        lv_tabview_set_act(objects.ui_val_settings_tab_view, CONFIG_TAB_VAL_CAB, LV_ANIM_OFF);
    }
    else if (target == objects.ui_icon_val_tc)
    {
        lv_tabview_set_act(objects.ui_val_settings_tab_view, CONFIG_TAB_VAL_NR, LV_ANIM_OFF);
    }
    else if (target == objects.ui_icon_val_eq)
    {
        lv_tabview_set_act(objects.ui_val_settings_tab_view, CONFIG_TAB_VAL_EQ, LV_ANIM_OFF);
    }
    else if (target == objects.ui_icon_val_mod)
    {
        lv_tabview_set_act(objects.ui_val_settings_tab_view, CONFIG_TAB_VAL_MOD, LV_ANIM_OFF);
    }
    else if (target == objects.ui_icon_val_dly)
    {
        lv_tabview_set_act(objects.ui_val_settings_tab_view, CONFIG_TAB_VAL_DLY, LV_ANIM_OFF);
    }
    else if (target == objects.ui_icon_val_dst)
    {
        lv_tabview_set_act(objects.ui_val_settings_tab_view, CONFIG_TAB_VAL_DST, LV_ANIM_OFF);
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
void valeton_action_effect_icon_clicked(lv_event_t * e)
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
        if (event_object == objects.ui_icon_val_pre)
        {
            ESP_LOGI(TAG, "UI Toggle Pre");

            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                if (param_ptr[VALETON_PARAM_PRE_ENABLE].Value == 0.0f)
                {
                    value = 1.0f;
                }
                else
                {
                    value = 0.0f;
                }
                valeton_params_release_locked_access();

                usb_modify_parameter(VALETON_PARAM_PRE_ENABLE, value);   
            }
        }
        else if (event_object == objects.ui_icon_val_nr)
        {
            ESP_LOGI(TAG, "UI Toggle NR");
        
            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                if (param_ptr[VALETON_PARAM_NR_ENABLE].Value == 0.0f)
                {
                    value = 1.0f;
                }
                else
                {
                    value = 0.0f;
                }
                valeton_params_release_locked_access();

                usb_modify_parameter(VALETON_PARAM_NR_ENABLE, value);   
            }
        }
        else if (event_object == objects.ui_icon_val_rvb)
        {
            ESP_LOGI(TAG, "UI Toggle RVB");
            
            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                if (param_ptr[VALETON_PARAM_RVB_ENABLE].Value == 0.0f)
                {
                    value = 1.0f;
                }
                else
                {
                    value = 0.0f;
                }
                valeton_params_release_locked_access();

                usb_modify_parameter(VALETON_PARAM_RVB_ENABLE, value);  
            }
        } 
        else if (event_object == objects.ui_icon_val_eq)
        {
            ESP_LOGI(TAG, "UI Toggle EQ");
            
            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                if (param_ptr[VALETON_PARAM_EQ_ENABLE].Value == 0.0f)
                {
                    value = 1.0f;
                }
                else
                {
                    value = 0.0f;
                }
                valeton_params_release_locked_access();

                usb_modify_parameter(VALETON_PARAM_EQ_ENABLE, value);  
            }
        }
        else if (event_object == objects.ui_icon_val_mod)
        {
            ESP_LOGI(TAG, "UI Toggle Mod");
            
            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                if (param_ptr[VALETON_PARAM_MOD_ENABLE].Value == 0.0f)
                {
                    value = 1.0f;
                }
                else
                {
                    value = 0.0f;
                }
                valeton_params_release_locked_access();

                usb_modify_parameter(VALETON_PARAM_MOD_ENABLE, value); 
            }
        }
        else if (event_object == objects.ui_icon_val_dly)
        {
            ESP_LOGI(TAG, "UI Toggle Dly");
            
            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                if (param_ptr[VALETON_PARAM_DLY_ENABLE].Value == 0.0f)
                {
                    value = 1.0f;
                }
                else
                {
                    value = 0.0f;
                }
                valeton_params_release_locked_access();

                usb_modify_parameter(VALETON_PARAM_DLY_ENABLE, value); 
            }
        }
        else if (event_object == objects.ui_icon_val_dst)
        {
            ESP_LOGI(TAG, "UI Toggle Dst");
            
            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                if (param_ptr[VALETON_PARAM_DIST_ENABLE].Value == 0.0f)
                {
                    value = 1.0f;
                }
                else
                {
                    value = 0.0f;
                }
                valeton_params_release_locked_access();

                usb_modify_parameter(VALETON_PARAM_DIST_ENABLE, value);   
            }
        }
        else if (event_object == objects.ui_icon_val_amp)
        {   
            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                if (param_ptr[VALETON_PARAM_AMP_ENABLE].Value == 0.0f)
                {
                    value = 1.0f;
                }
                else
                {
                    value = 0.0f;
                }
                valeton_params_release_locked_access();

                usb_modify_parameter(VALETON_PARAM_AMP_ENABLE, value);   
            }
        } 
        else if (event_object == objects.ui_icon_val_cab)
        {   
            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                if (param_ptr[VALETON_PARAM_CAB_ENABLE].Value == 0.0f)
                {
                    value = 1.0f;
                }
                else
                {
                    value = 0.0f;
                }
                valeton_params_release_locked_access();

                usb_modify_parameter(VALETON_PARAM_CAB_ENABLE, value);   
            }
        } 
        else if (event_object == objects.ui_icon_val_tc)
        {   
            if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
            {
                if (param_ptr[VALETON_PARAM_NS_ENABLE].Value == 0.0f)
                {
                    value = 1.0f;
                }
                else
                {
                    value = 0.0f;
                }
                valeton_params_release_locked_access();

                usb_modify_parameter(VALETON_PARAM_NS_ENABLE, value);   
            }
        } 
        else
        {
            ESP_LOGI(TAG, "UI Toggle unknown");
        }
    }
    else if (event_code == LV_EVENT_LONG_PRESSED) 
    {
        // change to settings page and jump to relevant tab
        action_show_settings_page(e);
        valeton_show_settings_tab(e);      
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
void valeton_action_parameter_changed(lv_event_t * e)
{
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI          
    // get the object that was changed
    lv_obj_t* obj = lv_event_get_current_target(e);

    ESP_LOGI(TAG, "Valeton Parameter changed");

    //todo 

#endif // CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI          
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void valeton_update_icon_order(void)
{
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI          
    //todo
#endif    
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint8_t valeton_update_ui_parameters(void)
{
    tModellerParameter* param_ptr;
    __attribute__((unused)) char value_string[20];

#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI      
    
    for (uint16_t param = 0; param < VALETON_PARAM_LAST; param++)
    {                     
        if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
        {
            tModellerParameter* param_entry = &param_ptr[param];

            // debug
            //ESP_LOGI(TAG, "Param %d: val: %02f, min: %02f, max: %02f", param, param_entry->Value, param_entry->Min, param_entry->Max);
            
            switch (param)
            {
                case VALETON_PARAM_NR_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_img_set_src(objects.ui_icon_val_nr, (lv_obj_t*)&img_nr_on);
                        lv_obj_add_state(objects.ui_val_nr_block_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_img_set_src(objects.ui_icon_val_nr, (lv_obj_t*)&img_nr_off);
                        lv_obj_clear_state(objects.ui_val_nr_block_switch, LV_STATE_CHECKED);
                    }                    
                } break;

                case VALETON_PARAM_NR_TYPE:
                {
                    lv_dropdown_set_selected(objects.ui_val_nr_model_dropdown, (int)param_entry->Value);

                    switch ((int)param_entry->Value)
                    {
                        case VALETON_EFFECT_NR_GATE:
                        {
                            // enable param 0, hide the rest
                            lv_obj_clear_flag(objects.ui_val_nr_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_nr_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_nr_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_nr_param0_label, "Threshold");
                            lv_slider_set_range(objects.ui_val_nr_param0_slider, 0, 100);
                            
                            lv_obj_add_flag(objects.ui_val_nr_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_nr_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_nr_param1_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_nr_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_nr_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_nr_param2_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_nr_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_nr_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_nr_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_nr_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_nr_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_nr_param4_value, LV_OBJ_FLAG_HIDDEN);
                        } break;
                    } 
                } break;

                case VALETON_PARAM_NR_PARAM_0:
                {
                    lv_slider_set_value(objects.ui_val_nr_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_nr_param0_value, value_string);
                } break;

                case VALETON_PARAM_NR_PARAM_1: 
                {
                    lv_slider_set_value(objects.ui_val_nr_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_nr_param1_value, value_string);
                } break;

                case VALETON_PARAM_NR_PARAM_2: 
                {
                    lv_slider_set_value(objects.ui_val_nr_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_nr_param2_value, value_string);
                } break;

                case VALETON_PARAM_NR_PARAM_3: 
                {
                    lv_slider_set_value(objects.ui_val_nr_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_nr_param3_value, value_string);
                } break;

                case VALETON_PARAM_NR_PARAM_4:  
                {
                    lv_slider_set_value(objects.ui_val_nr_param4_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_nr_param4_value, value_string);
                } break;

                case VALETON_PARAM_NR_PARAM_5:  // fallthrough
                case VALETON_PARAM_NR_PARAM_6:  // fallthrough
                case VALETON_PARAM_NR_PARAM_7:  
                {
                    // not used
                } break;

                case VALETON_PARAM_PRE_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_img_set_src(objects.ui_icon_val_pre, (lv_obj_t*)&img_pre_on);
                        lv_obj_add_state(objects.ui_val_pre_block_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_img_set_src(objects.ui_icon_val_pre, (lv_obj_t*)&img_pre_off);
                        lv_obj_clear_state(objects.ui_val_pre_block_switch, LV_STATE_CHECKED);
                    }
                } break;

                case VALETON_PARAM_PRE_TYPE:
                {
                    lv_dropdown_set_selected(objects.ui_val_pre_model_dropdown, (int)param_entry->Value);

                    switch ((int)param_entry->Value)
                    {
                        case VALETON_EFFECT_PRE_COMP:
                        {
                            lv_obj_clear_flag(objects.ui_val_pre_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param0_label, "Sustain");
                            lv_slider_set_range(objects.ui_val_pre_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Volume");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 100);
 
                            lv_obj_add_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_pre_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_pre_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_value, LV_OBJ_FLAG_HIDDEN);  
                        } break;

                        case VALETON_EFFECT_PRE_COMP4:
                        {
                            lv_obj_clear_flag(objects.ui_val_pre_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param0_label, "Sustain");
                            lv_slider_set_range(objects.ui_val_pre_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Attack");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Volume");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_pre_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Clip");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 100);

                            lv_obj_add_flag(objects.ui_val_pre_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_value, LV_OBJ_FLAG_HIDDEN);  
                        } break;

                        case VALETON_EFFECT_PRE_BOOST:
                        {
                            lv_obj_clear_flag(objects.ui_val_pre_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_pre_param0_slider, 0, 100);
                            
                            //todo 2 switches here

                            lv_obj_add_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
 
                            lv_obj_add_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_pre_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_pre_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_value, LV_OBJ_FLAG_HIDDEN);  
                        } break;

                        case VALETON_EFFECT_PRE_MICRO_BOOST:
                        {
                            lv_obj_clear_flag(objects.ui_val_pre_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_pre_param0_slider, 0, 100);
                            
                            lv_obj_add_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
 
                            lv_obj_add_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_pre_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_pre_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_value, LV_OBJ_FLAG_HIDDEN);  
                        } break;

                        case VALETON_EFFECT_PRE_B_BOOST:
                        {
                            lv_obj_clear_flag(objects.ui_val_pre_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_pre_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Volume");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Bass");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_pre_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Treble");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 100);

                            lv_obj_add_flag(objects.ui_val_pre_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_value, LV_OBJ_FLAG_HIDDEN);  
                        } break;

                        case VALETON_EFFECT_PRE_TOUCHER:
                        {
                            lv_obj_clear_flag(objects.ui_val_pre_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param0_label, "Sense");
                            lv_slider_set_range(objects.ui_val_pre_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Range");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Q");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_pre_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Mix");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 100);

                            // todo switch here

                            lv_obj_add_flag(objects.ui_val_pre_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_value, LV_OBJ_FLAG_HIDDEN);  
                        } break;

                        case VALETON_EFFECT_PRE_CRIER:
                        {
                            lv_obj_clear_flag(objects.ui_val_pre_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param0_label, "Depth");
                            lv_slider_set_range(objects.ui_val_pre_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Rate");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 10);
 
                            lv_obj_clear_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Volume");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_pre_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Low");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_pre_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Q");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 100);

                            //todo one more slider
                        } break;

                        case VALETON_EFFECT_PRE_OCTA:
                        {
                            lv_obj_clear_flag(objects.ui_val_pre_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param0_label, "Low");
                            lv_slider_set_range(objects.ui_val_pre_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "High");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param2_label, "Dry");
                            lv_slider_set_range(objects.ui_val_pre_param2_slider, 0, 100);

                            lv_obj_add_flag(objects.ui_val_pre_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_pre_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_value, LV_OBJ_FLAG_HIDDEN);  
                        } break;

                        case VALETON_EFFECT_PRE_PITCH:
                        {
                            lv_obj_clear_flag(objects.ui_val_pre_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param0_label, "High");
                            lv_slider_set_range(objects.ui_val_pre_param0_slider, 0, 24);
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Low");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, -24, 0);
 
                            lv_obj_clear_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param2_label, "Dry");
                            lv_slider_set_range(objects.ui_val_pre_param2_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_pre_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param3_label, "H-Vol");
                            lv_slider_set_range(objects.ui_val_pre_param3_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_pre_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_pre_param4_label, "L-Vol");
                            lv_slider_set_range(objects.ui_val_pre_param4_slider, 0, 100);
                        } break;

                        case VALETON_EFFECT_PRE_DETUNE:
                        {
                            lv_obj_clear_flag(objects.ui_val_pre_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param0_label, "Detune");
                            lv_slider_set_range(objects.ui_val_pre_param0_slider, -50, 50);
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Wet");
                            lv_slider_set_range(objects.ui_val_pre_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param2_label, "Dry");
                            lv_slider_set_range(objects.ui_val_pre_param2_slider, 0, 100);

                            lv_obj_add_flag(objects.ui_val_pre_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_pre_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_pre_param4_value, LV_OBJ_FLAG_HIDDEN);  
                        } break;
                    } 
                } break;

                case VALETON_PARAM_PRE_PARAM_0:
                {
                    lv_slider_set_value(objects.ui_val_pre_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_pre_param0_value, value_string);
                } break;

                case VALETON_PARAM_PRE_PARAM_1:
                {
                    lv_slider_set_value(objects.ui_val_pre_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_pre_param1_value, value_string);
                } break;

                case VALETON_PARAM_PRE_PARAM_2:
                {
                    lv_slider_set_value(objects.ui_val_pre_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_pre_param2_value, value_string);
                } break;

                case VALETON_PARAM_PRE_PARAM_3:
                {
                    lv_slider_set_value(objects.ui_val_pre_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_pre_param3_value, value_string);
                } break;

                case VALETON_PARAM_PRE_PARAM_4:
                {
                    lv_slider_set_value(objects.ui_val_pre_param4_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_pre_param4_value, value_string);
                } break;

                case VALETON_PARAM_PRE_PARAM_5:
                {
                    // not used
                } break;
                
                case VALETON_PARAM_DIST_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_img_set_src(objects.ui_icon_val_dst, (lv_obj_t*)&img_dst_on);
                        lv_obj_add_state(objects.ui_val_dst_block_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_img_set_src(objects.ui_icon_val_dst, (lv_obj_t*)&img_dst_off);
                        lv_obj_clear_state(objects.ui_val_dst_block_switch, LV_STATE_CHECKED);
                    }
                } break;
                
                case VALETON_PARAM_DIST_TYPE:
                {
                    lv_dropdown_set_selected(objects.ui_val_dst_model_dropdown, (int)param_entry->Value);

                    switch ((int)param_entry->Value)
                    {
                        case VALETON_EFFECT_DIST_GREEN_OD:      // fallthrough
                        case VALETON_EFFECT_DIST_SUPER_OD:      // fallthrough
                        case VALETON_EFFECT_DIST_SM_DIST:       // fallthrough
                        case VALETON_EFFECT_DIST_LA_CHARGER:
                        {
                            lv_obj_clear_flag(objects.ui_val_dst_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_dst_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_dst_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param1_label, "Tone");
                            lv_slider_set_range(objects.ui_val_dst_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_dst_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param2_label, "Volume");
                            lv_slider_set_range(objects.ui_val_dst_param2_slider, 0, 100);

                            lv_obj_add_flag(objects.ui_val_dst_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_dst_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param4_value, LV_OBJ_FLAG_HIDDEN);  
                        } break;

                        case VALETON_EFFECT_DIST_YELLOW_OD:     // fallthrough
                        case VALETON_EFFECT_DIST_PLUSTORTION:
                        {
                            lv_obj_clear_flag(objects.ui_val_dst_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_dst_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_dst_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param1_label, "Volume");
                            lv_slider_set_range(objects.ui_val_dst_param1_slider, 0, 100);
 
                            lv_obj_add_flag(objects.ui_val_dst_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param2_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_dst_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_dst_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param4_value, LV_OBJ_FLAG_HIDDEN);  
                        } break;
                   
                        case VALETON_EFFECT_DIST_DARKTALE:
                        {
                            lv_obj_clear_flag(objects.ui_val_dst_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_dst_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_dst_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param1_label, "Filter");
                            lv_slider_set_range(objects.ui_val_dst_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_dst_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param2_label, "Volume");
                            lv_slider_set_range(objects.ui_val_dst_param2_slider, 0, 100);

                            lv_obj_add_flag(objects.ui_val_dst_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_dst_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param4_value, LV_OBJ_FLAG_HIDDEN);  
                        } break;
                        
                        case VALETON_EFFECT_DIST_SORA_FUZZ:     // fallthrough
                        case VALETON_EFFECT_DIST_RED_HAZE:
                        {
                            lv_obj_clear_flag(objects.ui_val_dst_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param0_label, "Fuzz");
                            lv_slider_set_range(objects.ui_val_dst_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_dst_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param1_label, "Volume");
                            lv_slider_set_range(objects.ui_val_dst_param1_slider, 0, 100);
 
                            lv_obj_add_flag(objects.ui_val_dst_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param2_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_dst_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_dst_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dst_param4_value, LV_OBJ_FLAG_HIDDEN);  
                        } break;

                        case VALETON_EFFECT_DIST_BASS_OD:
                        {
                            lv_obj_clear_flag(objects.ui_val_dst_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_dst_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_dst_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param1_label, "Blend");
                            lv_slider_set_range(objects.ui_val_dst_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_dst_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param2_label, "Volume");
                            lv_slider_set_range(objects.ui_val_dst_param2_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_dst_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param3_label, "Bass");
                            lv_slider_set_range(objects.ui_val_dst_param3_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_dst_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_dst_param4_label, "Treble");
                            lv_slider_set_range(objects.ui_val_dst_param4_slider, 0, 100);
                        } break;
                    }
                } break;

                case VALETON_PARAM_DIST_PARAM_0:
                {
                    lv_slider_set_value(objects.ui_val_dst_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dst_param0_value, value_string);
                } break;

                case VALETON_PARAM_DIST_PARAM_1:
                {
                    lv_slider_set_value(objects.ui_val_dst_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dst_param1_value, value_string);
                } break;

                case VALETON_PARAM_DIST_PARAM_2:
                {
                    lv_slider_set_value(objects.ui_val_dst_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dst_param2_value, value_string);
                } break;

                case VALETON_PARAM_DIST_PARAM_3:
                {
                    lv_slider_set_value(objects.ui_val_dst_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dst_param3_value, value_string);
                } break;
                
                case VALETON_PARAM_DIST_PARAM_4:
                {
                    lv_slider_set_value(objects.ui_val_dst_param4_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dst_param4_value, value_string);
                } break;

                case VALETON_PARAM_DIST_PARAM_5:    // fallthrough
                case VALETON_PARAM_DIST_PARAM_6:    // fallthrough
                case VALETON_PARAM_DIST_PARAM_7:    
                {
                    // not used
                } break;

                case VALETON_PARAM_AMP_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_img_set_src(objects.ui_icon_val_amp, (lv_obj_t*)&img_amp_on);
                        lv_obj_add_state(objects.ui_val_amp_block_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_img_set_src(objects.ui_icon_val_amp, (lv_obj_t*)&img_amp_off);
                        lv_obj_clear_state(objects.ui_val_amp_block_switch, LV_STATE_CHECKED);
                    }
                } break;

                case VALETON_PARAM_AMP_TYPE:
                {
                    lv_dropdown_set_selected(objects.ui_val_amp_model_dropdown, (int)param_entry->Value);

                    switch ((int)param_entry->Value)
                    {
                        case VALETON_EFFECT_AMP_TWEEDY:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_amp_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Tone");
                            lv_slider_set_range(objects.ui_val_amp_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Volume");
                            lv_slider_set_range(objects.ui_val_amp_param2_slider, 0, 100);

                            lv_obj_add_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                        } break;

                        case VALETON_EFFECT_AMP_BELLMAN_59N:    // fallthrough
                        case VALETON_EFFECT_AMP_MATCH_CL:       // fallthrough
                        case VALETON_EFFECT_AMP_L_STAR_CL:      // fallthrough
                        case VALETON_EFFECT_AMP_UK_45:          // fallthough
                        case VALETON_EFFECT_AMP_UK_800:         // fallthrough
                        case VALETON_EFFECT_AMP_BELLMAN_59B:    // fallthough
                        case VALETON_EFFECT_AMP_SOLO100_OD:     // fallthrough
                        case VALETON_EFFECT_AMP_BAD_KT_OD:      // fallthough
                        case VALETON_EFFECT_AMP_DIZZ_VH:        // fallthrough
                        case VALETON_EFFECT_AMP_DIZZ_VH_PLUS:   // fallthrough
                        case VALETON_EFFECT_AMP_EAGLE_120:      // fallthrough
                        case VALETON_EFFECT_AMP_SOLO100_LD:     // fallthrough
                        case VALETON_EFFECT_AMP_MESS_DUALV:     // fallthrough
                        case VALETON_EFFECT_AMP_MESS_DUALM:     // fallthrough
                        case VALETON_EFFECT_AMP_POWER_LD:       // fallthrough
                        case VALETON_EFFECT_AMP_FLAGMAN_PLUS:   // fallthrough
                        case VALETON_EFFECT_AMP_BOG_REDV:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_amp_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Presence");
                            lv_slider_set_range(objects.ui_val_amp_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Volume");
                            lv_slider_set_range(objects.ui_val_amp_param2_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Bass");
                            lv_slider_set_range(objects.ui_val_amp_param3_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Middle");
                            lv_slider_set_range(objects.ui_val_amp_param4_slider, 0, 100);

                            // todo Treble 
                        } break;

                        case VALETON_EFFECT_AMP_DARK_TWIN:
                        case VALETON_EFFECT_AMP_JUICE_R100:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_amp_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Volume");
                            lv_slider_set_range(objects.ui_val_amp_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Bass");
                            lv_slider_set_range(objects.ui_val_amp_param2_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Middle");
                            lv_slider_set_range(objects.ui_val_amp_param3_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Treble");
                            lv_slider_set_range(objects.ui_val_amp_param4_slider, 0, 100);

                            //todo bright switch
                        } break;

                        case VALETON_EFFECT_AMP_FOXY_30N:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_amp_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Tone Cut");
                            lv_slider_set_range(objects.ui_val_amp_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Volume");
                            lv_slider_set_range(objects.ui_val_amp_param2_slider, 0, 100);

                            lv_obj_add_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  

                            //todo tone cut switch
                        } break;

                        case VALETON_EFFECT_AMP_J_120_CL:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Volume");
                            lv_slider_set_range(objects.ui_val_amp_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Bass");
                            lv_slider_set_range(objects.ui_val_amp_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Middle");
                            lv_slider_set_range(objects.ui_val_amp_param2_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Treble");
                            lv_slider_set_range(objects.ui_val_amp_param3_slider, 0, 100);

                            lv_obj_add_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  

                            //todo bright switch
                        } break;

                        case VALETON_EFFECT_AMP_UK_50JP:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain 1");
                            lv_slider_set_range(objects.ui_val_amp_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Presence");
                            lv_slider_set_range(objects.ui_val_amp_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Volume");
                            lv_slider_set_range(objects.ui_val_amp_param2_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Bass");
                            lv_slider_set_range(objects.ui_val_amp_param3_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Middle");
                            lv_slider_set_range(objects.ui_val_amp_param4_slider, 0, 100);

                            //todo: treble, gain 2
                        } break;

                        case VALETON_EFFECT_AMP_FOXY_30TB:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_amp_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Tone Cut");
                            lv_slider_set_range(objects.ui_val_amp_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Volume");
                            lv_slider_set_range(objects.ui_val_amp_param2_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Bass");
                            lv_slider_set_range(objects.ui_val_amp_param3_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Treble");
                            lv_slider_set_range(objects.ui_val_amp_param4_slider, 0, 100);

                            //todo: add character switch
                        } break;

                        case VALETON_EFFECT_AMP_SUPDUAL_OD:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain 1");
                            lv_slider_set_range(objects.ui_val_amp_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Tone 1");
                            lv_slider_set_range(objects.ui_val_amp_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Gain 2");
                            lv_slider_set_range(objects.ui_val_amp_param2_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Tone 2");
                            lv_slider_set_range(objects.ui_val_amp_param3_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Volume");
                            lv_slider_set_range(objects.ui_val_amp_param4_slider, 0, 100);
                        } break;
                        
                        case VALETON_EFFECT_AMP_Z38_OD:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_amp_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Tone Cut");
                            lv_slider_set_range(objects.ui_val_amp_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Volume");
                            lv_slider_set_range(objects.ui_val_amp_param2_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Bass");
                            lv_slider_set_range(objects.ui_val_amp_param3_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Middle");
                            lv_slider_set_range(objects.ui_val_amp_param4_slider, 0, 100);

                            //todo: treble
                        } break;

                        case VALETON_EFFECT_AMP_EV_51:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_amp_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Volume");
                            lv_slider_set_range(objects.ui_val_amp_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Bass");
                            lv_slider_set_range(objects.ui_val_amp_param2_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Mid");
                            lv_slider_set_range(objects.ui_val_amp_param3_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Treble");
                            lv_slider_set_range(objects.ui_val_amp_param4_slider, 0, 100);

                            // todo presence
                        } break;

                        case VALETON_EFFECT_AMP_CLASSIC_BASS:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_amp_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Bass");
                            lv_slider_set_range(objects.ui_val_amp_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Middle");
                            lv_slider_set_range(objects.ui_val_amp_param2_slider, 0, 100);

                            //todo: middle freq
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Treble");
                            lv_slider_set_range(objects.ui_val_amp_param3_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Volume");
                            lv_slider_set_range(objects.ui_val_amp_param4_slider, 0, 100);
                        } break;
                        
                        case VALETON_EFFECT_AMP_FOXY_BASS:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Volume");
                            lv_slider_set_range(objects.ui_val_amp_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Bass");
                            lv_slider_set_range(objects.ui_val_amp_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Treble");
                            lv_slider_set_range(objects.ui_val_amp_param2_slider, 0, 100);

                            lv_obj_add_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  

                        } break;

                        case VALETON_EFFECT_AMP_MESS_BASS:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain");
                            lv_slider_set_range(objects.ui_val_amp_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Volume");
                            lv_slider_set_range(objects.ui_val_amp_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Bass");
                            lv_slider_set_range(objects.ui_val_amp_param2_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Mid");
                            lv_slider_set_range(objects.ui_val_amp_param3_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Treble");
                            lv_slider_set_range(objects.ui_val_amp_param4_slider, 0, 100);
                        } break;

                        case VALETON_EFFECT_AMP_AC_PRE1:    // fallthrough
                        case VALETON_EFFECT_AMP_AC_PRE2:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Volume");
                            lv_slider_set_range(objects.ui_val_amp_param0_slider, 0, 100);
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Tone");
                            lv_slider_set_range(objects.ui_val_amp_param1_slider, 0, 100);
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Balance");
                            lv_slider_set_range(objects.ui_val_amp_param2_slider, 0, 100);

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "EQ Freq");
                            lv_slider_set_range(objects.ui_val_amp_param3_slider, 0, 100);

                            // todo: EQ Q
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "EQ Gain");
                            lv_slider_set_range(objects.ui_val_amp_param4_slider, 0, 100);
                        } break;
                    }
                } break;

                case VALETON_PARAM_AMP_PARAM_0:
                {
                    lv_slider_set_value(objects.ui_val_amp_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_amp_param0_value, value_string);
                } break;
                
                case VALETON_PARAM_AMP_PARAM_1:
                {
                    lv_slider_set_value(objects.ui_val_amp_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_amp_param1_value, value_string);
                } break;

                case VALETON_PARAM_AMP_PARAM_2:
                {
                    lv_slider_set_value(objects.ui_val_amp_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_amp_param2_value, value_string);
                } break;

                case VALETON_PARAM_AMP_PARAM_3:
                {
                    lv_slider_set_value(objects.ui_val_amp_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_amp_param3_value, value_string);
                } break;

                case VALETON_PARAM_AMP_PARAM_4:
                {
                    lv_slider_set_value(objects.ui_val_amp_param4_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_amp_param4_value, value_string);
                } break;

                case VALETON_PARAM_AMP_PARAM_5:     // fallthrough
                case VALETON_PARAM_AMP_PARAM_6:     // fallthrough
                case VALETON_PARAM_AMP_PARAM_7:     
                {
                    // not used
                } break;

                case VALETON_PARAM_CAB_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_img_set_src(objects.ui_icon_val_cab, (lv_obj_t*)&img_cab_on);
                        lv_obj_add_state(objects.ui_val_cab_block_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_img_set_src(objects.ui_icon_val_cab, (lv_obj_t*)&img_cab_off);
                        lv_obj_clear_state(objects.ui_val_cab_block_switch, LV_STATE_CHECKED);
                    }
                } break;

                case VALETON_PARAM_CAB_TYPE:
                {
                    lv_dropdown_set_selected(objects.ui_val_cab_model_dropdown, (int)param_entry->Value);

                    // all cabs have volume only
                    lv_obj_clear_flag(objects.ui_val_cab_param0_label, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_val_cab_param0_slider, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_val_cab_param0_value, LV_OBJ_FLAG_HIDDEN);
                    lv_label_set_text(objects.ui_val_cab_param0_label, "Volume");
                    lv_slider_set_range(objects.ui_val_cab_param0_slider, 0, 100);
                    
                    lv_obj_add_flag(objects.ui_val_cab_param1_label, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(objects.ui_val_cab_param1_slider, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(objects.ui_val_cab_param1_value, LV_OBJ_FLAG_HIDDEN);

                    lv_obj_add_flag(objects.ui_val_cab_param2_label, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(objects.ui_val_cab_param2_slider, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(objects.ui_val_cab_param2_value, LV_OBJ_FLAG_HIDDEN);

                    lv_obj_add_flag(objects.ui_val_cab_param3_label, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(objects.ui_val_cab_param3_slider, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(objects.ui_val_cab_param3_value, LV_OBJ_FLAG_HIDDEN);
                    
                    lv_obj_add_flag(objects.ui_val_cab_param4_label, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(objects.ui_val_cab_param4_slider, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(objects.ui_val_cab_param4_value, LV_OBJ_FLAG_HIDDEN);  
                } break;

                case VALETON_PARAM_CAB_PARAM_0:
                {
                    lv_slider_set_value(objects.ui_val_cab_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_cab_param0_value, value_string);
                } break;

                case VALETON_PARAM_CAB_PARAM_1:     
                {
                    lv_slider_set_value(objects.ui_val_cab_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_cab_param1_value, value_string);
                } break;

                case VALETON_PARAM_CAB_PARAM_2: 
                {
                    lv_slider_set_value(objects.ui_val_cab_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_cab_param2_value, value_string);
                } break;

                case VALETON_PARAM_CAB_PARAM_3:
                {
                    lv_slider_set_value(objects.ui_val_cab_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_cab_param3_value, value_string);
                } break;

                case VALETON_PARAM_CAB_PARAM_4:     
                {
                    lv_slider_set_value(objects.ui_val_cab_param4_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_cab_param4_value, value_string);
                } break;

                case VALETON_PARAM_CAB_PARAM_5:     // fallthrough
                case VALETON_PARAM_CAB_PARAM_6:     // fallthrough
                case VALETON_PARAM_CAB_PARAM_7:     
                {
                    // not used
                } break;

                case VALETON_PARAM_EQ_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_img_set_src(objects.ui_icon_val_eq, (lv_obj_t*)&img_eq_on);
                        lv_obj_add_state(objects.ui_val_eq_block_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_img_set_src(objects.ui_icon_val_eq, (lv_obj_t*)&img_eq_off);
                        lv_obj_clear_state(objects.ui_val_eq_block_switch, LV_STATE_CHECKED);
                    }
                } break;

                case VALETON_PARAM_EQ_TYPE:
                {
                    lv_dropdown_set_selected(objects.ui_val_eq_model_dropdown, (int)param_entry->Value);

                    switch ((int)param_entry->Value)
                    {
                        case VALETON_EFFECT_EQ_GUITAR_EQ1:
                        {

                        } break;

                        case VALETON_EFFECT_EQ_GUITAR_EQ2:
                        {

                        } break;

                        case VALETON_EFFECT_EQ_BASS_EQ1:
                        {

                        } break;

                        case VALETON_EFFECT_EQ_BASS_EQ2:
                        {

                        } break;

                        case VALETON_EFFECT_EQ_MESS_EQ:
                        {

                        } break;
                    }
                } break;

                case VALETON_PARAM_EQ_PARAM_0:
                {
                    lv_slider_set_value(objects.ui_val_eq_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_eq_param0_value, value_string);
                } break;

                case VALETON_PARAM_EQ_PARAM_1:
                {
                    lv_slider_set_value(objects.ui_val_eq_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_eq_param1_value, value_string);
                } break;

                case VALETON_PARAM_EQ_PARAM_2:
                {
                    lv_slider_set_value(objects.ui_val_eq_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_eq_param2_value, value_string);
                } break;

                case VALETON_PARAM_EQ_PARAM_3:
                {
                    lv_slider_set_value(objects.ui_val_eq_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_eq_param3_value, value_string);
                } break;

                case VALETON_PARAM_EQ_PARAM_4:
                {
                    lv_slider_set_value(objects.ui_val_eq_param4_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_eq_param4_value, value_string);
                } break;

                case VALETON_PARAM_EQ_PARAM_5:      // fallthrough
                case VALETON_PARAM_EQ_PARAM_6:      // fallthrough
                case VALETON_PARAM_EQ_PARAM_7:      
                {
                    // not used
                } break;

                case VALETON_PARAM_MOD_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_img_set_src(objects.ui_icon_val_mod, (lv_obj_t*)&img_mod_on);
                        lv_obj_add_state(objects.ui_val_mod_block_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_img_set_src(objects.ui_icon_val_mod, (lv_obj_t*)&img_mod_off);
                        lv_obj_clear_state(objects.ui_val_mod_block_switch, LV_STATE_CHECKED);
                    }
                } break;

                case VALETON_PARAM_MOD_TYPE:
                {
                    lv_dropdown_set_selected(objects.ui_val_mod_model_dropdown, (int)param_entry->Value);

                    switch ((int)param_entry->Value)
                    {
                        case VALETON_EFFECT_MOD_A_CHORUS:
                        {

                        } break;

                        // VALETON_EFFECT_MOD_B_CHORUS,
                        // VALETON_EFFECT_MOD_JET,
                        // VALETON_EFFECT_MOD_N_JET,
                        // VALETON_EFFECT_MOD_O_PHASE,
                        // VALETON_EFFECT_MOD_M_VIBE,
                        // VALETON_EFFECT_MOD_V_ROTO,
                        // VALETON_EFFECT_MOD_VIBRATO,
                        // VALETON_EFFECT_MOD_O_TREM,
                        // VALETON_EFFECT_MOD_SINE_TREM,
                        // VALETON_EFFECT_MOD_BIAS_TREM
                    }
                } break;

                case VALETON_PARAM_MOD_PARAM_0:
                {
                    lv_slider_set_value(objects.ui_val_mod_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_mod_param0_value, value_string);
                } break;
                
                case VALETON_PARAM_MOD_PARAM_1:
                {
                    lv_slider_set_value(objects.ui_val_mod_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_mod_param1_value, value_string);
                } break;

                case VALETON_PARAM_MOD_PARAM_2:
                {
                    lv_slider_set_value(objects.ui_val_mod_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_mod_param2_value, value_string);
                } break;

                case VALETON_PARAM_MOD_PARAM_3:
                {
                    lv_slider_set_value(objects.ui_val_mod_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_mod_param3_value, value_string);
                } break;

                case VALETON_PARAM_MOD_PARAM_4:
                {
                    lv_slider_set_value(objects.ui_val_mod_param4_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_mod_param4_value, value_string);
                } break;

                case VALETON_PARAM_MOD_PARAM_5:     // fallthrough
                case VALETON_PARAM_MOD_PARAM_6:     // fallthrough
                case VALETON_PARAM_MOD_PARAM_7:     
                {
                    // not used
                } break;

                case VALETON_PARAM_DLY_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_img_set_src(objects.ui_icon_val_dly, (lv_obj_t*)&img_dly_on);
                        lv_obj_add_state(objects.ui_val_dly_block_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_img_set_src(objects.ui_icon_val_dly, (lv_obj_t*)&img_dly_off);
                        lv_obj_clear_state(objects.ui_val_dly_block_switch, LV_STATE_CHECKED);
                    }
                } break;

                case VALETON_PARAM_DLY_TYPE:
                {
                    lv_dropdown_set_selected(objects.ui_val_dly_model_dropdown, (int)param_entry->Value);

                    switch ((int)param_entry->Value)
                    {
                        case VALETON_EFFECT_DLY_PURE:
                        {

                        } break;

                        // VALETON_EFFECT_DLY_ANALOG,
                        // VALETON_EFFECT_DLY_SLAPBACK,
                        // VALETON_EFFECT_DLY_SWEET_ECHO,
                        // VALETON_EFFECT_DLY_TAPE,
                        // VALETON_EFFECT_DLY_TUBE,
                        // VALETON_EFFECT_DLY_REV_ECHO,
                        // VALETON_EFFECT_DLY_RING_ECHO,
                        // VALETON_EFFECT_DLY_SWEEP_ECHO,
                        // VALETON_EFFECT_DLY_PING_PONG
                    }
                } break;

                case VALETON_PARAM_DLY_PARAM_0:
                {
                    lv_slider_set_value(objects.ui_val_dly_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dly_param0_value, value_string);
                } break;

                case VALETON_PARAM_DLY_PARAM_1:
                {
                    lv_slider_set_value(objects.ui_val_dly_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dly_param1_value, value_string);
                } break;

                case VALETON_PARAM_DLY_PARAM_2:
                {
                    lv_slider_set_value(objects.ui_val_dly_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dly_param2_value, value_string);
                } break;

                case VALETON_PARAM_DLY_PARAM_3:
                {
                    lv_slider_set_value(objects.ui_val_dly_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dly_param3_value, value_string);
                } break;

                case VALETON_PARAM_DLY_PARAM_4:
                {
                    lv_slider_set_value(objects.ui_val_dly_param4_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dly_param4_value, value_string);
                } break;

                case VALETON_PARAM_DLY_PARAM_5:     // fallthrough
                case VALETON_PARAM_DLY_PARAM_6:     // fallthrough
                case VALETON_PARAM_DLY_PARAM_7:     
                {
                    // not used
                } break;

                case VALETON_PARAM_RVB_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_img_set_src(objects.ui_icon_val_rvb, (lv_obj_t*)&img_rvb_on);
                        lv_obj_add_state(objects.ui_val_rvb_block_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_img_set_src(objects.ui_icon_val_rvb, (lv_obj_t*)&img_rvb_off);
                        lv_obj_clear_state(objects.ui_val_rvb_block_switch, LV_STATE_CHECKED);
                    }
                } break;

                case VALETON_PARAM_RVB_TYPE:
                {
                    lv_dropdown_set_selected(objects.ui_val_rvb_model_dropdown, (int)param_entry->Value);

                    switch ((int)param_entry->Value)
                    {
                        case VALETON_EFFECT_RVB_AIR:
                        {

                        } break;

                        // VALETON_EFFECT_RVB_ROOM,
                        // VALETON_EFFECT_RVB_HALL,
                        // VALETON_EFFECT_RVB_CHURCH,
                        // VALETON_EFFECT_RVB_PLATE_L,
                        // VALETON_EFFECT_RVB_PLATE,
                        // VALETON_EFFECT_RVB_SPRING,
                        // VALETON_EFFECT_RVB_N_STAR,
                        // VALETON_EFFECT_RVB_DEEPSEA,
                        // VALETON_EFFECT_RVB_SWEET_SPACE
                    }
                } break;

                case VALETON_PARAM_RVB_PARAM_0:
                {
                    lv_slider_set_value(objects.ui_val_rvb_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_rvb_param0_value, value_string);
                } break;
                
                case VALETON_PARAM_RVB_PARAM_1:
                {
                    lv_slider_set_value(objects.ui_val_rvb_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_rvb_param1_value, value_string);
                } break;                
                
                case VALETON_PARAM_RVB_PARAM_2:
                {
                    lv_slider_set_value(objects.ui_val_rvb_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_rvb_param2_value, value_string);
                } break;
                
                case VALETON_PARAM_RVB_PARAM_3:
                {
                    lv_slider_set_value(objects.ui_val_rvb_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_rvb_param3_value, value_string);
                } break;
                
                case VALETON_PARAM_RVB_PARAM_4:
                {
                    lv_slider_set_value(objects.ui_val_rvb_param4_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_rvb_param4_value, value_string);
                } break;                

                case VALETON_PARAM_RVB_PARAM_5:     // fallthrough
                case VALETON_PARAM_RVB_PARAM_6:     // fallthrough
                case VALETON_PARAM_RVB_PARAM_7:     
                {
                    // not used
                } break;

                case VALETON_PARAM_NS_ENABLE:
                {
                    if (param_entry->Value)
                    {
                        lv_img_set_src(objects.ui_icon_val_tc, (lv_obj_t*)&img_tc_on);
                        lv_obj_add_state(objects.ui_val_ns_block_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_img_set_src(objects.ui_icon_val_tc, (lv_obj_t*)&img_tc_off);
                        lv_obj_clear_state(objects.ui_val_ns_block_switch, LV_STATE_CHECKED);
                    }
                } break;

                case VALETON_PARAM_NS_TYPE:
                {
                    lv_dropdown_set_selected(objects.ui_val_ns_model_dropdown, (int)param_entry->Value);

                    //switch ((int)param_entry->Value)
                    //{
                    // todo
                    //}
                } break;

                case VALETON_PARAM_NS_PARAM_0:
                {
                    lv_slider_set_value(objects.ui_val_ns_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_ns_param0_value, value_string);
                } break;

                case VALETON_PARAM_NS_PARAM_1:
                {
                    lv_slider_set_value(objects.ui_val_ns_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_ns_param1_value, value_string);
                } break;                
                
                case VALETON_PARAM_NS_PARAM_2:
                {
                    lv_slider_set_value(objects.ui_val_ns_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_ns_param2_value, value_string);
                } break;                
                
                case VALETON_PARAM_NS_PARAM_3:
                {
                    lv_slider_set_value(objects.ui_val_ns_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_ns_param3_value, value_string);
                } break;
                
                case VALETON_PARAM_NS_PARAM_4:
                {
                    lv_slider_set_value(objects.ui_val_ns_param4_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_ns_param4_value, value_string);
                } break;

                case VALETON_PARAM_NS_PARAM_5:      // fallthrough
                case VALETON_PARAM_NS_PARAM_6:      // fallthrough
                case VALETON_PARAM_NS_PARAM_7:      // fallthrough
                case VALETON_PARAM_NS_PARAM_8:      // fallthrough
                case VALETON_PARAM_NS_PARAM_9:      // fallthrough
                case VALETON_PARAM_NS_PARAM_10:
                {
                    // not used
                } break;
            }

            valeton_params_release_locked_access();
        }               
    }
#endif 

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169 \
    || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169TOUCH \
    || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_M5ATOMS3R \
    || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_LILYGO_TDISPLAY_S3 \
    || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_19TOUCH

    for (uint16_t param = 0; param < VALETON_PARAM_LAST; param++)
    {                     
        if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
        {
            tModellerParameter* param_entry = &param_ptr[param];

            // debug
            //ESP_LOGI(TAG, "Param %d: val: %02f, min: %02f, max: %02f", param, param_entry->Value, param_entry->Min, param_entry->Max);

            //todo

            valeton_params_release_locked_access();
        }               
    }
#endif

    return 1;
}
