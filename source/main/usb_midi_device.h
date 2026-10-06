/*
 Copyright (C) 2026  Greg Smith

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


#ifndef _USB_MIDI_DEVICE_H
#define _USB_MIDI_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

void usb_midi_device_handle(class_driver_t* driver_obj);
void usb_midi_device_init(class_driver_t* driver_obj, uint16_t vendor_id, uint16_t product_id, uint16_t interface_index, QueueHandle_t comms_queue);
void usb_midi_device_deinit(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif