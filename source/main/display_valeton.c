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

    // Block Switches
    if (obj == objects.ui_val_nr_block_switch)
    {
        usb_modify_parameter(VALETON_PARAM_NR_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    } 
    else if (obj == objects.ui_val_pre_block_switch)
    {
        usb_modify_parameter(VALETON_PARAM_PRE_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    } 
    else if (obj == objects.ui_val_dst_block_switch)
    {
        usb_modify_parameter(VALETON_PARAM_DIST_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    } 
    else if (obj == objects.ui_val_amp_block_switch)
    {
        usb_modify_parameter(VALETON_PARAM_AMP_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    } 
    else if (obj == objects.ui_val_cab_block_switch)
    {
        usb_modify_parameter(VALETON_PARAM_CAB_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    } 
    else if (obj == objects.ui_val_eq_block_switch)
    {
        usb_modify_parameter(VALETON_PARAM_EQ_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    } 
    else if (obj == objects.ui_val_mod_block_switch)
    {
        usb_modify_parameter(VALETON_PARAM_MOD_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    } 
    else if (obj == objects.ui_val_dly_block_switch)
    {
        usb_modify_parameter(VALETON_PARAM_DLY_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    } 
    else if (obj == objects.ui_val_rvb_block_switch)
    {
        usb_modify_parameter(VALETON_PARAM_RVB_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    } 
    else if (obj == objects.ui_val_ns_block_switch)
    {
        usb_modify_parameter(VALETON_PARAM_NS_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    } 
    // Models
    else if (obj == objects.ui_val_nr_model_dropdown)
    {
        usb_modify_parameter(VALETON_PARAM_NR_TYPE, lv_dropdown_get_selected(obj));
    }
    else if (obj == objects.ui_val_pre_model_dropdown)
    {
        usb_modify_parameter(VALETON_PARAM_PRE_TYPE, lv_dropdown_get_selected(obj));
    }
    else if (obj == objects.ui_val_dst_model_dropdown)
    {
        usb_modify_parameter(VALETON_PARAM_DIST_TYPE, lv_dropdown_get_selected(obj));
    }
    else if (obj == objects.ui_val_amp_model_dropdown)
    {
        usb_modify_parameter(VALETON_PARAM_AMP_TYPE, lv_dropdown_get_selected(obj));
    }
    else if (obj == objects.ui_val_cab_model_dropdown)
    {
        usb_modify_parameter(VALETON_PARAM_CAB_TYPE, lv_dropdown_get_selected(obj));
    }
    else if (obj == objects.ui_val_eq_model_dropdown)
    {
        usb_modify_parameter(VALETON_PARAM_EQ_TYPE, lv_dropdown_get_selected(obj));
    }
    else if (obj == objects.ui_val_mod_model_dropdown)
    {
        usb_modify_parameter(VALETON_PARAM_MOD_TYPE, lv_dropdown_get_selected(obj));
    }
    else if (obj == objects.ui_val_dly_model_dropdown)
    {
        usb_modify_parameter(VALETON_PARAM_DLY_TYPE, lv_dropdown_get_selected(obj));
    }
    else if (obj == objects.ui_val_rvb_model_dropdown)
    {
        usb_modify_parameter(VALETON_PARAM_RVB_TYPE, lv_dropdown_get_selected(obj));
    }
    else if (obj == objects.ui_val_ns_model_dropdown)
    {
        usb_modify_parameter(VALETON_PARAM_NS_TYPE, lv_dropdown_get_selected(obj));
    }
    // NR params
    else if (obj == objects.ui_val_nr_param0_slider)
    {
        usb_modify_parameter(VALETON_PARAM_NR_PARAM_0, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_nr_param1_slider)
    {
        usb_modify_parameter(VALETON_PARAM_NR_PARAM_1, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_nr_param2_slider)
    {
        usb_modify_parameter(VALETON_PARAM_NR_PARAM_2, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_nr_param3_slider)
    {
        usb_modify_parameter(VALETON_PARAM_NR_PARAM_3, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_nr_param4_slider)
    {
        usb_modify_parameter(VALETON_PARAM_NR_PARAM_4, ((float)lv_slider_get_value(obj)));
    }
    // Pre block
    else if (obj == objects.ui_val_pre_param0_slider)
    {
        usb_modify_parameter(VALETON_PARAM_PRE_PARAM_0, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_pre_param1_slider)
    {
        usb_modify_parameter(VALETON_PARAM_PRE_PARAM_1, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_pre_param2_slider)
    {
        usb_modify_parameter(VALETON_PARAM_PRE_PARAM_2, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_pre_param3_slider)
    {
        usb_modify_parameter(VALETON_PARAM_PRE_PARAM_3, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_pre_param4_slider)
    {
        usb_modify_parameter(VALETON_PARAM_PRE_PARAM_4, ((float)lv_slider_get_value(obj)));
    }
    // Dst block
    else if (obj == objects.ui_val_dst_param0_slider)
    {
        usb_modify_parameter(VALETON_PARAM_DIST_PARAM_0, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_dst_param1_slider)
    {
        usb_modify_parameter(VALETON_PARAM_DIST_PARAM_1, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_dst_param2_slider)
    {
        usb_modify_parameter(VALETON_PARAM_DIST_PARAM_2, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_dst_param3_slider)
    {
        usb_modify_parameter(VALETON_PARAM_DIST_PARAM_3, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_dst_param4_slider)
    {
        usb_modify_parameter(VALETON_PARAM_DIST_PARAM_4, ((float)lv_slider_get_value(obj)));
    }
    // Amp block
    else if (obj == objects.ui_val_amp_param0_slider)
    {
        usb_modify_parameter(VALETON_PARAM_AMP_PARAM_0, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_amp_param1_slider)
    {
        usb_modify_parameter(VALETON_PARAM_AMP_PARAM_1, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_amp_param2_slider)
    {
        usb_modify_parameter(VALETON_PARAM_AMP_PARAM_2, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_amp_param3_slider)
    {
        usb_modify_parameter(VALETON_PARAM_AMP_PARAM_3, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_amp_param4_slider)
    {
        usb_modify_parameter(VALETON_PARAM_AMP_PARAM_4, ((float)lv_slider_get_value(obj)));
    }
    // Cab block
    else if (obj == objects.ui_val_cab_param0_slider)
    {
        usb_modify_parameter(VALETON_PARAM_CAB_PARAM_0, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_cab_param1_slider)
    {
        usb_modify_parameter(VALETON_PARAM_CAB_PARAM_1, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_cab_param2_slider)
    {
        usb_modify_parameter(VALETON_PARAM_CAB_PARAM_2, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_cab_param3_slider)
    {
        usb_modify_parameter(VALETON_PARAM_CAB_PARAM_3, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_cab_param4_slider)
    {
        usb_modify_parameter(VALETON_PARAM_CAB_PARAM_4, ((float)lv_slider_get_value(obj)));
    }
    // EQ block
    else if (obj == objects.ui_val_eq_param0_slider)
    {
        usb_modify_parameter(VALETON_PARAM_EQ_PARAM_0, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_eq_param1_slider)
    {
        usb_modify_parameter(VALETON_PARAM_EQ_PARAM_1, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_eq_param2_slider)
    {
        usb_modify_parameter(VALETON_PARAM_EQ_PARAM_2, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_eq_param3_slider)
    {
        usb_modify_parameter(VALETON_PARAM_EQ_PARAM_3, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_eq_param4_slider)
    {
        usb_modify_parameter(VALETON_PARAM_EQ_PARAM_4, ((float)lv_slider_get_value(obj)));
    }
    // Mod block
    else if (obj == objects.ui_val_mod_param0_slider)
    {
        usb_modify_parameter(VALETON_PARAM_MOD_PARAM_0, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_mod_param1_slider)
    {
        usb_modify_parameter(VALETON_PARAM_MOD_PARAM_1, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_mod_param2_slider)
    {
        usb_modify_parameter(VALETON_PARAM_MOD_PARAM_2, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_mod_param3_slider)
    {
        usb_modify_parameter(VALETON_PARAM_MOD_PARAM_3, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_mod_param4_slider)
    {
        usb_modify_parameter(VALETON_PARAM_MOD_PARAM_4, ((float)lv_slider_get_value(obj)));
    }
    // Dly block
    else if (obj == objects.ui_val_dly_param0_slider)
    {
        usb_modify_parameter(VALETON_PARAM_DLY_PARAM_0, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_dly_param1_slider)
    {
        usb_modify_parameter(VALETON_PARAM_DLY_PARAM_1, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_dly_param2_slider)
    {
        usb_modify_parameter(VALETON_PARAM_DLY_PARAM_2, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_dly_param3_slider)
    {
        usb_modify_parameter(VALETON_PARAM_DLY_PARAM_3, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_dly_param4_slider)
    {
        usb_modify_parameter(VALETON_PARAM_DLY_PARAM_4, ((float)lv_slider_get_value(obj)));
    }
    // Rvb block
    else if (obj == objects.ui_val_rvb_param0_slider)
    {
        usb_modify_parameter(VALETON_PARAM_RVB_PARAM_0, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_rvb_param1_slider)
    {
        usb_modify_parameter(VALETON_PARAM_RVB_PARAM_1, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_rvb_param2_slider)
    {
        usb_modify_parameter(VALETON_PARAM_RVB_PARAM_2, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_rvb_param3_slider)
    {
        usb_modify_parameter(VALETON_PARAM_RVB_PARAM_3, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_rvb_param4_slider)
    {
        usb_modify_parameter(VALETON_PARAM_RVB_PARAM_4, ((float)lv_slider_get_value(obj)));
    }
    // NS block
    else if (obj == objects.ui_val_ns_param0_slider)
    {
        usb_modify_parameter(VALETON_PARAM_NS_PARAM_0, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_ns_param1_slider)
    {
        usb_modify_parameter(VALETON_PARAM_NS_PARAM_1, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_ns_param2_slider)
    {
        usb_modify_parameter(VALETON_PARAM_NS_PARAM_2, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_ns_param3_slider)
    {
        usb_modify_parameter(VALETON_PARAM_NS_PARAM_3, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_ns_param4_slider)
    {
        usb_modify_parameter(VALETON_PARAM_NS_PARAM_4, ((float)lv_slider_get_value(obj)));
    }
    // Globals
    else if (obj == objects.ui_val_glob_no_cab_switch)
    {
        usb_modify_parameter(VALETON_GLOBAL_CABSIM_BYPASS, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    } 
    else if (obj == objects.ui_val_glob_input_level_slider)
    {
        usb_modify_parameter(VALETON_GLOBAL_INPUT_TRIM, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_glob_master_vol_slider)
    {
        usb_modify_parameter(VALETON_GLOBAL_MASTER_VOLUME, ((float)lv_slider_get_value(obj)));
    }
    else if (obj == objects.ui_val_patch_vol_slider)
    {
        usb_modify_parameter(VALETON_PARAM_PATCH_VOLUME, ((float)lv_slider_get_value(obj)));
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
void valeton_update_icon_order(void)
{
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI          
    tModellerParameter* param_ptr;
    lv_obj_t* icons[VALETON_EFFECT_BLOCK_LAST];
    uint8_t index = 0;
    int16_t offsets[VALETON_EFFECT_BLOCK_LAST];

    ESP_LOGI(TAG, "Update effect icon order");

    if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
    {
        // build array with lvgl object in the current effect order
        for (uint32_t loop = VALETON_PARAM_EFFECT_SLOT_0; loop <= VALETON_PARAM_EFFECT_SLOT_9; loop++)
        {
            switch ((int)param_ptr[loop].Value)
            {
                case VALETON_EFFECT_BLOCK_NR:
                {
                    icons[index] = objects.ui_icon_val_nr;
                } break;

                case VALETON_EFFECT_BLOCK_PRE:
                {
                    icons[index] = objects.ui_icon_val_pre;
                } break;

                case VALETON_EFFECT_BLOCK_DST:
                {
                    icons[index] = objects.ui_icon_val_dst;
                } break;

                case VALETON_EFFECT_BLOCK_AMP:
                {
                    icons[index] = objects.ui_icon_val_amp;
                } break;

                case VALETON_EFFECT_BLOCK_CAB:
                {
                    icons[index] = objects.ui_icon_val_cab;
                } break;

                case VALETON_EFFECT_BLOCK_EQ:
                {
                    icons[index] = objects.ui_icon_val_eq;
                } break;

                case VALETON_EFFECT_BLOCK_MOD:
                {
                    icons[index] = objects.ui_icon_val_mod;
                } break;
                
                case VALETON_EFFECT_BLOCK_DLY:
                {
                    icons[index] = objects.ui_icon_val_dly;
                } break;

                case VALETON_EFFECT_BLOCK_RVB:
                {
                    icons[index] = objects.ui_icon_val_rvb;
                } break;

                case VALETON_EFFECT_BLOCK_NS:
                {
                    icons[index] = objects.ui_icon_val_tc;
                } break;
            }
            
            index++;
        }

        valeton_params_release_locked_access();
        
        platform_get_icon_coords(offsets, sizeof(offsets) / sizeof(int16_t));

        // update X pos of each to set the correct order
        for (uint8_t i = 0; i < VALETON_EFFECT_BLOCK_LAST; i++)
        {
            lv_obj_t* icon = icons[i];
            int16_t offset = offsets[i];
            lv_obj_set_x(icon, offset);
        }
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
uint8_t valeton_update_ui_parameters(void)
{
#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY           
    tModellerParameter* param_ptr;
    __attribute__((unused)) char value_string[20];

#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI      
    
    for (uint16_t param = 0; param < VALETON_GLOBAL_LAST; param++)
    {                     
        if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
        {
            tModellerParameter* param_entry = &param_ptr[param];

            // debug
            //ESP_LOGI(TAG, "Param %d: val: %02f, min: %02f, max: %02f", param, param_entry->Value, param_entry->Min, param_entry->Max);
            
            switch (param)
            {
                case VALETON_PARAM_PATCH_VOLUME:
                {
                    lv_slider_set_value(objects.ui_val_patch_vol_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_patch_vol_value, value_string);
                } break;

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
                    lv_slider_set_range(objects.ui_val_nr_param0_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_nr_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_nr_param0_value, value_string);
                } break;

                case VALETON_PARAM_NR_PARAM_1: 
                {
                    lv_slider_set_range(objects.ui_val_nr_param1_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_nr_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_nr_param1_value, value_string);
                } break;

                case VALETON_PARAM_NR_PARAM_2: 
                {
                    lv_slider_set_range(objects.ui_val_nr_param2_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_nr_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_nr_param2_value, value_string);
                } break;

                case VALETON_PARAM_NR_PARAM_3: 
                {
                    lv_slider_set_range(objects.ui_val_nr_param3_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_nr_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_nr_param3_value, value_string);
                } break;

                case VALETON_PARAM_NR_PARAM_4:  
                {
                    lv_slider_set_range(objects.ui_val_nr_param4_slider, param_entry->Min, param_entry->Max);
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
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Volume");
 
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
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Attack");
 
                            lv_obj_clear_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param2_label, "Volume");

                            lv_obj_clear_flag(objects.ui_val_pre_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param3_label, "Clip");

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
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Volume");
 
                            lv_obj_clear_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param2_label, "Bass");

                            lv_obj_clear_flag(objects.ui_val_pre_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param3_label, "Treble");

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
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Range");
 
                            lv_obj_clear_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param2_label, "Q");

                            lv_obj_clear_flag(objects.ui_val_pre_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param3_label, "Mix");

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
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Rate");
 
                            lv_obj_clear_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param2_label, "Volume");

                            lv_obj_clear_flag(objects.ui_val_pre_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param3_label, "Low");

                            lv_obj_clear_flag(objects.ui_val_pre_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_pre_param4_label, "Q");

                            //todo one more slider
                        } break;

                        case VALETON_EFFECT_PRE_OCTA:
                        {
                            lv_obj_clear_flag(objects.ui_val_pre_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param0_label, "Low");
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "High");
 
                            lv_obj_clear_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param2_label, "Dry");

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
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Low");
 
                            lv_obj_clear_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param2_label, "Dry");

                            lv_obj_clear_flag(objects.ui_val_pre_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param3_label, "H-Vol");

                            lv_obj_clear_flag(objects.ui_val_pre_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_pre_param4_label, "L-Vol");
                        } break;

                        case VALETON_EFFECT_PRE_DETUNE:
                        {
                            lv_obj_clear_flag(objects.ui_val_pre_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param0_label, "Detune");
                            
                            lv_obj_clear_flag(objects.ui_val_pre_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param1_label, "Wet");
 
                            lv_obj_clear_flag(objects.ui_val_pre_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_pre_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_pre_param2_label, "Dry");

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
                    lv_slider_set_range(objects.ui_val_pre_param0_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_pre_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_pre_param0_value, value_string);
                } break;

                case VALETON_PARAM_PRE_PARAM_1:
                {
                    lv_slider_set_range(objects.ui_val_pre_param1_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_pre_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_pre_param1_value, value_string);
                } break;

                case VALETON_PARAM_PRE_PARAM_2:
                {
                    lv_slider_set_range(objects.ui_val_pre_param2_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_pre_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_pre_param2_value, value_string);
                } break;

                case VALETON_PARAM_PRE_PARAM_3:
                {
                    lv_slider_set_range(objects.ui_val_pre_param3_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_pre_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_pre_param3_value, value_string);
                } break;

                case VALETON_PARAM_PRE_PARAM_4:
                {
                    lv_slider_set_range(objects.ui_val_pre_param4_slider, param_entry->Min, param_entry->Max);
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
                            
                            lv_obj_clear_flag(objects.ui_val_dst_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param1_label, "Tone");
 
                            lv_obj_clear_flag(objects.ui_val_dst_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param2_label, "Volume");

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
                            
                            lv_obj_clear_flag(objects.ui_val_dst_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param1_label, "Volume");
 
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
                            
                            lv_obj_clear_flag(objects.ui_val_dst_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param1_label, "Filter");
 
                            lv_obj_clear_flag(objects.ui_val_dst_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param2_label, "Volume");

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
                            
                            lv_obj_clear_flag(objects.ui_val_dst_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param1_label, "Volume");
 
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
                            
                            lv_obj_clear_flag(objects.ui_val_dst_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param1_label, "Blend");
 
                            lv_obj_clear_flag(objects.ui_val_dst_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param2_label, "Volume");

                            lv_obj_clear_flag(objects.ui_val_dst_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dst_param3_label, "Bass");

                            lv_obj_clear_flag(objects.ui_val_dst_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dst_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_dst_param4_label, "Treble");
                        } break;
                    }
                } break;

                case VALETON_PARAM_DIST_PARAM_0:
                {
                    lv_slider_set_range(objects.ui_val_dst_param0_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_dst_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dst_param0_value, value_string);
                } break;

                case VALETON_PARAM_DIST_PARAM_1:
                {
                    lv_slider_set_range(objects.ui_val_dst_param1_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_dst_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dst_param1_value, value_string);
                } break;

                case VALETON_PARAM_DIST_PARAM_2:
                {
                    lv_slider_set_range(objects.ui_val_dst_param2_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_dst_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dst_param2_value, value_string);
                } break;

                case VALETON_PARAM_DIST_PARAM_3:
                {
                    lv_slider_set_range(objects.ui_val_dst_param3_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_dst_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dst_param3_value, value_string);
                } break;
                
                case VALETON_PARAM_DIST_PARAM_4:
                {
                    lv_slider_set_range(objects.ui_val_dst_param4_slider, param_entry->Min, param_entry->Max);
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
                        if (param_ptr[VALETON_PARAM_NS_ENABLE].Value != 0.0f)
                        {
                            // show disabled
                            lv_img_set_src(objects.ui_icon_val_amp, (lv_obj_t*)&img_amp_disabled);
                        }
                        else
                        {
                            lv_img_set_src(objects.ui_icon_val_amp, (lv_obj_t*)&img_amp_on);
                        }
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
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Tone");
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Volume");

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
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Presence");
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Volume");

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Bass");

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Middle");

                            // todo Treble 
                        } break;

                        case VALETON_EFFECT_AMP_DARK_TWIN:
                        case VALETON_EFFECT_AMP_JUICE_R100:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain");
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Volume");
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Bass");

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Middle");

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Treble");

                            //todo bright switch
                        } break;

                        case VALETON_EFFECT_AMP_FOXY_30N:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain");
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Tone Cut");
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Volume");
 
                            //todo bright switch

                            lv_obj_add_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                        } break;

                        case VALETON_EFFECT_AMP_J_120_CL:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Volume");
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Bass");
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Middle");

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Treble");

                            //todo bright switch

                            lv_obj_add_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);                          
                        } break;

                        case VALETON_EFFECT_AMP_UK_50JP:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain 1");
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Presence");
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Volume");

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Bass");

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Middle");

                            //todo: treble, gain 2
                        } break;

                        case VALETON_EFFECT_AMP_FOXY_30TB:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain");
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Tone Cut");
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Volume");

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Bass");

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Treble");

                            //todo: add character switch
                        } break;

                        case VALETON_EFFECT_AMP_SUPDUAL_OD:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain 1");
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Tone 1");
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Gain 2");

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Tone 2");

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Volume");
                        } break;
                        
                        case VALETON_EFFECT_AMP_Z38_OD:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain");
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Tone Cut");
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Volume");

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Bass");

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Middle");

                            //todo: treble
                        } break;

                        case VALETON_EFFECT_AMP_EV_51:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain");
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Volume");
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Bass");

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Mid");

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Treble");

                            // todo presence
                        } break;

                        case VALETON_EFFECT_AMP_CLASSIC_BASS:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Gain");
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Bass");
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Middle");
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Mid Freq");

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Treble");

                            //todo vol
                        } break;
                        
                        case VALETON_EFFECT_AMP_FOXY_BASS:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Volume");
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Bass");
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Treble");

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
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Volume");
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Bass");

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "Mid");

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "Treble");
                        } break;

                        case VALETON_EFFECT_AMP_AC_PRE1:    // fallthrough
                        case VALETON_EFFECT_AMP_AC_PRE2:
                        {
                            lv_obj_clear_flag(objects.ui_val_amp_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param0_label, "Volume");
                            
                            lv_obj_clear_flag(objects.ui_val_amp_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param1_label, "Tone");
 
                            lv_obj_clear_flag(objects.ui_val_amp_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param2_label, "Balance");

                            lv_obj_clear_flag(objects.ui_val_amp_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_amp_param3_label, "EQ Freq");

                            lv_obj_clear_flag(objects.ui_val_amp_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_amp_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_amp_param4_label, "EQ Q");

                            //todo : EQ gain
                        } break;
                    }
                } break;

                case VALETON_PARAM_AMP_PARAM_0:
                {
                    lv_slider_set_range(objects.ui_val_amp_param0_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_amp_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_amp_param0_value, value_string);
                } break;
                
                case VALETON_PARAM_AMP_PARAM_1:
                {
                    lv_slider_set_range(objects.ui_val_amp_param1_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_amp_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_amp_param1_value, value_string);
                } break;

                case VALETON_PARAM_AMP_PARAM_2:
                {
                    lv_slider_set_range(objects.ui_val_amp_param2_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_amp_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_amp_param2_value, value_string);
                } break;

                case VALETON_PARAM_AMP_PARAM_3:
                {
                    lv_slider_set_range(objects.ui_val_amp_param3_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_amp_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_amp_param3_value, value_string);
                } break;

                case VALETON_PARAM_AMP_PARAM_4:
                {
                    lv_slider_set_range(objects.ui_val_amp_param4_slider, param_entry->Min, param_entry->Max);
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
                        if (param_ptr[VALETON_PARAM_NS_ENABLE].Value != 0.0f)
                        {
                            // show disabled
                            lv_img_set_src(objects.ui_icon_val_cab, (lv_obj_t*)&img_cab_disabled);
                        }
                        else
                        {
                            lv_img_set_src(objects.ui_icon_val_cab, (lv_obj_t*)&img_cab_on);
                        }
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
                    lv_slider_set_range(objects.ui_val_cab_param0_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_cab_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_cab_param0_value, value_string);
                } break;

                case VALETON_PARAM_CAB_PARAM_1:     
                {
                    lv_slider_set_range(objects.ui_val_cab_param1_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_cab_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_cab_param1_value, value_string);
                } break;

                case VALETON_PARAM_CAB_PARAM_2: 
                {
                    lv_slider_set_range(objects.ui_val_cab_param2_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_cab_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_cab_param2_value, value_string);
                } break;

                case VALETON_PARAM_CAB_PARAM_3:
                {
                    lv_slider_set_range(objects.ui_val_cab_param3_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_cab_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_cab_param3_value, value_string);
                } break;

                case VALETON_PARAM_CAB_PARAM_4:     
                {
                    lv_slider_set_range(objects.ui_val_cab_param4_slider, param_entry->Min, param_entry->Max);
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
                            lv_obj_clear_flag(objects.ui_val_eq_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param0_label, "125 Hz");
                            
                            lv_obj_clear_flag(objects.ui_val_eq_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param1_label, "400 Hz");
 
                            lv_obj_clear_flag(objects.ui_val_eq_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param2_label, "800 Hz");

                            lv_obj_clear_flag(objects.ui_val_eq_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param3_label, "1.6 kHz");
                            
                            lv_obj_clear_flag(objects.ui_val_eq_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_eq_param4_label, "4 kHz");

                            //todo Volume
                        } break;

                        case VALETON_EFFECT_EQ_GUITAR_EQ2:
                        {
                            lv_obj_clear_flag(objects.ui_val_eq_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param0_label, "100 Hz");
                            
                            lv_obj_clear_flag(objects.ui_val_eq_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param1_label, "500 Hz");
 
                            lv_obj_clear_flag(objects.ui_val_eq_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param2_label, "1 kHz");

                            lv_obj_clear_flag(objects.ui_val_eq_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param3_label, "3 kHz");
                            
                            lv_obj_clear_flag(objects.ui_val_eq_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_eq_param4_label, "6 kHz");

                            //todo volume
                        } break;

                        case VALETON_EFFECT_EQ_BASS_EQ1:
                        {
                            lv_obj_clear_flag(objects.ui_val_eq_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param0_label, "33 Hz");
                            
                            lv_obj_clear_flag(objects.ui_val_eq_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param1_label, "150 Hz");
 
                            lv_obj_clear_flag(objects.ui_val_eq_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param2_label, "600 Hz");

                            lv_obj_clear_flag(objects.ui_val_eq_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param3_label, "2 kHz");
                            
                            lv_obj_clear_flag(objects.ui_val_eq_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_eq_param4_label, "8 kHz");

                            //todo Volume
                        } break;

                        case VALETON_EFFECT_EQ_BASS_EQ2:
                        {
                            lv_obj_clear_flag(objects.ui_val_eq_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param0_label, "50 Hz");
                            
                            lv_obj_clear_flag(objects.ui_val_eq_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param1_label, "120 Hz");
 
                            lv_obj_clear_flag(objects.ui_val_eq_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param2_label, "400 Hz");

                            lv_obj_clear_flag(objects.ui_val_eq_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param3_label, "800 Hz");
                            
                            lv_obj_clear_flag(objects.ui_val_eq_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_eq_param4_label, "4.5 kHz");

                            //todo Volume
                        } break;

                        case VALETON_EFFECT_EQ_MESS_EQ:
                        {
                            lv_obj_clear_flag(objects.ui_val_eq_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param0_label, "80 Hz");
                            
                            lv_obj_clear_flag(objects.ui_val_eq_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param1_label, "240 Hz");
 
                            lv_obj_clear_flag(objects.ui_val_eq_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param2_label, "750 Hz");

                            lv_obj_clear_flag(objects.ui_val_eq_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_eq_param3_label, "2.2 kHz");
                            
                            lv_obj_clear_flag(objects.ui_val_eq_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_eq_param4_value, LV_OBJ_FLAG_HIDDEN);  
                            lv_label_set_text(objects.ui_val_eq_param4_label, "6.6 kHz");
                        } break;
                    }
                } break;

                case VALETON_PARAM_EQ_PARAM_0:
                {
                    lv_slider_set_range(objects.ui_val_eq_param0_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_eq_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_eq_param0_value, value_string);
                } break;

                case VALETON_PARAM_EQ_PARAM_1:
                {
                    lv_slider_set_range(objects.ui_val_eq_param1_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_eq_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_eq_param1_value, value_string);
                } break;

                case VALETON_PARAM_EQ_PARAM_2:
                {
                    lv_slider_set_range(objects.ui_val_eq_param2_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_eq_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_eq_param2_value, value_string);
                } break;

                case VALETON_PARAM_EQ_PARAM_3:
                {
                    lv_slider_set_range(objects.ui_val_eq_param3_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_eq_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_eq_param3_value, value_string);
                } break;

                case VALETON_PARAM_EQ_PARAM_4:
                {
                    lv_slider_set_range(objects.ui_val_eq_param4_slider, param_entry->Min, param_entry->Max);
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
                        case VALETON_EFFECT_MOD_A_CHORUS:   // fallthrough
                        case VALETON_EFFECT_MOD_B_CHORUS:
                        {
                            lv_obj_clear_flag(objects.ui_val_mod_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param0_label, "Depth");
                            
                            lv_obj_clear_flag(objects.ui_val_mod_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param1_label, "Rate");
 
                            lv_obj_clear_flag(objects.ui_val_mod_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param2_label, "Tone");

                            lv_obj_add_flag(objects.ui_val_mod_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param3_value, LV_OBJ_FLAG_HIDDEN);
                        
                            lv_obj_add_flag(objects.ui_val_mod_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param4_value, LV_OBJ_FLAG_HIDDEN);                         
                        } break;
                        
                        case VALETON_EFFECT_MOD_JET:       // Fallthrough
                        case VALETON_EFFECT_MOD_N_JET:
                        {
                            lv_obj_clear_flag(objects.ui_val_mod_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param0_label, "Depth");
                            
                            lv_obj_clear_flag(objects.ui_val_mod_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param1_label, "Rate");
 
                            lv_obj_clear_flag(objects.ui_val_mod_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param2_label, "P.Delay");

                            lv_obj_clear_flag(objects.ui_val_mod_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param3_label, "F.Back");

                            lv_obj_add_flag(objects.ui_val_mod_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param4_value, LV_OBJ_FLAG_HIDDEN);      
                        } break;

                        case VALETON_EFFECT_MOD_O_PHASE:
                        {
                            lv_obj_clear_flag(objects.ui_val_mod_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param0_label, "Rate");
                            
                            lv_obj_add_flag(objects.ui_val_mod_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param1_value, LV_OBJ_FLAG_HIDDEN);
 
                            lv_obj_add_flag(objects.ui_val_mod_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param2_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_mod_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_mod_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param4_value, LV_OBJ_FLAG_HIDDEN);      
                        } break;

                        case VALETON_EFFECT_MOD_M_VIBE:     // fallthrough
                        case VALETON_EFFECT_MOD_V_ROTO:     // fallthrough
                        case VALETON_EFFECT_MOD_O_TREM:
                        {
                            lv_obj_clear_flag(objects.ui_val_mod_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param0_label, "Depth");
                            
                            lv_obj_clear_flag(objects.ui_val_mod_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param1_label, "Rate");
 
                            lv_obj_add_flag(objects.ui_val_mod_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param2_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_mod_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_mod_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param4_value, LV_OBJ_FLAG_HIDDEN);      
                        } break;

                        case VALETON_EFFECT_MOD_VIBRATO:        // fallthrough
                        case VALETON_EFFECT_MOD_SINE_TREM:
                        {
                            lv_obj_clear_flag(objects.ui_val_mod_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param0_label, "Depth");
                            
                            lv_obj_clear_flag(objects.ui_val_mod_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param1_label, "Rate");
 
                            lv_obj_clear_flag(objects.ui_val_mod_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param2_label, "Volume");

                            lv_obj_add_flag(objects.ui_val_mod_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param3_value, LV_OBJ_FLAG_HIDDEN);
                        
                            lv_obj_add_flag(objects.ui_val_mod_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param4_value, LV_OBJ_FLAG_HIDDEN);                         
                        } break;

                        case VALETON_EFFECT_MOD_BIAS_TREM:
                        {
                            lv_obj_clear_flag(objects.ui_val_mod_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param0_label, "Depth");
                            
                            lv_obj_clear_flag(objects.ui_val_mod_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param1_label, "Rate");
 
                            lv_obj_clear_flag(objects.ui_val_mod_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param2_label, "Volume");

                            lv_obj_clear_flag(objects.ui_val_mod_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_mod_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_mod_param3_label, "Bias");

                            lv_obj_add_flag(objects.ui_val_mod_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_mod_param4_value, LV_OBJ_FLAG_HIDDEN);      
                        } break;
                    }
                } break;

                case VALETON_PARAM_MOD_PARAM_0:
                {
                    lv_slider_set_range(objects.ui_val_mod_param0_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_mod_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_mod_param0_value, value_string);
                } break;
                
                case VALETON_PARAM_MOD_PARAM_1:
                {
                    lv_slider_set_range(objects.ui_val_mod_param1_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_mod_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_mod_param1_value, value_string);
                } break;

                case VALETON_PARAM_MOD_PARAM_2:
                {
                    lv_slider_set_range(objects.ui_val_mod_param2_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_mod_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_mod_param2_value, value_string);
                } break;

                case VALETON_PARAM_MOD_PARAM_3:
                {
                    lv_slider_set_range(objects.ui_val_mod_param3_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_mod_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_mod_param3_value, value_string);
                } break;

                case VALETON_PARAM_MOD_PARAM_4:
                {
                    lv_slider_set_range(objects.ui_val_mod_param4_slider, param_entry->Min, param_entry->Max);
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
                        case VALETON_EFFECT_DLY_PURE:       // fallthrough
                        case VALETON_EFFECT_DLY_SWEET_ECHO: // fallthrough
                        case VALETON_EFFECT_DLY_TAPE:       // fallthrough
                        case VALETON_EFFECT_DLY_TUBE:
                        {
                            lv_obj_clear_flag(objects.ui_val_dly_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param0_label, "Mix");
                            
                            lv_obj_clear_flag(objects.ui_val_dly_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param1_label, "Time");
 
                            lv_obj_clear_flag(objects.ui_val_dly_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param2_label, "F.Back");

                            // todo trail switch

                            lv_obj_add_flag(objects.ui_val_dly_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_dly_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param4_value, LV_OBJ_FLAG_HIDDEN);      
                        } break;

                        case VALETON_EFFECT_DLY_ANALOG:    
                        {
                            lv_obj_clear_flag(objects.ui_val_dly_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param0_label, "Mix");
                            
                            lv_obj_clear_flag(objects.ui_val_dly_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param1_label, "Time");
 
                            lv_obj_clear_flag(objects.ui_val_dly_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param2_label, "F.Back");

                            // todo trail switch

                            lv_obj_add_flag(objects.ui_val_dly_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_dly_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param4_value, LV_OBJ_FLAG_HIDDEN);      
                        } break;

                        case VALETON_EFFECT_DLY_SLAPBACK:   
                        {
                            lv_obj_clear_flag(objects.ui_val_dly_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param0_label, "Mix");
                            
                            lv_obj_clear_flag(objects.ui_val_dly_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param1_label, "Time");
 
                            lv_obj_clear_flag(objects.ui_val_dly_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param2_label, "F.Back");

                            // todo trail switch

                            lv_obj_add_flag(objects.ui_val_dly_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_dly_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param4_value, LV_OBJ_FLAG_HIDDEN);      
                        } break;

                        case VALETON_EFFECT_DLY_REV_ECHO:   
                        {
                            lv_obj_clear_flag(objects.ui_val_dly_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param0_label, "Mix");
                            
                            lv_obj_clear_flag(objects.ui_val_dly_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param1_label, "Time");
 
                            lv_obj_clear_flag(objects.ui_val_dly_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param2_label, "F.Back");

                            // todo trail switch

                            lv_obj_add_flag(objects.ui_val_dly_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_dly_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param4_value, LV_OBJ_FLAG_HIDDEN);      
                        } break;

                        case VALETON_EFFECT_DLY_PING_PONG:
                        {
                            lv_obj_clear_flag(objects.ui_val_dly_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param0_label, "Mix");
                            
                            lv_obj_clear_flag(objects.ui_val_dly_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param1_label, "Time");
 
                            lv_obj_clear_flag(objects.ui_val_dly_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param2_label, "F.Back");

                            // todo trail switch

                            lv_obj_add_flag(objects.ui_val_dly_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_dly_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param4_value, LV_OBJ_FLAG_HIDDEN);      
                        } break;

                        case VALETON_EFFECT_DLY_RING_ECHO:
                        {
                            lv_obj_clear_flag(objects.ui_val_dly_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param0_label, "Mix");
                            
                            lv_obj_clear_flag(objects.ui_val_dly_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param1_label, "Time");
 
                            lv_obj_clear_flag(objects.ui_val_dly_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param2_label, "F.Back");

                            lv_obj_add_flag(objects.ui_val_dly_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param3_label, "R-Mix");

                            lv_obj_add_flag(objects.ui_val_dly_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_dly_param4_value, LV_OBJ_FLAG_HIDDEN);      
                            lv_label_set_text(objects.ui_val_dly_param4_label, "Freq");

                            // todo Tone
                            // todo trail switch
                        } break;

                        case VALETON_EFFECT_DLY_SWEEP_ECHO:
                        {
                            lv_obj_clear_flag(objects.ui_val_dly_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param0_label, "Mix");
                            
                            lv_obj_clear_flag(objects.ui_val_dly_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param1_label, "Time");
 
                            lv_obj_clear_flag(objects.ui_val_dly_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param2_label, "F.Back");

                            lv_obj_clear_flag(objects.ui_val_dly_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param3_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_dly_param3_label, "S-Depth");

                            lv_obj_clear_flag(objects.ui_val_dly_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_dly_param4_value, LV_OBJ_FLAG_HIDDEN);      
                            lv_label_set_text(objects.ui_val_dly_param4_label, "S-Rate");

                            // todo trail switch
                        } break;
                    }
                } break;

                case VALETON_PARAM_DLY_PARAM_0:
                {
                    lv_slider_set_range(objects.ui_val_dly_param0_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_dly_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dly_param0_value, value_string);
                } break;

                case VALETON_PARAM_DLY_PARAM_1:
                {
                    lv_slider_set_range(objects.ui_val_dly_param1_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_dly_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dly_param1_value, value_string);
                } break;

                case VALETON_PARAM_DLY_PARAM_2:
                {
                    lv_slider_set_range(objects.ui_val_dly_param2_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_dly_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dly_param2_value, value_string);
                } break;

                case VALETON_PARAM_DLY_PARAM_3:
                {
                    lv_slider_set_range(objects.ui_val_dly_param3_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_dly_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_dly_param3_value, value_string);
                } break;

                case VALETON_PARAM_DLY_PARAM_4:
                {
                    lv_slider_set_range(objects.ui_val_dly_param4_slider, param_entry->Min, param_entry->Max);
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
                        case VALETON_EFFECT_RVB_AIR:       // fallthrough
                        case VALETON_EFFECT_RVB_PLATE:
                        {
                            lv_obj_clear_flag(objects.ui_val_rvb_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_rvb_param0_label, "Mix");
                            
                            lv_obj_clear_flag(objects.ui_val_rvb_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_rvb_param1_label, "Decay");
 
                            lv_obj_clear_flag(objects.ui_val_rvb_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_rvb_param2_label, "Damp");

                            //todo trail switch

                            lv_obj_add_flag(objects.ui_val_rvb_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_rvb_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_rvb_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_rvb_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_rvb_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_rvb_param4_value, LV_OBJ_FLAG_HIDDEN);      
                        } break;

                        case VALETON_EFFECT_RVB_ROOM:       // fallthrough
                        case VALETON_EFFECT_RVB_HALL:       // fallthrough
                        case VALETON_EFFECT_RVB_CHURCH:     // fallthrough
                        case VALETON_EFFECT_RVB_PLATE_L:    // fallthrough
                        case VALETON_EFFECT_RVB_SPRING:     // fallthrough
                        case VALETON_EFFECT_RVB_N_STAR:     // fallthrough
                        case VALETON_EFFECT_RVB_DEEPSEA:
                        {
                            lv_obj_clear_flag(objects.ui_val_rvb_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_rvb_param0_label, "Mix");
                            
                            lv_obj_clear_flag(objects.ui_val_rvb_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_rvb_param1_label, "Decay");
 
                            //todo trail switch

                            lv_obj_add_flag(objects.ui_val_rvb_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_rvb_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_rvb_param2_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_rvb_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_rvb_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_rvb_param3_value, LV_OBJ_FLAG_HIDDEN);

                            lv_obj_add_flag(objects.ui_val_rvb_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_rvb_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_rvb_param4_value, LV_OBJ_FLAG_HIDDEN);      
                        } break;
                
                        case VALETON_EFFECT_RVB_SWEET_SPACE:
                        {
                            lv_obj_clear_flag(objects.ui_val_rvb_param0_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param0_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param0_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_rvb_param0_label, "Mix");
                            
                            lv_obj_clear_flag(objects.ui_val_rvb_param1_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param1_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param1_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_rvb_param1_label, "Decay");
 
                            lv_obj_clear_flag(objects.ui_val_rvb_param2_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param2_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param2_value, LV_OBJ_FLAG_HIDDEN);
                            lv_label_set_text(objects.ui_val_rvb_param2_label, "Damp");
                            
                            lv_obj_clear_flag(objects.ui_val_rvb_param3_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param3_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(objects.ui_val_rvb_param3_value, LV_OBJ_FLAG_HIDDEN);
                             lv_label_set_text(objects.ui_val_rvb_param3_label, "Mod");

                            //todo trail switch

                            lv_obj_add_flag(objects.ui_val_rvb_param4_label, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_rvb_param4_slider, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_add_flag(objects.ui_val_rvb_param4_value, LV_OBJ_FLAG_HIDDEN);      
                        } break;
                    }
                } break;

                case VALETON_PARAM_RVB_PARAM_0:
                {
                    lv_slider_set_range(objects.ui_val_rvb_param0_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_rvb_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_rvb_param0_value, value_string);
                } break;
                
                case VALETON_PARAM_RVB_PARAM_1:
                {
                    lv_slider_set_range(objects.ui_val_rvb_param1_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_rvb_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_rvb_param1_value, value_string);
                } break;                
                
                case VALETON_PARAM_RVB_PARAM_2:
                {
                    lv_slider_set_range(objects.ui_val_rvb_param2_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_rvb_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_rvb_param2_value, value_string);
                } break;
                
                case VALETON_PARAM_RVB_PARAM_3:
                {
                    lv_slider_set_range(objects.ui_val_rvb_param3_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_rvb_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_rvb_param3_value, value_string);
                } break;
                
                case VALETON_PARAM_RVB_PARAM_4:
                {
                    lv_slider_set_range(objects.ui_val_rvb_param4_slider, param_entry->Min, param_entry->Max);
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

                    // all models have same params
                    lv_obj_clear_flag(objects.ui_val_ns_param0_label, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_val_ns_param0_slider, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_val_ns_param0_value, LV_OBJ_FLAG_HIDDEN);
                    lv_label_set_text(objects.ui_val_ns_param0_label, "Gain");
                    
                    lv_obj_clear_flag(objects.ui_val_ns_param1_label, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_val_ns_param1_slider, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_val_ns_param1_value, LV_OBJ_FLAG_HIDDEN);
                    lv_label_set_text(objects.ui_val_ns_param1_label, "Volume");

                    lv_obj_clear_flag(objects.ui_val_ns_param2_label, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_val_ns_param2_slider, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_val_ns_param2_value, LV_OBJ_FLAG_HIDDEN);
                    lv_label_set_text(objects.ui_val_ns_param2_label, "Bass");

                    lv_obj_clear_flag(objects.ui_val_ns_param3_label, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_val_ns_param3_slider, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_val_ns_param3_value, LV_OBJ_FLAG_HIDDEN);
                    lv_label_set_text(objects.ui_val_ns_param3_label, "Middle");

                    lv_obj_clear_flag(objects.ui_val_ns_param4_label, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_val_ns_param4_slider, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_val_ns_param4_value, LV_OBJ_FLAG_HIDDEN);      
                    lv_label_set_text(objects.ui_val_ns_param4_label, "Treble");
                } break;

                case VALETON_PARAM_NS_PARAM_0:
                {
                    lv_slider_set_range(objects.ui_val_ns_param0_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_ns_param0_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_ns_param0_value, value_string);
                } break;

                case VALETON_PARAM_NS_PARAM_1:
                {
                    lv_slider_set_range(objects.ui_val_ns_param1_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_ns_param1_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_ns_param1_value, value_string);
                } break;                
                
                case VALETON_PARAM_NS_PARAM_2:
                {
                    lv_slider_set_range(objects.ui_val_ns_param2_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_ns_param2_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_ns_param2_value, value_string);
                } break;                
                
                case VALETON_PARAM_NS_PARAM_3:
                {
                    lv_slider_set_range(objects.ui_val_ns_param3_slider, param_entry->Min, param_entry->Max);
                    lv_slider_set_value(objects.ui_val_ns_param3_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_ns_param3_value, value_string);
                } break;
                
                case VALETON_PARAM_NS_PARAM_4:
                {
                    lv_slider_set_range(objects.ui_val_ns_param4_slider, param_entry->Min, param_entry->Max);
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

                case VALETON_GLOBAL_BPM:
                {
                    //todo
                } break;
    
                case VALETON_GLOBAL_INPUT_TRIM:
                {
                    lv_slider_set_value(objects.ui_val_glob_input_level_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_glob_input_level_value, value_string);
                } break;

                case VALETON_GLOBAL_CABSIM_BYPASS:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_val_glob_no_cab_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_val_glob_no_cab_switch, LV_STATE_CHECKED);
                    }                    
                } break;

                case VALETON_GLOBAL_MASTER_VOLUME:
                {
                    lv_slider_set_value(objects.ui_val_glob_master_vol_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_glob_master_vol_value, value_string);
                } break;

                case VALETON_GLOBAL_RECORD_LEVEL:   // fallthrough
                case VALETON_GLOBAL_MONITOR_LEVEL:  // fallthrough
                case VALETON_GLOBAL_BT_LEVEL:       // fallthrough
                case VALETON_PARAM_EFFECT_SLOT_0:   // fallthrough
                case VALETON_PARAM_EFFECT_SLOT_1:   // fallthrough
                case VALETON_PARAM_EFFECT_SLOT_2:   // fallthrough
                case VALETON_PARAM_EFFECT_SLOT_3:   // fallthrough
                case VALETON_PARAM_EFFECT_SLOT_4:   // fallthrough
                case VALETON_PARAM_EFFECT_SLOT_5:   // fallthrough
                case VALETON_PARAM_EFFECT_SLOT_6:   // fallthrough
                case VALETON_PARAM_EFFECT_SLOT_7:   // fallthrough
                case VALETON_PARAM_EFFECT_SLOT_8:   // fallthrough
                case VALETON_PARAM_EFFECT_SLOT_9:
                {
                    // not exposed to UI
                } break;
            }

            valeton_params_release_locked_access();
        }               
    }
#else //CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
    // Small UI
    for (uint16_t param = 0; param < VALETON_PARAM_LAST; param++)
    {                     
        if (valeton_params_get_locked_access(&param_ptr) == ESP_OK)
        {
            tModellerParameter* param_entry = &param_ptr[param];

            // debug
            //ESP_LOGI(TAG, "Param %d: val: %02f, min: %02f, max: %02f", param, param_entry->Value, param_entry->Min, param_entry->Max);

            switch (param)
            {
                case VALETON_GLOBAL_BPM:
                {
                    char buf[128];
                    sprintf(buf, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_bpm, buf);  
                } break;

                case VALETON_PARAM_DIST_ENABLE:
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

                case VALETON_PARAM_MOD_ENABLE:
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

                case VALETON_PARAM_DLY_ENABLE:
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

                case VALETON_PARAM_RVB_ENABLE:
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

                case VALETON_GLOBAL_BPM:
                {
                    // not exposed on UI
                } break;

                case VALETON_GLOBAL_INPUT_TRIM:
                {
                    lv_slider_set_value(objects.ui_val_glob_input_level_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_glob_input_level_value, value_string);
                } break;

                case VALETON_GLOBAL_CABSIM_BYPASS:
                {
                    if (param_entry->Value)
                    {
                        lv_obj_add_state(objects.ui_val_glob_no_cab_switch, LV_STATE_CHECKED);
                    }
                    else
                    {
                        lv_obj_clear_state(objects.ui_val_glob_no_cab_switch, LV_STATE_CHECKED);
                    }                    
                } break;

                case VALETON_GLOBAL_MASTER_VOLUME:
                {
                    lv_slider_set_value(objects.ui_val_glob_master_vol_slider, round(param_entry->Value), LV_ANIM_OFF);
                    sprintf(value_string, "%d", (int)round(param_entry->Value));
                    lv_label_set_text(objects.ui_val_glob_master_vol_value, value_string);

                    ESP_LOGI(TAG, "VALETON_GLOBAL_MASTER_VOLUME: %f", param_entry->Value);
                } break;

                case VALETON_GLOBAL_RECORD_LEVEL:
                {
                    // not exposed on UI
                } break;

                case VALETON_GLOBAL_MONITOR_LEVEL:
                {
                    // not exposed on UI
                } break;

                case VALETON_GLOBAL_BT_LEVEL:
                {
                    // not exposed on UI
                } break;

            }

            valeton_params_release_locked_access();
        }               
    }
#endif  //CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
#endif  //CONFIG_TONEX_CONTROLLER_HAS_DISPLAY

    return 1;
}
