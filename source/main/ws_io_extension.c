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
		res = i2c_master_transmit_receive(dev_handle, &reg, sizeof(reg), val, sizeof(val), pdMS_TO_TICKS(I2C_TIMEOUT_MS));

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
    esp_err_t ret;

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

    uint8_t data[2] = {IO_EXTENSION_IO_OUTPUT_ADDR, io_extension_obj.Last_io_value}; 
        
    // do the transfer
    if (xSemaphoreTake(I2CMutexHandle, pdMS_TO_TICKS(200)) == pdTRUE)
    {		        
        if (i2c_master_transmit(dev_handle, data, 2, pdMS_TO_TICKS(I2C_TIMEOUT_MS)) != ESP_OK)
        {
            ESP_LOGE(TAG, "ws_io_extension_output write failed");
        }
        else
        {
            ret = ESP_OK;
        }
        xSemaphoreGive(I2CMutexHandle);
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
    esp_err_t ret;
    uint8_t value = 0;
    *state = 0;

    uint8_t data[1] = {IO_EXTENSION_IO_INPUT_ADDR}; 
        
    // do the transfer
    if (xSemaphoreTake(I2CMutexHandle, pdMS_TO_TICKS(200)) == pdTRUE)
    {		      
        if (i2c_master_transmit_receive(dev_handle, data, 1, &value, 1, pdMS_TO_TICKS(I2C_TIMEOUT_MS)) != ESP_OK)
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
        xSemaphoreGive(I2CMutexHandle);
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
    esp_err_t ret;
    uint8_t value = 0;
    *state = 0;

    uint8_t data[1] = {IO_EXTENSION_IO_INPUT_ADDR}; 
        
    // do the transfer
    if (xSemaphoreTake(I2CMutexHandle, pdMS_TO_TICKS(200)) == pdTRUE)
    {		      
        if (i2c_master_transmit_receive(dev_handle, data, 1, &value, 1, pdMS_TO_TICKS(I2C_TIMEOUT_MS)) != ESP_OK)
        {
            ESP_LOGE(TAG, "ws_io_extension_input_all failed");
        }
        else
        {
            *state = value;
            ret = ESP_OK;
        }
        xSemaphoreGive(I2CMutexHandle);
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
    esp_err_t ret;
    
    // hack from Waveshare code to clamp
    if (value >= 97)
    {
        value = 97;
    }
    
    uint8_t data[2] = {IO_EXTENSION_PWM_ADDR, value}; 

    // Calculate the duty cycle based on the resolution (12 bits??)
    data[1] = value * (255 / 100.0);
    
    // do the transfer
    if (xSemaphoreTake(I2CMutexHandle, pdMS_TO_TICKS(200)) == pdTRUE)
    {		        
        if (i2c_master_transmit(dev_handle, data, 2, pdMS_TO_TICKS(I2C_TIMEOUT_MS)) != ESP_OK)
        {
            ESP_LOGE(TAG, "ws_io_extension_pwm_output write failed");
        }
        else
        {
            ret = ESP_OK;
        }
        xSemaphoreGive(I2CMutexHandle);
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
    esp_err_t ret;
    
    *value = 0;

    uint8_t data[1] = {IO_EXTENSION_ADC_ADDR}; 
        
    // do the transfer
    if (xSemaphoreTake(I2CMutexHandle, pdMS_TO_TICKS(200)) == pdTRUE)
    {		      
        if (i2c_master_transmit_receive(dev_handle, data, 1, &val, 1, pdMS_TO_TICKS(I2C_TIMEOUT_MS)) != ESP_OK)
        {
            ESP_LOGE(TAG, "ws_io_extension_read_adc write failed");
        }
        else
        {
            *value = val;
            ret = ESP_OK;
        }
        xSemaphoreGive(I2CMutexHandle);
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
    esp_err_t ret;
    uint8_t data[2] = {IO_EXTENSION_Mode, pin}; 
    
    // Write the 8-bit value to the IO mode register 
    if (xSemaphoreTake(I2CMutexHandle, pdMS_TO_TICKS(200)) == pdTRUE)
    {		        
        if (i2c_master_transmit(dev_handle, data, 2, pdMS_TO_TICKS(I2C_TIMEOUT_MS)) != ESP_OK)
        {
            ESP_LOGE(TAG, "ws_io_extension_io_mode write failed");
        }
        else
        {
            ret = ESP_OK;
        }
        xSemaphoreGive(I2CMutexHandle);
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
esp_err_t ws_io_extension_init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex)
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

    // Set IO states for pins all pins (1 = output)   
    ws_io_extension_io_mode(0xff); 

    // Initialize control flags for IO output enable and open-drain output mode
    io_extension_obj.Last_io_value = 0xF7;
    io_extension_obj.Last_od_value = 0xF7;
#endif

	return ESP_OK;
}
