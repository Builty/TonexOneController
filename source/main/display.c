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
#include "esp_partition.h"
#include "usb_comms.h"
#include "usb_tonex_common.h"
#include "usb_tonex_one.h"
#include "usb_tonex.h"
#include "display.h"
#include "display_tonex.h"
#include "display_valeton.h"
#include "CH422G.h"
#include "control.h"
#include "task_priorities.h" 
#include "midi_control.h"
#include "LP5562.h"
#include "tonex_params.h"
#include "platform_common.h"

static const char *TAG = "app_display";

#define DISPLAY_TASK_STACK_SIZE   (6 * 1024)

#if CONFIG_TONEX_CONTROLLER_SHOW_BPM_INDICATOR
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
#define MAX_SKIN_IMAGES                 100
#define SKIN_PARTITION_TYPE             0x40
#define SKIN_PARTITION_NAME             "skins"

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

typedef struct __attribute__ ((packed)) 
{
    uint32_t offset;
    uint32_t length;
} tSkinTOC;


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
static uint16_t current_skin_image = 0xFFFF;
static tSkinTOC SkinTOC[MAX_SKIN_IMAGES];
static const void* skin_data_map_ptr;
static esp_partition_mmap_handle_t skin_data_map_handle = 0;
static const esp_partition_t* skin_partition;
static lv_img_dsc_t skin_img_dsc;
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
void __attribute__((unused)) action_gesture(lv_event_t * e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());

    // let platform adjust it
    dir = platform_adjust_gesture(dir);

    // called from LVGL 
    if (dir == LV_DIR_RIGHT)
    {
        ESP_LOGI(TAG, "UI Previous Swipe");      
        control_request_preset_down();      
    }
    else if (dir == LV_DIR_LEFT)
    {
        ESP_LOGI(TAG, "UI Next Swipe");    
        control_request_preset_up();      
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
    ESP_LOGI(TAG, "UI Previous Click");      

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
    ESP_LOGI(TAG, "UI Next Click");    

    control_request_preset_up();        
}

#else   //CONFIG_TONEX_CONTROLLER_HAS_TOUCH

// Dummy functions so that 1.69 and 1.69 Touch can share the same UI project
void __attribute__((unused)) action_previous_clicked(lv_event_t * e)
{
}

void __attribute__((unused)) action_next_clicked(lv_event_t * e)
{
}

void __attribute__((unused)) action_gesture(lv_event_t * e)
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
    switch (usb_get_connected_modeller_type())
    {
        case AMP_MODELLER_TONEX_ONE:        // fallthrough
        case AMP_MODELLER_TONEX:            // fallthrough
        default:
        {
            tonex_show_settings_tab(e);
        } break;

        case AMP_MODELLER_VALETON_GP5:
        {
            valeton_show_settings_tab(e);
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
void action_effect_icon_clicked(lv_event_t * e)
{
    switch (usb_get_connected_modeller_type())
    {
        case AMP_MODELLER_TONEX_ONE:        // fallthrough
        case AMP_MODELLER_TONEX:            // fallthrough
        default:
        {
            tonex_action_effect_icon_clicked(e);
        } break;

        case AMP_MODELLER_VALETON_GP5:
        {
            valeton_action_effect_icon_clicked(e);
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
    // save preset
    usb_save_preset();

    // close settings screen
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
    switch (usb_get_connected_modeller_type())
    {
        case AMP_MODELLER_TONEX_ONE:        // fallthrough
        case AMP_MODELLER_TONEX:            // fallthrough
        default:
        {
            lv_scr_load_anim(objects.settings, LV_SCR_LOAD_ANIM_FADE_IN, 0, 0, false);
        } break;

        case AMP_MODELLER_VALETON_GP5:
        {
            lv_scr_load_anim(objects.val_settings, LV_SCR_LOAD_ANIM_FADE_IN, 0, 0, false);
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

#if (CONFIG_TONEX_CONTROLLER_SHOW_BPM_INDICATOR)
    lv_obj_clear_flag(objects.ui_bpm_indicator, LV_OBJ_FLAG_HIDDEN);
#endif    
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
void action_value_clicked(lv_event_t *e) 
{
    ESP_LOGI(TAG, "action_value_clicked");

    switch (usb_get_connected_modeller_type())
    {
        case AMP_MODELLER_TONEX_ONE:        // fallthrough
        case AMP_MODELLER_TONEX:            // fallthrough
        default:
        {
            tonex_value_clicked(e);         
        } break;

        case AMP_MODELLER_VALETON_GP5:
        {
            valeton_value_clicked(e);
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
void action_value_keyboard_ok(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_READY) 
    {
        ESP_LOGI(TAG, "action_value_keyboard_ok");

        switch (usb_get_connected_modeller_type())
        {
            case AMP_MODELLER_TONEX_ONE:        // fallthrough
            case AMP_MODELLER_TONEX:            // fallthrough
            default:
            {
                tonex_value_changed(e);
            } break;

            case AMP_MODELLER_VALETON_GP5:
            {
                valeton_value_changed(e);               
            } break;
        }            
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
    switch (usb_get_connected_modeller_type())
    {
        case AMP_MODELLER_TONEX_ONE:        // fallthrough
        case AMP_MODELLER_TONEX:            // fallthrough
        default:
        {
            tonex_action_parameter_changed(e);
        } break;

        case AMP_MODELLER_VALETON_GP5:
        {
            valeton_action_parameter_changed(e);
        } break;
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
static void ui_load_skin_toc(void)
{
    // Find the skin partition by name
    skin_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, SKIN_PARTITION_TYPE, SKIN_PARTITION_NAME);
    if (skin_partition == NULL) 
    {
        ESP_LOGE(TAG, "TOC: Could not find partition 'skins'");
        return;
    }

    esp_err_t err = esp_partition_read(skin_partition, 0, (uint8_t*)&SkinTOC, sizeof(SkinTOC));
    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "TOC: Failed to read skins partition: %s", esp_err_to_name(err));
        return;
    }
    else
    {
        ESP_LOGI(TAG, "Skin TOC loaded OK");
    }
    
    // debug code to dump the skin TOC
    //for (uint8_t index = 0; index < MAX_SKIN_IMAGES; index++)
    //{
    //    ESP_LOGI(TAG, "TOC: %d, %d %d", (int)index, (int)SkinTOC[index].offset, (int)SkinTOC[index].length);
    //}
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static uint32_t ui_get_skin_image(uint16_t index)
{
    if (index > MAX_SKIN_IMAGES)
    {
        ESP_LOGW(TAG, "Invalid skin image index: %d", (int)index);
        return 0;
    }

    if (SkinTOC[index].length == 0)
    {
        // no data for this skin index
        ESP_LOGW(TAG, "No skin data for index: %d", (int)index);
        return 0;
    }

    if (skin_partition == NULL) 
    {
        // no skin partition
        ESP_LOGW(TAG, "No skin data partition");
        return 0;
    }

    // unmap any mapped data
    if (skin_data_map_handle != 0)
    {
        esp_partition_munmap(skin_data_map_handle);
        skin_data_map_handle = 0;
    }

    // map the skin partition chunk into memory
    esp_err_t err = esp_partition_mmap(skin_partition, SkinTOC[index].offset, SkinTOC[index].length, ESP_PARTITION_MMAP_DATA, &skin_data_map_ptr, &skin_data_map_handle);

    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to read partition: %s", esp_err_to_name(err));
        return 0;
    }
    else
    {
        ESP_LOGI(TAG, "Skin partition data mapped to %X length %d", (int)skin_data_map_ptr, SkinTOC[index].length);
    }

    // debug code
    //ESP_LOGI(TAG, "Skin data:");
    //ESP_LOG_BUFFER_HEX(TAG, (uint8_t*)skin_data_map_ptr, 32);

    return SkinTOC[index].length;
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
void updateIconOrder(void)
{
    switch (usb_get_connected_modeller_type())
    {
        case AMP_MODELLER_TONEX_ONE:        // fallthrough
        case AMP_MODELLER_TONEX:            // fallthrough
        default:
        {
            tonex_update_icon_order();
        } break;

        case AMP_MODELLER_VALETON_GP5:
        {
            valeton_update_icon_order();
        } break;
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
static  __attribute__((unused)) uint8_t update_ui_element(tUIUpdate* update)
{
#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY
    __attribute__((unused)) char value_string[20];
    lv_obj_t* element_1 = NULL;

    switch (update->ElementID)
    {
        case UI_ELEMENT_USB_STATUS:
        {
            element_1 = objects.ui_usb_status_fail;

            if (update->Value == 1)
            {
                // if enabled, adjust UI to suit modeller
                switch (usb_get_connected_modeller_type())
                {
                    case AMP_MODELLER_TONEX_ONE:        // fallthrough
                    case AMP_MODELLER_TONEX:            // fallthrough
                    default:
                    {
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI                                    
                        lv_obj_clear_flag(objects.ui_bottom_panel_tonex, LV_OBJ_FLAG_HIDDEN);
                        lv_obj_add_flag(objects.ui_bottom_panel_valeton, LV_OBJ_FLAG_HIDDEN);

                        lv_label_set_text(objects.ui_project_heading_label, "Tonex Controller"); 
#else                    
                        // set effect letter to "C" (Compressor)
                        lv_label_set_text(objects.ui_cstatus, "C");
#endif    

                    } break;

                    case AMP_MODELLER_VALETON_GP5:
                    {
#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI                                                            
                        lv_obj_add_flag(objects.ui_bottom_panel_tonex, LV_OBJ_FLAG_HIDDEN);
                        lv_obj_clear_flag(objects.ui_bottom_panel_valeton, LV_OBJ_FLAG_HIDDEN);

                        lv_label_set_text(objects.ui_project_heading_label, "Valeton Controller"); 
#else            
                        // set effect letter to "T" (Distortion)
                        lv_label_set_text(objects.ui_cstatus, "T");
#endif    
                    } break;
                }
            }
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
            ESP_LOGI(TAG, "Syncing params to UI");

            switch (usb_get_connected_modeller_type())
            {
                case AMP_MODELLER_TONEX_ONE:        // fallthrough
                case AMP_MODELLER_TONEX:            // fallthrough
                default:
                {
                    tonex_update_ui_parameters();
                } break;

                case AMP_MODELLER_VALETON_GP5:
                {
                    valeton_update_ui_parameters();
                } break;
            }

#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI            
            updateIconOrder();
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

                    // map skin image data into skin_data_map_ptr
                    uint32_t skin_len = ui_get_skin_image(update->Value);
                    
                    if (skin_len > 0)
                    {
                        // build the image descriptor
                        // Copy the 4-byte header
                        memcpy((void*)&skin_img_dsc.header, (void*)skin_data_map_ptr, sizeof(lv_img_header_t));  

                        // debug code
                        //ESP_LOGI(TAG, "Skin CF:%d Width:%d Height:%d", (int)skin_img_dsc.header.cf, (int)skin_img_dsc.header.w, (int)skin_img_dsc.header.h);

                        // set the size
                        skin_img_dsc.data_size = skin_len - sizeof(lv_img_header_t);

                        // set the data
                        skin_img_dsc.data = (const uint8_t*)((uint8_t*)skin_data_map_ptr + sizeof(lv_img_header_t));
                        
                        lv_img_set_src(objects.ui_skin_image, &skin_img_dsc);
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

#if CONFIG_TONEX_CONTROLLER_SHOW_BPM_INDICATOR

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

#if CONFIG_TONEX_CONTROLLER_HAS_DISPLAY        
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
#endif //CONFIG_TONEX_CONTROLLER_HAS_DISPLAY

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
#if CONFIG_LV_USE_LOG
static void __attribute__((unused)) lv_log_cb(const char * buf)
{
    ESP_LOGI("LVGL", "%s", buf);
}
#endif  //CONFIG_LV_USE_LOG

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

    // init mem
    memset((void*)&msgbox_data, 0, sizeof(msgbox_data));

    // init toast
    ui_init_toast();

#if CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI
    memset((void*)&SkinTOC, 0, sizeof(SkinTOC));
     
    // load skin table of contents
    ui_load_skin_toc();
#endif

#if CONFIG_LV_USE_LOG
    // register log handler for lvgl
    lv_log_register_print_cb(lv_log_cb);
#endif  //CONFIG_LV_USE_LOG

    // create display task
    xTaskCreatePinnedToCore(display_task, "Dsp", DISPLAY_TASK_STACK_SIZE, NULL, DISPLAY_TASK_PRIORITY, NULL, 1);
#endif
}
