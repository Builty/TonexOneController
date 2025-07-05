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


#ifndef _USB_COMMS_H
#define _USB_COMMS_H

#ifdef __cplusplus
extern "C" {
#endif

#define IK_MULTIMEDIA_USB_VENDOR        0x1963
#define TONEX_ONE_PRODUCT_ID            0x00D1
#define TONEX_PRODUCT_ID                0x0068

// Amp Modeller types
enum AmpModellers
{
    AMP_MODELLER_NONE,
    AMP_MODELLER_TONEX_ONE,
    AMP_MODELLER_TONEX,     // full size pedal with 3 footswitches
};

enum USB_Commands
{
    USB_COMMAND_SET_PRESET,
    USB_COMMAND_MODIFY_PARAMETER
};

typedef struct 
{
    usb_host_client_handle_t client_hdl;
    uint8_t dev_addr;
    usb_device_handle_t dev_hdl;
    uint32_t actions;
} class_driver_t;

typedef struct 
{
    uint8_t Command;
    uint32_t Payload;
    float PayloadFloat;
} tUSBMessage;

void init_usb_comms(void);

// thread safe public API
void usb_set_preset(uint32_t preset);
void usb_modify_parameter(uint16_t index, float value);
uint8_t usb_get_max_presets_for_connected_tonex(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif