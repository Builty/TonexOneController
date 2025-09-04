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

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_LILYGO_TDISPLAY_S3

static const char *TAG = "platform Lilygo T-Display S3";

#define LILYGO_TDISPLAY_S3_LCD_H_RES             (320)
#define LILYGO_TDISPLAY_S3_LCD_V_RES             (170)

#define LILYGO_TDISPLAY_S3_LCD_PIXEL_CLOCK_HZ     (10 * 1000 * 1000)

static lv_disp_draw_buf_t disp_buf; 
static lv_disp_drv_t* disp_drv;      
static esp_lcd_panel_io_handle_t lcd_io = NULL;
static esp_lcd_panel_handle_t lcd_panel = NULL;
static esp_lcd_touch_handle_t tp = NULL;
static esp_lcd_panel_io_handle_t tp_io_handle = NULL;
static lv_indev_drv_t indev_drv;    // Input device driver (Touch)

typedef struct {
    uint32_t addr;
    uint8_t param[20];
    uint32_t len;
} lcd_cmd_t;

static lcd_cmd_t lcd_st7789v[] = {
    {0x11, {0}, 0 | 0x80},
    {0x3A, {0X05}, 1},
    {0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5},
    {0xB7, {0X75}, 1},
    {0xBB, {0X28}, 1},
    {0xC0, {0X2C}, 1},
    {0xC2, {0X01}, 1},
    {0xC3, {0X1F}, 1},
    {0xC6, {0X13}, 1},
    {0xD0, {0XA7}, 1},
    {0xD0, {0XA4, 0XA1}, 2},
    {0xD6, {0XA1}, 1},
    {0xE0, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14},
    {0xE1, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14},
};

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

    // 1.9 in landscape mode needs the co-ordinates adjusted
    *x = LILYGO_TDISPLAY_S3_LCD_H_RES - xpos;

    if (*x < 0)
    {
        *x = 0;
    }
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
   // nothing needed
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
    return &lv_font_montserrat_20;
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
    return 15;
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
    ESP_LOGI(TAG, "Platform Init");
    disp_drv = pdisp_drv;
    __attribute__((unused)) esp_err_t ret = ESP_OK;
    uint8_t touch_ok = 0;

    gpio_config_t rd_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << LG_TDISP_S3_TFT_RD) | (1ULL << LG_TDISP_S3_POWERON)
    };
    ESP_ERROR_CHECK(gpio_config(&rd_gpio_config));
    gpio_set_level(LG_TDISP_S3_TFT_RD, 1);
    gpio_set_level(LG_TDISP_S3_POWERON, 1);

    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = LG_TDISP_S3_TFT_DC,
        .wr_gpio_num = LG_TDISP_S3_TFT_WR,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .data_gpio_nums =
        {
            LG_TDISP_S3_TFT_DATA0,
            LG_TDISP_S3_TFT_DATA1,
            LG_TDISP_S3_TFT_DATA2,
            LG_TDISP_S3_TFT_DATA3,
            LG_TDISP_S3_TFT_DATA4,
            LG_TDISP_S3_TFT_DATA5,
            LG_TDISP_S3_TFT_DATA6,
            LG_TDISP_S3_TFT_DATA7,
        },
        .bus_width = 8,
        .max_transfer_bytes = LILYGO_TDISPLAY_S3_LCD_V_RES * 100 * sizeof(uint16_t),
        .psram_trans_align = 64,
        .sram_trans_align = 4
    };
    esp_lcd_new_i80_bus(&bus_config, &i80_bus);

    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = LG_TDISP_S3_TFT_CS,
        .pclk_hz = LILYGO_TDISPLAY_S3_LCD_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 10,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_levels =
        {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .on_color_trans_done = display_notify_lvgl_flush_ready,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &lcd_io));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = LG_TDISP_S3_TFT_RST,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
        .vendor_config = NULL
    };
    esp_lcd_new_panel_st7789(lcd_io, &panel_config, &lcd_panel);

    esp_lcd_panel_reset(lcd_panel);
    esp_lcd_panel_init(lcd_panel);
    esp_lcd_panel_invert_color(lcd_panel, true);
    esp_lcd_panel_swap_xy(lcd_panel, true);

    //The screen faces you, and the USB is on the left
    esp_lcd_panel_mirror(lcd_panel, false, true);

    // the gap is LCD panel specific, even panels with the same driver IC, can
    // have different gap value
    esp_lcd_panel_set_gap(lcd_panel, 0, 35);

    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++) 
    {
        esp_lcd_panel_io_tx_param(lcd_io, lcd_st7789v[i].addr, lcd_st7789v[i].param, lcd_st7789v[i].len & 0x7f);
        if (lcd_st7789v[i].len & 0x80)
        {
            vTaskDelay(pdMS_TO_TICKS(120));
        }
    }

    esp_lcd_panel_disp_on_off(lcd_panel, true);

    // backlight on
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LG_TDISP_S3_TFT_BL
    };
    
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    gpio_set_level(LG_TDISP_S3_TFT_BL, 1);

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();

    void *buf1 = NULL;
    void *buf2 = NULL;
    ESP_LOGI(TAG, "Allocate separate LVGL draw buffers from PSRAM");
    buf1 = heap_caps_malloc(LILYGO_TDISPLAY_S3_LCD_H_RES * 32 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf1);
    buf2 = heap_caps_malloc(LILYGO_TDISPLAY_S3_LCD_H_RES * 32 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf2);
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, LILYGO_TDISPLAY_S3_LCD_H_RES * 32);

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(disp_drv);
    disp_drv->hor_res = LILYGO_TDISPLAY_S3_LCD_H_RES;
    disp_drv->ver_res = LILYGO_TDISPLAY_S3_LCD_V_RES;
    disp_drv->flush_cb = display_lvgl_flush_cb;
    disp_drv->draw_buf = &disp_buf;
    disp_drv->full_refresh = false;
    disp_drv->user_data = lcd_panel;

    lv_disp_t* __attribute__((unused)) disp = lv_disp_drv_register(disp_drv);

    // note: basic version of T-Display S3 has no touch screen.
    // init touch screen    
    const esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_CST816S_CONFIG();
    
    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = LILYGO_TDISPLAY_S3_LCD_H_RES,
        .y_max = LILYGO_TDISPLAY_S3_LCD_V_RES,
        .rst_gpio_num = TOUCH_RESET,
        .int_gpio_num = TOUCH_INT,
        .interrupt_callback = touch_data_ready,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 1,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };

    // Touch IO handle
    if (esp_lcd_new_panel_io_i2c(bus_handle, &tp_io_config, &tp_io_handle) != ESP_OK)
    {
        ESP_LOGE(TAG, "Touch IO handle failed!");
    }
    
    // Initialize touch
    ESP_LOGI(TAG, "Initialize touch controller CST816");

   if (xSemaphoreTake(I2CMutex, (TickType_t)10000) == pdTRUE)
    {
        ret = esp_lcd_touch_new_i2c_cst816s(tp_io_handle, &tp_cfg, &tp);
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
        disp_drv->rotated = LV_DISP_ROT_180;

        // can only do software rotation, with a drop in frame rate
        disp_drv->sw_rotate = 1;
    }
}

#endif //CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_LILYGO_TDISPLAY_S3