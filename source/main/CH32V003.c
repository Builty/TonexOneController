/*
 Copyright (C) 2026  Zolfer Figueiredo

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

#include "esp_log.h"
#include "CH32V003.h"

#define CH32V003_I2C_ADDRESS        (0x24)
#define DEVICE_I2C_MASTER_FREQUENCY 400000
#define I2C_TIMEOUT_MS              (50)

// Register map of Waveshare's CH32V003 IO expander firmware
#define REG_DIRECTION               (0x02)
#define REG_OUTPUT                  (0x03)

static const char *TAG = "app_ch32v003";
static i2c_master_dev_handle_t dev_handle;
static SemaphoreHandle_t I2CMutexHandle;
static uint8_t output_shadow;

static esp_err_t CH32V003_write_reg(uint8_t reg, uint8_t value)
{
    esp_err_t res = ESP_FAIL;
    uint8_t data[2] = {reg, value};

    if (xSemaphoreTake(I2CMutexHandle, (TickType_t)100) == pdTRUE)
    {
        res = i2c_master_transmit(dev_handle, data, sizeof(data), I2C_TIMEOUT_MS);

        xSemaphoreGive(I2CMutexHandle);
    }

    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Write reg 0x%02X failed: %s", (unsigned int)reg, esp_err_to_name(res));
    }

    return res;
}

esp_err_t CH32V003_init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex, uint8_t output_values)
{
    esp_err_t res;

    I2CMutexHandle = I2CMutex;
    output_shadow = output_values;

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = CH32V003_I2C_ADDRESS,
        .scl_speed_hz = DEVICE_I2C_MASTER_FREQUENCY,
    };

    res = i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle);

    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add I2C device: %s", esp_err_to_name(res));
        return res;
    }

    res = CH32V003_write_reg(REG_DIRECTION, 0xFF);

    if (res != ESP_OK)
    {
        return res;
    }

    return CH32V003_write_reg(REG_OUTPUT, output_shadow);
}

esp_err_t CH32V003_write_output(uint8_t pin, uint8_t value)
{
    if (value)
    {
        output_shadow |= (1 << pin);
    }
    else
    {
        output_shadow &= ~(1 << pin);
    }

    return CH32V003_write_reg(REG_OUTPUT, output_shadow);
}
