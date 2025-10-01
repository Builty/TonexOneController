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
#include "usb_comms.h"
#include "usb_tonex_common.h"
#include "usb_tonex_one.h"
#include "usb_tonex.h"
#include "display.h"
#include "CH422G.h"
#include "control.h"
#include "task_priorities.h" 
#include "midi_control.h"
#include "LP5562.h"
#include "tonex_params.h"
#include "platform_common.h"

static const char *TAG = "app_display";

#define DISPLAY_TASK_STACK_SIZE   (6 * 1024)

#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI && CONFIG_TONEX_CONTROLLER_SHOW_BPM_INDICATOR
    //static lv_anim_t *ui_BPMAnimation = NULL;
    //static lv_anim_t PropertyAnimation_0;
    void ui_BPMAnimate(lv_obj_t *TargetObject, uint32_t duration);
#endif

#define DISPLAY_LVGL_TICK_PERIOD_MS     2
#define DISPLAY_LVGL_TASK_MAX_DELAY_MS  500
#define DISPLAY_LVGL_TASK_MIN_DELAY_MS  1
#define BUF_SIZE                        (1024)
#define I2C_MASTER_TIMEOUT_MS           1000
#define MAX_UI_TEXT                     130
#define MAX_SKIN_IMAGE_SIZE             180000

enum UIElements
{
    UI_ELEMENT_USB_STATUS,
    UI_ELEMENT_BT_STATUS,
    UI_ELEMENT_WIFI_STATUS,
    UI_ELEMENT_PRESET_NAME,
    UI_ELEMENT_BANK_INDEX,
    UI_ELEMENT_AMP_SKIN,
    UI_ELEMENT_PRESET_DESCRIPTION,
    UI_ELEMENT_PARAMETERS,
    UI_ELEMENT_TOAST,
};

enum UIAction
{
    UI_ACTION_SET_STATE,
    UI_ACTION_SET_LABEL_TEXT,
    UI_ACTION_SET_ENTRY_TEXT,
    UI_ACTION_NONE = 0xFF
};

typedef struct 
{
    uint8_t ElementID;
    uint8_t Action;
    uint32_t Value;
    char Text[MAX_UI_TEXT];
} tUIUpdate;

typedef struct 
{
    lv_obj_t *mbox;
    lv_style_t *style_main;
    lv_style_t *style_text;
    
    uint32_t timer;
    uint8_t active;
} msgbox_data_t;

static QueueHandle_t ui_update_queue;
static SemaphoreHandle_t I2CMutexHandle;
static SemaphoreHandle_t lvgl_mux = NULL;
static lv_disp_drv_t* disp_drv; 
static msgbox_data_t msgbox_data;

#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
static void ui_show_toast(char* contents);

#if CONFIG_TONEX_CONTROLLER_HAS_TOUCH
static uint8_t __attribute__((unused)) touch_data_ready_to_read = 0;
#endif

#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
static uint8_t* skin_image_buffer = NULL;
static uint16_t current_skin_image = 0xFFFF;
#endif    

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void __attribute__((unused)) display_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
    int offsetx1;
    int offsetx2;
    int offsety1;
    int offsety2;

    // let platform adjust area
    platform_adjust_display_flush_area((lv_area_t*)area);

    offsetx1 = area->x1;
    offsetx2 = area->x2;
    offsety1 = area->y1;
    offsety2 = area->y2;

#if CONFIG_DISPLAY_AVOID_TEAR_EFFECT_WITH_SEM
    xSemaphoreGive(sem_gui_ready);
    xSemaphoreTake(sem_vsync_end, portMAX_DELAY);
#endif
    // pass the draw buffer to the driver
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
    lv_disp_flush_ready(drv);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
bool __attribute__((unused)) display_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    lv_disp_flush_ready(disp_drv);
    return false;
}
#endif  //CONFIG_TONEX_CONTROLLER_HAS_DISPLAY

#if CONFIG_TONEX_CONTROLLER_HAS_TOUCH

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void __attribute__((unused)) touch_data_ready(esp_lcd_touch_t *handle)
{
    touch_data_ready_to_read = 1;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void display_lvgl_touch_cb(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    uint16_t touchpad_x[1] = {0};
    uint16_t touchpad_y[1] = {0};
    uint8_t touchpad_cnt = 0;
    bool touchpad_pressed = false;

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_LILYGO_TDISPLAY_S3 || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169TOUCH || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_19TOUCH
    // CST816 driver has to set interrupt before data is valid to read
    if (touch_data_ready_to_read)
    {
        if (xSemaphoreTake(I2CMutexHandle, (TickType_t)10) == pdTRUE)
        {
            // Read touch controller data
            esp_lcd_touch_read_data(drv->user_data);

            // Get coordinates 
            touchpad_pressed = esp_lcd_touch_get_coordinates(drv->user_data, touchpad_x, touchpad_y, NULL, &touchpad_cnt, 1);

            // reset flag
            touch_data_ready_to_read = 0;

            xSemaphoreGive(I2CMutexHandle);
        }
    }

#else

    // poll the driver chip
    if (xSemaphoreTake(I2CMutexHandle, (TickType_t)10) == pdTRUE)
    {
        /* Read touch controller data */
        esp_lcd_touch_read_data(drv->user_data);

        /* Get coordinates */
        touchpad_pressed = esp_lcd_touch_get_coordinates(drv->user_data, touchpad_x, touchpad_y, NULL, &touchpad_cnt, 1);

        xSemaphoreGive(I2CMutexHandle);
    }
    else
    {
        ESP_LOGE(TAG, "Touch cb mutex timeout");
    }
#endif 

    if (touchpad_pressed && touchpad_cnt > 0) 
    {
        data->point.x = touchpad_x[0];
        data->point.y = touchpad_y[0];

        // allow platform to adjust if needed
        platform_adjust_touch_coords(&data->point.x, &data->point.y);

        data->state = LV_INDEV_STATE_PR;

        // debug
        //ESP_LOGI(TAG, "Touch X:%d Y:%d", (int)data->point.x, (int)data->point.y);
    } 
    else 
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void action_previous_clicked(lv_event_t * e)
{
    // called from LVGL 
    ESP_LOGI(TAG, "UI Previous Click/Swipe");      

    control_request_preset_down();      
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void action_next_clicked(lv_event_t * e)
{
    // called from LVGL 
    ESP_LOGI(TAG, "UI Next Click/Swipe");    

    control_request_preset_up();        
}

#else   //CONFIG_TONEX_CONTROLLER_HAS_TOUCH

// Dummy functions so that 1.69 and 1.69 Touch can share the same UI project
void action_previous_clicked(lv_event_t * e)
{
}

void action_next_clicked(lv_event_t * e)
{
}
#endif  //CONFIG_TONEX_CONTROLLER_HAS_TOUCH

#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
// we use two semaphores to sync the VSYNC event and the LVGL task, to avoid potential tearing effect
#if CONFIG_DISPLAY_AVOID_TEAR_EFFECT_WITH_SEM
SemaphoreHandle_t sem_vsync_end;
SemaphoreHandle_t sem_gui_ready;
#endif  //CONFIG_DISPLAY_AVOID_TEAR_EFFECT_WITH_SEM

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
bool display_on_vsync_event(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
#if CONFIG_DISPLAY_AVOID_TEAR_EFFECT_WITH_SEM
    if (xSemaphoreTakeFromISR(sem_gui_ready, &high_task_awoken) == pdTRUE) {
        xSemaphoreGiveFromISR(sem_vsync_end, &high_task_awoken);
    }
#endif
    return high_task_awoken == pdTRUE;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void ui_show_settings_tab(lv_event_t * e)
{
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
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void action_effect_icon_clicked(lv_event_t * e)
{
    tTonexParameter* param_ptr;
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
        ui_show_settings_tab(e);      
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void action_amp_skin_previous(lv_event_t * e)
{
    control_set_skin_previous();
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void action_amp_skin_next(lv_event_t * e)
{
    control_set_skin_next();
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void action_close_settings_page(lv_event_t * e)
{
    lv_scr_load_anim(objects.screen1, LV_SCR_LOAD_ANIM_FADE_IN, 0, 0, false);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void action_show_settings_page(lv_event_t * e)
{
    lv_scr_load_anim(objects.settings, LV_SCR_LOAD_ANIM_FADE_IN, 0, 0, false);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void action_enable_skin_edit(lv_event_t * e)
{
    ESP_LOGI(TAG, "UI Skin edit mode");

    lv_obj_clear_flag(objects.ui_left_arrow, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.ui_right_arrow, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_state(objects.ui_preset_details_text_area, LV_STATE_DISABLED);
    lv_obj_clear_flag(objects.ui_ok_tick, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.ui_bank_title_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.ui_bank_value_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.ui_bpm_title_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.ui_bpm_value_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.ui_bpm_indicator, LV_OBJ_FLAG_HIDDEN);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void action_save_skin_edit(lv_event_t * e)
{
    ESP_LOGI(TAG, "UI save skin edit");

    action_keyboard_ok(e);
    control_save_user_data(0);
    
    lv_obj_add_flag(objects.ui_ok_tick, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.ui_entry_keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.ui_left_arrow, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.ui_right_arrow, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_state(objects.ui_preset_details_text_area, LV_STATE_DISABLED);
    lv_obj_clear_flag(objects.ui_bank_title_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.ui_bank_value_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.ui_bpm_title_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.ui_bpm_value_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.ui_bpm_indicator, LV_OBJ_FLAG_HIDDEN);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void action_preset_description_pressed(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_PRESSED) 
    {
        lv_keyboard_set_textarea(objects.ui_entry_keyboard,  objects.ui_preset_details_text_area);
        lv_obj_clear_flag(objects.ui_entry_keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void BTBondsClearRequest(lv_event_t * e)
{
    // request to clear bluetooth bonds
    midi_delete_bluetooth_bonds();
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void action_keyboard_ok(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_READY) 
    {
        // hide keyboard
        lv_obj_add_flag(objects.ui_entry_keyboard, LV_OBJ_FLAG_HIDDEN);

        char* text = (char*)lv_textarea_get_text(objects.ui_preset_details_text_area);

        ESP_LOGI(TAG, "action_keyboard_ok: %s", text);

        control_set_user_text(text);  
    }    
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void action_parameter_changed(lv_event_t * e)
{
    // get the object that was changed
    lv_obj_t* obj = lv_event_get_current_target(e);

    ESP_LOGI(TAG, "Parameter changed");

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
}
#endif  //CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void __attribute__((unused)) display_increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(DISPLAY_LVGL_TICK_PERIOD_MS);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
bool display_lvgl_lock(int timeout_ms)
{
    // Convert timeout in milliseconds to FreeRTOS ticks
    // If `timeout_ms` is set to -1, the program will block until the condition is met
    const TickType_t timeout_ticks = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTakeRecursive(lvgl_mux, timeout_ticks) == pdTRUE;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void display_lvgl_unlock(void)
{
    xSemaphoreGiveRecursive(lvgl_mux);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void UI_SetUSBStatus(uint8_t state)
{
    tUIUpdate ui_update;

    // build command
    ui_update.ElementID = UI_ELEMENT_USB_STATUS;
    ui_update.Action = UI_ACTION_SET_STATE;
    ui_update.Value = state;

    // send to queue
    if (xQueueSend(ui_update_queue, (void*)&ui_update, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "UI Update queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void UI_SetBTStatus(uint8_t state)
{
    tUIUpdate ui_update;

    // build command
    ui_update.ElementID = UI_ELEMENT_BT_STATUS;
    ui_update.Action = UI_ACTION_SET_STATE;
    ui_update.Value = state;

    // send to queue
    if (xQueueSend(ui_update_queue, (void*)&ui_update, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "UI Update queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void UI_SetWiFiStatus(uint8_t state)
{
    tUIUpdate ui_update;

    // build command
    ui_update.ElementID = UI_ELEMENT_WIFI_STATUS;
    ui_update.Action = UI_ACTION_SET_STATE;
    ui_update.Value = state;

    // send to queue
    if (xQueueSend(ui_update_queue, (void*)&ui_update, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "UI Update queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void UI_SetPresetLabel(uint16_t index, char* name)
{
    tUIUpdate ui_update;

    // build command
    ui_update.ElementID = UI_ELEMENT_PRESET_NAME;
    ui_update.Action = UI_ACTION_SET_LABEL_TEXT;
    sprintf(ui_update.Text, "%d: ", (int)index + usb_get_first_preset_index_for_connected_modeller());
    strncat(ui_update.Text, name, MAX_UI_TEXT - 1);

    // send to queue
    if (xQueueSend(ui_update_queue, (void*)&ui_update, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "UI Update queue send failed!");            
    }
}


/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
__attribute__((unused)) void UI_ShowToast(char* text)
{
    tUIUpdate ui_update;

    // build command
    ui_update.ElementID = UI_ELEMENT_TOAST;
    ui_update.Action = UI_ACTION_NONE;
    strncpy(ui_update.Text, text, MAX_UI_TEXT - 1);

    // send to queue
    if (xQueueSend(ui_update_queue, (void*)&ui_update, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "UI Update queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void UI_SetBankIndex(uint16_t index)
{
    tUIUpdate ui_update;

    // build command
    ui_update.ElementID = UI_ELEMENT_BANK_INDEX;
    ui_update.Action = UI_ACTION_SET_STATE;
    ui_update.Value = index;

    // send to queue
    if (xQueueSend(ui_update_queue, (void*)&ui_update, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "UI Update queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void UI_SetAmpSkin(uint16_t index)
{
    tUIUpdate ui_update;

    // build commands
    ui_update.ElementID = UI_ELEMENT_AMP_SKIN;
    ui_update.Action = UI_ACTION_SET_STATE;
    ui_update.Value = index;

    // send to queue
    if (xQueueSend(ui_update_queue, (void*)&ui_update, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "UI Update queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void UI_SetPresetDescription(char* text)
{
    tUIUpdate ui_update;

    // build command
    ui_update.ElementID = UI_ELEMENT_PRESET_DESCRIPTION;
    ui_update.Action = UI_ACTION_SET_ENTRY_TEXT;
    strncpy(ui_update.Text, text, MAX_UI_TEXT - 1);

    // send to queue
    if (xQueueSend(ui_update_queue, (void*)&ui_update, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "UI Update queue send failed!");            
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void UI_RefreshParameterValues(void)
{
#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
    tUIUpdate ui_update;

    // build command
    ui_update.Action = UI_ACTION_NONE;
    ui_update.ElementID = UI_ELEMENT_PARAMETERS;
    
    // send to queue
    if (xQueueSend(ui_update_queue, (void*)&ui_update, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "UI Update parameters send failed!");            
    }
#endif    
}

#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static uint32_t ui_get_skin_image(uint16_t index, uint8_t* buffer)
{
    char filename[50];
    lv_fs_file_t file;
    uint32_t size = 0;
    lv_fs_res_t res;
    uint32_t bytes_read = 0;
    uint32_t bytes_to_read;
    uint32_t chunk_bytes;

    switch (index)
    {
        // amps
        case AMP_SKIN_5150:
        {
            sprintf(filename, "A:/spiflash/images/5150.bin");
        } break;

        case AMP_SKIN_AC30:
        {
            sprintf(filename, "A:/spiflash/images/ac30.bin");
        } break;

        case AMP_SKIN_AMPEGCHROME:
        {
            sprintf(filename, "A:/spiflash/images/ampgchrm.bin");
        } break;

        case AMP_SKIN_BA500:
        {
            sprintf(filename, "A:/spiflash/images/ba500.bin");
        } break;

        case AMP_SKIN_DIEZEL:
        {
            sprintf(filename, "A:/spiflash/images/diezel.bin");
        } break;

        case AMP_SKIN_ELEGANTBLUE:
        {
            sprintf(filename, "A:/spiflash/images/elgntblu.bin");
        } break;

        case AMP_SKIN_EVH:
        {
            sprintf(filename, "A:/spiflash/images/evh.bin");
        } break;

        case AMP_SKIN_FENDERHOTROD:
        {
            sprintf(filename, "A:/spiflash/images/fndrhtrd.bin");
        } break;

        case AMP_SKIN_FENDERTWEEDBIG:
        {
            sprintf(filename, "A:/spiflash/images/fndrtwbg.bin");
        } break;

        case AMP_SKIN_FENDERTWIN:
        {
            sprintf(filename, "A:/spiflash/images/fndrtwin.bin");
        } break;

        case AMP_SKIN_FRIEDMANN:
        {
            sprintf(filename, "A:/spiflash/images/friedman.bin");
        } break;

        case AMP_SKIN_JBDUMBLE1:
        {
            sprintf(filename, "A:/spiflash/images/jbdumble.bin");
        } break;

        case AMP_SKIN_JCM:
        {
            sprintf(filename, "A:/spiflash/images/jcm.bin");
        } break;

        case AMP_SKIN_JETCITY:
        {
            sprintf(filename, "A:/spiflash/images/jetcity.bin");
        } break;

        case AMP_SKIN_JTM:
        {
            sprintf(filename, "A:/spiflash/images/jtm.bin");
        } break;

        case AMP_SKIN_MESABOOGIEDUAL:
        {
            sprintf(filename, "A:/spiflash/images/msbogdul.bin");
        } break;

        case AMP_SKIN_MESAMARKV:
        {
            sprintf(filename, "A:/spiflash/images/mesamkv.bin");
        } break;

        case AMP_SKIN_MESAMARKWOOD:
        {
            sprintf(filename, "A:/spiflash/images/msmkwd.bin");
        } break;

        case AMP_SKIN_MODERNBLACKPLEXI:
        {
            sprintf(filename, "A:/spiflash/images/mdnbkplx.bin");
        } break;

        case AMP_SKIN_MODERNWHITEPLEXI:
        {
            sprintf(filename, "A:/spiflash/images/mdnwhplx.bin");
        } break;

        case AMP_SKIN_ORANGEOR120:
        {
            sprintf(filename, "A:/spiflash/images/missing.bin");    //todo
        } break;

        case AMP_SKIN_ROLANDJAZZ:
        {
            sprintf(filename, "A:/spiflash/images/rolljazz.bin");
        } break;

        case AMP_SKIN_TONEXAMPBLACK:
        {
            sprintf(filename, "A:/spiflash/images/tnxablk.bin");
        } break;

        case AMP_SKIN_TONEXAMPRED:
        {
            sprintf(filename, "A:/spiflash/images/tnxared.bin");
        } break;

        case AMP_SKIN_SILVERFACE:
        {
            sprintf(filename, "A:/spiflash/images/slvrface.bin");
        } break;

        case AMP_SKIN_SUPRO:
        {
            sprintf(filename, "A:/spiflash/images/supro.bin");
        } break;

        case AMP_SKIN_WHITEMODERN:
        {
            sprintf(filename, "A:/spiflash/images/whtmdrn.bin");
        } break;

        case AMP_SKIN_WOODAMP:
        {
            sprintf(filename, "A:/spiflash/images/woodamp.bin");
        } break;

        // pedals
#if 0   //to do        
        case PEDAL_SKIN_BIGMUFF:
        {
            result = (lv_obj_t*)&img_pskin_bigmuff;
        } break;

        case PEDAL_SKIN_BOSSBLACK:
        {
            result = (lv_obj_t*)&img_pskin_bossblack;
        } break;

        case PEDAL_SKIN_BOSSSILVER:
        {
            result = (lv_obj_t*)&img_pskin_bosssilver;
        } break;

        case PEDAL_SKIN_BOSSYELLOW:
        {
            result = (lv_obj_t*)&img_pskin_bossyellow;
        } break;

        case PEDAL_SKIN_FUZZRED:
        {
            result = (lv_obj_t*)&img_pskin_fuzzred;
        } break;

        case PEDAL_SKIN_FUZZSILVER:
        {
            result = (lv_obj_t*)&img_pskin_fuzzsilver;
        } break;

        case PEDAL_SKIN_IBANEZBLUE:
        {
            result = (lv_obj_t*)&img_pskin_ibanezblue;
        } break;

        case PEDAL_SKIN_IBANEZDARKBLUE:
        {
            result = (lv_obj_t*)&img_pskin_ibanezdarkblue;
        } break;

        case PEDAL_SKIN_IBANEZGREEN:
        {
            result = (lv_obj_t*)&img_pskin_ibanezgreen;
        } break;

        case PEDAL_SKIN_IBANEZRED:
        {
            result = (lv_obj_t*)&img_pskin_ibanezred;
        } break;

        case PEDAL_SKIN_KLONGOLD:
        {
            result = (lv_obj_t*)&img_pskin_klongold;
        } break;

        case PEDAL_SKIN_LIFEPEDAL:
        {
            result = (lv_obj_t*)&img_pskin_lifepedal;
        } break;

        case PEDAL_SKIN_MORNINGGLORY:
        {
            result = (lv_obj_t*)&img_pskin_morningglory;
        } break;

        case PEDAL_SKIN_MXRDOUBLEBLACK:
        {
            result = (lv_obj_t*)&img_pskin_mxrdoubleblack;
        } break;

        case PEDAL_SKIN_MXRDOUBLERED:
        {
            result = (lv_obj_t*)&img_pskin_mxrdoublered;
        } break;

        case PEDAL_SKIN_MXRSINGLEBLACK:
        {
            result = (lv_obj_t*)&img_pskin_mxrsingleblack;
        } break;

        case PEDAL_SKIN_MXRSINGLEGOLD:
        {
            result = (lv_obj_t*)&img_pskin_mxrsinglegold;
        } break;

        case PEDAL_SKIN_MXRSINGLEGREEN:
        {
            result = (lv_obj_t*)&img_pskin_mxrsinglegreen;
        } break;

        case PEDAL_SKIN_MXRSINGLEORANGE:
        {
            result = (lv_obj_t*)&img_pskin_mxrsingleorange;
        } break;

        case PEDAL_SKIN_MXRSINGLEWHITE:
        {
            result = (lv_obj_t*)&img_pskin_mxrsinglewhite;
        } break;

        case PEDAL_SKIN_MXRSINGLEYELLOW:
        {
            result = (lv_obj_t*)&img_pskin_mxrsingleyellow;
        } break;

        case PEDAL_SKIN_RATYELLOW:
        {
            result = (lv_obj_t*)&img_pskin_ratyellow;
        } break;
#endif 

        default:
        {
            sprintf(filename, "A:/spiflash/images/jcm.bin");
        } break;
    }

    res = lv_fs_open(&file, filename, LV_FS_MODE_RD);
    if (res != LV_FS_RES_OK) 
    {
        ESP_LOGW(TAG, "Failed to open skin %s", filename);
        return 0;
    }

    // seek to end
    res = lv_fs_seek(&file, 0, LV_FS_SEEK_END);
    if (res != LV_FS_RES_OK)
    {
        ESP_LOGW(TAG, "Failed to seek skin %s", filename);
        return 0;
    }
    
    // get file size
    res = lv_fs_tell(&file, &size);
    if (res != LV_FS_RES_OK) 
    {
        ESP_LOGW(TAG, "Failed to get file size of skin %s", filename);
        return 0;
    }

    // seek back to start of file
    lv_fs_seek(&file, 0, LV_FS_SEEK_SET);

    bytes_to_read = size;
    #define CHUNK_SIZE  4096
    uint8_t* temp_buf = malloc(CHUNK_SIZE);

    // read entire file into PSRAM buffer, but in chunks to help avoid DMA stalls on display
    while (bytes_to_read > 0)
    {
        if (bytes_to_read > CHUNK_SIZE)
        {
            chunk_bytes = CHUNK_SIZE;
        }
        else
        {
            chunk_bytes = bytes_to_read;
        }
        
        // read to temp buffer in IRAM
        res = lv_fs_read(&file, (char*)temp_buf, chunk_bytes, &bytes_read);
        if (res == LV_FS_RES_OK) 
        {
            // copy to PSRAM buffer
            memcpy((void*)buffer, (void*)temp_buf, bytes_read);
            bytes_to_read -= bytes_read;
            buffer += bytes_read;
            //vTaskDelay(1);

            // debug
            //ESP_LOGW(TAG, "Read chunk %d %d %d", chunk_bytes, bytes_read, bytes_to_read);
        } 
        else 
        {
            ESP_LOGW(TAG, "Failed to read skin %s", filename);
            break;
        }
    }

    free(temp_buf);

    if (res == LV_FS_RES_OK) 
    {
        ESP_LOGI(TAG, "Skin %s loaded, size %d", filename, (int)size);
    } 
    else 
    {
        ESP_LOGW(TAG, "Failed to read skin %s", filename);
    }

    lv_fs_close(&file);

    if (res == LV_FS_RES_OK) 
    {
        return size;
    }
    else
    {
        return 0;
    }
}
#endif 

#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void updateIconOrder()
{
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
}
#endif

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static uint8_t update_ui_element(tUIUpdate* update)
{
#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
    lv_obj_t* element_1 = NULL;

    switch (update->ElementID)
    {
        case UI_ELEMENT_USB_STATUS:
        {
            element_1 = objects.ui_usb_status_fail;
        } break;

        case UI_ELEMENT_BT_STATUS:
        {
            element_1 = objects.ui_bt_status_conn;
        } break;

        case UI_ELEMENT_WIFI_STATUS:
        {
            element_1 = objects.ui_wi_fi_status_conn;
        } break;

        case UI_ELEMENT_PRESET_NAME:
        {
            element_1 = objects.ui_preset_heading_label;
        } break;

        case UI_ELEMENT_BANK_INDEX:
        {
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
            element_1 = objects.ui_bank_value_label;
#endif
        } break;

        case UI_ELEMENT_AMP_SKIN:
        {
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
            element_1 = objects.ui_skin_image;
#endif            
        } break;

        case UI_ELEMENT_PRESET_DESCRIPTION:
        {
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
            element_1 = objects.ui_preset_details_text_area;
#endif            
        } break;

        case UI_ELEMENT_PARAMETERS:
        {
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
            ESP_LOGI(TAG, "Syncing params to UI");

            tTonexParameter* param_ptr;

            for (uint16_t param = 0; param < TONEX_GLOBAL_LAST; param++)
            {                     
                if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
                {
                    tTonexParameter* param_entry = &param_ptr[param];

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
                            updateIconOrder();
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
                        } break;

                        case TONEX_PARAM_NOISE_GATE_RELEASE:
                        {
                            lv_slider_set_range(objects.ui_noise_gate_release_slider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(objects.ui_noise_gate_release_slider, round(param_entry->Value), LV_ANIM_OFF);                            
                        } break;

                        case TONEX_PARAM_NOISE_GATE_DEPTH:
                        {                            
                            lv_slider_set_range(objects.ui_noise_gate_depth_slider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(objects.ui_noise_gate_depth_slider, round(param_entry->Value), LV_ANIM_OFF);
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
                            updateIconOrder();
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
                        } break;

                        case TONEX_PARAM_COMP_MAKE_UP:
                        {
                            lv_slider_set_range(objects.ui_compressor_gain_slider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(objects.ui_compressor_gain_slider, round(param_entry->Value), LV_ANIM_OFF);                            
                        } break;

                        case TONEX_PARAM_COMP_ATTACK:
                        {
                            lv_slider_set_range(objects.ui_compressor_attack_slider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(objects.ui_compressor_attack_slider, round(param_entry->Value), LV_ANIM_OFF);                            
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
                            updateIconOrder();
                        } break;

                        case TONEX_PARAM_EQ_BASS:
                        {
                            lv_slider_set_range(objects.ui_eq_bass_slider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                            lv_slider_set_value(objects.ui_eq_bass_slider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);                        
                        } break;

                        case TONEX_PARAM_EQ_BASS_FREQ:
                        {
                            // not exposed via UI
                        } break;

                        case TONEX_PARAM_EQ_MID:
                        {
                            lv_slider_set_range(objects.ui_eq_mid_slider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                            lv_slider_set_value(objects.ui_eq_mid_slider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);                            
                        } break;

                        case TONEX_PARAM_EQ_MIDQ:
                        {
                            lv_slider_set_range(objects.ui_eq_mid_qslider, round(param_entry->Min * 10.0f), round(param_entry->Max * 10.0f));
                            lv_slider_set_value(objects.ui_eq_mid_qslider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);                            
                        } break;

                        case TONEX_PARAM_EQ_MID_FREQ:
                        {
                            // not exposed via UI
                        } break;

                        case TONEX_PARAM_EQ_TREBLE:
                        {                            
                            lv_slider_set_range(objects.ui_eq_treble_slider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                            lv_slider_set_value(objects.ui_eq_treble_slider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);
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
                        } break;

                        case TONEX_PARAM_MODEL_VOLUME:
                        {
                            lv_slider_set_range(objects.ui_amplifier_volume_slider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                            lv_slider_set_value(objects.ui_amplifier_volume_slider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);                            
                        } break;

                        case TONEX_PARAM_MODEX_MIX:
                        {
                            // not exposed via UI
                        } break;

                        case TONEX_PARAM_MODEL_PRESENCE:
                        {                            
                            lv_slider_set_range(objects.ui_amplifier_presense_slider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                            lv_slider_set_value(objects.ui_amplifier_presense_slider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);
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
                            updateIconOrder();
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
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING1_PREDELAY:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_1)
                            {                          
                                lv_slider_set_range(objects.ui_reverb_predelay_slider, round(param_entry->Min), round(param_entry->Max));  
                                lv_slider_set_value(objects.ui_reverb_predelay_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING1_COLOR:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_1)
                            {                            
                                lv_slider_set_range(objects.ui_reverb_color_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_color_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING1_MIX:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_1)
                            {                           
                                lv_slider_set_range(objects.ui_reverb_mix_slider, round(param_entry->Min), round(param_entry->Max)); 
                                lv_slider_set_value(objects.ui_reverb_mix_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING2_TIME:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_2)
                            {                                                            
                                lv_slider_set_range(objects.ui_reverb_time_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_time_slider, round(param_entry->Value), LV_ANIM_OFF);
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING2_PREDELAY:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_2)
                            {                            
                                lv_slider_set_range(objects.ui_reverb_predelay_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_predelay_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING2_COLOR:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_2)
                            {                                                            
                                lv_slider_set_range(objects.ui_reverb_color_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_color_slider, round(param_entry->Value), LV_ANIM_OFF);
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING2_MIX:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_2)
                            {                            
                                lv_slider_set_range(objects.ui_reverb_mix_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_mix_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING3_TIME:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_3)
                            {             
                                lv_slider_set_range(objects.ui_reverb_time_slider, round(param_entry->Min), round(param_entry->Max));               
                                lv_slider_set_value(objects.ui_reverb_time_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING3_PREDELAY:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_3)
                            {              
                                lv_slider_set_range(objects.ui_reverb_predelay_slider, round(param_entry->Min), round(param_entry->Max));              
                                lv_slider_set_value(objects.ui_reverb_predelay_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING3_COLOR:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_3)
                            {                            
                                lv_slider_set_range(objects.ui_reverb_color_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_color_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING3_MIX:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_3)
                            {                            
                                lv_slider_set_range(objects.ui_reverb_mix_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_mix_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING4_TIME:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_4)
                            {                    
                                lv_slider_set_range(objects.ui_reverb_time_slider, round(param_entry->Min), round(param_entry->Max));        
                                lv_slider_set_value(objects.ui_reverb_time_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING4_PREDELAY:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_4)
                            {                            
                                lv_slider_set_range(objects.ui_reverb_predelay_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_predelay_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING4_COLOR:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_4)
                            {                            
                                lv_slider_set_range(objects.ui_reverb_color_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_color_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING4_MIX:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_4)
                            {                            
                                lv_slider_set_range(objects.ui_reverb_mix_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_mix_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_ROOM_TIME:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_ROOM)
                            {                        
                                lv_slider_set_range(objects.ui_reverb_time_slider, round(param_entry->Min), round(param_entry->Max));    
                                lv_slider_set_value(objects.ui_reverb_time_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_ROOM_PREDELAY:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_ROOM)
                            {                            
                                lv_slider_set_range(objects.ui_reverb_predelay_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_predelay_slider, round(param_entry->Value), LV_ANIM_OFF);                               
                            }
                        } break;

                        case TONEX_PARAM_REVERB_ROOM_COLOR:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_ROOM)
                            {                            
                                lv_slider_set_range(objects.ui_reverb_color_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_color_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_ROOM_MIX:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_ROOM)
                            {                            
                                lv_slider_set_range(objects.ui_reverb_mix_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_mix_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_PLATE_TIME:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_PLATE)
                            {                            
                                lv_slider_set_range(objects.ui_reverb_time_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_time_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_PLATE_PREDELAY:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_PLATE)
                            {                    
                                lv_slider_set_range(objects.ui_reverb_predelay_slider, round(param_entry->Min), round(param_entry->Max));        
                                lv_slider_set_value(objects.ui_reverb_predelay_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_PLATE_COLOR:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_PLATE)
                            {                            
                                lv_slider_set_range(objects.ui_reverb_color_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_color_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_PLATE_MIX:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_PLATE)
                            {                            
                                lv_slider_set_range(objects.ui_reverb_mix_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_reverb_mix_slider, round(param_entry->Value), LV_ANIM_OFF);                                
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
                            updateIconOrder();
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
                                } break;

                                case TONEX_MODULATION_TREMOLO:
                                {
                                    lv_label_set_text(objects.ui_modulation_param1_label, "Rate");
                                    lv_label_set_text(objects.ui_modulation_param2_label, "Shape");
                                    lv_label_set_text(objects.ui_modulation_param3_label, "Spread");
                                    lv_label_set_text(objects.ui_modulation_param4_label, "Level");
                                    lv_obj_clear_flag(objects.ui_modulation_param4_label, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_clear_flag(objects.ui_modulation_param4_slider, LV_OBJ_FLAG_HIDDEN);
                                } break;

                                case TONEX_MODULATION_PHASER:
                                {
                                    lv_label_set_text(objects.ui_modulation_param1_label, "Rate");
                                    lv_label_set_text(objects.ui_modulation_param2_label, "Depth");
                                    lv_label_set_text(objects.ui_modulation_param3_label, "Level");
                                    lv_obj_add_flag(objects.ui_modulation_param4_label, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_add_flag(objects.ui_modulation_param4_slider, LV_OBJ_FLAG_HIDDEN);
                                } break;

                                case TONEX_MODULATION_FLANGER:
                                {
                                    lv_label_set_text(objects.ui_modulation_param1_label, "Rate");
                                    lv_label_set_text(objects.ui_modulation_param2_label, "Depth");
                                    lv_label_set_text(objects.ui_modulation_param3_label, "Feedback");
                                    lv_label_set_text(objects.ui_modulation_param4_label, "Level");
                                    lv_obj_clear_flag(objects.ui_modulation_param4_label, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_clear_flag(objects.ui_modulation_param4_slider, LV_OBJ_FLAG_HIDDEN);
                                } break;

                                case TONEX_MODULATION_ROTARY:
                                {
                                    lv_label_set_text(objects.ui_modulation_param1_label, "Speed");
                                    lv_label_set_text(objects.ui_modulation_param2_label, "Radius");
                                    lv_label_set_text(objects.ui_modulation_param3_label, "Spread");
                                    lv_label_set_text(objects.ui_modulation_param4_label, "Level");
                                    lv_obj_clear_flag(objects.ui_modulation_param4_label, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_clear_flag(objects.ui_modulation_param4_slider, LV_OBJ_FLAG_HIDDEN);
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
                                    lv_obj_clear_flag(objects.ui_modulation_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                                else
                                {
                                    lv_obj_clear_state(objects.ui_modulation_sync_switch, LV_STATE_CHECKED);
                                    lv_obj_clear_flag(objects.ui_modulation_param1_slider, LV_OBJ_FLAG_HIDDEN);
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
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_CHORUS_DEPTH:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_CHORUS)
                            { 
                                lv_slider_set_range(objects.ui_modulation_param2_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_modulation_param2_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_CHORUS_LEVEL:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_CHORUS)
                            { 
                                lv_slider_set_range(objects.ui_modulation_param3_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_modulation_param3_slider, round(param_entry->Value), LV_ANIM_OFF);                                
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
                                    lv_obj_clear_flag(objects.ui_modulation_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                                else
                                {
                                    lv_obj_clear_state(objects.ui_modulation_sync_switch, LV_STATE_CHECKED);
                                    lv_obj_clear_flag(objects.ui_modulation_param1_slider, LV_OBJ_FLAG_HIDDEN);
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
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_TREMOLO_SHAPE:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_TREMOLO)
                            { 
                                lv_slider_set_range(objects.ui_modulation_param2_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_modulation_param2_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_TREMOLO_SPREAD:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_TREMOLO)
                            { 
                                lv_slider_set_range(objects.ui_modulation_param3_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_modulation_param3_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_TREMOLO_LEVEL:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_TREMOLO)
                            { 
                                lv_slider_set_range(objects.ui_modulation_param4_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_modulation_param4_slider, round(param_entry->Value), LV_ANIM_OFF);                                
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
                                    lv_obj_clear_flag(objects.ui_modulation_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                                else
                                {
                                    lv_obj_clear_state(objects.ui_modulation_sync_switch, LV_STATE_CHECKED);
                                    lv_obj_clear_flag(objects.ui_modulation_param1_slider, LV_OBJ_FLAG_HIDDEN);
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
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_PHASER_DEPTH:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_PHASER)
                            { 
                                lv_slider_set_range(objects.ui_modulation_param2_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_modulation_param2_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_PHASER_LEVEL:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_PHASER)
                            { 
                                lv_slider_set_range(objects.ui_modulation_param3_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_modulation_param3_slider, round(param_entry->Value), LV_ANIM_OFF);                                
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
                                    lv_obj_clear_flag(objects.ui_modulation_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                                else
                                {
                                    lv_obj_clear_state(objects.ui_modulation_sync_switch, LV_STATE_CHECKED);
                                    lv_obj_clear_flag(objects.ui_modulation_param1_slider, LV_OBJ_FLAG_HIDDEN);
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
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_FLANGER_DEPTH:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_FLANGER)
                            { 
                                lv_slider_set_range(objects.ui_modulation_param2_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_modulation_param2_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_FLANGER_FEEDBACK:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_FLANGER)
                            { 
                                lv_slider_set_range(objects.ui_modulation_param3_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_modulation_param3_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_FLANGER_LEVEL:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_FLANGER)
                            { 
                                lv_slider_set_range(objects.ui_modulation_param4_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_modulation_param4_slider, round(param_entry->Value), LV_ANIM_OFF);                                
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
                                    lv_obj_clear_flag(objects.ui_modulation_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                                else
                                {
                                    lv_obj_clear_state(objects.ui_modulation_sync_switch, LV_STATE_CHECKED);
                                    lv_obj_clear_flag(objects.ui_modulation_param1_slider, LV_OBJ_FLAG_HIDDEN);
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
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_ROTARY_RADIUS:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_ROTARY)
                            { 
                                lv_slider_set_range(objects.ui_modulation_param2_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_modulation_param2_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_ROTARY_SPREAD:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_ROTARY)
                            { 
                                lv_slider_set_range(objects.ui_modulation_param3_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_modulation_param3_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_ROTARY_LEVEL:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_ROTARY)
                            { 
                                lv_slider_set_range(objects.ui_modulation_param4_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_modulation_param4_slider, round(param_entry->Value), LV_ANIM_OFF);                                
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
                            updateIconOrder();
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
                                    lv_obj_clear_flag(objects.ui_delay_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                                else
                                {
                                    lv_obj_clear_state(objects.ui_delay_sync_switch, LV_STATE_CHECKED);
                                    lv_obj_clear_flag(objects.ui_delay_ts_slider, LV_OBJ_FLAG_HIDDEN);
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
                            }
                        } break;

                        case TONEX_PARAM_DELAY_DIGITAL_FEEDBACK:
                        {
                            if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_DIGITAL)
                            { 
                                lv_slider_set_range(objects.ui_delay_feedback_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_delay_feedback_slider, round(param_entry->Value), LV_ANIM_OFF);                                
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
                                    lv_obj_clear_flag(objects.ui_delay_ts_dropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                                else
                                {
                                    lv_obj_clear_state(objects.ui_delay_sync_switch, LV_STATE_CHECKED);
                                    lv_obj_clear_flag(objects.ui_delay_ts_slider, LV_OBJ_FLAG_HIDDEN);
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
                            }
                        } break;

                        case TONEX_PARAM_DELAY_TAPE_FEEDBACK:
                        {
                            if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_TAPE)
                            { 
                                lv_slider_set_range(objects.ui_delay_feedback_slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(objects.ui_delay_feedback_slider, round(param_entry->Value), LV_ANIM_OFF);                                
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
                        } break;
                        
                        case TONEX_GLOBAL_TUNING_REFERENCE:
                        {                            
                            lv_slider_set_range(objects.ui_tuning_reference_slider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(objects.ui_tuning_reference_slider, round(param_entry->Value), LV_ANIM_OFF);                                
                        } break;

                        case TONEX_GLOBAL_MASTER_VOLUME:
                        {                            
                            lv_slider_set_range(objects.ui_volume_slider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(objects.ui_volume_slider, round(param_entry->Value), LV_ANIM_OFF);                                
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
            ESP_LOGI(TAG, "Syncing params to UI");

            tTonexParameter* param_ptr;

            for (uint16_t param = 0; param < TONEX_GLOBAL_LAST; param++)
            {                     
                if (tonex_params_get_locked_access(&param_ptr) == ESP_OK)
                {
                    tTonexParameter* param_entry = &param_ptr[param];

                    // debug
                    //ESP_LOGI(TAG, "Param %d: val: %02f, min: %02f, max: %02f", param, param_entry->Value, param_entry->Min, param_entry->Max);

                    switch (param)
                    {
                        case TONEX_GLOBAL_BPM:
                        {
                            char buf[128];
                            sprintf(buf, "%d", (int)round(param_entry->Value));
                            lv_label_set_text(ui_BPM, buf);  
                        } break;

                        case TONEX_PARAM_COMP_ENABLE:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_set_style_border_color(ui_CStatus, lv_color_hex(0xDDDD00), LV_PART_MAIN | LV_STATE_DEFAULT);
                            }
                            else
                            {
                                lv_obj_set_style_border_color(ui_CStatus, lv_color_hex(0x563F2A), LV_PART_MAIN | LV_STATE_DEFAULT);
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_ENABLE:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_set_style_border_color(ui_MStatus, lv_color_hex(0xEEAA00), LV_PART_MAIN | LV_STATE_DEFAULT);
                            }
                            else
                            {
                                lv_obj_set_style_border_color(ui_MStatus, lv_color_hex(0x563F2A), LV_PART_MAIN | LV_STATE_DEFAULT);
                            }
                        } break;

                        case TONEX_PARAM_DELAY_ENABLE:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_set_style_border_color(ui_DStatus, lv_color_hex(0x00CC00), LV_PART_MAIN | LV_STATE_DEFAULT);
                            }
                            else
                            {
                                lv_obj_set_style_border_color(ui_DStatus, lv_color_hex(0x563F2A), LV_PART_MAIN | LV_STATE_DEFAULT);
                            }
                        } break;

                        case TONEX_PARAM_REVERB_ENABLE:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_set_style_border_color(ui_RStatus, lv_color_hex(0x33FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
                            }
                            else
                            {
                                lv_obj_set_style_border_color(ui_RStatus, lv_color_hex(0x563F2A), LV_PART_MAIN | LV_STATE_DEFAULT);
                            }
                        } break;

                    }

                    tonex_params_release_locked_access();
                }               
            }
#endif
        } break;

        case UI_ELEMENT_TOAST:
        {
            ui_show_toast(update->Text);
        } break;

        default:
        {
            ESP_LOGE(TAG, "Unknown display elment %d", update->ElementID);     
            return 0;        
        } break;
    }
    
    // check the action
    switch (update->Action)
    {
        case UI_ACTION_SET_STATE:
        {
            // check the element
            if (element_1 == objects.ui_usb_status_fail)
            {
                if (update->Value == 0)
                {
                    // show the USB disconnected image
                    lv_obj_add_flag(objects.ui_usb_status_ok, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_usb_status_fail, LV_OBJ_FLAG_HIDDEN);
                }
                else
                {
                    // show the USB connected image
                    lv_obj_add_flag(objects.ui_usb_status_fail, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_usb_status_ok, LV_OBJ_FLAG_HIDDEN);
                }
            }
            else if (element_1 == objects.ui_bt_status_conn)
            {
                if (update->Value == 0)
                {
                    // show the BT disconnected image
                    lv_obj_add_flag(objects.ui_bt_status_conn, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_bt_status_disconn, LV_OBJ_FLAG_HIDDEN);
                }
                else
                {
                    // show the BT connected image
                    lv_obj_add_flag(objects.ui_bt_status_disconn, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_bt_status_conn, LV_OBJ_FLAG_HIDDEN);
                }
            }
            else if (element_1 == objects.ui_wi_fi_status_conn)
            {
                if (update->Value == 0)
                {
                    ESP_LOGI(TAG, "Show WiFi disconn");

                    // show the Wifi disconnected image
                    lv_obj_add_flag(objects.ui_wi_fi_status_conn, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_wi_fi_status_disconn, LV_OBJ_FLAG_HIDDEN);
                }
                else
                {
                    ESP_LOGI(TAG, "Show WiFi conn");

                    // show the WiFi connected image
                    lv_obj_add_flag(objects.ui_wi_fi_status_disconn, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(objects.ui_wi_fi_status_conn, LV_OBJ_FLAG_HIDDEN);
                }
            }
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
            else if (element_1 == objects.ui_skin_image)
            {
                // is the skin image different from what we have loaded?
                if (update->Value != current_skin_image)
                {
                    current_skin_image = update->Value;

                    // try to load skin image
                    uint32_t file_size = ui_get_skin_image(update->Value, skin_image_buffer);
                    if (file_size > 0)
                    {        
                        // build the image descriptor
                        lv_img_dsc_t img_dsc;

                        // Copy the 4-byte header
                        memcpy((void*)&img_dsc.header, (void*)skin_image_buffer, sizeof(lv_img_header_t));  

                        // set the size
                        img_dsc.data_size = file_size - sizeof(lv_img_header_t);

                        // set the data
                        img_dsc.data = (const uint8_t *)(skin_image_buffer + sizeof(lv_img_header_t));

                        lv_img_set_src(objects.ui_skin_image, &img_dsc);
                    }
                    else
                    {
                        ESP_LOGW(TAG, "Skin image load failed");
                    }
                }
            }
            else if (element_1 == objects.ui_bank_value_label)
            {
                // set Bank index
                char buf[128];
                sprintf(buf, "%d", (int)round(update->Value) + 1);
                lv_label_set_text(objects.ui_bank_value_label, buf);
            }
#endif            
        } break;

        case UI_ACTION_SET_LABEL_TEXT:
        {
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
            lv_label_set_text(element_1, update->Text);
#elif CONFIG_TONEX_CONTROLLER_DISPLAY_SMALL
            if (element_1 == objects.ui_preset_heading_label)
            {
                // split up preset into 2 text lines.
                // incoming has "XX: Name"
                char preset_index[16];
                char preset_name[33];

                // get the preset number
                sprintf(preset_index, "%d", atoi(update->Text));
                lv_label_set_text(objects.ui_preset_heading_label, preset_index);

                // get the preset name
                for (uint8_t loop = 0; loop < 4; loop++)
                {
                    if (update->Text[loop] == ':')
                    {
                        strncpy(preset_name, (const char*)&update->Text[loop + 2], sizeof(preset_name) - 1);
                        lv_label_set_text(objects.ui_preset_heading_label2, preset_name);
                        break;
                    }
                }
            }
            else
            {
                lv_label_set_text(element_1, update->Text);
            }
#endif            
        } break;

        case UI_ACTION_SET_ENTRY_TEXT:
        {
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
            lv_textarea_set_text(element_1, update->Text);
#endif            
        } break;

        case UI_ACTION_NONE:
        {
            // nothing needed
        } break;

        default:
        {
            ESP_LOGE(TAG, "Unknown display action %d, element %d", (int)update->Action, (int)update->ElementID);
        } break;
    }
#endif 

    return 1;
}

#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI && CONFIG_TONEX_CONTROLLER_SHOW_BPM_INDICATOR

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void ui_anim_opacity_cb(void *obj, int32_t value) 
{
    lv_obj_set_style_opa((lv_obj_t*)obj, value, 0);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void ui_anim_deleted_cb(lv_anim_t *anim) 
{
    if (anim->user_data) 
    {
        lv_mem_free(anim->user_data);
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void ui_BPMAnimate(lv_obj_t *target_obj, uint32_t duration)
{    
    // Delete any existing animations on the target object to avoid conflicts
    lv_anim_del(target_obj, (lv_anim_exec_xcb_t)ui_anim_opacity_cb);

    // Allocate user data for the animation
    void *user_data = lv_mem_alloc(sizeof(uint8_t));
    if (!user_data) 
    {
        return;
    }

    lv_anim_t anim;

    lv_anim_init(&anim);
    lv_anim_set_var(&anim, target_obj);
    lv_anim_set_time(&anim, duration);
    lv_anim_set_user_data(&anim, user_data);
    lv_anim_set_exec_cb(&anim, ui_anim_opacity_cb);
    lv_anim_set_values(&anim, 255, 0); // Fade from opaque to transparent
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
    lv_anim_set_delay(&anim, 0);
    lv_anim_set_deleted_cb(&anim, ui_anim_deleted_cb);
    lv_anim_set_repeat_count(&anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&anim, 0);
    lv_anim_set_early_apply(&anim, true);

    // Start the animation
    lv_anim_start(&anim);
}
#endif

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void __attribute__((unused)) ui_toast_close(void) 
{
    ESP_LOGI(TAG, "Closing message box");

    // Close and delete the message box
    lv_msgbox_close(msgbox_data.mbox);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void __attribute__((unused)) ui_init_toast(void) 
{
    // Initialize styles
    msgbox_data.style_main = lv_mem_alloc(sizeof(lv_style_t));
    msgbox_data.style_text = lv_mem_alloc(sizeof(lv_style_t));
    if (!msgbox_data.style_main || !msgbox_data.style_text) 
    {
        ESP_LOGE(TAG, "Failed to allocate memory for styles");
        free(msgbox_data.style_main);
        free(msgbox_data.style_text);
        return;
    }

    lv_style_init(msgbox_data.style_main);
    lv_style_set_bg_color(msgbox_data.style_main, lv_color_hex(0x2A2A2A));
    lv_style_set_border_width(msgbox_data.style_main, 6);                 
    lv_style_set_radius(msgbox_data.style_main, 10);                      
    lv_style_set_bg_opa(msgbox_data.style_main, LV_OPA_COVER);            
    lv_style_set_pad_all(msgbox_data.style_main, platform_get_toast_padding());      
    lv_style_set_border_color(msgbox_data.style_main, lv_color_hex(0x563F2A));

    lv_style_init(msgbox_data.style_text);
    lv_style_set_text_color(msgbox_data.style_text, lv_color_hex(0xFFFFFF));

    // font size depends on screen size, let platform tell us
    lv_style_set_text_font(msgbox_data.style_text, platform_get_toast_font()); 
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void __attribute__((unused)) ui_show_toast(char* contents) 
{
    if (msgbox_data.mbox != NULL)
    {
        lv_obj_del(msgbox_data.mbox);
        msgbox_data.mbox = NULL;
    }

    // Create message box (no buttons for auto-close)
    static const char *btns[] = {""}; // Empty button list
    msgbox_data.mbox = lv_msgbox_create(NULL, NULL, contents, btns, false);
    if (!msgbox_data.mbox) 
    {
        ESP_LOGE(TAG, "Failed to create message box");
        return;
    }
    
    lv_obj_center(msgbox_data.mbox);

    // Apply styles
    lv_obj_add_style(msgbox_data.mbox, msgbox_data.style_main, LV_PART_MAIN); // Style background
    lv_obj_add_style(lv_msgbox_get_text(msgbox_data.mbox), msgbox_data.style_text, 0);  // Style message

    // Create timer to close and delete message box after 3 seconds
    msgbox_data.timer = xTaskGetTickCount() + 3000; 
    msgbox_data.active = 1;

    ESP_LOGI(TAG, "Message box created");
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void display_task(void *arg)
{
    tUIUpdate ui_update;

    ESP_LOGI(TAG, "Display task start");

    while (1) 
    {
        // Lock the mutex due to the LVGL APIs are not thread-safe
        if (display_lvgl_lock(pdMS_TO_TICKS(1000))) 
        {
            lv_task_handler();
            ui_tick();

            // check for any UI update messages
            if (xQueueReceive(ui_update_queue, (void*)&ui_update, 0) == pdPASS)
            {
                // process it
                update_ui_element(&ui_update);
            }

            // handle timed toast messages
            if (msgbox_data.active)
            {
                if (xTaskGetTickCount() >= msgbox_data.timer)
                {
                    // clean up and reset
                    ui_toast_close();
                    msgbox_data.active = 0;
                }
            }

            // Release the mutex
            display_lvgl_unlock();
	    }
        else
        {
            ESP_LOGW(TAG, "Display lock timeout");
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void display_init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex, lv_disp_drv_t* pdisp_drv)
{    
    I2CMutexHandle = I2CMutex;
    disp_drv = pdisp_drv;

    // create queue for UI updates from other threads
    ui_update_queue = xQueueCreate(20, sizeof(tUIUpdate));
    if (ui_update_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create UI update queue!");
    }

    lvgl_mux = xSemaphoreCreateRecursiveMutex();
    assert(lvgl_mux);

#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &display_increase_lvgl_tick,
        .name = "lvgl_tick"
    };

    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, DISPLAY_LVGL_TICK_PERIOD_MS * 1000));

    vTaskDelay(pdMS_TO_TICKS(10));

    // allocate buffer for Skin images
    skin_image_buffer = (uint8_t*)heap_caps_malloc(MAX_SKIN_IMAGE_SIZE, MALLOC_CAP_SPIRAM);
    if (skin_image_buffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate skin image buffer!");    
    }
    memset((void*)skin_image_buffer, 0, MAX_SKIN_IMAGE_SIZE);

    // init GUI
    ESP_LOGI(TAG, "Init UI");
    ui_init();

    // init mem
    memset((void*)&msgbox_data, 0, sizeof(msgbox_data));

    // init toast
    ui_init_toast();

    // create display task
    xTaskCreatePinnedToCore(display_task, "Dsp", DISPLAY_TASK_STACK_SIZE, NULL, DISPLAY_TASK_PRIORITY, NULL, 1);
#endif
}
