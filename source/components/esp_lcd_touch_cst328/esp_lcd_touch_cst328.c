/*
 * SPDX-FileCopyrightText: 2025 Greg Smith
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "esp_lcd_touch_cst328.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_touch.h"

#define POINT_NUM_MAX                           1       // desired max
#define CST328_LCD_TOUCH_MAX_POINTS             5       // chip max

/* registers */
#define ESP_LCD_TOUCH_CST328_READ_NUMBER_REG    0xD005
#define ESP_LCD_TOUCH_CST328_READ_XY_REG        0xD000
#define ESP_LCD_TOUCH_CST328_READ_CHECKSUM_REG  0x80FF
#define ESP_LCD_TOUCH_CST328_CONFIG_REG         0x8047

#define HYN_REG_MUT_DEBUG_INFO_IC_CHECKSUM      0xD208
#define HYN_REG_MUT_DEBUG_INFO_FW_VERSION       0xD204
#define HYN_REG_MUT_DEBUG_INFO_IC_TYPE			0xD202
#define HYN_REG_MUT_DEBUG_INFO_PROJECT_ID		0xD200 
#define HYN_REG_MUT_DEBUG_INFO_BOOT_TIME        0xD1FC
#define HYN_REG_MUT_DEBUG_INFO_RES_Y            0xD1FA
#define HYN_REG_MUT_DEBUG_INFO_RES_X            0xD1F8
#define HYN_REG_MUT_DEBUG_INFO_KEY_NUM          0xD1F7
#define HYN_REG_MUT_DEBUG_INFO_TP_NRX           0xD1F6
#define HYN_REG_MUT_DEBUG_INFO_TP_NTX           0xD1F4

#define HYN_REG_MUT_DEBUG_INFO_MODE             0xD101
#define HYN_REG_MUT_RESET_MODE            		0xD102
#define HYN_REG_MUT_DEBUG_RECALIBRATION_MODE    0xD104
#define HYN_REG_MUT_DEEP_SLEEP_MODE    			0xD105
#define HYN_REG_MUT_DEBUG_POINT_MODE	    	0xD108
#define HYN_REG_MUT_NORMAL_MODE                 0xD109

#define HYN_REG_MUT_DEBUG_RAWDATA_MODE          0xD10A
#define HYN_REG_MUT_DEBUG_DIFF_MODE             0xD10D
#define HYN_REG_MUT_DEBUG_FACTORY_MODE          0xD119
#define HYN_REG_MUT_DEBUG_FACTORY_MODE_2        0xD120

    
static const char *TAG = "CST328";

static esp_err_t read_data(esp_lcd_touch_handle_t tp);
static bool get_xy(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num);
static esp_err_t del(esp_lcd_touch_handle_t tp);

static esp_err_t i2c_read_bytes(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t *data, uint8_t len);
static esp_err_t i2c_write_bytes(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t *data, uint8_t len);

static esp_err_t reset(esp_lcd_touch_handle_t tp);
static esp_err_t read_id(esp_lcd_touch_handle_t tp);

esp_err_t esp_lcd_touch_new_i2c_CST328(const esp_lcd_panel_io_handle_t io, const esp_lcd_touch_config_t *config, esp_lcd_touch_handle_t *tp)
{
    ESP_RETURN_ON_FALSE(io, ESP_ERR_INVALID_ARG, TAG, "Invalid io");
    ESP_RETURN_ON_FALSE(config, ESP_ERR_INVALID_ARG, TAG, "Invalid config");
    ESP_RETURN_ON_FALSE(tp, ESP_ERR_INVALID_ARG, TAG, "Invalid touch handle");

    /* Prepare main structure */
    esp_err_t ret = ESP_OK;
    esp_lcd_touch_handle_t CST328 = calloc(1, sizeof(esp_lcd_touch_t));
    ESP_GOTO_ON_FALSE(CST328, ESP_ERR_NO_MEM, err, TAG, "Touch handle malloc failed");

    /* Communication interface */
    CST328->io = io;
    
    /* Only supported callbacks are set */
    CST328->read_data = read_data;
    CST328->get_xy = get_xy;
    CST328->del = del;
    
    /* Mutex */
    CST328->data.lock.owner = portMUX_FREE_VAL;
    
    /* Save config */
    memcpy(&CST328->config, config, sizeof(esp_lcd_touch_config_t));

    /* Prepare pin for touch interrupt */
    if (CST328->config.int_gpio_num != GPIO_NUM_NC) {
        const gpio_config_t int_gpio_config = {
            .mode = GPIO_MODE_INPUT,
            .intr_type = (CST328->config.levels.interrupt ? GPIO_INTR_POSEDGE : GPIO_INTR_NEGEDGE),
            .pin_bit_mask = BIT64(CST328->config.int_gpio_num)
        };
        ESP_GOTO_ON_ERROR(gpio_config(&int_gpio_config), err, TAG, "GPIO intr config failed");

        /* Register interrupt callback */
        if (CST328->config.interrupt_callback) {
            esp_lcd_touch_register_interrupt_callback(CST328, CST328->config.interrupt_callback);
        }
    }
    
    /* Prepare pin for touch controller reset */
    if (CST328->config.rst_gpio_num != GPIO_NUM_NC) {
        const gpio_config_t rst_gpio_config = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = BIT64(CST328->config.rst_gpio_num)
        };
        ESP_GOTO_ON_ERROR(gpio_config(&rst_gpio_config), err, TAG, "GPIO reset config failed");
    }
    
    /* Reset controller */
    ESP_GOTO_ON_ERROR(reset(CST328), err, TAG, "Reset failed");
    
    /* Read product id */
    ESP_GOTO_ON_ERROR(read_id(CST328), err, TAG, "Read ID failed");

    *tp = CST328;

    return ESP_OK;
    
err:
    if (CST328) {
        del(CST328);
    }
    ESP_LOGE(TAG, "Initialization failed!");
    return ret;
}

static esp_err_t read_data(esp_lcd_touch_handle_t tp)
{   
    uint8_t buf[41];
    uint8_t touch_cnt = 0;
    uint8_t clear = 0;
    uint8_t i;
    uint8_t num = 0;
    
    /* get number of points */
    ESP_RETURN_ON_ERROR(i2c_read_bytes(tp, ESP_LCD_TOUCH_CST328_READ_NUMBER_REG, buf, 1), TAG, "I2C read failed");
    
    if ((buf[0] & 0x0F) == 0x00) {   
        /* no touch data */
        ESP_RETURN_ON_ERROR(i2c_write_bytes(tp, ESP_LCD_TOUCH_CST328_READ_NUMBER_REG, &clear, 1), TAG, "I2C read failed");
    } 
    else {
        /* Count of touched points */
        touch_cnt = buf[0] & 0x0F;
        if ((touch_cnt > CST328_LCD_TOUCH_MAX_POINTS) || (touch_cnt == 0)) {
            // invalid, clear
            ESP_RETURN_ON_ERROR(i2c_write_bytes(tp, ESP_LCD_TOUCH_CST328_READ_NUMBER_REG, &clear, 1), TAG, "I2C read failed");
            return ESP_OK;
        }
        
        /* Read all points */
        ESP_RETURN_ON_ERROR(i2c_read_bytes(tp, ESP_LCD_TOUCH_CST328_READ_XY_REG, &buf[1], 27), TAG, "I2C read failed");
        
        /* Clear all */
        ESP_RETURN_ON_ERROR(i2c_write_bytes(tp, ESP_LCD_TOUCH_CST328_READ_NUMBER_REG, &clear, 1), TAG, "I2C read failed");
        
        /* Number of touched points */
        if (touch_cnt > POINT_NUM_MAX) {
            /* clamp at max */
            touch_cnt = POINT_NUM_MAX;
        }
        
        portENTER_CRITICAL(&tp->data.lock);
        tp->data.points = (uint8_t)touch_cnt;
        
        /* Fill all coordinates */
        for (i = 0; i < touch_cnt; i++) {
            if (i > 0) {
                num = 2;
            }
                      
            tp->data.coords[i].x = (uint16_t)(((uint16_t)buf[(i * 5) + 2 + num] << 4) + ((buf[(i * 5) + 4 + num] & 0xF0) >> 4));               
            tp->data.coords[i].y = (uint16_t)(((uint16_t)buf[(i * 5) + 3 + num] << 4) + ( buf[(i * 5) + 4 + num] & 0x0F));
        }  
        
        portEXIT_CRITICAL(&tp->data.lock);        
    }
    
    return ESP_OK;
}

static bool get_xy(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num)
{
    portENTER_CRITICAL(&tp->data.lock);
    
    /* Count of points */
    *point_num = (tp->data.points > max_point_num ? max_point_num : tp->data.points);
    
    for (size_t i = 0; i < *point_num; i++) {
        x[i] = tp->data.coords[i].x;
        y[i] = tp->data.coords[i].y;

        if (strength) {
            strength[i] = tp->data.coords[i].strength;
        }
    }
    
    /* Invalidate */
    tp->data.points = 0;
    portEXIT_CRITICAL(&tp->data.lock);

    return (*point_num > 0);
}

static esp_err_t del(esp_lcd_touch_handle_t tp)
{
    /* Reset GPIO pin settings */
    if (tp->config.int_gpio_num != GPIO_NUM_NC) {
        gpio_reset_pin(tp->config.int_gpio_num);
        if (tp->config.interrupt_callback) {
            gpio_isr_handler_remove(tp->config.int_gpio_num);
        }
    }
    if (tp->config.rst_gpio_num != GPIO_NUM_NC) {
        gpio_reset_pin(tp->config.rst_gpio_num);
    }
    /* Release memory */
    free(tp);

    return ESP_OK;
}

static esp_err_t reset(esp_lcd_touch_handle_t tp)
{
    if (tp->config.rst_gpio_num != GPIO_NUM_NC) {
        ESP_RETURN_ON_ERROR(gpio_set_level(tp->config.rst_gpio_num, tp->config.levels.reset), TAG, "GPIO set level failed");
        vTaskDelay(pdMS_TO_TICKS(200));
        ESP_RETURN_ON_ERROR(gpio_set_level(tp->config.rst_gpio_num, !tp->config.levels.reset), TAG, "GPIO set level failed");
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    return ESP_OK;
}

static esp_err_t read_id(esp_lcd_touch_handle_t tp)
{
    uint8_t id;
    ESP_RETURN_ON_ERROR(i2c_read_bytes(tp, HYN_REG_MUT_DEBUG_INFO_IC_TYPE, &id, 1), TAG, "I2C read failed");
    ESP_LOGI(TAG, "IC id: %d", id);
    return ESP_OK;
}

static esp_err_t i2c_read_bytes(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t *data, uint8_t len)
{
    ESP_RETURN_ON_FALSE(data, ESP_ERR_INVALID_ARG, TAG, "Invalid data");

    return esp_lcd_panel_io_rx_param(tp->io, reg, data, len);
}

static esp_err_t i2c_write_bytes(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t *data, uint8_t len)
{
    ESP_RETURN_ON_FALSE(data, ESP_ERR_INVALID_ARG, TAG, "Invalid data");

    return esp_lcd_panel_io_tx_param(tp->io, reg, data, len);
}
