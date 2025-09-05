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
#include "driver/ledc.h"
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
#include "esp_lcd_touch_ft6336.h"
#include "esp_lcd_touch_cst816s.h"
#include "esp_lcd_axs15231b.h"
#include "esp_lcd_st7796.h"
#include "esp_io_expander_tca9554.h"
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

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_JC3248W535

static const char *TAG = "platform_jc3248w";

// LCD panel config
#define JC3248W_LCD_H_RES               (480)
#define JC3248W_LCD_V_RES               (320)

/* LCD settings */
#define JC3248W_LCD_SPI_NUM             (SPI2_HOST)
#define JC3248W_LCD_PIXEL_CLK_HZ        (40 * 1000 * 1000)
#define JC3248W_LCD_DRAW_BUFF_DOUBLE    (1)
#define JC3248W_LCD_DRAW_BUFF_HEIGHT    (50)
#define JC3248W_LCD_BL_ON_LEVEL         (0)

#define LCD_BUFFER_SIZE                      (JC3248W_LCD_H_RES * JC3248W_LCD_V_RES)

#define BUF_SIZE                            (1024)
#define I2C_MASTER_TIMEOUT_MS               1000
#define I2C_AXS15231B_ADDRESS               (0x3B)

typedef bool (*lvgl_port_wait_cb)(void *handle);

static SemaphoreHandle_t I2CMutexHandle;
static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
static lv_disp_drv_t* disp_drv;      // contains callback functions
static esp_lcd_panel_io_handle_t lcd_io = NULL;
static esp_lcd_panel_handle_t lcd_panel = NULL;
static esp_lcd_touch_handle_t tp = NULL;
static esp_lcd_panel_io_handle_t tp_io_handle = NULL;
static lv_indev_drv_t indev_drv;    // Input device driver (Touch)
static uint32_t trans_size = LCD_BUFFER_SIZE / 15;
static lv_color_t* trans_buf_1 = NULL;
static lv_color_t* trans_buf_2 = NULL;
static lv_color_t* trans_act = NULL;
static SemaphoreHandle_t trans_done_sem = NULL;
static lvgl_port_wait_cb draw_wait_cb;     /* Callback function for drawing */
static int rotation_setting = LV_DISP_ROT_90;

static const axs15231b_lcd_init_cmd_t lcd_init_cmds[] = {
    {0xBB, (uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5A, 0xA5}, 8, 0},
    {0xA0, (uint8_t[]){0xC0, 0x10, 0x00, 0x02, 0x00, 0x00, 0x04, 0x3F, 0x20, 0x05, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00}, 17, 0},
    {0xA2, (uint8_t[]){0x30, 0x3C, 0x24, 0x14, 0xD0, 0x20, 0xFF, 0xE0, 0x40, 0x19, 0x80, 0x80, 0x80, 0x20, 0xf9, 0x10, 0x02, 0xff, 0xff, 0xF0, 0x90, 0x01, 0x32, 0xA0, 0x91, 0xE0, 0x20, 0x7F, 0xFF, 0x00, 0x5A}, 31, 0},
    {0xD0, (uint8_t[]){0xE0, 0x40, 0x51, 0x24, 0x08, 0x05, 0x10, 0x01, 0x20, 0x15, 0x42, 0xC2, 0x22, 0x22, 0xAA, 0x03, 0x10, 0x12, 0x60, 0x14, 0x1E, 0x51, 0x15, 0x00, 0x8A, 0x20, 0x00, 0x03, 0x3A, 0x12}, 30, 0},
    {0xA3, (uint8_t[]){0xA0, 0x06, 0xAa, 0x00, 0x08, 0x02, 0x0A, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x55, 0x55}, 22, 0},
    {0xC1, (uint8_t[]){0x31, 0x04, 0x02, 0x02, 0x71, 0x05, 0x24, 0x55, 0x02, 0x00, 0x41, 0x00, 0x53, 0xFF, 0xFF, 0xFF, 0x4F, 0x52, 0x00, 0x4F, 0x52, 0x00, 0x45, 0x3B, 0x0B, 0x02, 0x0d, 0x00, 0xFF, 0x40}, 30, 0},
    {0xC3, (uint8_t[]){0x00, 0x00, 0x00, 0x50, 0x03, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01}, 11, 0},
    {0xC4, (uint8_t[]){0x00, 0x24, 0x33, 0x80, 0x00, 0xea, 0x64, 0x32, 0xC8, 0x64, 0xC8, 0x32, 0x90, 0x90, 0x11, 0x06, 0xDC, 0xFA, 0x00, 0x00, 0x80, 0xFE, 0x10, 0x10, 0x00, 0x0A, 0x0A, 0x44, 0x50}, 29, 0},
    {0xC5, (uint8_t[]){0x18, 0x00, 0x00, 0x03, 0xFE, 0x3A, 0x4A, 0x20, 0x30, 0x10, 0x88, 0xDE, 0x0D, 0x08, 0x0F, 0x0F, 0x01, 0x3A, 0x4A, 0x20, 0x10, 0x10, 0x00}, 23, 0},
    {0xC6, (uint8_t[]){0x05, 0x0A, 0x05, 0x0A, 0x00, 0xE0, 0x2E, 0x0B, 0x12, 0x22, 0x12, 0x22, 0x01, 0x03, 0x00, 0x3F, 0x6A, 0x18, 0xC8, 0x22}, 20, 0},
    {0xC7, (uint8_t[]){0x50, 0x32, 0x28, 0x00, 0xa2, 0x80, 0x8f, 0x00, 0x80, 0xff, 0x07, 0x11, 0x9c, 0x67, 0xff, 0x24, 0x0c, 0x0d, 0x0e, 0x0f}, 20, 0},
    {0xC9, (uint8_t[]){0x33, 0x44, 0x44, 0x01}, 4, 0},
    {0xCF, (uint8_t[]){0x2C, 0x1E, 0x88, 0x58, 0x13, 0x18, 0x56, 0x18, 0x1E, 0x68, 0x88, 0x00, 0x65, 0x09, 0x22, 0xC4, 0x0C, 0x77, 0x22, 0x44, 0xAA, 0x55, 0x08, 0x08, 0x12, 0xA0, 0x08}, 27, 0},
    {0xD5, (uint8_t[]){0x40, 0x8E, 0x8D, 0x01, 0x35, 0x04, 0x92, 0x74, 0x04, 0x92, 0x74, 0x04, 0x08, 0x6A, 0x04, 0x46, 0x03, 0x03, 0x03, 0x03, 0x82, 0x01, 0x03, 0x00, 0xE0, 0x51, 0xA1, 0x00, 0x00, 0x00}, 30, 0},
    {0xD6, (uint8_t[]){0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE, 0x93, 0x00, 0x01, 0x83, 0x07, 0x07, 0x00, 0x07, 0x07, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x84, 0x00, 0x20, 0x01, 0x00}, 30, 0},
    {0xD7, (uint8_t[]){0x03, 0x01, 0x0b, 0x09, 0x0f, 0x0d, 0x1E, 0x1F, 0x18, 0x1d, 0x1f, 0x19, 0x40, 0x8E, 0x04, 0x00, 0x20, 0xA0, 0x1F}, 19, 0},
    {0xD8, (uint8_t[]){0x02, 0x00, 0x0a, 0x08, 0x0e, 0x0c, 0x1E, 0x1F, 0x18, 0x1d, 0x1f, 0x19}, 12, 0},
    {0xD9, (uint8_t[]){0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F}, 12, 0},
    {0xDD, (uint8_t[]){0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F}, 12, 0},
    {0xDF, (uint8_t[]){0x44, 0x73, 0x4B, 0x69, 0x00, 0x0A, 0x02, 0x90}, 8, 0},
    {0xE0, (uint8_t[]){0x3B, 0x28, 0x10, 0x16, 0x0c, 0x06, 0x11, 0x28, 0x5c, 0x21, 0x0D, 0x35, 0x13, 0x2C, 0x33, 0x28, 0x0D}, 17, 0},
    {0xE1, (uint8_t[]){0x37, 0x28, 0x10, 0x16, 0x0b, 0x06, 0x11, 0x28, 0x5C, 0x21, 0x0D, 0x35, 0x14, 0x2C, 0x33, 0x28, 0x0F}, 17, 0},
    {0xE2, (uint8_t[]){0x3B, 0x07, 0x12, 0x18, 0x0E, 0x0D, 0x17, 0x35, 0x44, 0x32, 0x0C, 0x14, 0x14, 0x36, 0x3A, 0x2F, 0x0D}, 17, 0},
    {0xE3, (uint8_t[]){0x37, 0x07, 0x12, 0x18, 0x0E, 0x0D, 0x17, 0x35, 0x44, 0x32, 0x0C, 0x14, 0x14, 0x36, 0x32, 0x2F, 0x0F}, 17, 0},
    {0xE4, (uint8_t[]){0x3B, 0x07, 0x12, 0x18, 0x0E, 0x0D, 0x17, 0x39, 0x44, 0x2E, 0x0C, 0x14, 0x14, 0x36, 0x3A, 0x2F, 0x0D}, 17, 0},
    {0xE5, (uint8_t[]){0x37, 0x07, 0x12, 0x18, 0x0E, 0x0D, 0x17, 0x39, 0x44, 0x2E, 0x0C, 0x14, 0x14, 0x36, 0x3A, 0x2F, 0x0F}, 17, 0},
    {0xA4, (uint8_t[]){0x85, 0x85, 0x95, 0x82, 0xAF, 0xAA, 0xAA, 0x80, 0x10, 0x30, 0x40, 0x40, 0x20, 0xFF, 0x60, 0x30}, 16, 0},
    {0xA4, (uint8_t[]){0x85, 0x85, 0x95, 0x85}, 4, 0},
    {0xBB, (uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 8, 0},
    {0x13, (uint8_t[]){0x00}, 0, 0},
    {0x11, (uint8_t[]){0x00}, 0, 120},
    {0x2C, (uint8_t[]){0x00, 0x00, 0x00, 0x00}, 4, 0},
};

static bool lvgl_port_flush_ready_callback(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx);

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
__attribute__((unused)) void platform_adjust_touch_coords(lv_coord_t* x, lv_coord_t* y)
{
    lv_coord_t xpos = *x;
    lv_coord_t ypos = *y;

    // 90 degree screen rotation
    *x = ypos;
    *y = JC3248W_LCD_V_RES - xpos;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
__attribute__((unused)) void platform_adjust_display_flush_area(lv_area_t *area)
{
    // nothing needed}
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
__attribute__((unused)) void platform_get_icon_coords(int16_t* dest, uint8_t max_entries)
{
    if (max_entries <= 8)
    {
        dest[0] = -168;
        dest[1] = -126;
        dest[2] = -84;
        dest[3] = -42;
        dest[4] = 0;
        dest[5] = 42;
        dest[6] = 84;
        dest[7] = 126;
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
__attribute__((unused)) const lv_font_t* platform_get_toast_font(void)
{
    return &lv_font_montserrat_30;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
__attribute__((unused)) uint16_t platform_get_toast_padding(void)
{
    return 25;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static bool lvgl_port_flush_ready_callback(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    BaseType_t taskAwake = pdFALSE;

    if (trans_done_sem) 
    {
        xSemaphoreGiveFromISR(trans_done_sem, &taskAwake);
    }

    return false;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void lvgl_port_flush_callback(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    assert(drv != NULL);
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;

    const int x_start = area->x1;
    const int x_end = area->x2;
    const int y_start = area->y1;
    const int y_end = area->y2;
    const int width = x_end - x_start + 1;
    const int height = y_end - y_start + 1;

    lv_color_t *from = color_map;
    lv_color_t *to = NULL;

    int x_draw_start = 0;
    int x_draw_end = 0;
    int y_draw_start = 0;
    int y_draw_end = 0;
    int trans_count = 0;

    trans_act = trans_buf_1;

    int x_start_tmp = 0;
    int x_end_tmp = 0;
    int max_width = 0;
    int trans_width = 0;

    int y_start_tmp = 0;
    int y_end_tmp = 0;
    int max_height = 0;
    int trans_height = 0;
  
    if (LV_DISP_ROT_270 == rotation_setting || LV_DISP_ROT_90 == rotation_setting) 
    {
        max_width = ((trans_size / height) > width) ? (width) : (trans_size / height);
        trans_count = width / max_width + (width % max_width ? (1) : (0));

        x_start_tmp = x_start;
        x_end_tmp = x_end;
    } 
    else 
    {
        max_height = ((trans_size / width) > height) ? (height) : (trans_size / width);
        trans_count = height / max_height + (height % max_height ? (1) : (0));

        y_start_tmp = y_start;
        y_end_tmp = y_end;
    }

    for (int i = 0; i < trans_count; i++) 
    {
        if (LV_DISP_ROT_90 == rotation_setting) 
        {
            trans_width = (x_end - x_start_tmp + 1) > max_width ? max_width : (x_end - x_start_tmp + 1);
            x_end_tmp = (x_end - x_start_tmp + 1) > max_width ? (x_start_tmp + max_width - 1) : x_end;
        } 
        else if (LV_DISP_ROT_270 == rotation_setting) 
        {
            trans_width = (x_end_tmp - x_start + 1) > max_width ? max_width : (x_end_tmp - x_start + 1);
            x_start_tmp = (x_end_tmp - x_start + 1) > max_width ? (x_end_tmp - trans_width + 1) : x_start;
        } 
        else if (LV_DISP_ROT_NONE == rotation_setting) 
        {
            trans_height = (y_end - y_start_tmp + 1) > max_height ? max_height : (y_end - y_start_tmp + 1);
            y_end_tmp = (y_end - y_start_tmp + 1) > max_height ? (y_start_tmp + max_height - 1) : y_end;
        } 
        else 
        {
            trans_height = (y_end_tmp - y_start + 1) > max_height ? max_height : (y_end_tmp - y_start + 1);
            y_start_tmp = (y_end_tmp - y_start + 1) > max_height ? (y_end_tmp - max_height + 1) : y_start;
        }
     
        trans_act = (trans_act == trans_buf_1) ? (trans_buf_2) : (trans_buf_1);
        to = trans_act;
        
        switch (rotation_setting) 
        {
            case LV_DISP_ROT_90:
                for (int y = 0; y < height; y++) 
                {
                    for (int x = 0; x < trans_width; x++) 
                    {
                        *(to + x * height + (height - y - 1)) = *(from + y * width + x_start_tmp + x);
                    }
                }
                x_draw_start = drv->ver_res - y_end - 1;
                x_draw_end = drv->ver_res - y_start - 1;
                y_draw_start = x_start_tmp;
                y_draw_end = x_end_tmp;
                break;

            case LV_DISP_ROT_270:
                for (int y = 0; y < height; y++) 
                {
                    for (int x = 0; x < trans_width; x++) 
                    {
                        *(to + (trans_width - x - 1) * height + y) = *(from + y * width + x_start_tmp + x);
                    }
                }
                x_draw_start = y_start;
                x_draw_end = y_end;
                y_draw_start = drv->hor_res - x_end_tmp - 1;
                y_draw_end = drv->hor_res - x_start_tmp - 1;
                break;

            case LV_DISP_ROT_180:
                for (int y = 0; y < trans_height; y++) 
                {
                    for (int x = 0; x < width; x++) 
                    {
                        *(to + (trans_height - y - 1)*width + (width - x - 1)) = *(from + y_start_tmp * width + y * (width) + x);
                    }
                }
                x_draw_start = drv->hor_res - x_end - 1;
                x_draw_end = drv->hor_res - x_start - 1;
                y_draw_start = drv->ver_res - y_end_tmp - 1;
                y_draw_end = drv->ver_res - y_start_tmp - 1;
                break;

            case LV_DISP_ROT_NONE:
                for (int y = 0; y < trans_height; y++) 
                {
                    for (int x = 0; x < width; x++) 
                    {
                        *(to + y * (width) + x) = *(from + y_start_tmp * width + y * (width) + x);
                    }
                }
                x_draw_start = x_start;
                x_draw_end = x_end;
                y_draw_start = y_start_tmp;
                y_draw_end = y_end_tmp;
                break;

            default:
                break;
        }

        if (0 == i) 
        {
            if (draw_wait_cb) 
            {
                draw_wait_cb(drv->user_data);
            }
            xSemaphoreGive(trans_done_sem);
        }

        xSemaphoreTake(trans_done_sem, portMAX_DELAY);
         
        esp_lcd_panel_draw_bitmap(panel_handle, x_draw_start, y_draw_start, x_draw_end + 1, y_draw_end + 1, to);

        if (LV_DISP_ROT_90 == rotation_setting) 
        {
            x_start_tmp += max_width;
        } 
        else if (LV_DISP_ROT_270 == rotation_setting) 
        {
            x_end_tmp -= max_width;
        } 
        if (LV_DISP_ROT_NONE == rotation_setting) 
        {
            y_start_tmp += max_height;
        } 
        else 
        {
            y_end_tmp -= max_height;
        } 
    }

    lv_disp_flush_ready(drv);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void platform_init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex, lv_disp_drv_t* pdisp_drv)
{    
    __attribute__((unused)) esp_err_t ret = ESP_OK;
    uint8_t touch_ok = 0;
    I2CMutexHandle = I2CMutex;
    disp_drv = pdisp_drv;

    ESP_LOGI(TAG, "Platform Init");

    /* LCD initialization */
    ESP_LOGD(TAG, "Initialize SPI bus");
    const spi_bus_config_t buscfg = {
        .sclk_io_num = JC3248W_LCD_GPIO_SCLK,
        .data0_io_num = JC3248W_LCD_GPIO_QSPI_0,
        .data1_io_num = JC3248W_LCD_GPIO_QSPI_1,
        .data2_io_num = JC3248W_LCD_GPIO_QSPI_2,
        .data3_io_num = JC3248W_LCD_GPIO_QSPI_3,
        .max_transfer_sz = LCD_BUFFER_SIZE,
    };
    
    if (spi_bus_initialize(JC3248W_LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO) != ESP_OK)
    {
        ESP_LOGE(TAG, "Platform Init spi_bus_initialize() failed");
    }

    ESP_LOGD(TAG, "Install panel IO");
    esp_lcd_panel_io_spi_config_t io_config = AXS15231B_PANEL_IO_QSPI_CONFIG(JC3248W_LCD_GPIO_CS, NULL, NULL);
    io_config.pclk_hz = JC3248W_LCD_PIXEL_CLK_HZ;

    if (esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)JC3248W_LCD_SPI_NUM, &io_config, &lcd_io) != ESP_OK)
    {
        ESP_LOGE(TAG, "Platform Init esp_lcd_new_panel_io_spi() failed");
    }

    axs15231b_vendor_config_t vendor_config = {};
    vendor_config.init_cmds = lcd_init_cmds; 
    vendor_config.init_cmds_size = sizeof(lcd_init_cmds) / sizeof(lcd_init_cmds[0]);
    vendor_config.flags.use_qspi_interface = 1;

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = JC3248W_LCD_GPIO_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    panel_config.vendor_config = (void *)&vendor_config;
    esp_lcd_new_panel_axs15231b(lcd_io, &panel_config, &lcd_panel);

    if (esp_lcd_panel_reset(lcd_panel) != ESP_OK)
    {
        ESP_LOGE(TAG, "LCD panel reset failed");
    }
    
    if (esp_lcd_panel_init(lcd_panel) != ESP_OK)
    {
        ESP_LOGE(TAG, "LCD panel init failed");
    }

    esp_lcd_panel_disp_on_off(lcd_panel, false);

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();

    // allocate draw buffers
    lv_color_t* buf1 = NULL;
    lv_color_t* buf2 = NULL;
    lv_color_t* buf3 = NULL;

    buf1 = heap_caps_malloc(LCD_BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);

    buf2 = heap_caps_malloc(trans_size * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf2);
    trans_buf_1 = buf2;

    buf3 = heap_caps_malloc(trans_size * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf3);
    trans_buf_2 = buf3;

    trans_done_sem = xSemaphoreCreateCounting(1, 0);
    assert(trans_done_sem);
    trans_done_sem = trans_done_sem;

    lv_disp_draw_buf_init(&disp_buf, buf1, NULL, LCD_BUFFER_SIZE);

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(disp_drv);
    disp_drv->hor_res = JC3248W_LCD_H_RES;
    disp_drv->ver_res = JC3248W_LCD_V_RES;
    disp_drv->flush_cb = lvgl_port_flush_callback;
    disp_drv->draw_buf = &disp_buf;
    disp_drv->user_data = lcd_panel;
    disp_drv->full_refresh = 1;

    const esp_lcd_panel_io_callbacks_t cbs = {
        .on_color_trans_done = lvgl_port_flush_ready_callback,
    };
    esp_lcd_panel_io_register_event_callbacks(lcd_io, &cbs, &disp_drv);

    lv_disp_t* __attribute__((unused)) disp = lv_disp_drv_register(disp_drv);

    // init LCD backlight
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {};
    ledc_timer.speed_mode = JC3248W_LCD_BL_LEDC_MODE,
    ledc_timer.timer_num = JC3248W_LCD_BL_LEDC_TIMER;
    ledc_timer.duty_resolution = JC3248W_LCD_BL_LEDC_DUTY_RES;
    ledc_timer.freq_hz = JC3248W_LCD_BL_LEDC_FREQUENCY;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {};
    ledc_channel.speed_mode = JC3248W_LCD_BL_LEDC_MODE;
    ledc_channel.channel = JC3248W_LCD_BL_LEDC_CHANNEL;
    ledc_channel.timer_sel = JC3248W_LCD_BL_LEDC_TIMER;
    ledc_channel.intr_type = LEDC_INTR_DISABLE;
    ledc_channel.gpio_num = JC3248W_LCD_GPIO_BL;
    ledc_channel.duty = 0, // Set duty to 0%
    ledc_channel.hpoint = 0;
    ledc_channel_config(&ledc_channel);

    // set brightness
    uint8_t brightness = 70;
    uint32_t duty = (brightness * (JC3248W_LCD_BL_LEDC_DUTY - 1)) / 100;
    ledc_set_duty(JC3248W_LCD_BL_LEDC_MODE, JC3248W_LCD_BL_LEDC_CHANNEL, duty);
    ledc_update_duty(JC3248W_LCD_BL_LEDC_MODE, JC3248W_LCD_BL_LEDC_CHANNEL);

    // init touch screen   
    ESP_LOGI(TAG, "Initialize touch controller");

    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_AXS15231B_CONFIG();
    tp_io_config.scl_speed_hz = 400000;

    esp_lcd_touch_config_t tp_cfg = {};
    tp_cfg.x_max = JC3248W_LCD_H_RES;
    tp_cfg.y_max = JC3248W_LCD_V_RES;
    tp_cfg.rst_gpio_num = GPIO_NUM_NC;
    tp_cfg.int_gpio_num = GPIO_NUM_NC;

    // 90 degree rotation
    tp_cfg.flags.swap_xy = 0;
    tp_cfg.flags.mirror_x = 0;
    tp_cfg.flags.mirror_y = 0;
    
    if (xSemaphoreTake(I2CMutex, (TickType_t)10000) == pdTRUE)
    {        
        esp_lcd_new_panel_io_i2c(bus_handle, &tp_io_config, &tp_io_handle);
        ret = esp_lcd_touch_new_i2c_axs15231b(tp_io_handle, &tp_cfg, &tp);
        xSemaphoreGive(I2CMutex);
    }
    else
    {
        ESP_LOGE(TAG, "Initialize touch mutex timeout");
    }
        
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Touch controller init OK");
        touch_ok = 1;
    }
    else
    {
        ESP_LOGW(TAG, "Touch controller init failed %s", esp_err_to_name(ret));
    }

    if (touch_ok)
    {
        lv_indev_drv_init(&indev_drv);
        indev_drv.type = LV_INDEV_TYPE_POINTER;
        indev_drv.disp = disp;
        indev_drv.read_cb = display_lvgl_touch_cb;
        indev_drv.user_data = tp;

        lv_indev_drv_register(&indev_drv);
    }   
    
    if (control_get_config_item_int(CONFIG_ITEM_SCREEN_ROTATION) == SCREEN_ROTATION_180)
    {
        rotation_setting = LV_DISP_ROT_180;
    }
}

#endif //CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_JC3248W535