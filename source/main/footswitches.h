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

#ifndef _FOOTSWITCHES_H
#define _FOOTSWITCHES_H

#ifdef __cplusplus
extern "C" {
#endif

void footswitches_init(i2c_master_bus_handle_t bus_handle, SemaphoreHandle_t I2CMutex);
void footswitches_handle(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif