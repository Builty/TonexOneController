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
#include "esp_task_wdt.h"

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
#include "esp_hosted_ota.h"
#include "esp_cache.h"
#include "driver/ppa.h"

static const char *TAG = "platform_jc4880p4";

#define BUF_SIZE                            (1024)
#define I2C_MASTER_TIMEOUT_MS               1000
#define PANEL_W                             480
#define PANEL_H                             800
#define LV_HOR                              800
#define LV_VER                              480

static SemaphoreHandle_t I2CMutexHandle;
static lv_disp_drv_t* disp_drv;      // contains callback functions
static esp_lcd_panel_handle_t disp_panel = NULL;
static esp_lcd_touch_handle_t tp = NULL;
static lv_indev_drv_t indev_drv;    // Input device driver (Touch)
static ppa_client_handle_t s_ppa;
static void *s_fb;                  // 480*800 RGB565 from DPI
static size_t s_fb_bytes;
static uint32_t rotation_angle = 90;

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(6, 0, 0)
#define ST7701_480_360_PANEL_60HZ_DPI_CONFIG(color_fmt) \
    {                                                   \
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,    \
        .dpi_clock_freq_mhz = 34,                       \
        .virtual_channel = 0,                           \
        .in_color_format = (color_fmt),                 \
        .out_color_format = (color_fmt),                \
        .num_fbs = 1,                                   \
        .video_timing = {                               \
            .h_size = 480,                              \
            .v_size = 800,                              \
            .hsync_back_porch = 42,                     \
            .hsync_pulse_width = 12,                    \
            .hsync_front_porch = 42,                    \
            .vsync_back_porch = 8,                      \
            .vsync_pulse_width = 2,                     \
            .vsync_front_porch = 371,                   \
        },                                              \
    }
#else
#define ST7701_480_360_PANEL_60HZ_DPI_CONFIG(px_format) \
    {                                                   \
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,    \
        .dpi_clock_freq_mhz = 34,                       \
        .virtual_channel = 0,                           \
        .pixel_format = px_format,                      \
        .num_fbs = 1,                                   \
        .video_timing = {                               \
            .h_size = 480,                              \
            .v_size = 800,                              \
            .hsync_back_porch = 42,                     \
            .hsync_pulse_width = 12,                    \
            .hsync_front_porch = 42,                    \
            .vsync_back_porch = 8,                      \
            .vsync_pulse_width = 2,                     \
            .vsync_front_porch = 371,                   \
        },                                              \
        .flags.use_dma2d = true,                        \
    }
#endif

// ST7701 power/gamma initialisation sequence for the JC4880P443C_I_W panel.
static const st7701_lcd_init_cmd_t s_st7701_init_cmds[] = {
    {0xFF, (uint8_t []){0x77, 0x01, 0x00, 0x00, 0x13}, 5, 0},
    {0xEF, (uint8_t []){0x08}, 1, 0},
    {0xFF, (uint8_t []){0x77, 0x01, 0x00, 0x00, 0x10}, 5, 0},
    {0xC0, (uint8_t []){0x63, 0x00}, 2, 0},
    {0xC1, (uint8_t []){0x0D, 0x02}, 2, 0},
    {0xC2, (uint8_t []){0x10, 0x08}, 2, 0},
    {0xCC, (uint8_t []){0x10}, 1, 0},
    {0xB0, (uint8_t []){0x80, 0x09, 0x53, 0x0C, 0xD0, 0x07, 0x0C, 0x09, 0x09, 0x28, 0x06, 0xD4, 0x13, 0x69, 0x2B, 0x71}, 16, 0},
    {0xB1, (uint8_t []){0x80, 0x94, 0x5A, 0x10, 0xD3, 0x06, 0x0A, 0x08, 0x08, 0x25, 0x03, 0xD3, 0x12, 0x66, 0x6A, 0x0D}, 16, 0},
    {0xFF, (uint8_t []){0x77, 0x01, 0x00, 0x00, 0x11}, 5, 0},
    {0xB0, (uint8_t []){0x5D}, 1, 0},
    {0xB1, (uint8_t []){0x58}, 1, 0},
    {0xB2, (uint8_t []){0x87}, 1, 0},
    {0xB3, (uint8_t []){0x80}, 1, 0},
    {0xB5, (uint8_t []){0x4E}, 1, 0},
    {0xB7, (uint8_t []){0x85}, 1, 0},
    {0xB8, (uint8_t []){0x21}, 1, 0},
    {0xB9, (uint8_t []){0x10, 0x1F}, 2, 0},
    {0xBB, (uint8_t []){0x03}, 1, 0},
    {0xBC, (uint8_t []){0x00}, 1, 0},
    {0xC1, (uint8_t []){0x78}, 1, 0},
    {0xC2, (uint8_t []){0x78}, 1, 0},
    {0xD0, (uint8_t []){0x88}, 1, 0},
    {0xE0, (uint8_t []){0x00, 0x3A, 0x02}, 3, 0},
    {0xE1, (uint8_t []){0x04, 0xA0, 0x00, 0xA0, 0x05, 0xA0, 0x00, 0xA0, 0x00, 0x40, 0x40}, 11, 0},
    {0xE2, (uint8_t []){0x30, 0x00, 0x40, 0x40, 0x32, 0xA0, 0x00, 0xA0, 0x00, 0xA0, 0x00, 0xA0, 0x00}, 13, 0},
    {0xE3, (uint8_t []){0x00, 0x00, 0x33, 0x33}, 4, 0},
    {0xE4, (uint8_t []){0x44, 0x44}, 2, 0},
    {0xE5, (uint8_t []){0x09, 0x2E, 0xA0, 0xA0, 0x0B, 0x30, 0xA0, 0xA0, 0x05, 0x2A, 0xA0, 0xA0, 0x07, 0x2C, 0xA0, 0xA0}, 16, 0},
    {0xE6, (uint8_t []){0x00, 0x00, 0x33, 0x33}, 4, 0},
    {0xE7, (uint8_t []){0x44, 0x44}, 2, 0},
    {0xE8, (uint8_t []){0x08, 0x2D, 0xA0, 0xA0, 0x0A, 0x2F, 0xA0, 0xA0, 0x04, 0x29, 0xA0, 0xA0, 0x06, 0x2B, 0xA0, 0xA0}, 16, 0},
    {0xEB, (uint8_t []){0x00, 0x00, 0x4E, 0x4E, 0x00, 0x00, 0x00}, 7, 0},
    {0xEC, (uint8_t []){0x08, 0x01}, 2, 0},
    {0xED, (uint8_t []){0xB0, 0x2B, 0x98, 0xA4, 0x56, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0x65, 0x4A, 0x89, 0xB2, 0x0B}, 16, 0},
    {0xEF, (uint8_t []){0x08, 0x08, 0x08, 0x45, 0x3F, 0x54}, 6, 0},
    {0xFF, (uint8_t []){0x77, 0x01, 0x00, 0x00, 0x00}, 5, 0},
    {0x11, (uint8_t []){0x00}, 1, 120},   // sleep out, 120 ms delay
    {0x29, (uint8_t []){0x00}, 1, 20},    // display on, 20 ms delay
};

// embedded C6 slave firmware
extern const uint8_t network_adapter_esp32c6_bin_start[] asm("_binary_network_adapter_esp32c6_bin_start");
extern const uint8_t network_adapter_esp32c6_bin_end[]   asm("_binary_network_adapter_esp32c6_bin_end");
#define C6_OTA_CHUNK       1024

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void c6_reset(void)
{
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << GPIO_NUM_54,
        .mode = GPIO_MODE_OUTPUT,
    };

    // reset the C6 co-processor
    gpio_config(&io);
    gpio_set_level(GPIO_NUM_54, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(GPIO_NUM_54, 1);
    vTaskDelay(pdMS_TO_TICKS(1500));
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t c6_ota_push(void)
{
    const uint8_t *p   = network_adapter_esp32c6_bin_start;
    const size_t   total = (size_t)(network_adapter_esp32c6_bin_end - p);
    size_t sent = 0;

    ESP_LOGI(TAG, "C6 OTA begin, %u bytes", (unsigned)total);
    ESP_RETURN_ON_ERROR(esp_hosted_slave_ota_begin(), TAG, "ota_begin");

    while (sent < total) 
    {
        size_t n = total - sent;
        if (n > C6_OTA_CHUNK) 
        {
            n = C6_OTA_CHUNK;
        }
        
        ESP_RETURN_ON_ERROR(esp_hosted_slave_ota_write((void *)(p + sent), n), TAG, "ota_write @ %u", (unsigned)sent);
        sent += n;
        
        if ((sent & 0x7FFF) == 0) 
        {
            ESP_LOGI(TAG, "C6 OTA %u / %u", (unsigned)sent, (unsigned)total);
        }
        //esp_task_wdt_reset();
        vTaskDelay(1);
    }

    ESP_RETURN_ON_ERROR(esp_hosted_slave_ota_end(), TAG, "ota_end");

    /* Required on C6 >= 2.6; 2.3.2 often returns an error — ignore */
    esp_err_t act = esp_hosted_slave_ota_activate();
    if (act != ESP_OK) 
    {
        ESP_LOGW(TAG, "ota_activate: %s (ok on 2.3.2)", esp_err_to_name(act));
    }

    ESP_LOGI(TAG, "C6 OTA done, resetting C6");
    c6_reset();
    return ESP_OK;
}

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
                dest[0] = -4;
                dest[1] = 85;
                dest[2] = 170;
                dest[3] = 255;
                dest[4] = 340;
                dest[5] = 425;
                dest[6] = 505;
                dest[7] = 590;
            }
        } break;

        case AMP_MODELLER_VALETON_GP5:
        {
            // Valeton
            if (max_entries <= 10)
            {
                dest[0] = -15;
                dest[1] = 55;
                dest[2] = 125;
                dest[3] = 195;
                dest[4] = 265;
                dest[5] = 335;
                dest[6] = 405;
                dest[7] = 475;
                dest[8] = 545;
                dest[9] = 615;
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
static void platform_display_lvgl_flush_cb(lv_disp_drv_t *drv,
                                           const lv_area_t *area,
                                           lv_color_t *color_map)
{
    int dest_x;
    int dest_y;
    int ppa_rot;

    if (!drv || !area || !color_map || !s_ppa || !s_fb) 
    {
        if (drv) 
        {
            lv_disp_flush_ready(drv);
        }
        return;
    }

    const int w = area->x2 - area->x1 + 1;
    const int h = area->y2 - area->y1 + 1;
    
    if (w <= 0 || h <= 0) 
    {
        lv_disp_flush_ready(drv);
        return;
    }

    if (rotation_angle == 90)
    {
        dest_x = area->y1;
        dest_y = PANEL_H - (area->x2 + 1);
        ppa_rot = PPA_SRM_ROTATION_ANGLE_90;
    }
    else
    {
        // 270° CCW: dest_x mirrored in Y, dest_y = x1 
        dest_x = PANEL_W - (area->y2 + 1);
        dest_y = area->x1;
        ppa_rot = PPA_SRM_ROTATION_ANGLE_270;
    }

    if ((dest_x < 0) || (dest_y < 0) || ((dest_x + h) > PANEL_W) || ((dest_y + w) > PANEL_H)) 
    {
        ESP_LOGE(TAG, "PPA dest OOB dest=%d,%d wh=%d,%d", dest_x, dest_y, w, h);
        lv_disp_flush_ready(drv);
        return;
    }

    const size_t src_bytes = (size_t)w * (size_t)h * sizeof(lv_color_t);
    esp_cache_msync(color_map, src_bytes, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);

    ppa_srm_oper_config_t op = {
        .in.buffer          = color_map,
        .in.pic_w           = (uint32_t)w,
        .in.pic_h           = (uint32_t)h,
        .in.block_w         = (uint32_t)w,
        .in.block_h         = (uint32_t)h,
        .in.block_offset_x  = 0,
        .in.block_offset_y  = 0,
        .in.srm_cm          = PPA_SRM_COLOR_MODE_RGB565,

        .out.buffer         = s_fb,
        .out.buffer_size    = s_fb_bytes,
        .out.pic_w          = PANEL_W,
        .out.pic_h          = PANEL_H,
        .out.block_offset_x = (uint32_t)dest_x,
        .out.block_offset_y = (uint32_t)dest_y,
        .out.srm_cm         = PPA_SRM_COLOR_MODE_RGB565,

        .rotation_angle     = ppa_rot,
        .scale_x            = 1.0f,
        .scale_y            = 1.0f,
        .mode               = PPA_TRANS_MODE_BLOCKING,
    };

    ppa_do_scale_rotate_mirror(s_ppa, &op);
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
    lv_color_t *buf1 = NULL;
    lv_color_t *buf2 = NULL;
    uint32_t buffer_size = 24000;

    ESP_LOGI(TAG, "Platform Init");

    // Setup LEDC peripheral for PWM backlight control
    const ledc_timer_config_t LCD_backlight_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK
    };

    ESP_ERROR_CHECK(ledc_timer_config(&LCD_backlight_timer));

    const ledc_channel_config_t LCD_backlight_channel = {
        .gpio_num = LCD_BL,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    
    ESP_ERROR_CHECK(ledc_channel_config(&LCD_backlight_channel));

    // init C6 hosted 
    esp_hosted_init();

    esp_err_t err = esp_hosted_connect_to_slave();
    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "hosted connect failed: %s", esp_err_to_name(err));
    }

    esp_hosted_coprocessor_fwver_t fw = {0};
    if (esp_hosted_get_coprocessor_fwversion(&fw) != ESP_OK) 
    {
        ESP_LOGE(TAG, "CP not responding");
    }
    else
    {
        // check if upate is needed for the Hosted co-processor
        // 3 here is host major version. No define exists from hosted component
        if (fw.major1 < 3) 
        {
            // update the slave f/w
            ESP_LOGW(TAG, "Updating Slave firmware");

            if (c6_ota_push() != ESP_OK)
            {
                ESP_LOGE(TAG, "Slave firmware update failed!");
            }

            // get firmware again
            memset((void*)&fw, 0, sizeof(fw));
            if (esp_hosted_get_coprocessor_fwversion(&fw) == ESP_OK) 
            {
                ESP_LOGI(TAG, "C6 now %u.%u.%u", fw.major1, fw.minor1, fw.patch1);
            }
        }
    }

    // display init
    esp_ldo_channel_handle_t ldo = NULL;
    esp_ldo_channel_config_t ldo_cfg = {
        .chan_id = DSI_LDO_CHAN,
        .voltage_mv = DSI_LDO_MV,
    };
    ESP_ERROR_CHECK(esp_ldo_acquire_channel(&ldo_cfg, &ldo));
    ESP_LOGI(TAG, "MIPI DSI PHY Powered on");

    // create MIPI DSI bus first, it will initialize the DSI PHY as well
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus;
    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = 2,
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = 500,
    };
    ESP_ERROR_CHECK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

    ESP_LOGI(TAG, "Install MIPI DSI LCD control panel");
    
    // we use DBI interface to send LCD commands and parameters
    esp_lcd_panel_io_handle_t io;
    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,   // according to the LCD ILI9881C spec
        .lcd_param_bits = 8, // according to the LCD ILI9881C spec
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &io));

    // create control panel
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(6, 0, 0)
    esp_lcd_dpi_panel_config_t dpi_config = ST7701_480_360_PANEL_60HZ_DPI_CONFIG(LCD_COLOR_FMT_RGB565);
#else    
    esp_lcd_dpi_panel_config_t dpi_config = ST7701_480_360_PANEL_60HZ_DPI_CONFIG(LCD_COLOR_PIXEL_FORMAT_RGB565);
#endif    

    dpi_config.num_fbs = 1;

    st7701_vendor_config_t vendor_config = {
        .init_cmds      = s_st7701_init_cmds,
        .init_cmds_size = sizeof(s_st7701_init_cmds) / sizeof(s_st7701_init_cmds[0]),
        .mipi_config = {
            .dsi_bus = mipi_dsi_bus,
            .dpi_config = &dpi_config,
        },
        .flags = {
            .use_mipi_interface = 1,
        }
    };
    
    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .bits_per_pixel = 16,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .reset_gpio_num = GPIO_NUM_5,
        .vendor_config = &vendor_config,
    };
    
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7701(io, &lcd_dev_config, &disp_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(disp_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(disp_panel));

    esp_lcd_dpi_panel_get_frame_buffer(disp_panel, 1, &s_fb);

    // clear frame buffer
    memset((void*)s_fb, 0, PANEL_W * PANEL_H * 2);
    esp_cache_msync(s_fb, PANEL_W * PANEL_H * 2, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);

    s_fb_bytes = PANEL_W * PANEL_H * 2;
    ppa_client_config_t ppa_cfg = { .oper_type = PPA_OPERATION_SRM };
    ppa_register_client(&ppa_cfg, &s_ppa);


    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();

    // alloc draw buffers used by LVGL
    buf1 = heap_caps_malloc(buffer_size * sizeof(lv_color_t), MALLOC_CAP_DMA);
    if (buf1 == NULL)
    {
        ESP_LOGE(TAG, "Not enough memory for LVGL buffer (buf1) allocation!");   
    }

    lv_disp_draw_buf_t *disp_buf = malloc(sizeof(lv_disp_draw_buf_t));
    if (disp_buf == NULL)
    {
        ESP_LOGE(TAG, "Not enough memory for LVGL display buffer allocation!");   
    }

    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(disp_buf, buf1, buf2, buffer_size);

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(disp_drv);
    disp_drv->hor_res = LV_HOR;
    disp_drv->ver_res = LV_VER;
    disp_drv->flush_cb = platform_display_lvgl_flush_cb;
    disp_drv->draw_buf = disp_buf;
    disp_drv->user_data = disp_panel;

    lv_disp_t* __attribute__((unused)) disp = lv_disp_drv_register(disp_drv);

    if (disp == NULL) 
    {
        ESP_LOGE(TAG, "lv_disp_drv_register failed");
    }
    else
    {
        if (control_get_config_item_int(CONFIG_ITEM_SCREEN_ROTATION) == SCREEN_ROTATION_180)
        {
            rotation_angle = 270;
        }
        else
        {
            rotation_angle = 90;
        }
    }

    // set backlight on
    uint8_t brightness_percent = 100;
    uint32_t duty_cycle = (1023 * brightness_percent) / 100; // LEDC resolution set to 10bits, thus: 100% = 1023
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty_cycle));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

    // init touch
    if (xSemaphoreTake(I2CMutex, pdMS_TO_TICKS(10000)) == pdTRUE) 
    {
        uint8_t addr = ESP_LCD_TOUCH_IO_I2C_GT911_ADDRESS;

        esp_lcd_panel_io_handle_t tp_io = NULL;
        esp_lcd_panel_io_i2c_config_t io_cfg = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();
        io_cfg.dev_addr = addr;
        io_cfg.scl_speed_hz = 100000;
        ret = esp_lcd_new_panel_io_i2c(bus_handle, &io_cfg, &tp_io);
        
        if (ret == ESP_OK) 
        {
            esp_lcd_touch_config_t tp_cfg = {
                .x_max = LV_VER,
                .y_max = LV_HOR,
                .rst_gpio_num = GPIO_NUM_NC,
                .int_gpio_num = GPIO_NUM_NC,
                .levels = { .reset = 0, .interrupt = 0 },
                .flags = { .swap_xy = 1, .mirror_x = 1, .mirror_y = 0 },
            };

            if (rotation_angle == 90)
            {
                tp_cfg.flags.swap_xy = 1;
                tp_cfg.flags.mirror_x = 0;
                tp_cfg.flags.mirror_y = 1;
            }
            else if (rotation_angle == 270)
            {
                tp_cfg.flags.swap_xy = 1;
                tp_cfg.flags.mirror_x = 1;
                tp_cfg.flags.mirror_y = 0;
            }
            
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
}

#endif //CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_JC4880P4