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
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sys/param.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "driver/rmt_tx.h"
#include "main.h"
#include "CH422G.h"
#include "control.h"
#include "task_priorities.h"
#include "usb/usb_host.h"
#include "usb_comms.h"
#include "usb_tonex_one.h"
#include "leds.h"

#if !CONFIG_TONEX_CONTROLLER_LED_CONTROL_DISABLED

#define RMT_LED_STRIP_RESOLUTION_HZ     10000000 // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)

enum LedStates
{
    LED_STATE_BOOT_FLASH_ON,
    LED_STATE_BOOT_FLASH_OFF,
    LED_STATE_BOOT_WAIT,
    LED_STATE_IDLE
};

typedef struct 
{
    uint32_t resolution; /*!< Encoder resolution, in Hz */
} led_strip_encoder_config_t;

typedef struct 
{
    rmt_encoder_t base;
    rmt_encoder_t *bytes_encoder;
    rmt_encoder_t *copy_encoder;
    int state;
    rmt_symbol_word_t reset_code;
} rmt_led_strip_encoder_t;

typedef struct 
{
    uint8_t state; 
    uint8_t queued_state; 
    uint32_t timer;
    uint32_t counter;

    uint8_t led_strip_pixels[CONFIG_TONEX_CONTROLLER_LED_NUMBER * 3];
    rmt_channel_handle_t led_chan;
    rmt_encoder_handle_t led_encoder;
    rmt_transmit_config_t tx_config;
} tLedControl;

static const char *TAG = "app_leds";

static tLedControl LedControl;

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       Wiki: https://en.wikipedia.org/wiki/HSL_and_HSV
*****************************************************************************/
static void __attribute__((unused)) led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b)
{
    h %= 360; // h -> [0,360]
    uint32_t rgb_max = v * 2.55f;
    uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

    uint32_t i = h / 60;
    uint32_t diff = h % 60;

    // RGB adjustment amount by hue
    uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i) 
    {
        case 0:
            *r = rgb_max;
            *g = rgb_min + rgb_adj;
            *b = rgb_min;
            break;

        case 1:
            *r = rgb_max - rgb_adj;
            *g = rgb_max;
            *b = rgb_min;
            break;

        case 2:
            *r = rgb_min;
            *g = rgb_max;
            *b = rgb_min + rgb_adj;
            break;

        case 3:
            *r = rgb_min;
            *g = rgb_max - rgb_adj;
            *b = rgb_max;
            break;

        case 4:
            *r = rgb_min + rgb_adj;
            *g = rgb_min;
            *b = rgb_max;
            break;

        default:
            *r = rgb_max;
            *g = rgb_min;
            *b = rgb_max - rgb_adj;
            break;
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static size_t __attribute__((unused)) rmt_encode_led_strip(rmt_encoder_t *encoder, rmt_channel_handle_t channel, const void *primary_data, size_t data_size, rmt_encode_state_t *ret_state)
{
    rmt_led_strip_encoder_t *led_encoder = __containerof(encoder, rmt_led_strip_encoder_t, base);
    rmt_encoder_handle_t bytes_encoder = led_encoder->bytes_encoder;
    rmt_encoder_handle_t copy_encoder = led_encoder->copy_encoder;
    rmt_encode_state_t session_state = 0;
    rmt_encode_state_t state = 0;
    size_t encoded_symbols = 0;
    
    switch (led_encoder->state) 
    {
        case 0: // send RGB data
            encoded_symbols += bytes_encoder->encode(bytes_encoder, channel, primary_data, data_size, &session_state);
            if (session_state & RMT_ENCODING_COMPLETE) 
            {
                led_encoder->state = 1; // switch to next state when current encoding session finished
            }
            
            if (session_state & RMT_ENCODING_MEM_FULL) 
            {
                state |= RMT_ENCODING_MEM_FULL;
                goto out; // yield if there's no free space for encoding artifacts
            }

        // fall-through
        case 1: // send reset code
            encoded_symbols += copy_encoder->encode(copy_encoder, channel, &led_encoder->reset_code, sizeof(led_encoder->reset_code), &session_state);
            if (session_state & RMT_ENCODING_COMPLETE) 
            {
                led_encoder->state = 0; // back to the initial encoding session
                state |= RMT_ENCODING_COMPLETE;
            }
            
            if (session_state & RMT_ENCODING_MEM_FULL) 
            {
                state |= RMT_ENCODING_MEM_FULL;
                goto out; // yield if there's no free space for encoding artifacts
            }
    }

out:
    *ret_state = state;
    return encoded_symbols;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t __attribute__((unused)) rmt_del_led_strip_encoder(rmt_encoder_t *encoder)
{
    rmt_led_strip_encoder_t *led_encoder = __containerof(encoder, rmt_led_strip_encoder_t, base);
    rmt_del_encoder(led_encoder->bytes_encoder);
    rmt_del_encoder(led_encoder->copy_encoder);
    free(led_encoder);

    return ESP_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t __attribute__((unused)) rmt_led_strip_encoder_reset(rmt_encoder_t *encoder)
{
    rmt_led_strip_encoder_t *led_encoder = __containerof(encoder, rmt_led_strip_encoder_t, base);
    rmt_encoder_reset(led_encoder->bytes_encoder);
    rmt_encoder_reset(led_encoder->copy_encoder);
    led_encoder->state = 0;

    return ESP_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t __attribute__((unused)) rmt_new_led_strip_encoder(const led_strip_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder)
{
    esp_err_t ret = ESP_OK;
    rmt_led_strip_encoder_t *led_encoder = NULL;
    
    if ((config == NULL) || (ret_encoder == NULL))
    {
        ESP_LOGE(TAG, "Invalid arg");
        goto err;
    }
    
    led_encoder = calloc(1, sizeof(rmt_led_strip_encoder_t));    
    if (led_encoder == NULL)
    {
        ESP_LOGE(TAG, "no mem for led strip encoder");
        goto err;
    }
    
    led_encoder->base.encode = rmt_encode_led_strip;
    led_encoder->base.del = rmt_del_led_strip_encoder;
    led_encoder->base.reset = rmt_led_strip_encoder_reset;

    // different led strip might have its own timing requirements, following parameter is for WS2812
    rmt_bytes_encoder_config_t bytes_encoder_config = {
        .bit0 = {
            .level0 = 1,
            .duration0 = 0.3 * config->resolution / 1000000, // T0H=0.3us
            .level1 = 0,
            .duration1 = 0.9 * config->resolution / 1000000, // T0L=0.9us
        },
        .bit1 = {
            .level0 = 1,
            .duration0 = 0.9 * config->resolution / 1000000, // T1H=0.9us
            .level1 = 0,
            .duration1 = 0.3 * config->resolution / 1000000, // T1L=0.3us
        },
        .flags.msb_first = 1 // WS2812 transfer bit order: G7...G0R7...R0B7...B0
    };
    
    if (rmt_new_bytes_encoder(&bytes_encoder_config, &led_encoder->bytes_encoder) != ESP_OK)
    {
        ESP_LOGE(TAG, "create bytes encoder failed");
        goto err;
    }

    rmt_copy_encoder_config_t copy_encoder_config = {};
    if (rmt_new_copy_encoder(&copy_encoder_config, &led_encoder->copy_encoder) != ESP_OK)
    {
        ESP_LOGE(TAG, "create copy encoder failed");
        goto err;
    }

    uint32_t reset_ticks = config->resolution / 1000000 * 50 / 2; // reset code duration defaults to 50us
    led_encoder->reset_code = (rmt_symbol_word_t) {
        .level0 = 0,
        .duration0 = reset_ticks,
        .level1 = 0,
        .duration1 = reset_ticks,
    };
    *ret_encoder = &led_encoder->base;
    return ESP_OK;

err:
    if (led_encoder) 
    {
        if (led_encoder->bytes_encoder) 
        {
            rmt_del_encoder(led_encoder->bytes_encoder);
        }
        if (led_encoder->copy_encoder) 
        {
            rmt_del_encoder(led_encoder->copy_encoder);
        }
        free(led_encoder);
    }

    return ret;
}
#endif //CONFIG_TONEX_CONTROLLER_LED_CONTROL_DISABLED

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void leds_handle(void)
{    
#if !CONFIG_TONEX_CONTROLLER_LED_CONTROL_DISABLED    
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 10;
   
    switch (LedControl.state)
    {
        case LED_STATE_BOOT_FLASH_ON:
        {
#if CONFIG_TONEX_CONTROLLER_LED_COLOUR_ORDER_GBR
            // led is GBR colour order
            LedControl.led_strip_pixels[0] = green;
            LedControl.led_strip_pixels[1] = blue;
            LedControl.led_strip_pixels[2] = red;
#endif        
#if CONFIG_TONEX_CONTROLLER_LED_COLOUR_ORDER_RGB
            // led is RGB colour order
            LedControl.led_strip_pixels[0] = red;
            LedControl.led_strip_pixels[1] = green;
            LedControl.led_strip_pixels[2] = blue;
#endif

            ESP_ERROR_CHECK(rmt_transmit(LedControl.led_chan, LedControl.led_encoder, LedControl.led_strip_pixels, sizeof(LedControl.led_strip_pixels), &LedControl.tx_config));

            LedControl.queued_state = LED_STATE_BOOT_FLASH_OFF;
            LedControl.state = LED_STATE_BOOT_WAIT;
            LedControl.timer = xTaskGetTickCount();
        } break;

        case LED_STATE_BOOT_FLASH_OFF:
        {
            memset(LedControl.led_strip_pixels, 0, sizeof(LedControl.led_strip_pixels));
            ESP_ERROR_CHECK(rmt_transmit(LedControl.led_chan, LedControl.led_encoder, LedControl.led_strip_pixels, sizeof(LedControl.led_strip_pixels), &LedControl.tx_config));

            // check hoiw many times we have flashed the led
            if (LedControl.counter > 0)
            {
                LedControl.counter--;
            }

            if (LedControl.counter == 0)
            {
                // all done
                LedControl.queued_state = LED_STATE_IDLE;
            }
            else
            {            
                // flash again
                LedControl.queued_state = LED_STATE_BOOT_FLASH_ON;
            }
            
            LedControl.state = LED_STATE_BOOT_WAIT;
            LedControl.timer = xTaskGetTickCount();
        } break;

        case LED_STATE_BOOT_WAIT:
        {
            // timed expired?
            if ((xTaskGetTickCount() - LedControl.timer) > 150)
            {
                // to next state
                LedControl.state = LedControl.queued_state;
            }
        } break;

        case LED_STATE_IDLE:
        {
            // nothing to do
        } break;
    }
#endif //CONFIG_TONEX_CONTROLLER_LED_CONTROL_DISABLED    
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void leds_init(void)
{
#if !CONFIG_TONEX_CONTROLLER_LED_CONTROL_DISABLED
    ESP_LOGI(TAG, "Leds Init start");

    // init memory
    memset((void*)&LedControl, 0, sizeof(LedControl));
    LedControl.tx_config.loop_count = 0; // no transfer loop

    LedControl.state = LED_STATE_BOOT_FLASH_ON;

    // set for 3 flashes at boot
    LedControl.counter = 3;

    ESP_LOGI(TAG, "Create RMT TX channel");
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
        .gpio_num = LED_OUTPUT_GPIO_NUM,
        .mem_block_symbols = 64, // increase the block size can make the LED less flickering
        .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
        .trans_queue_depth = 4, // set the number of transactions that can be pending in the background
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &LedControl.led_chan));

    ESP_LOGI(TAG, "Install led strip encoder");    

    led_strip_encoder_config_t encoder_config = {
        .resolution = RMT_LED_STRIP_RESOLUTION_HZ,
    };
    ESP_ERROR_CHECK(rmt_new_led_strip_encoder(&encoder_config, &LedControl.led_encoder));

    ESP_LOGI(TAG, "Enable RMT TX channel");
    ESP_ERROR_CHECK(rmt_enable(LedControl.led_chan));    
#endif    //CONFIG_TONEX_CONTROLLER_LED_CONTROL_DISABLED 
}
