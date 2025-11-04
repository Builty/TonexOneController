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
        //todo
    }
    else if (target == objects.ui_icon_val_nr)
    {
    }
    else if (target == objects.ui_icon_val_rvb)
    {
    }
    else if ((target == objects.ui_icon_val_amp) || (target == objects.ui_icon_val_cab))
    {
    }
    else if (target == objects.ui_icon_val_tc)
    {
    }
    else if (target == objects.ui_icon_val_eq)
    {
    }
    else if (target == objects.ui_icon_val_mod)
    {
    }
    else if (target == objects.ui_icon_val_dly)
    {
    }
    else if (target == objects.ui_icon_val_dst)
    {
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

            valeton_params_get_locked_access(&param_ptr);
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
        else if (event_object == objects.ui_icon_val_nr)
        {
            ESP_LOGI(TAG, "UI Toggle NR");
        
            valeton_params_get_locked_access(&param_ptr);
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
        else if (event_object == objects.ui_icon_val_rvb)
        {
            ESP_LOGI(TAG, "UI Toggle RVB");
            
            valeton_params_get_locked_access(&param_ptr);
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
        else if (event_object == objects.ui_icon_val_eq)
        {
            ESP_LOGI(TAG, "UI Toggle EQ");
            
            valeton_params_get_locked_access(&param_ptr);
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
        else if (event_object == objects.ui_icon_val_mod)
        {
            ESP_LOGI(TAG, "UI Toggle Mod");
            
            valeton_params_get_locked_access(&param_ptr);
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
        else if (event_object == objects.ui_icon_val_dly)
        {
            ESP_LOGI(TAG, "UI Toggle Dly");
            
            valeton_params_get_locked_access(&param_ptr);
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
        else if (event_object == objects.ui_icon_val_dst)
        {
            ESP_LOGI(TAG, "UI Toggle Dst");
            
            valeton_params_get_locked_access(&param_ptr);
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
        else if (event_object == objects.ui_icon_val_amp)
        {   
            //todo
        } 
        else if (event_object == objects.ui_icon_val_cab)
        {   
            //todo
        } 
        else if (event_object == objects.ui_icon_val_tc)
        {   
            //todo
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
            
            //todo
             
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
