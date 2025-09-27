/*******************************************************
      SX1509 Procedures
*******************************************************/
//---------------------- Include Files ------------------------------------
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
#include "main.h"
#include "SX1509.h"

/* 
** Defines
*/
#define SX1509_IC2_ADDRESS           0x71
#define I2C_TIMEOUT_MS          	 10
#define DEVICE_I2C_MASTER_FREQUENCY  400000

// Class flags
#define SX1509_FLAG_PRESERVE_STATE   0x0001
#define SX1509_FLAG_DEVICE_PRESENT   0x1000
#define SX1509_FLAG_PINS_CONFD       0x2000
#define SX1509_FLAG_INITIALIZED      0x4000
#define SX1509_FLAG_FROM_BLOB        0x8000

#define SX1509_FLAG_SERIAL_MASK      0x000F  // Only these bits are serialized.


// These are the i2c register indicies
enum
{
    SX1509_REG_INPUT_DISABLE_B,
    SX1509_REG_INPUT_DISABLE_A,
    SX1509_REG_LONG_SLEW_B,
    SX1509_REG_LONG_SLEW_A,
    SX1509_REG_LOW_DRIVE_B,
    SX1509_REG_LOW_DRIVE_A,
    SX1509_REG_PULLUP_B,
    SX1509_REG_PULLUP_A,
    SX1509_REG_PULLDOWN_B,
    SX1509_REG_PULLDOWN_A,
    SX1509_REG_OPEN_DRAIN_B,
    SX1509_REG_OPEN_DRAIN_A,
    SX1509_REG_POLARITY_B,
    SX1509_REG_POLARITY_A,
    SX1509_REG_DIR_B,
    SX1509_REG_DIR_A,
    SX1509_REG_DATA_B,
    SX1509_REG_DATA_A,
    SX1509_REG_INTERRUPT_MASK_B,
    SX1509_REG_INTERRUPT_MASK_A,
    SX1509_REG_SENSE_HIGH_B,
    SX1509_REG_SENSE_LOW_B,
    SX1509_REG_SENSE_HIGH_A,
    SX1509_REG_SENSE_LOW_A,
    SX1509_REG_INTERRUPT_SOURCE_B,
    SX1509_REG_INTERRUPT_SOURCE_A,
    SX1509_REG_EVENT_STATUS_B,
    SX1509_REG_EVENT_STATUS_A,
    SX1509_REG_LEVEL_SHIFTER_1,
    SX1509_REG_LEVEL_SHIFTER_2,
    SX1509_REG_CLOCK,
    SX1509_REG_MISC,
    SX1509_REG_LED_DRIVER_ENABLE_B,
    SX1509_REG_LED_DRIVER_ENABLE_A,

    LAST_USED_REGISTER
    // more regs for keypad and led driver, don't need them
};


/*
** Static vars
*/
static uint16_t _flags = 0;
static uint8_t registers[LAST_USED_REGISTER];
static const char *TAG = "app_SX1509";
static SemaphoreHandle_t I2CMutexHandle;
static i2c_master_dev_handle_t dev_handle;

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
static inline uint16_t _sx_flags(void) 
{ 
    return _flags;          
};

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
static inline uint8_t _sx_flag(uint16_t _flag) 
{ 
    return (_flags & _flag); 
};

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
static inline void _sx_clear_flag(uint16_t _flag) 
{ 
    _flags &= ~_flag;        
};

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
static inline void _sx_set_flag(uint16_t _flag) 
{   
    _flags |= _flag;         
};

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
static inline void _sx_set_flags(uint16_t _flag, uint8_t nu) 
{
    if (nu)
    {
        _flags |= _flag;
    }
    else
    {
        _flags &= ~_flag;
    }
};

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
static inline uint8_t initialized(void) 
{  
    return _sx_flag(SX1509_FLAG_INITIALIZED);  
};

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
static inline esp_err_t SX1509_ll_pin_init(void) 
{
    _sx_set_flag(SX1509_FLAG_PINS_CONFD);
    return ESP_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
static esp_err_t SX1509_write_register(uint8_t reg, uint8_t val) 
{
    esp_err_t ret = ESP_FAIL;
    uint8_t outbuffer[5];
  
    // build address at start of buffer
    outbuffer[0] = reg;
    outbuffer[1] = val;

    // do the transfer
    if (xSemaphoreTake(I2CMutexHandle, pdMS_TO_TICKS(200)) == pdTRUE)
    {		        
        if (i2c_master_transmit(dev_handle, outbuffer, 2, pdMS_TO_TICKS(I2C_TIMEOUT_MS)) != ESP_OK)
        {
            ESP_LOGE(TAG, "SX1509 write failed");
        }
        else
        {
            registers[reg] = val;
            ret = ESP_OK;
        }
        xSemaphoreGive(I2CMutexHandle);
    }

    return ret;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
static esp_err_t __attribute__((unused)) SX1509_write_registers(uint8_t reg, uint8_t* buf, uint8_t len) 
{
    esp_err_t ret = ESP_FAIL;    
    uint8_t outbuffer[255];
  
    // build address at start of buffer
    outbuffer[0] = reg;
    
    for (uint8_t i = 0; i < len; i++) 
    {
        outbuffer[i + 1] = buf[i];
    }

    // do the transfer
    if (xSemaphoreTake(I2CMutexHandle, pdMS_TO_TICKS(200)) == pdTRUE)
    {		
        if (i2c_master_transmit(dev_handle, outbuffer, len + 1, pdMS_TO_TICKS(I2C_TIMEOUT_MS)) != ESP_OK)
        {
            ESP_LOGE(TAG, "SX1509 writes failed");
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
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
static esp_err_t SX1509_read_register(uint8_t reg, uint8_t len) 
{  
    esp_err_t ret = ESP_FAIL;
    uint8_t outbuffer[5];
    uint8_t inbuffer[255];

    // do the transfer
    if (xSemaphoreTake(I2CMutexHandle, pdMS_TO_TICKS(200)) == pdTRUE)
    {		
        outbuffer[0] = reg;
        if (i2c_master_transmit_receive(dev_handle, outbuffer, 1, inbuffer, len, pdMS_TO_TICKS(I2C_TIMEOUT_MS)) != ESP_OK)
        {
            ESP_LOGE(TAG, "SX1509_read_register failed");
        }
        else
        {
            ret = ESP_OK;

            for (uint8_t i = 0; i < len; i++) 
            {
                registers[reg + i] = inbuffer[i];
            }
        }

        xSemaphoreGive(I2CMutexHandle);
    }

    return ret;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
esp_err_t SX1509_Init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex)
{ 
    esp_err_t ret = ESP_FAIL;
    
    // save handles
    I2CMutexHandle = I2CMutex;

    memset((void*)registers, 0, sizeof(registers));

    // create device instance
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = SX1509_IC2_ADDRESS,
        .scl_speed_hz = DEVICE_I2C_MASTER_FREQUENCY,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle));

    // check if chip is present by reading
    ret = SX1509_read_register(SX1509_REG_INPUT_DISABLE_B, 1);

    if (ret == ESP_OK)
    {
        _sx_clear_flag(SX1509_FLAG_INITIALIZED);
        if (!_sx_flag(SX1509_FLAG_PINS_CONFD)) 
        {
            SX1509_ll_pin_init();
        }
        
        _sx_set_flags(SX1509_FLAG_INITIALIZED | SX1509_FLAG_DEVICE_PRESENT, (0 == ret));

        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "SX1509 init OK");
        }    
    }
    else
    {
        ESP_LOGI(TAG, "SX1509 not detected");
    }

    return ret; 
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
esp_err_t SX1509_refresh(void) 
{
    esp_err_t ret;

    ret = SX1509_read_register(SX1509_REG_INPUT_DISABLE_B, LAST_USED_REGISTER);
  
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG,  "SX1509 refresh failed");
    }

    return ret;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
esp_err_t SX1509_digitalWrite(uint8_t pin, uint8_t value) 
{
    esp_err_t ret = ESP_FAIL;

    if (pin < 8) 
    {
        uint8_t reg0 = SX1509_REG_DATA_A;
        uint8_t val0 = registers[SX1509_REG_DATA_A];

        if (value > 0)
        {
            // set bit
            val0 |= (1 << pin);
        }
        else
        {
            // clear bit
            val0 &= ~(1 << pin);
        }

        ret = SX1509_write_register(reg0, val0);   
    }
    else if (pin < 16) 
    {
        uint8_t reg0 = SX1509_REG_DATA_B;
        uint8_t val0 = registers[SX1509_REG_DATA_B];

        if (value > 0)
        {
            // set bit
            val0 |= (1 << (pin - 8));
        }
        else
        {
            // clear bit
            val0 &= ~(1 << (pin- 8));
        }

        ret = SX1509_write_register(reg0, val0);   
    }
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG,  "SX1509 digital write failed %d", ret);
    }

    return ret;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
esp_err_t SX1509_digitalRead(uint8_t pin, uint8_t* value) 
{
    esp_err_t ret = ESP_FAIL;
    
    if (pin < 8) 
    {
        if (SX1509_read_register(SX1509_REG_DATA_A, 1) == ESP_OK)
        {
            *value = (registers[SX1509_REG_DATA_A] >> pin) & 0x01;
            ret = ESP_OK;
        }
    }
    else if (pin < 16) 
    {
        if (SX1509_read_register(SX1509_REG_DATA_B, 1) == ESP_OK)
        {
            *value = (registers[SX1509_REG_DATA_B] >> (pin - 8)) & 0x01;
            ret = ESP_OK;
        }
    }
    
    return ret;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
esp_err_t SX1509_getPinValues(uint16_t* values)
{
    esp_err_t ret = ESP_FAIL;
    *values = 0;

    // read first bank
    if (SX1509_read_register(SX1509_REG_DATA_A, 1) == ESP_OK)
    {
        *values = (uint16_t)registers[SX1509_REG_DATA_A];

        // read second bank
        if (SX1509_read_register(SX1509_REG_DATA_B, 1) == ESP_OK)
        {
            *values |= ((uint16_t)registers[SX1509_REG_DATA_B] << 8);
            ret = ESP_OK;
        }
    }
  
    return ret;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  none
* RETURN:      none
* NOTES:       none
*****************************************************************************/
esp_err_t SX1509_gpioMode(uint8_t pin, uint8_t mode) 
{
    esp_err_t ret = ESP_FAIL;
    uint8_t reg0;
    uint8_t reg1;
    uint8_t reg2;
    uint8_t reg3;
    uint8_t val0;
    uint8_t val1;
    uint8_t val2;
    uint8_t val3;

    if (pin < 8) 
    {
        reg0 = SX1509_REG_DIR_A;
        reg1 = SX1509_REG_PULLUP_A;
        reg2 = SX1509_REG_PULLDOWN_A;
        reg3 = SX1509_REG_INTERRUPT_MASK_A;
    }
    else if (pin < 16) 
    {
        reg0 = SX1509_REG_DIR_B;
        reg1 = SX1509_REG_PULLUP_B;
        reg2 = SX1509_REG_PULLDOWN_B;
        reg3 = SX1509_REG_INTERRUPT_MASK_B;

        pin -= 8;
    }
    else
    {
        // invalid pin
        ESP_LOGE(TAG,  "SX1509 gpio mode invalid pin %d", (int)pin);
        return ret;
    }

    val0 = registers[reg0];
    val1 = registers[reg1];
    val2 = registers[reg2];
    val3 = registers[reg3];

    switch (mode) 
    {
        case EXPANDER_OUTPUT_PULLDOWN:
        {
            // clear input bit
            val0 &= ~(1 << pin);

            // clear pullup bit
            val1 |= (1 << pin);

            // set pulldown bit
            val2 |= (1 << pin);                
        } break;

        case EXPANDER_OUTPUT_PULLUP:
        {
            // clear input bit
            val0 &= ~(1 << pin);

            // set pullup bit
            val1 |= (1 << pin);

            // clear pulldown bit
            val2 &= ~(1 << pin);    
        } break;

        case EXPANDER_OUTPUT:
        {
            // clear input bit
            val0 &= ~(1 << pin);

            // clear pullup bit
            val1 &= ~(1 << pin);

            // clear pulldown bit
            val2 &= ~(1 << pin);    
        } break;

        case EXPANDER_INPUT_PULLUP:
        {
            // set input bit
            val0 |= (1 << pin);

            // set pullup bit
            val1 |= (1 << pin);

            // clear pulldown bit
            val2 &= ~(1 << pin);    
        } break;

        case EXPANDER_INPUT_PULLDOWN:
        {
            // set input bit
            val0 |= (1 << pin);

            // clear pullup bit
            val1 &= ~(1 << pin);

            // set pulldown bit
            val2 |= (1 << pin);    
        } break;
        
        case EXPANDER_INPUT:
        default:
        {
            // set input bit
            val0 |= (1 << pin);

            // clear pullup bit
            val1 &= ~(1 << pin);

            // clear pulldown bit
            val2 &= ~(1 << pin);    
            break;
        }
    }

    ret = SX1509_write_register(reg0, val0);   
    ret = SX1509_write_register(reg1, val1);   
    ret = SX1509_write_register(reg2, val2);   
    ret = SX1509_write_register(reg3, val3);   
  
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG,  "SX1509 gpio mode failed");
    }

    return ret;
}
