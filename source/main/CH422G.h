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

#pragma once

#include <stdint.h>

#include "driver/i2c.h"
#include "esp_err.h"

/*
The chip does not have a slave address, the stated function register, used as an I2C slave address
For example:
Set working mode
Send slave address 0x24
Write function command
*/

#define CH422G_Mode          0x24
#define CH422G_Mode_IO_OE    0x01 // Output enabled
#define CH422G_Mode_A_SCAN   0x02 // Dynamic display automatic scanning enabled
#define CH422G_Mode_OD_EN    0x04 // The output pins OC3 ~ OC0 open drain output enable
#define CH422G_Mode_SLEEP    0x08 // Low power sleep control

#define CH422G_OD_OUT        0x23 // Control the OC pin output
#define CH422G_IO_OUT        0x38 // Control the IO pin output 
#define CH422G_IO_IN         0x26 // Read the IO pin status

#define CH422G_IO_0         0x01
#define CH422G_IO_1         0x02
#define CH422G_IO_2         0x04
#define CH422G_IO_3         0x08
#define CH422G_IO_4         0x10
#define CH422G_IO_5         0x20
#define CH422G_IO_6         0x40
#define CH422G_IO_7         0x80



/**
 * @brief IO Expander Pin direction
 *
 */
typedef enum 
{
    CH_IO_EXPANDER_INPUT,          /*!< Input direction */
    CH_IO_EXPANDER_OUTPUT,         /*!< Output dircetion */
} ch_io_expander_dir_t;

esp_err_t CH422G_init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex);
esp_err_t CH422G_reset(void);
esp_err_t CH422G_read_input(uint8_t pin_bit, uint8_t* value);
esp_err_t CH422G_read_all_input(uint16_t* values);
esp_err_t CH422G_write_direction(uint8_t pin_bit, uint8_t value);
esp_err_t CH422G_write_output(uint8_t pin_bit, uint8_t value);
esp_err_t CH422G_set_io_mode(uint8_t output_mode);