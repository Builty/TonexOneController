/************************************************
      Header File: SX1509.H
************************************************/

#ifndef _SX1509_H
#define _SX1509_H

enum Expander_PinModes
{    
    EXPANDER_INPUT,
    EXPANDER_INPUT_PULLUP,
    EXPANDER_INPUT_PULLDOWN,
    EXPANDER_OUTPUT,
    EXPANDER_OUTPUT_PULLUP,
    EXPANDER_OUTPUT_PULLDOWN
};

esp_err_t SX1509_Init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex);
esp_err_t SX1509_refresh(void);
esp_err_t SX1509_gpioMode(uint8_t pin, uint8_t mode);
esp_err_t SX1509_digitalWrite(uint8_t pin, uint8_t value);
esp_err_t SX1509_digitalRead(uint8_t pin, uint8_t* value); 
esp_err_t SX1509_getPinValues(uint16_t* values);


#endif      //_SX1509_H
