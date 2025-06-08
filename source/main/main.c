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
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "sdkconfig.h"
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
#include "lvgl.h"
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
#include "esp_intr_alloc.h"
#include "usb/usb_host.h"
#include "esp_private/periph_ctrl.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

#include "main.h"
#include "task_priorities.h"
#include "usb_comms.h"
#include "usb/usb_host.h"
#include "usb_tonex_one.h"
#include "display.h"
#include "footswitches.h"
#include "control.h"
#include "midi_control.h"
#include "CH422G.h"
#include "LP5562.h"
#include "midi_serial.h"
#include "wifi_config.h"
#include "leds.h"
#include "tonex_params.h"

#define I2C_MASTER_FREQ_HZ              400000      /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE       0           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE       0           /*!< I2C master doesn't need buffer */

#define I2C_CLR_BUS_SCL_NUM            (9)
#define I2C_CLR_BUS_HALF_PERIOD_US     (5)

static const char *TAG = "app_main";

__attribute__((unused)) SemaphoreHandle_t I2CMutex_1;
__attribute__((unused)) SemaphoreHandle_t I2CMutex_2;

static esp_err_t i2c_master_init(uint32_t port, uint32_t scl_pin, uint32_t sda_pin);

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t i2c_master_reset(void)
{
    int sda_io = I2C_MASTER_1_SDA_IO;
    int scl_io = I2C_MASTER_1_SCL_IO;
    const int scl_half_period = I2C_CLR_BUS_HALF_PERIOD_US; // use standard 100kHz data rate
    int i = 0;

    ESP_LOGI(TAG, "I2C bus reset");

    // nuke it
    i2c_reset_tx_fifo(I2C_MASTER_NUM_1);
    i2c_reset_rx_fifo(I2C_MASTER_NUM_1);
    periph_module_disable(PERIPH_I2C0_MODULE);
    periph_module_enable(PERIPH_I2C0_MODULE);
    i2c_driver_delete(I2C_MASTER_NUM_1);

    // manually clock the bus if SDA is stuck
    gpio_set_direction(scl_io, GPIO_MODE_OUTPUT_OD);
    gpio_set_direction(sda_io, GPIO_MODE_INPUT_OUTPUT_OD);

    // If a SLAVE device was in a read operation when the bus was interrupted, the SLAVE device is controlling SDA.
    // The only bit during the 9 clock cycles of a READ byte the MASTER(ESP32) is guaranteed control over is during the ACK bit
    // period. If the slave is sending a stream of ZERO bytes, it will only release SDA during the ACK bit period.
    // So, this reset code needs to synchronize the bit stream with, Either, the ACK bit, Or a 1 bit to correctly generate
    // a STOP condition.
    gpio_set_level(scl_io, 0);
    gpio_set_level(sda_io, 1);
    esp_rom_delay_us(scl_half_period);

    if (!gpio_get_level(sda_io))
    {
        ESP_LOGI(TAG, "I2C bus clearing stuck SDA");
    }

    while (!gpio_get_level(sda_io) && (i++ < I2C_CLR_BUS_SCL_NUM)) 
    {
        gpio_set_level(scl_io, 1);
        esp_rom_delay_us(scl_half_period);
        gpio_set_level(scl_io, 0);
        esp_rom_delay_us(scl_half_period);
    }
    
    gpio_set_level(sda_io, 0); // setup for STOP
    gpio_set_level(scl_io, 1);
    esp_rom_delay_us(scl_half_period);
    gpio_set_level(sda_io, 1); // STOP, SDA low -> high while SCL is HIGH

    // init again
    return i2c_master_init(I2C_MASTER_NUM_1, I2C_MASTER_1_SCL_IO, I2C_MASTER_1_SDA_IO);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t i2c_master_init(uint32_t port, uint32_t scl_pin, uint32_t sda_pin)
{
    esp_err_t res;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .scl_io_num = scl_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(port, &conf);

    res = i2c_driver_install(port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    return res;
}

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43B || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43DEVONLY
/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void InitIOExpander(i2c_port_t I2CNum, SemaphoreHandle_t I2CMutex)
{
    // init IO expander
    if (CH422G_init(I2CNum, I2CMutex) == ESP_OK)
    {
        // set IO expander to output mode. Can't do mixed pins
        // For inputs, we will temporarily flip the mode
        CH422G_set_io_mode(1);

        ESP_LOGI(TAG, "Onboard IO Expander init OK");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to init Onboard IO expander!");
    }
}
#endif  //CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43B || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43DEVONLY

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void app_main(void)
{
    ESP_LOGI(TAG, "ToneX One Controller App start");

    // load the config first
    control_load_config();

    // create mutexes for shared I2C buses
    I2CMutex_1 = xSemaphoreCreateMutex();
    if (I2CMutex_1 == NULL)
    {
        ESP_LOGE(TAG, "I2C Mutex 1 create failed!");
    }
    
    I2CMutex_2 = xSemaphoreCreateMutex();
    if (I2CMutex_2 == NULL)
    {
        ESP_LOGE(TAG, "I2C Mutex 2 create failed!");
    }

    // init I2C master 1
    ESP_ERROR_CHECK(i2c_master_init(I2C_MASTER_NUM_1, I2C_MASTER_1_SCL_IO, I2C_MASTER_1_SDA_IO));
    ESP_LOGI(TAG, "I2C 1 initialized successfully");

    if (I2C_MASTER_2_SCL_IO != -1)
    {
        ESP_ERROR_CHECK(i2c_master_init(I2C_MASTER_NUM_2, I2C_MASTER_2_SCL_IO, I2C_MASTER_2_SDA_IO));
        ESP_LOGI(TAG, "I2C 2 initialized successfully");    
    }

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43B || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43DEVONLY
    // init onboard IO expander
    ESP_LOGI(TAG, "Init Onboard IO Expander");
    InitIOExpander(I2C_MASTER_NUM_1, I2CMutex_1);
#endif

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_M5ATOMS3R
    // init LP5562 led driver
    ESP_LOGI(TAG, "Init LP5562 Led Driver");
    LP5562_init(I2C_MASTER_NUM_1, I2CMutex_1);
#endif

    // init parameters
    ESP_LOGI(TAG, "Init Params");
    tonex_params_init();

    // init control task
    ESP_LOGI(TAG, "Init Control");
    control_init();

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43B || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43DEVONLY
    // init GUI
    ESP_LOGI(TAG, "Init 43.B display");
    display_init(I2C_MASTER_NUM_1, I2CMutex_1);
#endif

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169 || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169TOUCH 
    // init GUI
    ESP_LOGI(TAG, "Init 1.69 display");
    display_init(I2C_MASTER_NUM_1, I2CMutex_1);
#endif

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_M5ATOMS3R
    // init GUI
    ESP_LOGI(TAG, "Init 0.85 display");
    display_init(I2C_MASTER_NUM_1, I2CMutex_1);
#endif

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_LILYGO_TDISPLAY_S3
    // init GUI
    ESP_LOGI(TAG, "Init 1.9 display");
    display_init(I2C_MASTER_NUM_1, I2CMutex_1);
#endif

    // init Footswitches
    ESP_LOGI(TAG, "Init footswitches");
    footswitches_init(EXTERNAL_IO_EXPANDER_BUS, EXTERNAL_IO_EXPANDER_MUTEX);

    if (control_get_config_item_int(CONFIG_ITEM_BT_MODE) != BT_MODE_DISABLED)
    {
        // init Midi Bluetooth
        ESP_LOGI(TAG, "Init MIDI BT");
        midi_init();
    }
    else
    {
        ESP_LOGI(TAG, "MIDI BT disabled");
    }

    if (control_get_config_item_int(CONFIG_ITEM_MIDI_ENABLE))
    {
        // init Midi serial
        ESP_LOGI(TAG, "Init MIDI Serial");
        midi_serial_init();
    }
    else
    {    
        ESP_LOGI(TAG, "Serial MIDI disabled");
    }

    // init USB
    ESP_LOGI(TAG, "Init USB");
    init_usb_comms();

    // init WiFi config
    wifi_config_init();
}
