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
#include "driver/i2c_master.h"
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
#include "platform_common.h"

#define I2C_MASTER_FREQ_HZ              400000      /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE       0           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE       0           /*!< I2C master doesn't need buffer */

#define I2C_CLR_BUS_SCL_NUM            (9)
#define I2C_CLR_BUS_HALF_PERIOD_US     (5)

static const char *TAG = "app_main";

__attribute__((unused)) SemaphoreHandle_t I2CMutex_1;
__attribute__((unused)) SemaphoreHandle_t I2CMutex_2;
static __attribute__((unused)) lv_disp_drv_t disp_drv;  
static __attribute__((unused)) i2c_master_bus_handle_t ic2_bus_handle_1;
static __attribute__((unused)) i2c_master_bus_handle_t ic2_bus_handle_2;
const char *spi_flash_fatfs_base_path = "/spiflash";
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

static esp_err_t i2c_master_init(i2c_master_bus_handle_t *bus_handle, uint32_t port, uint32_t scl_pin, uint32_t sda_pin);

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
    //i2c_reset_tx_fifo(I2C_MASTER_NUM_1);
    //i2c_reset_rx_fifo(I2C_MASTER_NUM_1);
    periph_module_disable(PERIPH_I2C0_MODULE);
    periph_module_enable(PERIPH_I2C0_MODULE);
    i2c_del_master_bus(ic2_bus_handle_1);

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
    return i2c_master_init(&ic2_bus_handle_1, I2C_MASTER_NUM_1, I2C_MASTER_1_SCL_IO, I2C_MASTER_1_SDA_IO);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t i2c_master_init(i2c_master_bus_handle_t *bus_handle, uint32_t port, uint32_t scl_pin, uint32_t sda_pin)
{
    esp_err_t res;

    // init master bus
    i2c_master_bus_config_t bus_config = {
        .i2c_port = port,
        .sda_io_num = sda_pin,
        .scl_io_num = scl_pin,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    res = i2c_new_master_bus(&bus_config, bus_handle);
    return res;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static __attribute__((unused)) void list_files(const char *path) 
{
    DIR *dir = opendir(path);
    if (dir == NULL) 
    {
        ESP_LOGE(TAG, "Failed to open directory: %s", path);
        return;
    }

    struct dirent *entry;
    ESP_LOGI(TAG, "Listing files in directory: %s", path);
    
    while ((entry = readdir(dir)) != NULL) 
    {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) 
        {
            continue;
        }
        
        char full_path[300];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        
        // Get file information
        struct stat stat_buf;
        if (stat(full_path, &stat_buf) == 0) 
        {
            if (S_ISDIR(stat_buf.st_mode)) 
            {
                ESP_LOGI(TAG, "[DIR]  %s", entry->d_name);
            } 
            else 
            {
                ESP_LOGI(TAG, "[FILE] %s (%ld bytes)", entry->d_name, stat_buf.st_size);
            }
        } 
        else 
        {
            ESP_LOGE(TAG, "Failed to stat %s", entry->d_name);
        }
    }

    closedir(dir);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      none
* NOTES:       none
****************************************************************************/
static void mount_file_system(void)
{    
    esp_err_t err;

    ESP_LOGI(TAG, "Mounting Fat file system");

    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 1,     // this is max open files, not max total files
            .format_if_mount_failed = true,
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };
    
    err = esp_vfs_fat_spiflash_mount_rw_wl(spi_flash_fatfs_base_path, "storage", &mount_config, &s_wl_handle);

    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(TAG, "Mounted file system OK");

        // debug
        //list_files("/spiflash/images");
    }
}

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

    // mount the virtual file system in flash
    mount_file_system();

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
    ESP_ERROR_CHECK(i2c_master_init(&ic2_bus_handle_1, I2C_MASTER_NUM_1, I2C_MASTER_1_SCL_IO, I2C_MASTER_1_SDA_IO));
    ESP_LOGI(TAG, "I2C 1 initialized successfully");

    if (I2C_MASTER_2_SCL_IO != -1)
    {
        ESP_ERROR_CHECK(i2c_master_init(&ic2_bus_handle_2, I2C_MASTER_NUM_2, I2C_MASTER_2_SCL_IO, I2C_MASTER_2_SDA_IO));
        ESP_LOGI(TAG, "I2C 2 initialized successfully");    
    }

    // init parameters
    ESP_LOGI(TAG, "Init Params");
    tonex_params_init();

    // init control task
    ESP_LOGI(TAG, "Init Control");
    control_init();


    

    // test**********************
    void* check_hack = heap_caps_malloc(4160000, MALLOC_CAP_SPIRAM);


    // init platform
    ESP_LOGI(TAG, "Init Platform");
    platform_init(ic2_bus_handle_1, I2CMutex_1, &disp_drv);



    // test**********************
    free(check_hack);




    // init display
    ESP_LOGI(TAG, "Init Display");
    display_init(ic2_bus_handle_1, I2CMutex_1, &disp_drv);

    // init Footswitches
    ESP_LOGI(TAG, "Init footswitches");
    if (EXTERNAL_IO_EXPANDER_BUS == I2C_MASTER_NUM_1)
    {
        footswitches_init(ic2_bus_handle_1, EXTERNAL_IO_EXPANDER_MUTEX);
    }
    else
    {
        footswitches_init(ic2_bus_handle_2, EXTERNAL_IO_EXPANDER_MUTEX);
    }

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
