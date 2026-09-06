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
#include "esp_heap_caps.h"
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
#include "driver/i2c_master.h"
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
#include "soc/lldesc.h"
#include "esp_lcd_touch_gt911.h"
#include "esp_lcd_st7701.h"
#include "esp_intr_alloc.h"
#include "esp_ldo_regulator.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"

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

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_JC4880P4
#include "esp_hosted.h"
#include "esp_cache.h"
static const char *TAG = "platform_jc4880p4";

#define BUF_SIZE                            (1024)
#define I2C_MASTER_TIMEOUT_MS               1000
#define PANEL_W                             480
#define PANEL_H                             800
#define LV_HOR                              800
#define LV_VER                              480

static SemaphoreHandle_t I2CMutexHandle;
static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
static lv_disp_drv_t* disp_drv;      // contains callback functions
static esp_lcd_panel_io_handle_t lcd_io = NULL;
static esp_lcd_panel_handle_t lcd_panel = NULL;
static esp_lcd_touch_handle_t tp = NULL;
static lv_indev_drv_t indev_drv;    // Input device driver (Touch)
static lv_color_t* s_lv_buf;
static uint16_t* s_fb;

// JC4880 / Guition ST7701S sequence
static const st7701_lcd_init_cmd_t s_st7701_cmds[] = {
    {0xFF, (uint8_t[]){0x77, 0x01, 0x00, 0x00, 0x13}, 5, 0},
    {0xEF, (uint8_t[]){0x08}, 1, 0},
    {0xFF, (uint8_t[]){0x77, 0x01, 0x00, 0x00, 0x10}, 5, 0},
    {0xC0, (uint8_t[]){0x63, 0x00}, 2, 0},
    {0xC1, (uint8_t[]){0x0D, 0x02}, 2, 0},
    {0xC2, (uint8_t[]){0x10, 0x08}, 2, 0},
    {0xCC, (uint8_t[]){0x10}, 1, 0},
    {0xB0, (uint8_t[]){0x80, 0x09, 0x53, 0x0C, 0xD0, 0x07, 0x0C, 0x09,
                       0x09, 0x28, 0x06, 0xD4, 0x13, 0x69, 0x2B, 0x71}, 16, 0},
    {0xB1, (uint8_t[]){0x80, 0x94, 0x5A, 0x10, 0xD3, 0x06, 0x0A, 0x08,
                       0x08, 0x25, 0x03, 0xD3, 0x12, 0x66, 0x6A, 0x0D}, 16, 0},
    {0xFF, (uint8_t[]){0x77, 0x01, 0x00, 0x00, 0x11}, 5, 0},
    {0xB0, (uint8_t[]){0x5D}, 1, 0},
    {0xB1, (uint8_t[]){0x58}, 1, 0},
    {0xB2, (uint8_t[]){0x87}, 1, 0},
    {0xB3, (uint8_t[]){0x80}, 1, 0},
    {0xB5, (uint8_t[]){0x4E}, 1, 0},
    {0xB7, (uint8_t[]){0x85}, 1, 0},
    {0xB8, (uint8_t[]){0x21}, 1, 0},
    {0xB9, (uint8_t[]){0x10, 0x1F}, 2, 0},
    {0xBB, (uint8_t[]){0x03}, 1, 0},
    {0xBC, (uint8_t[]){0x00}, 1, 0},
    {0xC1, (uint8_t[]){0x78}, 1, 0},
    {0xC2, (uint8_t[]){0x78}, 1, 0},
    {0xD0, (uint8_t[]){0x88}, 1, 0},
    {0xE0, (uint8_t[]){0x00, 0x3A, 0x02}, 3, 0},
    {0xE1, (uint8_t[]){0x04, 0xA0, 0x00, 0xA0, 0x05, 0xA0, 0x00, 0xA0,
                       0x00, 0x40, 0x40}, 11, 0},
    {0xE2, (uint8_t[]){0x30, 0x00, 0x40, 0x40, 0x32, 0xA0, 0x00, 0xA0,
                       0x00, 0xA0, 0x00, 0xA0, 0x00}, 13, 0},
    {0xE3, (uint8_t[]){0x00, 0x00, 0x33, 0x33}, 4, 0},
    {0xE4, (uint8_t[]){0x44, 0x44}, 2, 0},
    {0xE5, (uint8_t[]){0x09, 0x2E, 0xA0, 0xA0, 0x0B, 0x30, 0xA0, 0xA0,
                       0x05, 0x2A, 0xA0, 0xA0, 0x07, 0x2C, 0xA0, 0xA0}, 16, 0},
    {0xE6, (uint8_t[]){0x00, 0x00, 0x33, 0x33}, 4, 0},
    {0xE7, (uint8_t[]){0x44, 0x44}, 2, 0},
    {0xE8, (uint8_t[]){0x08, 0x2D, 0xA0, 0xA0, 0x0A, 0x2F, 0xA0, 0xA0,
                       0x04, 0x29, 0xA0, 0xA0, 0x06, 0x2B, 0xA0, 0xA0}, 16, 0},
    {0xEB, (uint8_t[]){0x00, 0x00, 0x4E, 0x4E, 0x00, 0x00, 0x00}, 7, 0},
    {0xEC, (uint8_t[]){0x08, 0x01}, 2, 0},
    {0xFF, (uint8_t[]){0x77, 0x01, 0x00, 0x00, 0x00}, 5, 0},
    {0x3A, (uint8_t[]){0x55}, 1, 0},   /* RGB565 */
    {0x11, (uint8_t[]){0x00}, 0, 120},
    {0x29, (uint8_t[]){0x00}, 0, 20},
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
    // nothing needed
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
    switch (usb_get_connected_modeller_type())
    {
        case AMP_MODELLER_TONEX_ONE:    // fallthrough
        case AMP_MODELLER_TONEX:        // fallthrough    
        case AMP_MODELLER_TONEX_ONE_PLUS:   // fallthrough
        case AMP_MODELLER_TONEX_PLUG:
        default:
        {
            // Tonex
            if (max_entries <= 8)
            {
                dest[0] = -12;
                dest[1] = 44;
                dest[2] = 96;
                dest[3] = 148;
                dest[4] = 200;
                dest[5] = 252;
                dest[6] = 304;
                dest[7] = 356;
            }
        } break;

        case AMP_MODELLER_VALETON_GP5:
        {
            // Valeton
            if (max_entries <= 10)
            {
                dest[0] = -19;
                dest[1] = 24;
                dest[2] = 67;
                dest[3] = 110;
                dest[4] = 153;
                dest[5] = 196;
                dest[6] = 239;
                dest[7] = 282;
                dest[8] = 325;
                dest[9] = 368;
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
__attribute__((unused)) lv_dir_t platform_adjust_gesture(lv_dir_t gesture)
{
    // nothing special needed
    return gesture;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void platform_display_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    // rotate 90 degrees
    for (int y = area->y1; y <= area->y2; y++) 
    {
        for (int x = area->x1; x <= area->x2; x++) 
        {
            int dx = y;
            int dy = (LV_HOR - 1 - x);
            s_fb[dy * PANEL_W + dx] = color_map->full;
            color_map++;
        }
    }
    
    size_t fb_bytes = PANEL_W * PANEL_H * sizeof(uint16_t);
    esp_cache_msync(s_fb, fb_bytes, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

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
esp_err_t ret = ESP_OK;
    uint8_t touch_ok = 0;
    I2CMutexHandle = I2CMutex;
    disp_drv = pdisp_drv;

    ESP_LOGI(TAG, "Platform Init");

    // backlight 
    gpio_config_t bl_io = {
        .pin_bit_mask = 1ULL << LCD_BL,
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&bl_io);
    gpio_set_level(LCD_BL, 0);

    // init C6 hosted 
    esp_hosted_init();
    esp_hosted_connect_to_slave();

    // reset lcd
    gpio_config_t lcd_rst_io = {
        .pin_bit_mask = 1ULL << LCD_RST,
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&lcd_rst_io);
    gpio_set_level(LCD_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(LCD_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(120));

    // display init
    esp_ldo_channel_handle_t ldo = NULL;
    esp_ldo_channel_config_t ldo_cfg = {
        .chan_id = DSI_LDO_CHAN,
        .voltage_mv = DSI_LDO_MV,
    };
    ESP_ERROR_CHECK(esp_ldo_acquire_channel(&ldo_cfg, &ldo));

    esp_lcd_dsi_bus_handle_t dsi = NULL;
    esp_lcd_dsi_bus_config_t bus_cfg = {
        .bus_id = 0,
        .num_data_lanes = DSI_LANES,
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = DSI_MBPS,
    };
    ESP_ERROR_CHECK(esp_lcd_new_dsi_bus(&bus_cfg, &dsi));
    vTaskDelay(pdMS_TO_TICKS(50));

    esp_lcd_dbi_io_config_t dbi_cfg = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_dbi(dsi, &dbi_cfg, &lcd_io));

    esp_lcd_dpi_panel_config_t dpi_cfg = {
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = DPI_MHZ,
        .virtual_channel = 0,
        .in_color_format = LCD_COLOR_FMT_RGB565,
        .out_color_format = LCD_COLOR_FMT_RGB565,
        .num_fbs = 2,
        .video_timing = {
            .h_size = PANEL_W,
            .v_size = PANEL_H,
            .hsync_pulse_width = 12,
            .hsync_back_porch = 42,
            .hsync_front_porch = 42,
            .vsync_pulse_width = 2,
            .vsync_back_porch = 8,
            .vsync_front_porch = 166,
        },
    };

    st7701_vendor_config_t vendor = {
        .init_cmds = s_st7701_cmds,
        .init_cmds_size = sizeof(s_st7701_cmds) / sizeof(s_st7701_cmds[0]),
        .flags = { .use_mipi_interface = 1 },
        .mipi_config = {
            .dsi_bus = dsi,
            .dpi_config = &dpi_cfg,
        },
    };

    const esp_lcd_panel_dev_config_t dev_cfg = {
        .reset_gpio_num = GPIO_NUM_NC,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = &vendor,
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_st7701(lcd_io, &dev_cfg, &lcd_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(lcd_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(lcd_panel, true));
    (void)esp_lcd_dpi_panel_enable_dma2d(lcd_panel);

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();

    gpio_set_level(LCD_BL, 1);

    // init display driver
    void *fb0 = NULL;
    void *fb1 = NULL;
    ESP_ERROR_CHECK(esp_lcd_dpi_panel_get_frame_buffer(lcd_panel, 2, &fb0, &fb1));
    s_fb = (uint16_t *)fb0;

    const size_t partial = LV_HOR * 40;
    s_lv_buf = heap_caps_malloc(partial * sizeof(lv_color_t),  MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!s_lv_buf) 
    {
        s_lv_buf = heap_caps_malloc(partial * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    }
    assert(s_lv_buf);

    lv_disp_draw_buf_init(&disp_buf, s_lv_buf, NULL, partial);

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(pdisp_drv);
    pdisp_drv->hor_res  = LV_HOR;
    pdisp_drv->ver_res  = LV_VER;
    pdisp_drv->flush_cb = platform_display_lvgl_flush_cb;
    pdisp_drv->draw_buf = &disp_buf;
    pdisp_drv->full_refresh = 0;
    pdisp_drv->user_data = lcd_panel;

    lv_disp_t* disp = lv_disp_drv_register(pdisp_drv);
    if (disp == NULL) 
    {
        ESP_LOGE(TAG, "lv_disp_drv_register failed");
    }

    ESP_LOGI(TAG, "LVGL %dx%d  FB %p", lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL), s_fb);

    // reset touch screen
    gpio_config_t touch_rst_io = {
        .pin_bit_mask = 1ULL << TOUCH_RST,
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&touch_rst_io);
    gpio_set_level(TOUCH_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(TOUCH_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(50));

    // init touch
    if (xSemaphoreTake(I2CMutex, pdMS_TO_TICKS(10000)) == pdTRUE) 
    {
        uint8_t addr = ESP_LCD_TOUCH_IO_I2C_GT911_ADDRESS;

        // debug
        ESP_LOGI(TAG, "probe 0x5D %s", esp_err_to_name(i2c_master_probe(bus_handle, 0x5D, 100)));
        ESP_LOGI(TAG, "probe 0x14 %s", esp_err_to_name(i2c_master_probe(bus_handle, 0x14, 100)));

        if (i2c_master_probe(bus_handle, addr, 100) != ESP_OK) 
        {
            ESP_LOGI(TAG, "GT911 alternate addr");
            addr = ESP_LCD_TOUCH_IO_I2C_GT911_ADDRESS_BACKUP;
        }

        esp_lcd_panel_io_handle_t tp_io = NULL;
        esp_lcd_panel_io_i2c_config_t io_cfg = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();
        io_cfg.dev_addr = addr;
        io_cfg.scl_speed_hz = 100000;
        ret = esp_lcd_new_panel_io_i2c(bus_handle, &io_cfg, &tp_io);
        
        if (ret == ESP_OK) 
        {
            const esp_lcd_touch_config_t tp_cfg = {
                .x_max = LV_HOR,
                .y_max = LV_VER,
                .rst_gpio_num = GPIO_NUM_NC,
                .int_gpio_num = TOUCH_INT,
                .levels = { .reset = 0, .interrupt = 0 },
                .flags = { .swap_xy = 0, .mirror_x = 0, .mirror_y = 0 },
            };
            ret = esp_lcd_touch_new_i2c_gt911(tp_io, &tp_cfg, &tp);
        }
        xSemaphoreGive(I2CMutex);
    } 
    else 
    {
        ESP_LOGE(TAG, "Touch mutex timeout");
        ret = ESP_ERR_TIMEOUT;
    }

    if (ret == ESP_OK && tp) 
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
        indev_drv.read_cb = display_lvgl_touch_cb;
        indev_drv.user_data = tp;
        lv_indev_drv_register(&indev_drv);
    }

    if (control_get_config_item_int(CONFIG_ITEM_SCREEN_ROTATION) == SCREEN_ROTATION_180)
    {
        // 270 here as normal landscape mode is 90 degrees
        //?? rotation_setting = LV_DISP_ROT_270;
    }
}

#endif //CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_JC4880P4