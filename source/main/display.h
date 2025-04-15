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

#ifndef _DISPLAY_H
#define _DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

void display_init(i2c_port_t I2CNum, SemaphoreHandle_t I2CMutex);

// thread-safe API for other tasks to update the UI
void UI_SetUSBStatus(uint8_t state);
void UI_SetBTStatus(uint8_t state);
void UI_SetWiFiStatus(uint8_t state);
void UI_SetPresetLabel(char* text);
void UI_SetBankIndex(uint16_t index);
void UI_SetAmpSkin(uint16_t index);
void UI_SetPresetDescription(char* text);
void UI_RefreshParameterValues(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif