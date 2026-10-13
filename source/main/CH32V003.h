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

#pragma once

#include <stdint.h>

#include "esp_err.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

esp_err_t CH32V003_init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex, uint8_t output_values);
esp_err_t CH32V003_write_output(uint8_t pin, uint8_t value);
