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

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_35

// LCD: ST7796, touch: FT6336

static const char *TAG = "platform_ws35";

// LCD panel config
#define WAVESHARE_35_LCD_H_RES               (480)
#define WAVESHARE_35_LCD_V_RES               (320)

/* LCD settings */
#define WAVESHARE_35_LCD_SPI_NUM             (SPI2_HOST)
#define WAVESHARE_35_LCD_PIXEL_CLK_HZ        (80 * 1000 * 1000)
#define WAVESHARE_35_LCD_CMD_BITS            (8)
#define WAVESHARE_35_LCD_PARAM_BITS          (8)
#define WAVESHARE_35_LCD_COLOR_SPACE         (LCD_RGB_ELEMENT_ORDER_RGB)
#define WAVESHARE_35_LCD_BITS_PER_PIXEL      (16)
#define WAVESHARE_35_LCD_DRAW_BUFF_DOUBLE    (1)
#define WAVESHARE_35_LCD_DRAW_BUFF_HEIGHT    (50)
#define WAVESHARE_35_LCD_BL_ON_LEVEL         (0)

#define BUF_SIZE                        (1024)
#define I2C_MASTER_TIMEOUT_MS           1000

static SemaphoreHandle_t I2CMutexHandle;
static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
static lv_disp_drv_t* disp_drv;      // contains callback functions
static esp_lcd_panel_io_handle_t lcd_io = NULL;
static esp_lcd_panel_handle_t lcd_panel = NULL;
static esp_lcd_touch_handle_t tp = NULL;
static esp_lcd_panel_io_handle_t tp_io_handle = NULL;
static lv_indev_drv_t indev_drv;    // Input device driver (Touch)
static esp_io_expander_handle_t expander_handle = NULL;

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void InitIOExpander(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex)
{    
    if (xSemaphoreTake(I2CMutex, (TickType_t)100) == pdTRUE)
    {
        // init IO expander
        if (esp_io_expander_new_i2c_tca9554(bus_handle, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, &expander_handle) != ESP_OK)
        {
            ESP_LOGE(TAG, "Onboard IO Expander init 1 failed");
        }
        
        if (esp_io_expander_set_dir(expander_handle,  LCD_RESET, IO_EXPANDER_OUTPUT) != ESP_OK)
        {
            ESP_LOGE(TAG, "Onboard IO Expander init 2 failed");
        }
    
        // reset LCD
        esp_io_expander_set_level(expander_handle, LCD_RESET, 0);
        xSemaphoreGive(I2CMutexHandle);

        vTaskDelay(pdMS_TO_TICKS(100));

        if (xSemaphoreTake(I2CMutex, (TickType_t)100) == pdTRUE)
        {
            esp_io_expander_set_level(expander_handle, LCD_RESET, 1);
            xSemaphoreGive(I2CMutexHandle);
        }
        else
        {
            ESP_LOGE(TAG, "Onboard IO Expander mutex failed");
        }

        vTaskDelay(pdMS_TO_TICKS(100));    
    }
    else
    {
        ESP_LOGE(TAG, "Onboard IO Expander mutex failed");
    }
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
void platform_init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex, lv_disp_drv_t* pdisp_drv)
{    
    __attribute__((unused)) esp_err_t ret = ESP_OK;
    uint8_t touch_ok = 0;
    I2CMutexHandle = I2CMutex;
    disp_drv = pdisp_drv;

    ESP_LOGI(TAG, "Platform Init");

    // init onboard IO expander
    ESP_LOGI(TAG, "Init Onboard IO Expander");
    InitIOExpander(bus_handle, I2CMutex);

    /* LCD initialization */
    ESP_LOGD(TAG, "Initialize SPI bus");
    const spi_bus_config_t buscfg = {
        .sclk_io_num = WAVESHARE_35_LCD_GPIO_SCLK,
        .mosi_io_num = WAVESHARE_35_LCD_GPIO_MOSI,
        .miso_io_num = WAVESHARE_35_LCD_GPIO_MISO,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        // note here: this value needs to be: WAVESHARE_35_LCD_H_RES * WAVESHARE_35_LCD_DRAW_BUFF_HEIGHT * sizeof(uint16_t)
        // however, the ESP framework uses multiples of 4092 for DMA (LLDESC_MAX_NUM_PER_DESC).
        // this theoretical number is 5.8 times the DMA size, which gets rounded down and ends up too small.
        // so instead, manually setting it to a little larger
        .max_transfer_sz = 6 * LLDESC_MAX_NUM_PER_DESC,        
    };
    spi_bus_initialize(WAVESHARE_35_LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO);

    ESP_LOGD(TAG, "Install panel IO");
    const esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = WAVESHARE_35_LCD_GPIO_DC,
        .cs_gpio_num = WAVESHARE_35_LCD_GPIO_CS,
        .pclk_hz = WAVESHARE_35_LCD_PIXEL_CLK_HZ,
        .lcd_cmd_bits = WAVESHARE_35_LCD_CMD_BITS,
        .lcd_param_bits = WAVESHARE_35_LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 5,
        .on_color_trans_done = NULL,
        .user_ctx = disp_drv,
    };

    esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)WAVESHARE_35_LCD_SPI_NUM, &io_config, &lcd_io);

    ESP_LOGD(TAG, "Install LCD driver");
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = WAVESHARE_35_LCD_GPIO_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = WAVESHARE_35_LCD_BITS_PER_PIXEL,
    };

    if (esp_lcd_new_panel_st7796(lcd_io, &panel_config, &lcd_panel) != ESP_OK)
    {
        ESP_LOGE(TAG, "LCD panel create failed");
    }

    if (esp_lcd_panel_reset(lcd_panel) != ESP_OK)
    {
        ESP_LOGE(TAG, "LCD panel reset failed");
    }
    
    if (esp_lcd_panel_init(lcd_panel) != ESP_OK)
    {
        ESP_LOGE(TAG, "LCD panel init failed");
    }

    esp_lcd_panel_invert_color(lcd_panel, true);
    esp_lcd_panel_disp_on_off(lcd_panel, true);
    esp_lcd_panel_swap_xy(lcd_panel, true);
    esp_lcd_panel_mirror(lcd_panel, false, true);

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();

    void *buf1 = NULL;
    void *buf2 = NULL;
    ESP_LOGI(TAG, "Allocate separate LVGL draw buffers from PSRAM");
    buf1 = heap_caps_malloc(WAVESHARE_35_LCD_H_RES * 32 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf1);
    buf2 = heap_caps_malloc(WAVESHARE_35_LCD_H_RES * 32 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf2);
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, WAVESHARE_35_LCD_H_RES * 32);

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(disp_drv);
    disp_drv->hor_res = WAVESHARE_35_LCD_H_RES;
    disp_drv->ver_res = WAVESHARE_35_LCD_V_RES;
    disp_drv->flush_cb = display_lvgl_flush_cb;
    disp_drv->draw_buf = &disp_buf;
    disp_drv->user_data = lcd_panel;
    disp_drv->sw_rotate = 1;

    lv_disp_t* __attribute__((unused)) disp = lv_disp_drv_register(disp_drv);

    // init LCD backlight
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {};
    ledc_timer.speed_mode = WAVESHARE_35_LCD_BL_LEDC_MODE,
    ledc_timer.timer_num = WAVESHARE_35_LCD_BL_LEDC_TIMER;
    ledc_timer.duty_resolution = WAVESHARE_35_LCD_BL_LEDC_DUTY_RES;
    ledc_timer.freq_hz = WAVESHARE_35_LCD_BL_LEDC_FREQUENCY;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {};
    ledc_channel.speed_mode = WAVESHARE_35_LCD_BL_LEDC_MODE;
    ledc_channel.channel = WAVESHARE_35_LCD_BL_LEDC_CHANNEL;
    ledc_channel.timer_sel = WAVESHARE_35_LCD_BL_LEDC_TIMER;
    ledc_channel.intr_type = LEDC_INTR_DISABLE;
    ledc_channel.gpio_num = WAVESHARE_35_LCD_GPIO_BL;
    ledc_channel.duty = 0, // Set duty to 0%
    ledc_channel.hpoint = 0;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    // set brightness
    uint8_t brightness = 70;
    uint32_t duty = (brightness * (WAVESHARE_35_LCD_BL_LEDC_DUTY - 1)) / 100;
    ledc_set_duty(WAVESHARE_35_LCD_BL_LEDC_MODE, WAVESHARE_35_LCD_BL_LEDC_CHANNEL, duty);
    ledc_update_duty(WAVESHARE_35_LCD_BL_LEDC_MODE, WAVESHARE_35_LCD_BL_LEDC_CHANNEL);

    // init touch screen    
    const esp_lcd_panel_io_i2c_config_t tp_io_config = {
        .dev_addr = ESP_LCD_TOUCH_IO_I2C_FT6336_ADDRESS,
        .control_phase_bytes = 1,
        .dc_bit_offset = 0,
        .lcd_cmd_bits = 8,
        .flags.disable_control_phase = 1,
        .scl_speed_hz = 400 * 1000
    };
    
    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = WAVESHARE_35_LCD_H_RES,
        .y_max = WAVESHARE_35_LCD_V_RES,
        .rst_gpio_num = GPIO_NUM_NC,
        .int_gpio_num = GPIO_NUM_NC,
        .interrupt_callback = touch_data_ready,
        .flags = {
            .swap_xy = 1,
            .mirror_x = 0,
            .mirror_y = 1,
        },
    };

    // Touch IO handle
    if (esp_lcd_new_panel_io_i2c(bus_handle, &tp_io_config, &tp_io_handle) != ESP_OK)
    {
        ESP_LOGE(TAG, "Touch IO handle failed!");
    }
    
    // Initialize touch
    ESP_LOGI(TAG, "Initialize touch controller FT6336");

    if (xSemaphoreTake(I2CMutex, (TickType_t)10000) == pdTRUE)
    {
        ret = esp_lcd_touch_new_i2c_ft6336(tp_io_handle, &tp_cfg, &tp);
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
}

#endif //CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_35