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


#ifndef _USB_TONEX_H
#define _USB_TONEX_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PRESETS_TONEX               150


void usb_tonex_handle(class_driver_t* driver_obj);
void usb_tonex_init(class_driver_t* driver_obj, QueueHandle_t comms_queue);
void usb_tonex_deinit(void);
void usb_tonex_preallocate_memory(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif