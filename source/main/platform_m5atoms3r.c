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
#include "display.h"
#include "CH422G.h"
#include "control.h"
#include "task_priorities.h" 
#include "midi_control.h"
#include "LP5562.h"
#include "tonex_params.h"

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_M5ATOMS3R

static const char *TAG = "platform M5Stack Atom S3R";

#define ATOM3SR_LCD_H_RES               (128)
#define ATOM3SR_LCD_V_RES               (128)

/* LCD settings */
#define ATOM3SR_LCD_SPI_NUM             (SPI3_HOST)
#define ATOM3SR_LCD_PIXEL_CLK_HZ        (40 * 1000 * 1000)
#define ATOM3SR_LCD_CMD_BITS            (8)
#define ATOM3SR_LCD_PARAM_BITS          (8)
#define ATOM3SR_LCD_COLOR_SPACE         (ESP_LCD_COLOR_SPACE_BGR)
#define ATOM3SR_LCD_BITS_PER_PIXEL      (16)
#define ATOM3SR_LCD_DRAW_BUFF_DOUBLE    (1)
#define ATOM3SR_LCD_DRAW_BUFF_HEIGHT    (50)

static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
static lv_disp_drv_t* disp_drv;      // contains callback functions
static esp_lcd_panel_io_handle_t lcd_io = NULL;
static esp_lcd_panel_handle_t lcd_panel = NULL;
    
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
    return &lv_font_montserrat_12;
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
    return 5;
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
void platform_init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex, lv_disp_drv_t* pdisp_drv)
{    
    ESP_LOGI(TAG, "Platform Init");
    disp_drv = pdisp_drv;

    // init LP5562 led driver
    ESP_LOGI(TAG, "Init LP5562 Led Driver");
    LP5562_init(bus_handle, I2CMutex);

    // LCD initialization
    ESP_LOGD(TAG, "Initialize SPI bus");
    const spi_bus_config_t buscfg = {
        .sclk_io_num = ATOM3SR_LCD_GPIO_SCLK,
        .mosi_io_num = ATOM3SR_LCD_GPIO_MOSI,
        .miso_io_num = GPIO_NUM_NC,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        // note here: this value needs to be: ATOM3SR_LCD_H_RES * ATOM3SR_LCD_DRAW_BUFF_HEIGHT * sizeof(uint16_t)
        // however, the ESP framework uses multiples of 4092 for DMA (LLDESC_MAX_NUM_PER_DESC).
        // this theoretical number is 3.125 times the DMA size, which gets rounded down and ends up too small.
        // so instead, manually setting it to a little larger (4 rather than 3.125)
        .max_transfer_sz = 4 * LLDESC_MAX_NUM_PER_DESC,
    };
    spi_bus_initialize(ATOM3SR_LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO);

    ESP_LOGD(TAG, "Install panel IO");
    const esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = ATOM3SR_LCD_GPIO_DC,
        .cs_gpio_num = ATOM3SR_LCD_GPIO_CS,
        .pclk_hz = ATOM3SR_LCD_PIXEL_CLK_HZ,
        .lcd_cmd_bits = ATOM3SR_LCD_CMD_BITS,
        .lcd_param_bits = ATOM3SR_LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)ATOM3SR_LCD_SPI_NUM, &io_config, &lcd_io);

    ESP_LOGD(TAG, "Install LCD driver");
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = ATOM3SR_LCD_GPIO_RST,
        .color_space = ATOM3SR_LCD_COLOR_SPACE,
        .bits_per_pixel = ATOM3SR_LCD_BITS_PER_PIXEL,
    };
    esp_lcd_new_panel_gc9107(lcd_io, &panel_config, &lcd_panel);

    esp_lcd_panel_reset(lcd_panel);
    esp_lcd_panel_init(lcd_panel);
    esp_lcd_panel_mirror(lcd_panel, true, true);
    esp_lcd_panel_disp_on_off(lcd_panel, true);

    // LCD backlight on 
    LP5562_set_color(0, 0, 0, 180);

    esp_lcd_panel_set_gap(lcd_panel, 2, 1);
    esp_lcd_panel_invert_color(lcd_panel, true);

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();

    void *buf1 = NULL;
    void *buf2 = NULL;
    ESP_LOGI(TAG, "Allocate separate LVGL draw buffers from PSRAM");
    buf1 = heap_caps_malloc(ATOM3SR_LCD_H_RES * 32 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf1);
    buf2 = heap_caps_malloc(ATOM3SR_LCD_H_RES * 32 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf2);
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, ATOM3SR_LCD_H_RES * 32);

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(disp_drv);
    disp_drv->hor_res = ATOM3SR_LCD_H_RES;
    disp_drv->ver_res = ATOM3SR_LCD_V_RES;
    disp_drv->flush_cb = display_lvgl_flush_cb;
    disp_drv->draw_buf = &disp_buf;
    disp_drv->user_data = lcd_panel;

    lv_disp_t* __attribute__((unused)) disp = lv_disp_drv_register(disp_drv);

    if (control_get_config_item_int(CONFIG_ITEM_SCREEN_ROTATION) == SCREEN_ROTATION_180)
    {
        disp_drv->rotated = LV_DISP_ROT_180;

        // can only do software rotation, with a drop in frame rate
        disp_drv->sw_rotate = 1;
    }
}

#endif //CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_M5ATOMS3R