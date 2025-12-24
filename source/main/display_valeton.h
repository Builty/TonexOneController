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

#ifndef _DISPLAY_VALETON_H
#define _DISPLAY_VALETON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

void valeton_show_settings_tab(lv_event_t* e);
void valeton_action_effect_icon_clicked(lv_event_t* e);
void valeton_action_parameter_changed(lv_event_t* e);
void valeton_update_icon_order(void);
uint8_t valeton_update_ui_parameters(void);
void valeton_value_clicked(lv_event_t *e);
void valeton_value_changed(lv_event_t * e);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif