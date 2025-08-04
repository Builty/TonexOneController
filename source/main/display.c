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
    static lv_anim_t *ui_BPMAnimation = NULL;
    static lv_anim_t PropertyAnimation_0;
    void ui_BPMAnimate(lv_obj_t *TargetObject, uint32_t duration);
#endif

#define DISPLAY_LVGL_TICK_PERIOD_MS     2
#define DISPLAY_LVGL_TASK_MAX_DELAY_MS  500
#define DISPLAY_LVGL_TASK_MIN_DELAY_MS  1
#define BUF_SIZE                        (1024)
#define I2C_MASTER_TIMEOUT_MS           1000
#define MAX_UI_TEXT                     130

enum UIElements
{
    UI_ELEMENT_USB_STATUS,
    UI_ELEMENT_BT_STATUS,
    UI_ELEMENT_WIFI_STATUS,
    UI_ELEMENT_PRESET_NAME,
    UI_ELEMENT_BANK_INDEX,
    UI_ELEMENT_AMP_SKIN,
    UI_ELEMENT_PRESET_DESCRIPTION,
    UI_ELEMENT_PARAMETERS
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

static QueueHandle_t ui_update_queue;
static SemaphoreHandle_t I2CMutexHandle;
static SemaphoreHandle_t lvgl_mux = NULL;
static lv_disp_drv_t* disp_drv; 
                
#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY

#if CONFIG_TONEX_CONTROLLER_HAS_TOUCH
static uint8_t __attribute__((unused)) touch_data_ready_to_read = 0;
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
void PreviousClicked(lv_event_t * e)
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
void NextClicked(lv_event_t * e)
{
    // called from LVGL 
    ESP_LOGI(TAG, "UI Next Click/Swipe");    

    control_request_preset_up();        
}

#else   //CONFIG_TONEX_CONTROLLER_HAS_TOUCH

// Dummy functions so that 1.69 and 1.69 Touch can share the same UI project
void PreviousClicked(lv_event_t * e)
{
}

void NextClicked(lv_event_t * e)
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
void toggle_effect_gate(lv_event_t * e)
{
    tTonexParameter* param_ptr;
    float value;

    // called from LVGL 
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

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void toggle_effect_amp(lv_event_t * e)
{
    tTonexParameter* param_ptr;
    float value;

    // called from LVGL 
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

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void toggle_effect_cab(lv_event_t * e)
{
    tTonexParameter* param_ptr;
    float value;

    // called from LVGL 
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

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void toggle_effect_comp(lv_event_t * e)
{
    tTonexParameter* param_ptr;
    float value;

    // called from LVGL 
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

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void toggle_effect_mod(lv_event_t * e)
{
    tTonexParameter* param_ptr;
    float value;

    // called from LVGL 
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

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void toggle_effect_delay(lv_event_t * e)
{
    tTonexParameter* param_ptr;
    float value;

    // called from LVGL 
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

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void toggle_effect_reverb(lv_event_t * e)
{
    tTonexParameter* param_ptr;
    float value;

    // called from LVGL 
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

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void show_settings_tab(lv_event_t * e)
{
	lv_obj_t* target = lv_event_get_current_target(e);

    if (target == ui_IconEQ)
    {
        // show EQ settings
        lv_tabview_set_act(ui_SettingsTabview, CONFIG_TAB_EQ, LV_ANIM_OFF);
    }
    else if (target == ui_IconGate)
    {
        // show gate settings
        lv_tabview_set_act(ui_SettingsTabview, CONFIG_TAB_GATE, LV_ANIM_OFF);
    }
    else if ((target == ui_IconAmp) || (target == ui_IconCab))
    {
        // show amp settings
        lv_tabview_set_act(ui_SettingsTabview, CONFIG_TAB_AMPLIFIER, LV_ANIM_OFF);
    }
    else if (target == ui_IconComp)
    {
        // show comnpressor settings
        lv_tabview_set_act(ui_SettingsTabview, CONFIG_TAB_COMPRESSOR, LV_ANIM_OFF);
    }
    else if (target == ui_IconMod)
    {
        // show modulation settings
        lv_tabview_set_act(ui_SettingsTabview, CONFIG_TAB_MODULATION, LV_ANIM_OFF);
    }
    else if (target == ui_IconDelay)
    {
        // show delay settings
        lv_tabview_set_act(ui_SettingsTabview, CONFIG_TAB_DELAY, LV_ANIM_OFF);
    }
    else if (target == ui_IconReverb)
    {
        // show reverb settings
        lv_tabview_set_act(ui_SettingsTabview, CONFIG_TAB_REVERB, LV_ANIM_OFF);
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void AmpSkinPrevious(lv_event_t * e)
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
void AmpSkinNext(lv_event_t * e)
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
void SaveUserDataRequest(lv_event_t * e)
{
    control_save_user_data(0);
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
void PresetDescriptionChanged(lv_event_t * e)
{
    char* text = (char*)lv_textarea_get_text(ui_PresetDetailsTextArea);

    ESP_LOGI(TAG, "PresetDescriptionChanged: %s", text);

    control_set_user_text(text);      
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void ParameterChanged(lv_event_t * e)
{
    // get the object that was changed
    lv_obj_t* obj = lv_event_get_current_target(e);

    ESP_LOGI(TAG, "Parameter changed");

    // see what it was, and update the pedal
    if (obj == ui_NoiseGateSwitch)
    {
        usb_modify_parameter(TONEX_PARAM_NOISE_GATE_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == ui_NoiseGatePostSwitch)
    {
        usb_modify_parameter(TONEX_PARAM_NOISE_GATE_POST, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == ui_NoiseGateThresholdSlider)
    {
        usb_modify_parameter(TONEX_PARAM_NOISE_GATE_THRESHOLD, lv_slider_get_value(obj));
    }
    else if (obj == ui_NoiseGateReleaseSlider)
    {
        usb_modify_parameter(TONEX_PARAM_NOISE_GATE_RELEASE, lv_slider_get_value(obj));
    }
    else if (obj == ui_NoiseGateDepthSlider)
    {
        usb_modify_parameter(TONEX_PARAM_NOISE_GATE_DEPTH, lv_slider_get_value(obj));
    }
    else if (obj == ui_CompressorEnableSwitch)
    {
        usb_modify_parameter(TONEX_PARAM_COMP_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == ui_CompressorPostSwitch)
    {
        usb_modify_parameter(TONEX_PARAM_COMP_POST, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == ui_CompressorThresholdSlider)
    {
        usb_modify_parameter(TONEX_PARAM_COMP_THRESHOLD, lv_slider_get_value(obj));
    }
    else if (obj == ui_CompresorAttackSlider)
    {
        usb_modify_parameter(TONEX_PARAM_COMP_ATTACK, lv_slider_get_value(obj));
    }
    else if (obj == ui_CompressorGainSlider)
    {
        usb_modify_parameter(TONEX_PARAM_COMP_MAKE_UP, lv_slider_get_value(obj));
    }
    else if (obj == ui_EQPostSwitch)
    {
        usb_modify_parameter(TONEX_PARAM_EQ_POST, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == ui_EQBassSlider)
    {
        usb_modify_parameter(TONEX_PARAM_EQ_BASS, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == ui_EQMidSlider)
    {
        usb_modify_parameter(TONEX_PARAM_EQ_MID, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == ui_EQMidQSlider)
    {
        usb_modify_parameter(TONEX_PARAM_EQ_MIDQ, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == ui_EQTrebleSlider)
    {
        usb_modify_parameter(TONEX_PARAM_EQ_TREBLE, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == ui_ReverbEnableSwitch)
    {
        usb_modify_parameter(TONEX_PARAM_REVERB_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == ui_ReverbPostSwitch)
    {
        usb_modify_parameter(TONEX_PARAM_REVERB_POSITION, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == ui_ReverbModelDropdown)
    {
        usb_modify_parameter(TONEX_PARAM_REVERB_MODEL, lv_dropdown_get_selected(obj));
    }
    else if (obj == ui_ReverbMixSlider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(ui_ReverbModelDropdown))
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
    else if (obj == ui_ReverbTimeSlider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(ui_ReverbModelDropdown))
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
    else if (obj == ui_ReverbPredelaySlider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(ui_ReverbModelDropdown))
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
    else if (obj == ui_ReverbColorSlider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(ui_ReverbModelDropdown))
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
    else if (obj == ui_ModulationEnableSwitch)
    {
        usb_modify_parameter(TONEX_PARAM_MODULATION_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == ui_ModulationPostSwitch)
    {
        usb_modify_parameter(TONEX_PARAM_MODULATION_POST, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == ui_ModulationModelDropdown)
    {
        usb_modify_parameter(TONEX_PARAM_MODULATION_MODEL, lv_dropdown_get_selected(obj));
    }
    else if (obj == ui_ModulationSyncSwitch)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(ui_ModulationModelDropdown))
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
    else if (obj == ui_ModulationTSDropdown)
    {
        switch (lv_dropdown_get_selected(ui_ModulationModelDropdown))
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
    else if (obj == ui_ModulationParam1Slider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(ui_ModulationModelDropdown))
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
    else if (obj == ui_ModulationParam2Slider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(ui_ModulationModelDropdown))
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
    else if (obj == ui_ModulationParam3Slider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(ui_ModulationModelDropdown))
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
    else if (obj == ui_ModulationParam4Slider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(ui_ModulationModelDropdown))
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
    else if (obj == ui_DelayEnableSwitch)
    {
        usb_modify_parameter(TONEX_PARAM_DELAY_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == ui_DelayPostSwitch)
    {
        usb_modify_parameter(TONEX_PARAM_DELAY_POST, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == ui_DelayModelDropdown)
    {
        usb_modify_parameter(TONEX_PARAM_DELAY_MODEL, lv_dropdown_get_selected(obj));
    }
    else if (obj == ui_DelaySyncSwitch)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(ui_DelayModelDropdown))
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
    else if (obj == ui_DelayPingPongSwitch)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(ui_DelayModelDropdown))
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
    else if (obj == ui_DelayTSSlider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(ui_DelayModelDropdown))
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
    else if (obj == ui_DelayTSDropdown)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(ui_DelayModelDropdown))
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
    else if (obj == ui_DelayFeedbackSlider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(ui_DelayModelDropdown))
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
    else if (obj == ui_DelayMixSlider)
    {
        // check which model is set
        switch (lv_dropdown_get_selected(ui_DelayModelDropdown))
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
    else if (obj == ui_AmpEnableSwitch)
    {
        usb_modify_parameter(TONEX_PARAM_MODEL_AMP_ENABLE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == ui_CabinetModeDropdown)
    {
        usb_modify_parameter(TONEX_PARAM_CABINET_TYPE, lv_dropdown_get_selected(obj));
    } 
    else if (obj == ui_AmplifierGainSlider)
    {
        usb_modify_parameter(TONEX_PARAM_MODEL_GAIN, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == ui_AmplifierVolumeSlider)
    {
        usb_modify_parameter(TONEX_PARAM_MODEL_VOLUME, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == ui_AmplifierPresenseSlider)
    {
        usb_modify_parameter(TONEX_PARAM_MODEL_PRESENCE, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == ui_AmplifierDepthSlider)
    {
        usb_modify_parameter(TONEX_PARAM_MODEL_DEPTH, ((float)lv_slider_get_value(obj))/10.0f);
    }
    else if (obj == ui_BPMSlider)
    {
        usb_modify_parameter(TONEX_GLOBAL_BPM, lv_slider_get_value(obj));
    }
    else if (obj == ui_InputTrimSlider)
    {
        usb_modify_parameter(TONEX_GLOBAL_INPUT_TRIM, lv_slider_get_value(obj));
    }
    else if (obj == ui_CabBypassSwitch)
    {
        usb_modify_parameter(TONEX_GLOBAL_CABSIM_BYPASS, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == ui_TempoSourceSwitch)
    {
        usb_modify_parameter(TONEX_GLOBAL_TEMPO_SOURCE, lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0);
    }
    else if (obj == ui_TuningReferenceSlider)
    {
        usb_modify_parameter(TONEX_GLOBAL_TUNING_REFERENCE, lv_slider_get_value(obj));
    }
    else if (obj == ui_VolumeSlider)
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
static lv_obj_t* ui_get_skin_image(uint16_t index)
{
    lv_obj_t* result;

    switch (index)
    {
#if CONFIG_TONEX_CONTROLLER_SKINS_AMP        
        // amps
        case AMP_SKIN_5150:
        {
            result = (lv_obj_t*)&ui_img_skin_5150_png;
        } break;

        case AMP_SKIN_AC30:
        {
            result = (lv_obj_t*)&ui_img_skin_ac30_png;
        } break;

        case AMP_SKIN_AMPEGCHROME:
        {
            result = (lv_obj_t*)&ui_img_skin_ampegchrome_png;
        } break;

        case AMP_SKIN_BA500:
        {
            result = (lv_obj_t*)&ui_img_skin_ba500_png;
        } break;

        case AMP_SKIN_DIEZEL:
        {
            result = (lv_obj_t*)&ui_img_skin_diezel_png;
        } break;

        case AMP_SKIN_ELEGANTBLUE:
        {
            result = (lv_obj_t*)&ui_img_skin_elegantblue_png;
        } break;

        case AMP_SKIN_EVH:
        {
            result = (lv_obj_t*)&ui_img_skin_evh_png;
        } break;

        case AMP_SKIN_FENDERHOTROD:
        {
            result = (lv_obj_t*)&ui_img_skin_fenderhotrod_png;
        } break;

        case AMP_SKIN_FENDERTWEEDBIG:
        {
            result = (lv_obj_t*)&ui_img_skin_fendertweedbig_png;
        } break;

        case AMP_SKIN_FENDERTWIN:
        {
            result = (lv_obj_t*)&ui_img_skin_fendertwin_png;
        } break;

        case AMP_SKIN_FRIEDMANN:
        {
            result = (lv_obj_t*)&ui_img_skin_friedmann_png;
        } break;

        case AMP_SKIN_JBDUMBLE1:
        {
            result = (lv_obj_t*)&ui_img_skin_jbdumble1_png;
        } break;

        case AMP_SKIN_JCM:
        {
            result = (lv_obj_t*)&ui_img_skin_jcm_png;
        } break;

        case AMP_SKIN_JETCITY:
        {
            result = (lv_obj_t*)&ui_img_skin_jetcity_png;
        } break;

        case AMP_SKIN_JTM:
        {
            result = (lv_obj_t*)&ui_img_skin_jtm_png;
        } break;

        case AMP_SKIN_MESABOOGIEDUAL:
        {
            result = (lv_obj_t*)&ui_img_skin_mesaboogiedual_png;
        } break;

        case AMP_SKIN_MESAMARKV:
        {
            result = (lv_obj_t*)&ui_img_skin_mesamarkv_png;
        } break;

        case AMP_SKIN_MESAMARKWOOD:
        {
            result = (lv_obj_t*)&ui_img_skin_mesamarkwood_png;
        } break;

        case AMP_SKIN_MODERNBLACKPLEXI:
        {
            result = (lv_obj_t*)&ui_img_skin_modernblackplexi_png;
        } break;

        case AMP_SKIN_MODERNWHITEPLEXI:
        {
            result = (lv_obj_t*)&ui_img_skin_modernwhiteplexi_png;
        } break;

        case AMP_SKIN_ORANGEOR120:
        {
            result = (lv_obj_t*)&ui_img_skin_orangeor120_png;
        } break;

        case AMP_SKIN_ROLANDJAZZ:
        {
            result = (lv_obj_t*)&ui_img_skin_rolandjazz_png;
        } break;

        case AMP_SKIN_TONEXAMPBLACK:
        {
            result = (lv_obj_t*)&ui_img_skin_tonexampblack_png;
        } break;

        case AMP_SKIN_TONEXAMPRED:
        {
            result = (lv_obj_t*)&ui_img_skin_tonexampred_png;
        } break;

        case AMP_SKIN_SILVERFACE:
        {
            result = (lv_obj_t*)&ui_img_skin_silverface_png;
        } break;

        case AMP_SKIN_SUPRO:
        {
            result = (lv_obj_t*)&ui_img_skin_supro_png;
        } break;

        case AMP_SKIN_WHITEMODERN:
        {
            result = (lv_obj_t*)&ui_img_skin_whitemodern_png;
        } break;

        case AMP_SKIN_WOODAMP:
        {
            result = (lv_obj_t*)&ui_img_skin_woodamp_png;
        } break;
#endif  //CONFIG_TONEX_CONTROLLER_SKINS_AMP

#if CONFIG_TONEX_CONTROLLER_SKINS_PEDAL
        // pedals
        case PEDAL_SKIN_BIGMUFF:
        {
            result = (lv_obj_t*)&ui_img_pskin_bigmuff_png;
        } break;

        case PEDAL_SKIN_BOSSBLACK:
        {
            result = (lv_obj_t*)&ui_img_pskin_bossblack_png;
        } break;

        case PEDAL_SKIN_BOSSSILVER:
        {
            result = (lv_obj_t*)&ui_img_pskin_bosssilver_png;
        } break;

        case PEDAL_SKIN_BOSSYELLOW:
        {
            result = (lv_obj_t*)&ui_img_pskin_bossyellow_png;
        } break;

        case PEDAL_SKIN_FUZZRED:
        {
            result = (lv_obj_t*)&ui_img_pskin_fuzzred_png;
        } break;

        case PEDAL_SKIN_FUZZSILVER:
        {
            result = (lv_obj_t*)&ui_img_pskin_fuzzsilver_png;
        } break;

        case PEDAL_SKIN_IBANEZBLUE:
        {
            result = (lv_obj_t*)&ui_img_pskin_ibanezblue_png;
        } break;

        case PEDAL_SKIN_IBANEZDARKBLUE:
        {
            result = (lv_obj_t*)&ui_img_pskin_ibanezdarkblue_png;
        } break;

        case PEDAL_SKIN_IBANEZGREEN:
        {
            result = (lv_obj_t*)&ui_img_pskin_ibanezgreen_png;
        } break;

        case PEDAL_SKIN_IBANEZRED:
        {
            result = (lv_obj_t*)&ui_img_pskin_ibanezred_png;
        } break;

        case PEDAL_SKIN_KLONGOLD:
        {
            result = (lv_obj_t*)&ui_img_pskin_klongold_png;
        } break;

        case PEDAL_SKIN_LIFEPEDAL:
        {
            result = (lv_obj_t*)&ui_img_pskin_lifepedal_png;
        } break;

        case PEDAL_SKIN_MORNINGGLORY:
        {
            result = (lv_obj_t*)&ui_img_pskin_morningglory_png;
        } break;

        case PEDAL_SKIN_MXRDOUBLEBLACK:
        {
            result = (lv_obj_t*)&ui_img_pskin_mxrdoubleblack_png;
        } break;

        case PEDAL_SKIN_MXRDOUBLERED:
        {
            result = (lv_obj_t*)&ui_img_pskin_mxrdoublered_png;
        } break;

        case PEDAL_SKIN_MXRSINGLEBLACK:
        {
            result = (lv_obj_t*)&ui_img_pskin_mxrsingleblack_png;
        } break;

        case PEDAL_SKIN_MXRSINGLEGOLD:
        {
            result = (lv_obj_t*)&ui_img_pskin_mxrsinglegold_png;
        } break;

        case PEDAL_SKIN_MXRSINGLEGREEN:
        {
            result = (lv_obj_t*)&ui_img_pskin_mxrsinglegreen_png;
        } break;

        case PEDAL_SKIN_MXRSINGLEORANGE:
        {
            result = (lv_obj_t*)&ui_img_pskin_mxrsingleorange_png;
        } break;

        case PEDAL_SKIN_MXRSINGLEWHITE:
        {
            result = (lv_obj_t*)&ui_img_pskin_mxrsinglewhite_png;
        } break;

        case PEDAL_SKIN_MXRSINGLEYELLOW:
        {
            result = (lv_obj_t*)&ui_img_pskin_mxrsingleyellow_png;
        } break;

        case PEDAL_SKIN_RATYELLOW:
        {
            result = (lv_obj_t*)&ui_img_pskin_ratyellow_png;
        } break;
#endif //CONFIG_TONEX_CONTROLLER_SKINS_PEDAL

        default:
        {
            result = (lv_obj_t*)&ui_img_skin_jcm_png;            
        } break;
    }

    return result;
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
    bool gatePost = lv_obj_has_state(ui_NoiseGatePostSwitch, LV_STATE_CHECKED);
    bool compPost = lv_obj_has_state(ui_CompressorPostSwitch, LV_STATE_CHECKED);
    bool eqPost = lv_obj_has_state(ui_EQPostSwitch, LV_STATE_CHECKED);
    bool modPost = lv_obj_has_state(ui_ModulationPostSwitch, LV_STATE_CHECKED);
    bool delayPost = lv_obj_has_state(ui_DelayPostSwitch, LV_STATE_CHECKED);
    bool revPost = lv_obj_has_state(ui_ReverbPostSwitch, LV_STATE_CHECKED);

    lv_obj_t *icons[8];
    uint8_t index = 0;

    if (!gatePost)
    {
        icons[index++] = ui_IconGate;
    }
    if (!compPost)
    {
        icons[index++] = ui_IconComp;
    }
    if (!modPost)
    {
        icons[index++] = ui_IconMod;
    }
    if (!delayPost)
    {
        icons[index++] = ui_IconDelay;
    }
    if (!eqPost)
    {
        icons[index++] = ui_IconEQ;
    }
    icons[index++] = ui_IconAmp;
    icons[index++] = ui_IconCab;
    if (eqPost)
    {
        icons[index++] = ui_IconEQ;
    }
    if (gatePost)
    {
        icons[index++] = ui_IconGate;
    }
    if (compPost)
    {
        icons[index++] = ui_IconComp;
    }
    if (!revPost)
    {
        icons[index++] = ui_IconReverb;
    }
    if (modPost)
    {
        icons[index++] = ui_IconMod;
    }
    if (delayPost)
    {
        icons[index++] = ui_IconDelay;
    }
    if (revPost)
    {
        icons[index] = ui_IconReverb;
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
            element_1 = ui_USBStatusFail;
        } break;

        case UI_ELEMENT_BT_STATUS:
        {
            element_1 = ui_BTStatusConn;
        } break;

        case UI_ELEMENT_WIFI_STATUS:
        {
            element_1 = ui_WiFiStatusConn;
        } break;

        case UI_ELEMENT_PRESET_NAME:
        {
            element_1 = ui_PresetHeadingLabel;
        } break;

        case UI_ELEMENT_BANK_INDEX:
        {
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
            element_1 = ui_BankValueLabel;
#endif
        } break;

        case UI_ELEMENT_AMP_SKIN:
        {
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
            element_1 = ui_SkinImage;
#endif            
        } break;

        case UI_ELEMENT_PRESET_DESCRIPTION:
        {
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
            element_1 = ui_PresetDetailsTextArea;
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
                                lv_obj_add_state(ui_NoiseGatePostSwitch, LV_STATE_CHECKED);
                            }
                            else
                            {
                                lv_obj_clear_state(ui_NoiseGatePostSwitch, LV_STATE_CHECKED);
                            }
                            updateIconOrder();
                        } break;

                        case TONEX_PARAM_NOISE_GATE_ENABLE:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_add_state(ui_NoiseGateSwitch, LV_STATE_CHECKED);
                                lv_img_set_src(ui_IconGate, (lv_obj_t*)&ui_img_effect_icon_gate_on_png);
                            }
                            else
                            {
                                lv_obj_clear_state(ui_NoiseGateSwitch, LV_STATE_CHECKED);
                                lv_img_set_src(ui_IconGate, (lv_obj_t*)&ui_img_effect_icon_gate_off_png);
                            }
                        } break;

                        case TONEX_PARAM_NOISE_GATE_THRESHOLD:
                        {                            
                            lv_slider_set_range(ui_NoiseGateThresholdSlider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(ui_NoiseGateThresholdSlider, round(param_entry->Value), LV_ANIM_OFF);
                        } break;

                        case TONEX_PARAM_NOISE_GATE_RELEASE:
                        {
                            lv_slider_set_range(ui_NoiseGateReleaseSlider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(ui_NoiseGateReleaseSlider, round(param_entry->Value), LV_ANIM_OFF);                            
                        } break;

                        case TONEX_PARAM_NOISE_GATE_DEPTH:
                        {                            
                            lv_slider_set_range(ui_NoiseGateDepthSlider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(ui_NoiseGateDepthSlider, round(param_entry->Value), LV_ANIM_OFF);
                        } break;

                        case TONEX_PARAM_COMP_POST:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_add_state(ui_CompressorPostSwitch, LV_STATE_CHECKED);
                            }
                            else
                            {
                                lv_obj_clear_state(ui_CompressorPostSwitch, LV_STATE_CHECKED);
                            }
                            updateIconOrder();
                        } break;

                        case TONEX_PARAM_COMP_ENABLE:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_add_state(ui_CompressorEnableSwitch, LV_STATE_CHECKED);
                                lv_img_set_src(ui_IconComp, (lv_obj_t*)&ui_img_effect_icon_comp_on_png);
                            }
                            else
                            {
                                lv_obj_clear_state(ui_CompressorEnableSwitch, LV_STATE_CHECKED);
                                lv_img_set_src(ui_IconComp, (lv_obj_t*)&ui_img_effect_icon_comp_off_png);
                            }
                        } break;

                        case TONEX_PARAM_COMP_THRESHOLD:
                        {                            
                            lv_slider_set_range(ui_CompressorThresholdSlider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(ui_CompressorThresholdSlider, round(param_entry->Value), LV_ANIM_OFF);
                        } break;

                        case TONEX_PARAM_COMP_MAKE_UP:
                        {
                            lv_slider_set_range(ui_CompressorGainSlider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(ui_CompressorGainSlider, round(param_entry->Value), LV_ANIM_OFF);                            
                        } break;

                        case TONEX_PARAM_COMP_ATTACK:
                        {
                            lv_slider_set_range(ui_CompresorAttackSlider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(ui_CompresorAttackSlider, round(param_entry->Value), LV_ANIM_OFF);                            
                        } break;

                        case TONEX_PARAM_EQ_POST:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_add_state(ui_EQPostSwitch, LV_STATE_CHECKED);
                            }
                            else
                            {
                                lv_obj_clear_state(ui_EQPostSwitch, LV_STATE_CHECKED);
                            }
                            updateIconOrder();
                        } break;

                        case TONEX_PARAM_EQ_BASS:
                        {
                            lv_slider_set_range(ui_EQBassSlider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                            lv_slider_set_value(ui_EQBassSlider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);                        
                        } break;

                        case TONEX_PARAM_EQ_BASS_FREQ:
                        {
                            // not exposed via UI
                        } break;

                        case TONEX_PARAM_EQ_MID:
                        {
                            lv_slider_set_range(ui_EQMidSlider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                            lv_slider_set_value(ui_EQMidSlider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);                            
                        } break;

                        case TONEX_PARAM_EQ_MIDQ:
                        {
                            lv_slider_set_range(ui_EQMidQSlider, round(param_entry->Min * 10.0f), round(param_entry->Max * 10.0f));
                            lv_slider_set_value(ui_EQMidQSlider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);                            
                        } break;

                        case TONEX_PARAM_EQ_MID_FREQ:
                        {
                            // not exposed via UI
                        } break;

                        case TONEX_PARAM_EQ_TREBLE:
                        {                            
                            lv_slider_set_range(ui_EQTrebleSlider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                            lv_slider_set_value(ui_EQTrebleSlider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);
                        } break;

                        case TONEX_PARAM_EQ_TREBLE_FREQ:
                        {
                            // not exposed via UI    
                        } break;
                        
                        case TONEX_PARAM_MODEL_AMP_ENABLE:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_add_state(ui_AmpEnableSwitch, LV_STATE_CHECKED);
                                lv_img_set_src(ui_IconAmp, (lv_obj_t*)&ui_img_effect_icon_amp_on_png);
                            }
                            else
                            {
                                lv_obj_clear_state(ui_AmpEnableSwitch, LV_STATE_CHECKED);
                                lv_img_set_src(ui_IconAmp, (lv_obj_t*)&ui_img_effect_icon_amp_off_png);
                            }
                        } break;

                        case TONEX_PARAM_MODEL_SW1:
                        {
                            // not exposed via UI
                        } break;

                        case TONEX_PARAM_CABINET_TYPE:
                        {
                            lv_dropdown_set_selected(ui_CabinetModeDropdown, param_entry->Value);

                            if (param_entry->Value == TONEX_CABINET_DISABLED)
                            {
                                lv_img_set_src(ui_IconCab, (lv_obj_t*)&ui_img_effect_icon_cab_off_png);
                            }
                            else
                            {
                                lv_img_set_src(ui_IconCab, (lv_obj_t*)&ui_img_effect_icon_cab_on_png);
                            }
                        } break;

                        case TONEX_PARAM_MODEL_GAIN:
                        {
                            lv_slider_set_range(ui_AmplifierGainSlider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                            lv_slider_set_value(ui_AmplifierGainSlider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);                            
                        } break;

                        case TONEX_PARAM_MODEL_VOLUME:
                        {
                            lv_slider_set_range(ui_AmplifierVolumeSlider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                            lv_slider_set_value(ui_AmplifierVolumeSlider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);                            
                        } break;

                        case TONEX_PARAM_MODEX_MIX:
                        {
                            // not exposed via UI
                        } break;

                        case TONEX_PARAM_MODEL_PRESENCE:
                        {                            
                            lv_slider_set_range(ui_AmplifierPresenseSlider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                            lv_slider_set_value(ui_AmplifierPresenseSlider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);
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
                            lv_slider_set_range(ui_AmplifierDepthSlider, round(param_entry->Min), round(param_entry->Max * 10.0f));
                            lv_slider_set_value(ui_AmplifierDepthSlider, round(param_entry->Value * 10.0f), LV_ANIM_OFF);
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
                                lv_obj_add_state(ui_ReverbPostSwitch, LV_STATE_CHECKED);
                            }
                            else
                            {
                                lv_obj_clear_state(ui_ReverbPostSwitch, LV_STATE_CHECKED);
                            }
                            updateIconOrder();
                        } break;

                        case TONEX_PARAM_REVERB_ENABLE:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_add_state(ui_ReverbEnableSwitch, LV_STATE_CHECKED);

                                // show enabled icon with letter to indicate the type
                                switch ((int)param_ptr[TONEX_PARAM_REVERB_MODEL].Value)
                                {
                                    case TONEX_REVERB_SPRING_1:
                                    {
                                        lv_img_set_src(ui_IconReverb, (lv_obj_t*)&ui_img_effect_icon_reverb_on_s1_png);
                                    } break;

                                    case TONEX_REVERB_SPRING_2:
                                    {
                                        lv_img_set_src(ui_IconReverb, (lv_obj_t*)&ui_img_effect_icon_reverb_on_s2_png);
                                    } break;

                                    case TONEX_REVERB_SPRING_3:
                                    {
                                        lv_img_set_src(ui_IconReverb, (lv_obj_t*)&ui_img_effect_icon_reverb_on_s3_png);
                                    } break;

                                    case TONEX_REVERB_SPRING_4:
                                    {
                                        lv_img_set_src(ui_IconReverb, (lv_obj_t*)&ui_img_effect_icon_reverb_on_s4_png);
                                    } break;

                                    case TONEX_REVERB_ROOM:
                                    {
                                        lv_img_set_src(ui_IconReverb, (lv_obj_t*)&ui_img_effect_icon_reverb_on_r_png);
                                    } break;

                                    case TONEX_REVERB_PLATE:
                                    default:
                                    {
                                        lv_img_set_src(ui_IconReverb, (lv_obj_t*)&ui_img_effect_icon_reverb_on_p_png);
                                    } break;
                                }
                            }
                            else
                            {
                                lv_obj_clear_state(ui_ReverbEnableSwitch, LV_STATE_CHECKED);
                                lv_img_set_src(ui_IconReverb, (lv_obj_t*)&ui_img_effect_icon_reverb_off_png);
                            }
                        } break;

                        case TONEX_PARAM_REVERB_MODEL:
                        {
                            lv_dropdown_set_selected(ui_ReverbModelDropdown, param_entry->Value);
                        } break;

                        case TONEX_PARAM_REVERB_SPRING1_TIME:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_1)
                            {                            
                                lv_slider_set_range(ui_ReverbTimeSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbTimeSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING1_PREDELAY:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_1)
                            {                          
                                lv_slider_set_range(ui_ReverbPredelaySlider, round(param_entry->Min), round(param_entry->Max));  
                                lv_slider_set_value(ui_ReverbPredelaySlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING1_COLOR:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_1)
                            {                            
                                lv_slider_set_range(ui_ReverbColorSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbColorSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING1_MIX:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_1)
                            {                           
                                lv_slider_set_range(ui_ReverbMixSlider, round(param_entry->Min), round(param_entry->Max)); 
                                lv_slider_set_value(ui_ReverbMixSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING2_TIME:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_2)
                            {                                                            
                                lv_slider_set_range(ui_ReverbTimeSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbTimeSlider, round(param_entry->Value), LV_ANIM_OFF);
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING2_PREDELAY:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_2)
                            {                            
                                lv_slider_set_range(ui_ReverbPredelaySlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbPredelaySlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING2_COLOR:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_2)
                            {                                                            
                                lv_slider_set_range(ui_ReverbColorSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbColorSlider, round(param_entry->Value), LV_ANIM_OFF);
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING2_MIX:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_2)
                            {                            
                                lv_slider_set_range(ui_ReverbMixSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbMixSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING3_TIME:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_3)
                            {             
                                lv_slider_set_range(ui_ReverbTimeSlider, round(param_entry->Min), round(param_entry->Max));               
                                lv_slider_set_value(ui_ReverbTimeSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING3_PREDELAY:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_3)
                            {              
                                lv_slider_set_range(ui_ReverbPredelaySlider, round(param_entry->Min), round(param_entry->Max));              
                                lv_slider_set_value(ui_ReverbPredelaySlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING3_COLOR:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_3)
                            {                            
                                lv_slider_set_range(ui_ReverbColorSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbColorSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING3_MIX:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_3)
                            {                            
                                lv_slider_set_range(ui_ReverbMixSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbMixSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING4_TIME:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_4)
                            {                    
                                lv_slider_set_range(ui_ReverbTimeSlider, round(param_entry->Min), round(param_entry->Max));        
                                lv_slider_set_value(ui_ReverbTimeSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING4_PREDELAY:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_4)
                            {                            
                                lv_slider_set_range(ui_ReverbPredelaySlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbPredelaySlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING4_COLOR:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_4)
                            {                            
                                lv_slider_set_range(ui_ReverbColorSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbColorSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_SPRING4_MIX:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_SPRING_4)
                            {                            
                                lv_slider_set_range(ui_ReverbMixSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbMixSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_ROOM_TIME:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_ROOM)
                            {                        
                                lv_slider_set_range(ui_ReverbTimeSlider, round(param_entry->Min), round(param_entry->Max));    
                                lv_slider_set_value(ui_ReverbTimeSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_ROOM_PREDELAY:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_ROOM)
                            {                            
                                lv_slider_set_range(ui_ReverbPredelaySlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbPredelaySlider, round(param_entry->Value), LV_ANIM_OFF);                               
                            }
                        } break;

                        case TONEX_PARAM_REVERB_ROOM_COLOR:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_ROOM)
                            {                            
                                lv_slider_set_range(ui_ReverbColorSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbColorSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_ROOM_MIX:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_ROOM)
                            {                            
                                lv_slider_set_range(ui_ReverbMixSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbMixSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_PLATE_TIME:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_PLATE)
                            {                            
                                lv_slider_set_range(ui_ReverbTimeSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbTimeSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_PLATE_PREDELAY:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_PLATE)
                            {                    
                                lv_slider_set_range(ui_ReverbPredelaySlider, round(param_entry->Min), round(param_entry->Max));        
                                lv_slider_set_value(ui_ReverbPredelaySlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_PLATE_COLOR:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_PLATE)
                            {                            
                                lv_slider_set_range(ui_ReverbColorSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbColorSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_REVERB_PLATE_MIX:
                        {
                            if (param_ptr[TONEX_PARAM_REVERB_MODEL].Value == TONEX_REVERB_PLATE)
                            {                            
                                lv_slider_set_range(ui_ReverbMixSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ReverbMixSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_POST:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_add_state(ui_ModulationPostSwitch, LV_STATE_CHECKED);
                            }
                            else
                            {
                                lv_obj_clear_state(ui_ModulationPostSwitch, LV_STATE_CHECKED);
                            }
                            updateIconOrder();
                        } break;

                        case TONEX_PARAM_MODULATION_ENABLE:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_add_state(ui_ModulationEnableSwitch, LV_STATE_CHECKED);

                                // show enabled icon with letter to indicate the type
                                switch ((int)param_ptr[TONEX_PARAM_MODULATION_MODEL].Value)
                                {
                                    case TONEX_MODULATION_CHORUS:
                                    {
                                        lv_img_set_src(ui_IconMod, (lv_obj_t*)&ui_img_effect_icon_mod_on_chorus_png);
                                    } break;

                                    case TONEX_MODULATION_TREMOLO:
                                    {
                                        lv_img_set_src(ui_IconMod, (lv_obj_t*)&ui_img_effect_icon_mod_on_tremolo_png);
                                    } break;

                                    case TONEX_MODULATION_PHASER:
                                    {
                                        lv_img_set_src(ui_IconMod, (lv_obj_t*)&ui_img_effect_icon_mod_on_phaser_png);
                                    } break;

                                    case TONEX_MODULATION_FLANGER:
                                    {
                                        lv_img_set_src(ui_IconMod, (lv_obj_t*)&ui_img_effect_icon_mod_on_flanger_png);
                                    } break;

                                    case TONEX_MODULATION_ROTARY:
                                    default:
                                    {
                                        lv_img_set_src(ui_IconMod, (lv_obj_t*)&ui_img_effect_icon_mod_on_rotary_png);
                                    } break;
                                }
                            }
                            else
                            {
                                lv_obj_clear_state(ui_ModulationEnableSwitch, LV_STATE_CHECKED);
                                lv_img_set_src(ui_IconMod, (lv_obj_t*)&ui_img_effect_icon_mod_off_png);
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_MODEL:
                        {
                            lv_dropdown_set_selected(ui_ModulationModelDropdown, param_entry->Value);

                            // configure the variable UI items
                            switch ((int)param_entry->Value)
                            {
                                case TONEX_MODULATION_CHORUS:
                                {
                                    lv_label_set_text(ui_ModulationParam1Label, "Rate");
                                    lv_label_set_text(ui_ModulationParam2Label, "Depth");
                                    lv_label_set_text(ui_ModulationParam3Label, "Level");
                                    lv_obj_add_flag(ui_ModulationParam4Label, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_add_flag(ui_ModulationParam4Slider, LV_OBJ_FLAG_HIDDEN);
                                } break;

                                case TONEX_MODULATION_TREMOLO:
                                {
                                    lv_label_set_text(ui_ModulationParam1Label, "Rate");
                                    lv_label_set_text(ui_ModulationParam2Label, "Shape");
                                    lv_label_set_text(ui_ModulationParam3Label, "Spread");
                                    lv_label_set_text(ui_ModulationParam4Label, "Level");
                                    lv_obj_clear_flag(ui_ModulationParam4Label, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_clear_flag(ui_ModulationParam4Slider, LV_OBJ_FLAG_HIDDEN);
                                } break;

                                case TONEX_MODULATION_PHASER:
                                {
                                    lv_label_set_text(ui_ModulationParam1Label, "Rate");
                                    lv_label_set_text(ui_ModulationParam2Label, "Depth");
                                    lv_label_set_text(ui_ModulationParam3Label, "Level");
                                    lv_obj_add_flag(ui_ModulationParam4Label, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_add_flag(ui_ModulationParam4Slider, LV_OBJ_FLAG_HIDDEN);
                                } break;

                                case TONEX_MODULATION_FLANGER:
                                {
                                    lv_label_set_text(ui_ModulationParam1Label, "Rate");
                                    lv_label_set_text(ui_ModulationParam2Label, "Depth");
                                    lv_label_set_text(ui_ModulationParam3Label, "Feedback");
                                    lv_label_set_text(ui_ModulationParam4Label, "Level");
                                    lv_obj_clear_flag(ui_ModulationParam4Label, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_clear_flag(ui_ModulationParam4Slider, LV_OBJ_FLAG_HIDDEN);
                                } break;

                                case TONEX_MODULATION_ROTARY:
                                {
                                    lv_label_set_text(ui_ModulationParam1Label, "Speed");
                                    lv_label_set_text(ui_ModulationParam2Label, "Radius");
                                    lv_label_set_text(ui_ModulationParam3Label, "Spread");
                                    lv_label_set_text(ui_ModulationParam4Label, "Level");
                                    lv_obj_clear_flag(ui_ModulationParam4Label, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_clear_flag(ui_ModulationParam4Slider, LV_OBJ_FLAG_HIDDEN);
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
                                    lv_obj_add_state(ui_ModulationSyncSwitch, LV_STATE_CHECKED);
                                    lv_obj_add_flag(ui_ModulationParam1Slider, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_clear_flag(ui_ModulationTSDropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                                else
                                {
                                    lv_obj_clear_state(ui_ModulationSyncSwitch, LV_STATE_CHECKED);
                                    lv_obj_clear_flag(ui_ModulationParam1Slider, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_add_flag(ui_ModulationTSDropdown, LV_OBJ_FLAG_HIDDEN);
                                }                        
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_CHORUS_TS:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_CHORUS)
                            {
                                lv_dropdown_set_selected(ui_ModulationTSDropdown, param_entry->Value);
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_CHORUS_RATE:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_CHORUS)
                            { 
                                lv_slider_set_range(ui_ModulationParam1Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam1Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_CHORUS_DEPTH:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_CHORUS)
                            { 
                                lv_slider_set_range(ui_ModulationParam2Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam2Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_CHORUS_LEVEL:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_CHORUS)
                            { 
                                lv_slider_set_range(ui_ModulationParam3Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam3Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_TREMOLO_SYNC:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_TREMOLO)
                            {      
                                if (param_entry->Value)
                                {
                                    lv_obj_add_state(ui_ModulationSyncSwitch, LV_STATE_CHECKED);
                                    lv_obj_add_flag(ui_ModulationParam1Slider, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_clear_flag(ui_ModulationTSDropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                                else
                                {
                                    lv_obj_clear_state(ui_ModulationSyncSwitch, LV_STATE_CHECKED);
                                    lv_obj_clear_flag(ui_ModulationParam1Slider, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_add_flag(ui_ModulationTSDropdown, LV_OBJ_FLAG_HIDDEN);
                                }                        
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_TREMOLO_TS:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_TREMOLO)
                            {
                                lv_dropdown_set_selected(ui_ModulationTSDropdown, param_entry->Value);
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_TREMOLO_RATE:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_TREMOLO)
                            { 
                                lv_slider_set_range(ui_ModulationParam1Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam1Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_TREMOLO_SHAPE:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_TREMOLO)
                            { 
                                lv_slider_set_range(ui_ModulationParam2Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam2Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_TREMOLO_SPREAD:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_TREMOLO)
                            { 
                                lv_slider_set_range(ui_ModulationParam3Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam3Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_TREMOLO_LEVEL:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_TREMOLO)
                            { 
                                lv_slider_set_range(ui_ModulationParam4Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam4Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_PHASER_SYNC:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_PHASER)
                            {      
                                if (param_entry->Value)
                                {
                                    lv_obj_add_state(ui_ModulationSyncSwitch, LV_STATE_CHECKED);
                                    lv_obj_add_flag(ui_ModulationParam1Slider, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_clear_flag(ui_ModulationTSDropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                                else
                                {
                                    lv_obj_clear_state(ui_ModulationSyncSwitch, LV_STATE_CHECKED);
                                    lv_obj_clear_flag(ui_ModulationParam1Slider, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_add_flag(ui_ModulationTSDropdown, LV_OBJ_FLAG_HIDDEN);
                                }                        
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_PHASER_TS:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_PHASER)
                            {
                                lv_dropdown_set_selected(ui_ModulationTSDropdown, param_entry->Value);
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_PHASER_RATE:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_PHASER)
                            { 
                                lv_slider_set_range(ui_ModulationParam1Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam1Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_PHASER_DEPTH:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_PHASER)
                            { 
                                lv_slider_set_range(ui_ModulationParam2Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam2Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_PHASER_LEVEL:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_PHASER)
                            { 
                                lv_slider_set_range(ui_ModulationParam3Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam3Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_FLANGER_SYNC:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_FLANGER)
                            {      
                                if (param_entry->Value)
                                {
                                    lv_obj_add_state(ui_ModulationSyncSwitch, LV_STATE_CHECKED);
                                    lv_obj_add_flag(ui_ModulationParam1Slider, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_clear_flag(ui_ModulationTSDropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                                else
                                {
                                    lv_obj_clear_state(ui_ModulationSyncSwitch, LV_STATE_CHECKED);
                                    lv_obj_clear_flag(ui_ModulationParam1Slider, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_add_flag(ui_ModulationTSDropdown, LV_OBJ_FLAG_HIDDEN);
                                }                        
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_FLANGER_TS:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_FLANGER)
                            {
                                lv_dropdown_set_selected(ui_ModulationTSDropdown, param_entry->Value);
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_FLANGER_RATE:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_FLANGER)
                            { 
                                lv_slider_set_range(ui_ModulationParam1Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam1Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_FLANGER_DEPTH:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_FLANGER)
                            { 
                                lv_slider_set_range(ui_ModulationParam2Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam2Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_FLANGER_FEEDBACK:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_FLANGER)
                            { 
                                lv_slider_set_range(ui_ModulationParam3Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam3Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_FLANGER_LEVEL:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_FLANGER)
                            { 
                                lv_slider_set_range(ui_ModulationParam4Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam4Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_ROTARY_SYNC:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_ROTARY)
                            {      
                                if (param_entry->Value)
                                {
                                    lv_obj_add_state(ui_ModulationSyncSwitch, LV_STATE_CHECKED);
                                    lv_obj_add_flag(ui_ModulationParam1Slider, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_clear_flag(ui_ModulationTSDropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                                else
                                {
                                    lv_obj_clear_state(ui_ModulationSyncSwitch, LV_STATE_CHECKED);
                                    lv_obj_clear_flag(ui_ModulationParam1Slider, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_add_flag(ui_ModulationTSDropdown, LV_OBJ_FLAG_HIDDEN);
                                }                        
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_ROTARY_TS:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_ROTARY)
                            {
                                lv_dropdown_set_selected(ui_ModulationTSDropdown, param_entry->Value);
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_ROTARY_SPEED:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_ROTARY)
                            {                                 
                                lv_slider_set_range(ui_ModulationParam1Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam1Slider, round(param_entry->Value), LV_ANIM_OFF);
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_ROTARY_RADIUS:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_ROTARY)
                            { 
                                lv_slider_set_range(ui_ModulationParam2Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam2Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_ROTARY_SPREAD:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_ROTARY)
                            { 
                                lv_slider_set_range(ui_ModulationParam3Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam3Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_MODULATION_ROTARY_LEVEL:
                        {
                            if (param_ptr[TONEX_PARAM_MODULATION_MODEL].Value == TONEX_MODULATION_ROTARY)
                            { 
                                lv_slider_set_range(ui_ModulationParam4Slider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_ModulationParam4Slider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;
                        
                        case TONEX_PARAM_DELAY_POST:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_add_state(ui_DelayPostSwitch, LV_STATE_CHECKED);
                            }
                            else
                            {
                                lv_obj_clear_state(ui_DelayPostSwitch, LV_STATE_CHECKED);
                            }
                            updateIconOrder();
                        } break;

                        case TONEX_PARAM_DELAY_ENABLE:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_add_state(ui_DelayEnableSwitch, LV_STATE_CHECKED);

                                 // show enabled icon with letter to indicate the type
                                 switch ((int)param_ptr[TONEX_PARAM_DELAY_MODEL].Value)
                                 {
                                     case TONEX_DELAY_DIGITAL:
                                     {
                                         lv_img_set_src(ui_IconDelay, (lv_obj_t*)&ui_img_effect_icon_delay_on_d_png);
                                     } break;
 
                                     case TONEX_DELAY_TAPE:
                                     default:
                                     {
                                         lv_img_set_src(ui_IconDelay, (lv_obj_t*)&ui_img_effect_icon_delay_on_t_png);
                                     } break;
                                 }
                            }
                            else
                            {
                                lv_obj_clear_state(ui_DelayEnableSwitch, LV_STATE_CHECKED);
                                lv_img_set_src(ui_IconDelay, (lv_obj_t*)&ui_img_effect_icon_delay_off_png);
                            }
                        } break;

                        case TONEX_PARAM_DELAY_MODEL:
                        {
                            lv_dropdown_set_selected(ui_DelayModelDropdown, param_entry->Value);
                        } break;

                        case TONEX_PARAM_DELAY_DIGITAL_SYNC:
                        {
                            if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_DIGITAL)
                            {
                                if (param_entry->Value)
                                {
                                    lv_obj_add_state(ui_DelaySyncSwitch, LV_STATE_CHECKED);
                                    lv_obj_add_flag(ui_DelayTSSlider, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_clear_flag(ui_DelayTSDropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                                else
                                {
                                    lv_obj_clear_state(ui_DelaySyncSwitch, LV_STATE_CHECKED);
                                    lv_obj_clear_flag(ui_DelayTSSlider, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_add_flag(ui_DelayTSDropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                            }
                        } break;

                        case TONEX_PARAM_DELAY_DIGITAL_TS:
                        {
                            if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_DIGITAL)
                            {
                                lv_dropdown_set_selected(ui_DelayTSDropdown, param_entry->Value);
                            }
                        } break;

                        case TONEX_PARAM_DELAY_DIGITAL_TIME:
                        {
                            if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_DIGITAL)
                            { 
                                lv_slider_set_range(ui_DelayTSSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_DelayTSSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_DELAY_DIGITAL_FEEDBACK:
                        {
                            if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_DIGITAL)
                            { 
                                lv_slider_set_range(ui_DelayFeedbackSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_DelayFeedbackSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_DELAY_DIGITAL_MODE:
                        {
                            if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_DIGITAL)
                            {
                                if (param_entry->Value)
                                {
                                    lv_obj_add_state(ui_DelayPingPongSwitch, LV_STATE_CHECKED);
                                }
                                else
                                {
                                    lv_obj_clear_state(ui_DelayPingPongSwitch, LV_STATE_CHECKED);
                                }
                            }
                        } break;

                        case TONEX_PARAM_DELAY_DIGITAL_MIX:
                        {
                            if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_DIGITAL)
                            { 
                                lv_slider_set_range(ui_DelayMixSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_DelayMixSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_DELAY_TAPE_SYNC:
                        {
                            if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_TAPE)
                            {
                                if (param_entry->Value)
                                {
                                    lv_obj_add_state(ui_DelaySyncSwitch, LV_STATE_CHECKED);
                                    lv_obj_add_flag(ui_DelayTSSlider, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_clear_flag(ui_DelayTSDropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                                else
                                {
                                    lv_obj_clear_state(ui_DelaySyncSwitch, LV_STATE_CHECKED);
                                    lv_obj_clear_flag(ui_DelayTSSlider, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_add_flag(ui_DelayTSDropdown, LV_OBJ_FLAG_HIDDEN);
                                }
                            }
                        } break;

                        case TONEX_PARAM_DELAY_TAPE_TS:
                        {
                            if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_TAPE)
                            {
                                lv_dropdown_set_selected(ui_DelayTSDropdown, param_entry->Value);
                            }
                        } break;

                        case TONEX_PARAM_DELAY_TAPE_TIME:
                        {
                            if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_TAPE)
                            { 
                                lv_slider_set_range(ui_DelayTSSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_DelayTSSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_DELAY_TAPE_FEEDBACK:
                        {
                            if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_TAPE)
                            { 
                                lv_slider_set_range(ui_DelayFeedbackSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_DelayFeedbackSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_PARAM_DELAY_TAPE_MODE:
                        {
                            if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_TAPE)
                            {
                                if (param_entry->Value)
                                {
                                    lv_obj_add_state(ui_DelayPingPongSwitch, LV_STATE_CHECKED);
                                }
                                else
                                {
                                    lv_obj_clear_state(ui_DelayPingPongSwitch, LV_STATE_CHECKED);
                                }
                            }
                        } break;
                        
                        case TONEX_PARAM_DELAY_TAPE_MIX:
                        {
                            if (param_ptr[TONEX_PARAM_DELAY_MODEL].Value == TONEX_DELAY_TAPE)
                            { 
                                lv_slider_set_range(ui_DelayMixSlider, round(param_entry->Min), round(param_entry->Max));
                                lv_slider_set_value(ui_DelayMixSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                            }
                        } break;

                        case TONEX_GLOBAL_CABSIM_BYPASS:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_add_state(ui_CabBypassSwitch, LV_STATE_CHECKED);
                            }
                            else
                            {
                                lv_obj_clear_state(ui_CabBypassSwitch, LV_STATE_CHECKED);
                            }
                        } break;

                        case TONEX_GLOBAL_TEMPO_SOURCE:
                        {
                            if (param_entry->Value)
                            {
                                lv_obj_add_state(ui_TempoSourceSwitch, LV_STATE_CHECKED);
                            }
                            else
                            {
                                lv_obj_clear_state(ui_TempoSourceSwitch, LV_STATE_CHECKED);
                            }
                        } break;

                        case TONEX_GLOBAL_BPM:
                        {
                            lv_slider_set_range(ui_BPMSlider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(ui_BPMSlider, round(param_entry->Value), LV_ANIM_OFF); 

                            char buf[128];
                            sprintf(buf, "%.1f", param_entry->Value);
                            lv_label_set_text(ui_BPMValueLabel, buf);
                            
#if CONFIG_TONEX_CONTROLLER_SHOW_BPM_INDICATOR                            
                            ui_BPMAnimate(ui_BPMIndicator, 1000 * 60 / param_entry->Value);
#endif                            
                        } break;

                        case TONEX_GLOBAL_INPUT_TRIM:
                        {
                            lv_slider_set_range(ui_InputTrimSlider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(ui_InputTrimSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                        } break;
                        
                        case TONEX_GLOBAL_TUNING_REFERENCE:
                        {                            
                            lv_slider_set_range(ui_TuningReferenceSlider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(ui_TuningReferenceSlider, round(param_entry->Value), LV_ANIM_OFF);                                
                        } break;

                        case TONEX_GLOBAL_MASTER_VOLUME:
                        {                            
                            lv_slider_set_range(ui_VolumeSlider, round(param_entry->Min), round(param_entry->Max));
                            lv_slider_set_value(ui_VolumeSlider, round(param_entry->Value), LV_ANIM_OFF);                                
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
            if (element_1 == ui_USBStatusFail)
            {
                if (update->Value == 0)
                {
                    // show the USB disconnected image
                    lv_obj_add_flag(ui_USBStatusOK, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(ui_USBStatusFail, LV_OBJ_FLAG_HIDDEN);
                }
                else
                {
                    // show the USB connected image
                    lv_obj_add_flag(ui_USBStatusFail, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(ui_USBStatusOK, LV_OBJ_FLAG_HIDDEN);
                }
            }
            else if (element_1 == ui_BTStatusConn)
            {
                if (update->Value == 0)
                {
                    // show the BT disconnected image
                    lv_obj_add_flag(ui_BTStatusConn, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(ui_BTStatusDisconn, LV_OBJ_FLAG_HIDDEN);
                }
                else
                {
                    // show the BT connected image
                    lv_obj_add_flag(ui_BTStatusDisconn, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(ui_BTStatusConn, LV_OBJ_FLAG_HIDDEN);
                }
            }
            else if (element_1 == ui_WiFiStatusConn)
            {
                if (update->Value == 0)
                {
                    ESP_LOGI(TAG, "Show WiFi disconn");

                    // show the Wifi disconnected image
                    lv_obj_add_flag(ui_WiFiStatusConn, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(ui_WiFiStatusDisconn, LV_OBJ_FLAG_HIDDEN);
                }
                else
                {
                    ESP_LOGI(TAG, "Show WiFi conn");

                    // show the WiFi connected image
                    lv_obj_add_flag(ui_WiFiStatusDisconn, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(ui_WiFiStatusConn, LV_OBJ_FLAG_HIDDEN);
                }
            }
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
            else if (element_1 == ui_SkinImage)
            {
                // set skin
                lv_img_set_src(ui_SkinImage, ui_get_skin_image(update->Value));
            }
            else if (element_1 == ui_BankValueLabel)
            {
                // set Bank index
                char buf[128];
                sprintf(buf, "%d", (int)round(update->Value) + 1);
                lv_label_set_text(ui_BankValueLabel, buf);
            }
#endif            
        } break;

        case UI_ACTION_SET_LABEL_TEXT:
        {
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
            lv_label_set_text(element_1, update->Text);
#elif CONFIG_TONEX_CONTROLLER_DISPLAY_SMALL
            if (element_1 == ui_PresetHeadingLabel)
            {
                // split up preset into 2 text lines.
                // incoming has "XX: Name"
                char preset_index[16];
                char preset_name[33];

                // get the preset number
                sprintf(preset_index, "%d", atoi(update->Text));
                lv_label_set_text(ui_PresetHeadingLabel, preset_index);

                // get the preset name
                for (uint8_t loop = 0; loop < 4; loop++)
                {
                    if (update->Text[loop] == ':')
                    {
                        strncpy(preset_name, (const char*)&update->Text[loop + 2], sizeof(preset_name) - 1);
                        lv_label_set_text(ui_PresetHeadingLabel2, preset_name);
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
void ui_BPMAnimate(lv_obj_t *TargetObject, uint32_t duration)
{
    if (ui_BPMAnimation != NULL) {
        if (ui_BPMAnimation->time == duration) {
            // same BPM, no need to reset animation
            return;
        }
        // could not get this to work, using `lv_anim_del_all` since it's the only animation for now
        // lv_anim_del(ui_BPMAnimation->var, NULL);
        lv_anim_del_all();
    }

    ui_anim_user_data_t *PropertyAnimation_0_user_data = lv_mem_alloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, duration);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 255, 0);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_ease_in_out);
    lv_anim_set_delay(&PropertyAnimation_0, 0);
    lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, true);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_0);
    ui_BPMAnimation = &PropertyAnimation_0;
}
#endif

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

            // check for any UI update messages
            if (xQueueReceive(ui_update_queue, (void*)&ui_update, 0) == pdPASS)
            {
                // process it
                update_ui_element(&ui_update);
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

    // init GUI
    ESP_LOGI(TAG, "Init UI");
    ui_init();

    // create display task
    xTaskCreatePinnedToCore(display_task, "Dsp", DISPLAY_TASK_STACK_SIZE, NULL, DISPLAY_TASK_PRIORITY, NULL, 1);
#endif
}
