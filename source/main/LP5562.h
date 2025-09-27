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

#pragma once

#include <stdint.h>

#include "driver/i2c.h"
#include "esp_err.h"

esp_err_t LP5562_set_color(uint8_t red, uint8_t blue, uint8_t green, uint8_t white);
esp_err_t LP5562_set_engine(uint8_t r, uint8_t g, uint8_t b);
esp_err_t LP5562_engine_load(uint8_t engine, const uint8_t *program, uint8_t size);
esp_err_t LP5562_engine_control(uint8_t eng1, uint8_t eng2, uint8_t eng3);
uint8_t LP5562_get_engine_state(uint8_t engine);
uint8_t LP5562_get_pc(uint8_t engine);
esp_err_t LP5562_set_pc(uint8_t engine, uint8_t val);
esp_err_t LP5562_init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex);
