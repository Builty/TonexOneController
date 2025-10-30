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

void platform_init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex, lv_disp_drv_t* pdisp_drv);
void platform_adjust_touch_coords(lv_coord_t* x, lv_coord_t* y);
void platform_adjust_display_flush_area(lv_area_t *area);
void platform_get_icon_coords(int16_t* dest, uint8_t max_entries);
const lv_font_t* platform_get_toast_font(void);
uint16_t platform_get_toast_padding(void);
lv_dir_t platform_adjust_gesture(lv_dir_t gesture);