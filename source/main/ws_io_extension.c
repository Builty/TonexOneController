/*
 Copyright (C) 2026  Greg Smith

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

#include "freertos/FreeRTOS.h"
#include "driver/i2c_master.h"
#include "esp_bit_defs.h"
#include "esp_check.h"
#include "esp_log.h"
#include "ws_io_extension.h"


#define WS_IO_EXTENSION_ADDR      	0x24
#define DEVICE_I2C_MASTER_FREQUENCY 400000

// Timeout of each I2C communication 
#define I2C_TIMEOUT_MS          	(50)


// Mode control flags (from the chip manual)
#define IO_EXTENSION_Mode             0x02
#define IO_EXTENSION_IO_OUTPUT_ADDR   0x03
#define IO_EXTENSION_IO_INPUT_ADDR    0x04
#define IO_EXTENSION_PWM_ADDR         0x05
#define IO_EXTENSION_ADC_ADDR         0x06
#define IO_EXTENSION_RTC_INT_ADDR     0x07


// this chip used on Waveshare 4.3C
#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43C

typedef struct  
{ 
     uint8_t Last_io_value;
     uint8_t Last_od_value;
} io_extension_obj_t;
 
/**
 * @brief Device Structure Type
 *
 */
static const char *TAG = "app_ws_io_extension";
static SemaphoreHandle_t I2CMutexHandle;
static i2c_master_dev_handle_t dev_handle;
static io_extension_obj_t io_extension_obj;

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t __attribute__((unused)) ws_io_extension_write(uint8_t reg, uint8_t val)
{
	esp_err_t res = ESP_FAIL;
	uint8_t buff[2];
	
	buff[0] = reg;
	buff[1] = val;

    if (xSemaphoreTake(I2CMutexHandle, pdMS_TO_TICKS(200)) == pdTRUE)
    {		
		res = i2c_master_transmit(dev_handle, buff, sizeof(buff), pdMS_TO_TICKS(I2C_TIMEOUT_MS));

        if (res != ESP_OK)
        {
            ESP_LOGE(TAG, "ws_io_extension_write failed");
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
static esp_err_t __attribute__((unused)) ws_io_extension_read(uint8_t reg, uint8_t* val)
{
	esp_err_t res = ESP_FAIL;

	if (xSemaphoreTake(I2CMutexHandle, pdMS_TO_TICKS(200)) == pdTRUE)
    {		
		res = i2c_master_transmit_receive(dev_handle, &reg, sizeof(uint8_t), val, sizeof(uint8_t), pdMS_TO_TICKS(I2C_TIMEOUT_MS));

        if (res != ESP_OK)
        {
            ESP_LOGE(TAG, "ws_io_extension_read failed");
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
esp_err_t __attribute__((unused)) ws_io_extension_output(uint8_t pin, uint8_t value) 
{
    esp_err_t ret = ESP_FAIL;

    // Update the output value based on the pin and value
    if (value == 1)
    {
        // Set the pin high
        io_extension_obj.Last_io_value |= (1 << pin); 
    }
    else
    {
        // Set the pin low
        io_extension_obj.Last_io_value &= (~(1 << pin)); 
    }

    if (ws_io_extension_write(IO_EXTENSION_IO_OUTPUT_ADDR, io_extension_obj.Last_io_value) != ESP_OK)
    {
        ESP_LOGE(TAG, "ws_io_extension_output write failed");
    }
    else
    {
        ret = ESP_OK;
    }
    
    return ret;    
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t __attribute__((unused)) ws_io_extension_input(uint8_t pin, uint8_t* state) 
{
    esp_err_t ret = ESP_FAIL;
    uint8_t value = 0;
    *state = 0;

    if (ws_io_extension_read(IO_EXTENSION_IO_INPUT_ADDR, &value) != ESP_OK)
    {
        ESP_LOGE(TAG, "ws_io_extension_input failed");
    }
    else
    {
        if ((value & (1 << pin)) != 0)
        {
            *state = 1;
        }
        ret = ESP_OK;
    }
    
    return ret;    
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t __attribute__((unused)) ws_io_extension_input_all(uint8_t* state) 
{
    esp_err_t ret = ESP_FAIL;
    uint8_t value = 0;
    *state = 0;
        
    if (ws_io_extension_read(IO_EXTENSION_IO_INPUT_ADDR, &value) != ESP_OK)
    {
        ESP_LOGE(TAG, "ws_io_extension_input_all failed");
    }
    else
    {
        *state = value;
        ret = ESP_OK;
    }
    
    return ret;    
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t __attribute__((unused)) ws_io_extension_pwm_output(uint8_t value) 
{
    esp_err_t ret = ESP_FAIL;
    
    // hack from Waveshare code to clamp
    if (value >= 97)
    {
        value = 97;
    }
    
    // Calculate the duty cycle based on the resolution (12 bits??)
    if (ws_io_extension_write(IO_EXTENSION_PWM_ADDR, (uint8_t)((float)value * (255.0f / 100.0f))) != ESP_OK)
    {
        ESP_LOGE(TAG, "ws_io_extension_pwm_output write failed");
    }
    else
    {
        ret = ESP_OK;
    }
    
    return ret;    
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t __attribute__((unused)) ws_io_extension_read_adc(uint8_t* value) 
{
    uint8_t val = 0;
    esp_err_t ret = ESP_FAIL;
    
    *value = 0;

    if (ws_io_extension_read(IO_EXTENSION_ADC_ADDR, &val) != ESP_OK)
    {
        ESP_LOGE(TAG, "ws_io_extension_read_adc write failed");
    }
    else
    {
        *value = val;
        ret = ESP_OK;
    }
    
    return ret;    
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t __attribute__((unused)) ws_io_extension_io_mode(uint8_t pin) 
{
    esp_err_t ret = ESP_FAIL;
    uint8_t verify = 0;
    
    ESP_LOGI(TAG, "ws_io_extension_io_mode %d", pin);    

    if (ws_io_extension_write(IO_EXTENSION_Mode, pin) != ESP_OK)
    {
        ESP_LOGE(TAG, "ws_io_extension_io_mode write failed");
    }
    else
    {
        // debug: read back to verify
        vTaskDelay(10);
        if (ws_io_extension_read(IO_EXTENSION_Mode, &verify) == ESP_OK)
        {
            ESP_LOGI(TAG, "ws_io_extension_io_mode verify %d %d", verify, pin);
        }

        ret = ESP_OK;
    }
    
    return ret;    
}

#endif

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t ws_io_extension_init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex, uint8_t io_states)
{
#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43C
	// save handles
    I2CMutexHandle = I2CMutex;
	
    // create device instance
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = WS_IO_EXTENSION_ADDR,
        .scl_speed_hz = DEVICE_I2C_MASTER_FREQUENCY,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle));

    // Set IO states for pins (1 = output)   
    if (ws_io_extension_io_mode(io_states) != ESP_OK)
    {
        ESP_LOGE(TAG, "ws_io_extension_io_mode init failed");
    }

    // Initialize control flags for IO output enable and open-drain output mode
    io_extension_obj.Last_io_value = 0xF7;
    io_extension_obj.Last_od_value = 0xF7;
#endif

	return ESP_OK;
}
