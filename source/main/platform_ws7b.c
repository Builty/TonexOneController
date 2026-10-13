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
#include "CH32V003.h"
#include "control.h"
#include "task_priorities.h"
#include "midi_control.h"
#include "LP5562.h"
#include "tonex_params.h"

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_7B

static const char *TAG = "platform_ws7b";

// PSRAM bandwidth limits this: the bounce buffer refill competes with WiFi, BT and USB host for PSRAM.
// About 37 Hz refresh with the porches below; short porches at 36 MHz (49 Hz) glitched on hardware
#define DISPLAY_LCD_PIXEL_CLOCK_HZ     (34000000)

#define DISPLAY_LCD_H_RES              1024
#define DISPLAY_LCD_V_RES              600

// UI is designed for 800x480; it is scaled by 5/4 (nearest neighbour) and centered on the panel
#define UI_H_RES                       800
#define UI_V_RES                       480
#define UI_SCALE_NUM                   5
#define UI_SCALE_DEN                   4
#define UI_SCALED_H_RES                ((UI_H_RES * UI_SCALE_NUM) / UI_SCALE_DEN)
#define UI_SCALED_V_RES                ((UI_V_RES * UI_SCALE_NUM) / UI_SCALE_DEN)
#define UI_OFFSET_X                    ((DISPLAY_LCD_H_RES - UI_SCALED_H_RES) / 2)
#define UI_OFFSET_Y                    ((DISPLAY_LCD_V_RES - UI_SCALED_V_RES) / 2)

// The side strips continue each row's plain fill (header, footer, background) to the panel edges. A row counts
// as plain fill only when windows just past the theme's 2 px object border on both UI edges are one identical
// colour, so logos, icons, text and animations at either edge are never smeared into the strips. Only areas
// spanning both windows (full-width redraws) update the strips
#define UI_STRIP_WINDOW_X              2
#define UI_STRIP_WINDOW_W              6
#define UI_STRIP_RIGHT_WINDOW_X        (UI_H_RES - UI_STRIP_WINDOW_X - UI_STRIP_WINDOW_W)

#define DISPLAY_LCD_NUM_FB             1
#define DRAW_BUFFER_SIZE               6
#define GT911_CONFIG_SIZE              184

// GT911 I2C registers (not covered in the ESP driver)
#define ESP_LCD_TOUCH_GT911_CONFIG_REG          0x8047
#define ESP_LCD_TOUCH_GT911_FILTER_OFFSET       (0x8050 - ESP_LCD_TOUCH_GT911_CONFIG_REG)
#define ESP_LCD_TOUCH_GT911_TOUCH_LEVEL_OFFSET  (0x8053 - ESP_LCD_TOUCH_GT911_CONFIG_REG)   // higher value = less sensitive

static SemaphoreHandle_t I2CMutexHandle;
static lv_disp_draw_buf_t disp_buf;
static lv_disp_drv_t* disp_drv;
static esp_lcd_touch_handle_t tp = NULL;
static esp_lcd_panel_io_handle_t tp_io_handle = NULL;
static lv_indev_drv_t indev_drv;
static uint16_t scale_x_map[UI_SCALED_H_RES];
static lv_color_t* scale_line_buf = NULL;
static lv_color_t strip_buf[UI_OFFSET_X];
static lv_color_t strip_fill;
static bool strip_fill_valid = false;
static int32_t strip_last_y1 = -2;
static int32_t strip_last_y2 = -2;

static uint8_t GT911CalcChecksum(uint8_t* buf, uint16_t len)
{
    uint8_t checksum = 0;

    for (uint16_t i = 0; i < len; i++)
    {
        checksum += buf[i];
    }

    checksum = ~checksum + 1; // Two's complement
    return checksum;
}

__attribute__((unused)) void platform_adjust_touch_coords(lv_coord_t* x, lv_coord_t* y)
{
    *x = LV_CLAMP(0, ((*x - UI_OFFSET_X) * UI_SCALE_DEN) / UI_SCALE_NUM, UI_H_RES - 1);
    *y = LV_CLAMP(0, ((*y - UI_OFFSET_Y) * UI_SCALE_DEN) / UI_SCALE_NUM, UI_V_RES - 1);
}

__attribute__((unused)) void platform_adjust_display_flush_area(lv_area_t *area)
{
    // Not called on this platform: platform_display_lvgl_flush_cb does the scaling and centering
}

__attribute__((unused)) void platform_get_icon_coords(int16_t* dest, uint8_t max_entries)
{
    switch (usb_get_connected_modeller_type())
    {
        case AMP_MODELLER_TONEX_ONE:
        case AMP_MODELLER_TONEX:
        case AMP_MODELLER_TONEX_ONE_PLUS:
        case AMP_MODELLER_TONEX_PLUG:
        default:
        {
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

__attribute__((unused)) const lv_font_t* platform_get_toast_font(void)
{
    return &lv_font_montserrat_30;
}

__attribute__((unused)) uint16_t platform_get_toast_padding(void)
{
    return 30;
}

__attribute__((unused)) lv_dir_t platform_adjust_gesture(lv_dir_t gesture)
{
    return gesture;
}

static bool platform_strip_window_colour(const lv_color_t* window, lv_color_t* colour)
{
    for (int32_t i = 1; i < UI_STRIP_WINDOW_W; i++)
    {
        if (window[i].full != window[0].full)
        {
            return false;
        }
    }

    *colour = window[0];
    return true;
}

static void platform_update_side_strips(esp_lcd_panel_handle_t panel_handle, const lv_color_t* src_row, int32_t src_x1, int32_t y)
{
    lv_color_t left;
    lv_color_t right;

    if (platform_strip_window_colour(&src_row[UI_STRIP_WINDOW_X - src_x1], &left) &&
        platform_strip_window_colour(&src_row[UI_STRIP_RIGHT_WINDOW_X - src_x1], &right) &&
        (left.full == right.full))
    {
        strip_fill = left;
        strip_fill_valid = true;
    }

    if (!strip_fill_valid)
    {
        return;
    }

    for (int32_t i = 0; i < UI_OFFSET_X; i++)
    {
        strip_buf[i] = strip_fill;
    }

    esp_lcd_panel_draw_bitmap(panel_handle, 0, y, UI_OFFSET_X, y + 1, strip_buf);
    esp_lcd_panel_draw_bitmap(panel_handle, UI_OFFSET_X + UI_SCALED_H_RES, y, DISPLAY_LCD_H_RES, y + 1, strip_buf);
}

static void platform_display_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
    int32_t src_w = lv_area_get_width(area);

    // Only output pixels whose nearest source pixel lies inside this area, so adjacent areas never overlap
    int32_t out_x1 = ((area->x1 * UI_SCALE_NUM) + UI_SCALE_DEN - 1) / UI_SCALE_DEN;
    int32_t out_x2 = ((((area->x2 + 1) * UI_SCALE_NUM) + UI_SCALE_DEN - 1) / UI_SCALE_DEN) - 1;
    int32_t out_y1 = ((area->y1 * UI_SCALE_NUM) + UI_SCALE_DEN - 1) / UI_SCALE_DEN;
    int32_t out_y2 = ((((area->y2 + 1) * UI_SCALE_NUM) + UI_SCALE_DEN - 1) / UI_SCALE_DEN) - 1;
    int32_t out_w = out_x2 - out_x1 + 1;
    bool strip_area = (area->x1 <= UI_STRIP_WINDOW_X) && (area->x2 >= (UI_STRIP_RIGHT_WINDOW_X + UI_STRIP_WINDOW_W - 1));

    if (strip_area)
    {
        // A remembered fill is only reused by the area directly above or below it (the next chunk of the same redraw)
        if ((area->y1 != (strip_last_y2 + 1)) && ((area->y2 + 1) != strip_last_y1))
        {
            strip_fill_valid = false;
        }

        strip_last_y1 = area->y1;
        strip_last_y2 = area->y2;
    }

    for (int32_t out_y = out_y1; out_y <= out_y2; out_y++)
    {
        const lv_color_t* src_row = color_map + ((((out_y * UI_SCALE_DEN) / UI_SCALE_NUM) - area->y1) * src_w);

        for (int32_t i = 0; i < out_w; i++)
        {
            scale_line_buf[i] = src_row[scale_x_map[out_x1 + i] - area->x1];
        }

        // RGB panel draw_bitmap copies into the frame buffer before returning, so the buffers can be reused
        esp_lcd_panel_draw_bitmap(panel_handle, out_x1 + UI_OFFSET_X, out_y + UI_OFFSET_Y, out_x2 + 1 + UI_OFFSET_X, out_y + 1 + UI_OFFSET_Y, scale_line_buf);

        if (strip_area)
        {
            platform_update_side_strips(panel_handle, src_row, area->x1, out_y + UI_OFFSET_Y);
        }
    }

    lv_disp_flush_ready(drv);
}

void platform_init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex, lv_disp_drv_t* pdisp_drv)
{
    __attribute__((unused)) esp_err_t ret = ESP_OK;
    uint8_t touch_ok = 0;
    gpio_config_t gpio_config_struct;
    I2CMutexHandle = I2CMutex;
    disp_drv = pdisp_drv;

    ESP_LOGI(TAG, "Platform Init");

    // USB_SEL low connects GPIO19/20 to the USB-C port, high routes them to the CAN transceiver
    if (CH32V003_init(bus_handle, I2CMutex, (uint8_t)~(1 << USB_SEL)) == ESP_OK)
    {
        ESP_LOGI(TAG, "Onboard IO Expander init OK");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to init Onboard IO expander!");
    }

    ESP_LOGI(TAG, "Install RGB LCD panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(6, 0, 0)
        .in_color_format = LCD_COLOR_FMT_RGB565,
        .dma_burst_size = 64,
#else
        .bits_per_pixel = 16,
        .psram_trans_align = 64,
#endif
        .num_fbs = DISPLAY_LCD_NUM_FB,
        .bounce_buffer_size_px = 10 * DISPLAY_LCD_H_RES,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .disp_gpio_num = DISPLAY_PIN_NUM_DISP_EN,
        .pclk_gpio_num = DISPLAY_PIN_NUM_PCLK,
        .vsync_gpio_num = DISPLAY_PIN_NUM_VSYNC,
        .hsync_gpio_num = DISPLAY_PIN_NUM_HSYNC,
        .de_gpio_num = DISPLAY_PIN_NUM_DE,
        .data_gpio_nums = {
            DISPLAY_PIN_NUM_DATA0,
            DISPLAY_PIN_NUM_DATA1,
            DISPLAY_PIN_NUM_DATA2,
            DISPLAY_PIN_NUM_DATA3,
            DISPLAY_PIN_NUM_DATA4,
            DISPLAY_PIN_NUM_DATA5,
            DISPLAY_PIN_NUM_DATA6,
            DISPLAY_PIN_NUM_DATA7,
            DISPLAY_PIN_NUM_DATA8,
            DISPLAY_PIN_NUM_DATA9,
            DISPLAY_PIN_NUM_DATA10,
            DISPLAY_PIN_NUM_DATA11,
            DISPLAY_PIN_NUM_DATA12,
            DISPLAY_PIN_NUM_DATA13,
            DISPLAY_PIN_NUM_DATA14,
            DISPLAY_PIN_NUM_DATA15,
        },

        // Waveshare's reference porches. Shorter ones (20/70/70, 3/10/10) raise the refresh rate at the same
        // pixel clock but made this panel flicker more on hardware
        .timings = {
            .pclk_hz           = DISPLAY_LCD_PIXEL_CLOCK_HZ,
            .h_res             = DISPLAY_LCD_H_RES,
            .v_res             = DISPLAY_LCD_V_RES,
            .hsync_pulse_width = 162,
            .hsync_back_porch  = 152,
            .hsync_front_porch = 48,
            .vsync_pulse_width = 45,
            .vsync_back_porch  = 13,
            .vsync_front_porch = 3,
            .flags = {
                .hsync_idle_low   = 0,
                .vsync_idle_low   = 0,
                .de_idle_high     = 0,
                .pclk_active_neg  = 1,
            }
        },

        .flags.fb_in_psram = true,
    };

    ESP_LOGI(TAG, "LCD config: Pixel clock: %d", (int)panel_config.timings.pclk_hz);
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

    ESP_LOGI(TAG, "Register event callbacks");
    esp_lcd_rgb_panel_event_callbacks_t cbs = {
        .on_vsync = display_on_vsync_event,
    };
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(panel_handle, &cbs, disp_drv));

    ESP_LOGI(TAG, "Initialize RGB LCD panel");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();
    tp_io_config.scl_speed_hz = 400000;

    gpio_config_struct.pin_bit_mask = (uint64_t)1 << TOUCH_INT;
    gpio_config_struct.mode = GPIO_MODE_OUTPUT;
    gpio_config_struct.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config_struct.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&gpio_config_struct);

    // Holding INT low while reset is released selects GT911 I2C address 0x5D
    CH32V003_write_output(TOUCH_RESET, 0);
    esp_rom_delay_us(100 * 1000);

    gpio_set_level(TOUCH_INT, 0);
    esp_rom_delay_us(100 * 1000);

    CH32V003_write_output(TOUCH_RESET, 1);
    esp_rom_delay_us(200 * 1000);

    gpio_config_struct.mode = GPIO_MODE_INPUT;
    gpio_config_struct.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&gpio_config_struct);

    if (esp_lcd_new_panel_io_i2c(bus_handle, &tp_io_config, &tp_io_handle) != ESP_OK)
    {
        ESP_LOGE(TAG, "Touch reset 3 failed!");
    }

    esp_lcd_touch_config_t tp_cfg = {
        .x_max = DISPLAY_LCD_H_RES,
        .y_max = DISPLAY_LCD_V_RES,
        .rst_gpio_num = -1,
        .int_gpio_num = -1,
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };

    ESP_LOGI(TAG, "Initialize touch controller GT911");

    for (int loop = 0; loop < 5; loop++)
    {
        ret = ESP_FAIL;

        if (xSemaphoreTake(I2CMutexHandle, (TickType_t)10000) == pdTRUE)
        {
            ret = (esp_lcd_touch_new_i2c_gt911(tp_io_handle, &tp_cfg, &tp));

            xSemaphoreGive(I2CMutexHandle);
        }
        else
        {
            ESP_LOGE(TAG, "Initialize touch loop mutex timeout");
        }

        if (ret == ESP_OK)
        {
            if (xSemaphoreTake(I2CMutexHandle, (TickType_t)10000) == pdTRUE)
            {
                uint8_t config[GT911_CONFIG_SIZE];
                ret = esp_lcd_panel_io_rx_param(tp_io_handle, ESP_LCD_TOUCH_GT911_CONFIG_REG, config, sizeof(config));

                if (ret != ESP_OK)
                {
                    ESP_LOGE(TAG, "GT911 config read failed!");
                }
                else
                {
                    ESP_LOGI(TAG, "GT911 config: X max %d, Y max %d, touch level %d, filter %d", (int)(config[1] | (config[2] << 8)), (int)(config[3] | (config[4] << 8)), (int)config[ESP_LCD_TOUCH_GT911_TOUCH_LEVEL_OFFSET], (int)config[ESP_LCD_TOUCH_GT911_FILTER_OFFSET]);

                    if (control_get_config_item_int(CONFIG_ITEM_ENABLE_HIGHER_TOUCH_SENS) != 0)
                    {
                        ESP_LOGI(TAG, "GT911 setting higher touch sensitivity");

                        config[ESP_LCD_TOUCH_GT911_TOUCH_LEVEL_OFFSET] = 10;        // default is 100
                        config[ESP_LCD_TOUCH_GT911_FILTER_OFFSET] = 15;             // default is 31

                        // Set checksum at 0x80FD
                        config[182] = GT911CalcChecksum(config, 182);

                        // Set refresh flag at 0x80FE to apply changes
                        config[183] = 0x01;

                        ret = esp_lcd_panel_io_tx_param(tp_io_handle, ESP_LCD_TOUCH_GT911_CONFIG_REG, config, 184);
                        if (ret != ESP_OK)
                        {
                            ESP_LOGE(TAG, "GT911 Failed to write config: %s", esp_err_to_name(ret));
                        }
                    }
                }

                xSemaphoreGive(I2CMutexHandle);
            }

            ESP_LOGI(TAG, "Touch controller init OK");
            touch_ok = 1;
            break;
        }
        else
        {
            ESP_LOGI(TAG, "Touch controller init retry %s", esp_err_to_name(ret));

            i2c_master_reset();
        }

        vTaskDelay(pdMS_TO_TICKS(25));
    }

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to init touch screen");
    }

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();

    void *buf1 = NULL;
    void *buf2 = NULL;
    ESP_LOGI(TAG, "Allocate separate LVGL draw buffers from DMA capable ram");
    buf1 = heap_caps_malloc(UI_H_RES * DRAW_BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_32BIT);
    assert(buf1);

    buf2 = heap_caps_malloc(UI_H_RES * DRAW_BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_32BIT);
    assert(buf2);

    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, UI_H_RES * DRAW_BUFFER_SIZE);

    for (int i = 0; i < UI_SCALED_H_RES; i++)
    {
        scale_x_map[i] = (uint16_t)((i * UI_SCALE_DEN) / UI_SCALE_NUM);
    }

    scale_line_buf = heap_caps_malloc(UI_SCALED_H_RES * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(scale_line_buf);

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(disp_drv);
    disp_drv->hor_res = UI_H_RES;
    disp_drv->ver_res = UI_V_RES;
    disp_drv->flush_cb = platform_display_lvgl_flush_cb;
    disp_drv->draw_buf = &disp_buf;
    disp_drv->user_data = panel_handle;

    lv_disp_t* __attribute__((unused)) disp = lv_disp_drv_register(disp_drv);

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

#endif // CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_7B
