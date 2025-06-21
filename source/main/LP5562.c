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

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include "driver/i2c_master.h"
#include "esp_bit_defs.h"
#include "esp_check.h"
#include "esp_log.h"
#include "LP5562.h"


#define LP5562_I2C_ADDR         	0x30
#define DEVICE_I2C_MASTER_FREQUENCY 400000

// Timeout of each I2C communication 
#define I2C_TIMEOUT_MS          	(10)


#define LP5562_REG_ENABLE			0x00
#define LP5562_REG_OP_MODE			0x01
#define LP5562_REG_B_PWM			0x02
#define LP5562_REG_G_PWM			0x03
#define LP5562_REG_R_PWM			0x04
#define LP5562_REG_B_CURRENT		0x05
#define LP5562_REG_G_CURRENT		0x06
#define LP5562_REG_R_CURRENT		0x07
#define LP5562_REG_CONFIG			0x08
#define LP5562_REG_ENG1_PC			0x09
#define LP5562_REG_ENG2_PC			0x0a
#define LP5562_REG_ENG3_PC			0x0b
#define LP5562_REG_STATUS			0x0c
#define LP5562_REG_RESET			0x0d
#define LP5562_REG_W_PWM			0x0e
#define LP5562_REG_W_CURRENT		0x0f
#define LP5562_REG_LED_MAP			0x70
#define LP5562_REG_ENG_PROG(n)		(0x10 + ((n)-1) * 0x20)

/* Brightness range: 0x00 - 0xff */
#define LP5562_COLOR_NONE			0x000000
#define LP5562_COLOR_RED(b)			(0x010000 * (b))
#define LP5562_COLOR_GREEN(b)		(0x000100 * (b))
#define LP5562_COLOR_BLUE(b)		(0x000001 * (b))
#define LP5562_ENG_SEL_NONE			0x0
#define LP5562_ENG_SEL_1			0x1
#define LP5562_ENG_SEL_2			0x2
#define LP5562_ENG_SEL_3			0x3
#define LP5562_ENG_HOLD				0x0
#define LP5562_ENG_STEP				0x1
#define LP5562_ENG_RUN				0x2


// this chip used on AtomS3R to run led backlight
#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_M5ATOMS3R

/**
 * @brief Device Structure Type
 *
 */
static const char *TAG = "app_LP5562";
static SemaphoreHandle_t I2CMutexHandle;
static i2c_master_dev_handle_t dev_handle;

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t LP5562_write(uint8_t reg, uint8_t val)
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
            ESP_LOGE(TAG, "LP5562_write failed");
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
static esp_err_t LP5562_read(uint8_t reg, uint8_t* val)
{
	esp_err_t res = ESP_FAIL;

	if (xSemaphoreTake(I2CMutexHandle, pdMS_TO_TICKS(200)) == pdTRUE)
    {		
		res = i2c_master_transmit_receive(dev_handle, &reg, sizeof(reg), val, sizeof(val), pdMS_TO_TICKS(I2C_TIMEOUT_MS));

        if (res != ESP_OK)
        {
            ESP_LOGE(TAG, "LP5562_read failed");
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
static esp_err_t LP5562_poweron(void)
{
	esp_err_t ret = 0;
    
	if (LP5562_write(LP5562_REG_ENABLE, 0x40) != ESP_OK)
	{
		ESP_LOGE(TAG, "LP5562_poweron failed");
	}
    
    // start-up delay
	vTaskDelay(pdMS_TO_TICKS(1)); 
    
	// enable and set PWM clock to 558 Hz
	ret = LP5562_write(LP5562_REG_CONFIG, 0x41);
	ret = LP5562_write(LP5562_REG_LED_MAP, 0x0);
    
	return ret;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static esp_err_t __attribute__((unused)) LP5562_poweroff(void)
{
	return LP5562_write(LP5562_REG_ENABLE, 0x0);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t __attribute__((unused)) LP5562_set_color(uint8_t red, uint8_t blue, uint8_t green, uint8_t white)
{
	esp_err_t ret;

	ret = LP5562_write(LP5562_REG_B_PWM, blue);
	ret = LP5562_write(LP5562_REG_G_PWM, green);
	ret = LP5562_write(LP5562_REG_R_PWM, red);
	ret = LP5562_write(LP5562_REG_W_PWM, white);
    
	return ret;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t __attribute__((unused)) LP5562_set_engine(uint8_t r, uint8_t g, uint8_t b)
{
	return LP5562_write(LP5562_REG_LED_MAP, (r << 4) | (g << 2) | b);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t __attribute__((unused)) LP5562_engine_load(uint8_t engine, const uint8_t* program, uint8_t size)
{
	uint8_t prog_addr = LP5562_REG_ENG_PROG(engine);
	uint8_t i;
    esp_err_t ret;
	uint8_t val;
	uint8_t shift = 6 - (engine * 2);
    
	ret = LP5562_read(LP5562_REG_OP_MODE, &val);
    
	if (ret == ESP_FAIL)
    {
		return ret;
    }
	
    val &= ~(0x3 << shift);
	val |= 0x1 << shift;
    
	ret = LP5562_write(LP5562_REG_OP_MODE, val);
	if (ret == ESP_FAIL)
    {
		return ret;
    }
    
	for (i = 0; i < size; ++i) 
    {
		ret = LP5562_write(prog_addr + i, program[i]);
        
		if (ret == ESP_FAIL)
        {
			return ret;
        }
	}
	
    val &= ~(0x3 << shift);
	val |= 0x2 << shift;
    
	ret = LP5562_write(LP5562_REG_OP_MODE, val);
	return ret;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t __attribute__((unused)) LP5562_engine_control(uint8_t eng1, uint8_t eng2, uint8_t eng3)
{
	esp_err_t ret;
    uint8_t val;
	
    ret = LP5562_read(LP5562_REG_ENABLE, &val);
	if (ret == ESP_FAIL)
    {
		return ret;
    }
	
    val &= 0xc0;
	val |= (eng1 << 4) | (eng2 << 2) | eng3;
	
    return LP5562_write(LP5562_REG_ENABLE, val);
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint8_t __attribute__((unused)) LP5562_get_engine_state(uint8_t engine)
{
	uint8_t val;
    
	if (LP5562_read(LP5562_REG_ENABLE, &val) == ESP_FAIL)
    {
		return 0xee;
    }
    
	return (val >> (6 - (engine * 2))) & 0x3;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint8_t __attribute__((unused)) LP5562_get_pc(uint8_t engine)
{
	uint8_t ret;
    
	if (LP5562_read(LP5562_REG_ENG1_PC + engine - 1, &ret) == ESP_FAIL)
    {
        return 0xee;
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
esp_err_t __attribute__((unused)) LP5562_set_pc(uint8_t engine, uint8_t val)
{
	return LP5562_write(LP5562_REG_ENG1_PC + engine - 1, val);
}
#endif

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t LP5562_init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex)
{
#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_M5ATOMS3R
	// save handles
    I2CMutexHandle = I2CMutex;
	
    // create device instance
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = LP5562_I2C_ADDR,
        .scl_speed_hz = DEVICE_I2C_MASTER_FREQUENCY,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle));

    LP5562_poweron();    
#endif

	return ESP_OK;
}
