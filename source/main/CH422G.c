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

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include "driver/i2c_master.h"
#include "esp_bit_defs.h"
#include "esp_check.h"
#include "esp_log.h"
#include "CH422G.h"


#define ESP_IO_EXPANDER_I2C_CH422G_ADDRESS_000    (0x24)
#define DEVICE_I2C_MASTER_FREQUENCY               400000

// Timeout of each I2C communication 
#define I2C_TIMEOUT_MS          (10)

#define IO_COUNT                (8)
#define DIR_OUT_VALUE           (0xFFF)
#define DIR_IN_VALUE            (0xF00)

// Register address 
#define CH422G_REG_WR_SET       (0x48 >> 1)     // 0x24
#define CH422G_REG_WR_OC        (0x46 >> 1)     // 0x23
#define CH422G_REG_WR_IO        (0x70 >> 1)     // 0x38
#define CH422G_REG_RD_IO        (0x4D >> 1)     // 0x26

/* Default register value when reset */
#define REG_WR_SET_DEFAULT_VAL  (0x01UL)    // Bit:        |  7  |  6  |  5  |  4  |    3    |    2    |    1     |    0    |
                                            //             | --- | --- | --- | --- | ------- | ------- | -------- | ------- |
                                            // Value:      |  /  |  /  |  /  |  /  | [SLEEP] | [OD_EN] | [A_SCAN] | [IO_OE] |
                                            //             | --- | --- | --- | --- | ------- | ------- | -------- | ------- |
                                            // Default:    |  0  |  0  |  0  | 0   |    0    |    0    |    0     |    1    |

#define REG_WR_OC_DEFAULT_VAL   (0x0FUL)
#define REG_WR_IO_DEFAULT_VAL   (0xFFUL)
#define REG_OUT_DEFAULT_VAL     ((REG_WR_OC_DEFAULT_VAL << 8) | REG_WR_IO_DEFAULT_VAL)
#define REG_DIR_DEFAULT_VAL     (0xFFFUL)

#define REG_WR_SET_BIT_IO_OE    (1 << 0)
#define REG_WR_SET_BIT_OD_EN    (1 << 2)


typedef struct 
{
    i2c_master_dev_handle_t dev_handle_wr_set;
    i2c_master_dev_handle_t dev_handle_wr_io;
    i2c_master_dev_handle_t dev_handle_rd_io;
    i2c_master_dev_handle_t dev_handle_mode;
    
    struct 
    {
        uint8_t io_count;                       /*!< Count of device's IO, must be less or equal than `IO_COUNT_MAX` */
    } config;

	struct 
	{
        uint8_t wr_set;
        uint8_t wr_oc;
        uint8_t wr_io;
    } regs;
} esp_io_expander_ch422g_t;


/**
 * @brief Device Structure Type
 *
 */
static esp_io_expander_ch422g_t ch422g;
static const char *TAG = "app_ch422g";
static SemaphoreHandle_t I2CMutexHandle;

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t CH422G_enableAllIO_Input(void)
{
    uint8_t data = (uint8_t)(ch422g.regs.wr_set & ~REG_WR_SET_BIT_IO_OE);
    esp_err_t res = ESP_FAIL;

    // WR-SET
    if (xSemaphoreTake(I2CMutexHandle, (TickType_t)100) == pdTRUE)
    {
        res = i2c_master_transmit(ch422g.dev_handle_wr_set, &data, 1, pdMS_TO_TICKS(I2C_TIMEOUT_MS));

        if (res == ESP_OK)
        {
            ch422g.regs.wr_set = data;
        }
        else
        {
            ESP_LOGE(TAG, "CH422G_enableAllIO_Input() failed");
        }

        xSemaphoreGive(I2CMutexHandle);
    }
    
    // Delay 1ms to wait for the IO expander to switch to input mode
    vTaskDelay(pdMS_TO_TICKS(2));

    return res;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t CH422G_read_input(uint8_t pin_bit, uint8_t* value)
{
    esp_err_t res = ESP_FAIL;
    uint16_t values;

    // default to not pressed
    *value = 1;

    res = CH422G_read_all_input(&values);

    if (res == ESP_OK)
    {
        *value = (values >> pin_bit) & 0x01;
    }
    else
    {
        ESP_LOGE(TAG, "CH422G_read_input() failed");
    }

    return res;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t CH422G_read_all_input(uint16_t* values)
{
    uint8_t temp = 0;
    uint8_t write_buf;
    esp_err_t res = ESP_FAIL;
    *values = 0;

    if (xSemaphoreTake(I2CMutexHandle, (TickType_t)100) == pdTRUE)
    {
        // first set the pins to input mode (can't do separate input/output per pin)
        write_buf = 0;
        res = i2c_master_transmit(ch422g.dev_handle_mode, &write_buf, 1, pdMS_TO_TICKS(I2C_TIMEOUT_MS));
        esp_rom_delay_us(1000);

        if (res == ESP_OK)
        {
            res = i2c_master_receive(ch422g.dev_handle_mode, &temp, 1, pdMS_TO_TICKS(I2C_TIMEOUT_MS));

            if (res == ESP_OK)
            {
                // read pin state
                res = i2c_master_receive(ch422g.dev_handle_rd_io, &temp, 1, pdMS_TO_TICKS(I2C_TIMEOUT_MS));

                // return to output mode
                write_buf = CH422G_Mode_IO_OE;
                res = i2c_master_transmit(ch422g.dev_handle_mode, &write_buf, 1, pdMS_TO_TICKS(I2C_TIMEOUT_MS));
            }
        }

        xSemaphoreGive(I2CMutexHandle);
    }
    
    if (res == ESP_OK)
    {
        *values = (uint16_t)temp;
    }
    else
    {
        ESP_LOGE(TAG, "CH422G_read_all_input() failed");
    }

    return res;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t CH422G_write_output(uint8_t pin_bit, uint8_t value)
{
	esp_err_t res = ESP_FAIL;

    if (value)
    {
        ch422g.regs.wr_io |= (1 << pin_bit);
    }
    else
    {
        ch422g.regs.wr_io &= ~(1 << pin_bit);
    }
    
    // WR-IO
    if (xSemaphoreTake(I2CMutexHandle, (TickType_t)100) == pdTRUE)
    {
        res = i2c_master_transmit(ch422g.dev_handle_wr_io, &ch422g.regs.wr_io, sizeof(ch422g.regs.wr_io), pdMS_TO_TICKS(I2C_TIMEOUT_MS));
        
        if (res != ESP_OK)
        {
            ESP_LOGE(TAG, "CH422G_write_output_reg() failed 1");
        }

        xSemaphoreGive(I2CMutexHandle);
    }
	
    return res;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t CH422G_read_output_reg(uint32_t *value)
{
    *value = ch422g.regs.wr_io | (((uint32_t)ch422g.regs.wr_oc) << 8);
    return ESP_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t CH422G_write_direction(uint8_t pin_bit, uint8_t value)
{
    esp_err_t res = ESP_FAIL;
    uint8_t data = ch422g.regs.wr_set;

    // REG_WR_SET_BIT_IO_OE??
    if (value)
    {
        data |= 1 << pin_bit;
    }
    else
    {
        data &= ~(1 << pin_bit);
    }

    // WR-SET
    if (xSemaphoreTake(I2CMutexHandle, (TickType_t)100) == pdTRUE)
    {
        res = i2c_master_transmit(ch422g.dev_handle_wr_set, &data, 1, pdMS_TO_TICKS(I2C_TIMEOUT_MS));

        if (res == ESP_OK)
        {
            ch422g.regs.wr_set = data;
        }
        else
        {
            ESP_LOGE(TAG, "CH422G_write_direction_reg() failed 1");
        }

        xSemaphoreGive(I2CMutexHandle);
    }

    return res;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t CH422G_read_direction_reg(uint32_t *value)
{
    *value = (ch422g.regs.wr_set & REG_WR_SET_BIT_IO_OE) ? DIR_OUT_VALUE : DIR_IN_VALUE;

    return ESP_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t CH422G_set_io_mode(uint8_t output_mode)
{
    esp_err_t res = ESP_FAIL;
    uint8_t write_buf; 

    if (output_mode)
    {   
        write_buf = CH422G_Mode_IO_OE;
    }
    else
    {
        write_buf = 0;
    }

    if (xSemaphoreTake(I2CMutexHandle, (TickType_t)100) == pdTRUE)
    {
        res = i2c_master_transmit(ch422g.dev_handle_mode, &write_buf, 1, pdMS_TO_TICKS(I2C_TIMEOUT_MS));

        xSemaphoreGive(I2CMutexHandle);
    }

    return res;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t CH422G_reset(void)
{    
    //todo ESP_RETURN_ON_ERROR(CH422G_write_direction_reg(REG_DIR_DEFAULT_VAL), TAG, "Write direction reg (WR_SET) failed");
    //ESP_RETURN_ON_ERROR(CH422G_write_output_reg(REG_OUT_DEFAULT_VAL), TAG, "Write output reg (WR_OC & WR_IO) failed");

    return ESP_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t CH422G_init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex)
{
    I2CMutexHandle = I2CMutex;

    ch422g.config.io_count = IO_COUNT;
	ch422g.regs.wr_set = REG_WR_SET_DEFAULT_VAL;
    ch422g.regs.wr_oc = REG_WR_OC_DEFAULT_VAL;
    ch422g.regs.wr_io = REG_WR_IO_DEFAULT_VAL;

    // CH422G doesn't use single device address with regs. Uses different device
    // addresses for each reg, so need to create multiple
    i2c_device_config_t dev_config_1 = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = CH422G_REG_WR_SET,
        .scl_speed_hz = DEVICE_I2C_MASTER_FREQUENCY,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config_1, &ch422g.dev_handle_wr_set));

    i2c_device_config_t dev_config_2 = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = CH422G_REG_WR_IO,
        .scl_speed_hz = DEVICE_I2C_MASTER_FREQUENCY,
    };
    
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config_2, &ch422g.dev_handle_wr_io));

    i2c_device_config_t dev_config_3 = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = CH422G_Mode,
        .scl_speed_hz = DEVICE_I2C_MASTER_FREQUENCY,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config_3, &ch422g.dev_handle_mode));

    i2c_device_config_t dev_config_4 = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = CH422G_REG_RD_IO,
        .scl_speed_hz = DEVICE_I2C_MASTER_FREQUENCY,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config_4, &ch422g.dev_handle_rd_io));


    // Reset configuration and register status 
    return CH422G_reset();	
}
