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

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_ota_ops.h"
#include "sys/param.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_crc.h"
#include "esp_intr_alloc.h"
#include "usb/usb_host.h"
#include "esp_task_wdt.h"
#include "usb_comms.h"
#include "usb/cdc_acm_host.h"
#include "usb_tonex_common.h"
#include "usb_tonex_one.h"
#include "usb_tonex_one_plus.h"
#include "usb_tonex_plug.h"
#include "usb_tonex.h"
#include "usb_valeton_gp5.h"
#include "usb_midi_device.h"
#include "control.h"
#include "task_priorities.h"

#ifdef CONFIG_USB_HOST_ENABLE_ENUM_FILTER_CALLBACK
#define ENABLE_ENUM_FILTER_CALLBACK
#endif // CONFIG_USB_HOST_ENABLE_ENUM_FILTER_CALLBACK

#define USB_AUDIO_SUBCLASS_CONTROL         0x01
#define USB_AUDIO_SUBCLASS_MIDISTREAMING   0x03

#define CLIENT_NUM_EVENT_MSG            5
#define CLASS_DRIVER_ACTION_NONE        0

// action bits
#define CLASS_DRIVER_ACTION_OPEN_DEV    1
#define CLASS_DRIVER_ACTION_READ_DEV    2
#define CLASS_DRIVER_ACTION_TRANSFER    4
#define CLASS_DRIVER_ACTION_CLOSE_DEV   8

#define USB_ADDR_Q_LEN                  4
#define USB_MAX_SLOTS                   4

typedef struct 
{
    class_driver_t drv;
} usb_slot_t;

static const char *TAG = "app_usb";
static TaskHandle_t daemon_task_hdl;
static TaskHandle_t class_driver_task_hdl;
static uint8_t AmpModellerType = AMP_MODELLER_NONE;
static QueueHandle_t usb_input_queue;
static usb_slot_t slots[USB_MAX_SLOTS] = {0};
static uint8_t addr_q[USB_ADDR_Q_LEN] = {0};
static uint8_t addr_q_n = 0;

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static usb_slot_t* slot_by_hdl(usb_device_handle_t h)
{
    for (int i = 0; i < USB_MAX_SLOTS; i++) 
    {
        if (slots[i].drv.dev_hdl == h) 
        {
            return &slots[i];
        }
    }
    return NULL;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static usb_slot_t* slot_free(void)
{
    for (int i = 0; i < USB_MAX_SLOTS; i++) 
    {
        if (slots[i].drv.dev_hdl == NULL) 
        {
            return &slots[i];
        }
    }
    return NULL;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void client_event_cb(const usb_host_client_event_msg_t *event_msg, void *arg)
{
    switch (event_msg->event) 
    {
        case USB_HOST_CLIENT_EVENT_NEW_DEV:
            ESP_LOGI(TAG, "client_event_cb new device");   

            if (addr_q_n < USB_ADDR_Q_LEN) 
            {
                addr_q[addr_q_n++] = event_msg->new_dev.address;
            }
            break;

        case USB_HOST_CLIENT_EVENT_DEV_GONE:
            ESP_LOGI(TAG, "client_event_cb device gone");   

            for (int i = 0; i < USB_MAX_SLOTS; i++) 
            {
                if (slots[i].drv.dev_hdl == event_msg->dev_gone.dev_hdl) 
                {
                    slots[i].drv.actions |= CLASS_DRIVER_ACTION_CLOSE_DEV;
                    break;
                }
            }
            break;

        default:
            //Should never occur
            abort();
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static uint16_t find_midi_streaming(const usb_config_desc_t* cfg)
{
    if (!cfg) 
    {
        return 0xFF;
    }

    for (int num = 0; num < cfg->bNumInterfaces; num++) 
    {
        int offset = 0;
        const usb_intf_desc_t *intf = usb_parse_interface_descriptor(cfg, num, 0, &offset);

        if (!intf) 
        {
            ESP_LOGW(TAG, "no descriptor for iface %d alt 0", num);
            continue;
        }

        /* debug
        ESP_LOGI(TAG,
                 "iface=%u alt=%u class=0x%02x subclass=0x%02x proto=0x%02x eps=%u",
                 intf->bInterfaceNumber,
                 intf->bAlternateSetting,
                 intf->bInterfaceClass,
                 intf->bInterfaceSubClass,
                 intf->bInterfaceProtocol,
                 intf->bNumEndpoints);
        */

        if ((intf->bInterfaceClass == USB_CLASS_AUDIO) && (intf->bInterfaceSubClass == USB_AUDIO_SUBCLASS_MIDISTREAMING))
        {
            return num;
        }
    }
    
    return 0xFF;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void class_driver_task(void *arg)
{
    esp_err_t err;
    SemaphoreHandle_t signaling_sem = (SemaphoreHandle_t)arg;
    class_driver_t driver_obj = {0};
    uint8_t exit = 0;
    //const usb_device_desc_t* dev_desc;
    //usb_device_info_t dev_info;    

    ESP_LOGI(TAG, "class_driver_task() start");   

    //Wait until daemon task has installed USB Host Library
    xSemaphoreTake(signaling_sem, portMAX_DELAY);

    ESP_LOGI(TAG, "Registering Client");
    usb_host_client_config_t client_config = 
    {
        .is_synchronous = false,    //Synchronous clients currently not supported. Set this to false
        .max_num_event_msg = CLIENT_NUM_EVENT_MSG,
        .async = {
            .client_event_callback = client_event_cb,
            .callback_arg = (void *) &driver_obj,
        },
    };
    err = usb_host_client_register(&client_config, &driver_obj.client_hdl);

    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "usb_host_client_register() failed!");   
    }

    driver_obj.actions = CLASS_DRIVER_ACTION_NONE;

    while (!exit) 
    {
        // Call the client event handler function - no waiting
        usb_host_client_handle_events(driver_obj.client_hdl, pdMS_TO_TICKS(1));

        // check for any pending device attachments
        while (addr_q_n > 0) 
        {
            uint8_t addr = addr_q[0];

            // move next queued item to front of list
            memmove(&addr_q[0], &addr_q[1], --addr_q_n);

            // get a new slow for the device
            usb_slot_t* s = slot_free();
            if (!s) 
            {
                ESP_LOGE(TAG, "No free USB slots");
                break;
            }

            ESP_LOGI(TAG, "Found USB device");

            s->drv.client_hdl = driver_obj.client_hdl;
            s->drv.dev_addr = addr;
            
            // open device
            if (usb_host_device_open(driver_obj.client_hdl, addr, &s->drv.dev_hdl) != ESP_OK) 
            {
                ESP_LOGE(TAG, "Failed to open USB device");
                s->drv.dev_addr = 0;
                continue;
            }

            // read descriptors
            const usb_device_desc_t* dd;
            const usb_config_desc_t* cd;
            usb_host_get_device_descriptor(s->drv.dev_hdl, &dd);
            usb_host_get_active_config_descriptor(s->drv.dev_hdl, &cd);

            //ESP_LOGI(TAG, "USB device class %d", dd->bDeviceClass);

            if (dd->bDeviceClass == 0x09) 
            {
                ESP_LOGI(TAG, "Found USB Hub, close it");
                usb_host_device_close(driver_obj.client_hdl, s->drv.dev_hdl);
                s->drv.dev_hdl = NULL;
                s->drv.dev_addr = 0;
                continue;
            }

            usb_host_device_close(driver_obj.client_hdl, s->drv.dev_hdl);
            
            // check for IK Multimedia Vendor and Product ID 
            if ((dd->idVendor == IK_MULTIMEDIA_USB_VENDOR) && (dd->idProduct == TONEX_ONE_PRODUCT_ID))
            {
                // found Tonex One
                ESP_LOGI(TAG, "Found Tonex One");
                AmpModellerType = AMP_MODELLER_TONEX_ONE;

                usb_tonex_one_init(&s->drv, usb_input_queue);
            }
            else if ((dd->idVendor == IK_MULTIMEDIA_USB_VENDOR) && (dd->idProduct == TONEX_PRODUCT_ID))
            {
                // found Tonex 
                ESP_LOGI(TAG, "Found Tonex");
                AmpModellerType = AMP_MODELLER_TONEX;

                usb_tonex_init(&s->drv, usb_input_queue);
            }
            else if ((dd->idVendor == VALETON_USB_VENDOR) && (dd->idProduct == VALETON_GP5_PRODUCT_ID))
            {
                // found Valeton GP5
                ESP_LOGI(TAG, "Found Valeton GP5");
                AmpModellerType = AMP_MODELLER_VALETON_GP5;

                usb_valeton_gp5_init(&s->drv, usb_input_queue);
            }
            else if ((dd->idVendor == IK_MULTIMEDIA_USB_VENDOR) && (dd->idProduct == TONEX_PLUG_PRODUCT_ID))
            {
                // found Tonex Plug
                ESP_LOGI(TAG, "Found Tonex Plug");
                AmpModellerType = AMP_MODELLER_TONEX_PLUG;

                usb_tonex_plug_init(&s->drv, usb_input_queue);
            }
            else if ((dd->idVendor == IK_MULTIMEDIA_USB_VENDOR) && (dd->idProduct == TONEX_ONE_PLUS_PRODUCT_ID))
            {
                // found Tonex One Plus
                ESP_LOGI(TAG, "Found Tonex One Plus");
                AmpModellerType = AMP_MODELLER_TONEX_ONE_PLUS;

                usb_tonex_one_plus_init(&s->drv, usb_input_queue);
            }
            else
            {
                // check if Midi device
                uint16_t midi_interface_index = find_midi_streaming(cd);

                if (midi_interface_index != 0xFF)
                {
                    ESP_LOGI(TAG, "Found USB Midi device");

                    usb_midi_device_init(&s->drv, dd->idVendor, dd->idProduct, midi_interface_index, usb_input_queue);
                }
                else
                {
                    ESP_LOGI(TAG, "Found unexpected USB device");

                    // close it
                    usb_host_device_close(driver_obj.client_hdl, s->drv.dev_hdl);
                    s->drv.dev_hdl = NULL;
                    s->drv.dev_addr = 0;
                }
            }

            vTaskDelay(pdMS_TO_TICKS(5));
        }

        //service every live slot
        for (int i = 0; i < USB_MAX_SLOTS; i++) 
        {
            usb_slot_t* s = &slots[i];
            if (!s->drv.dev_hdl) 
            {
                continue;
            }

            if (s->drv.actions & CLASS_DRIVER_ACTION_CLOSE_DEV) 
            {
                uint8_t was_modeller = 0;

                ESP_LOGI(TAG, "USB close device");

                // Release the interface
                if (AmpModellerType != AMP_MODELLER_NONE)
                {
                    usb_host_interface_release(driver_obj.client_hdl, s->drv.dev_hdl, 1);
                    was_modeller = 1;
                }
                
                // clean up
                switch (AmpModellerType)
                {
                    case AMP_MODELLER_TONEX_ONE:
                    {
                        usb_tonex_one_deinit();
                    } break;

                    case AMP_MODELLER_TONEX:
                    {
                        usb_tonex_deinit();
                    } break;

                    case AMP_MODELLER_VALETON_GP5:
                    {
                        usb_valeton_gp5_deinit();
                    } break;

                    case AMP_MODELLER_TONEX_PLUG:
                    {
                        usb_tonex_plug_deinit();
                    } break;

                    case AMP_MODELLER_TONEX_ONE_PLUS:
                    {
                        usb_tonex_one_plus_deinit();
                    } break;

                    default:
                    {
                        // nothing needed
                    } break;
                }

                if (was_modeller)
                {
                    AmpModellerType = AMP_MODELLER_NONE;

                    // update UI
                    control_set_usb_status(0);
                }

                // close device
                usb_host_device_close(driver_obj.client_hdl, s->drv.dev_hdl);

                memset((void*)s, 0, sizeof(*s));

                driver_obj.actions &= ~CLASS_DRIVER_ACTION_CLOSE_DEV;
            }

            // handle device
            switch (AmpModellerType)
            {
                case AMP_MODELLER_TONEX_ONE:
                {
                    usb_tonex_one_handle(&driver_obj);
                } break;

                case AMP_MODELLER_TONEX:
                {
                    usb_tonex_handle(&driver_obj);
                } break;

                case AMP_MODELLER_VALETON_GP5:
                {
                    usb_valeton_gp5_handle(&driver_obj);
                } break;
                
                case AMP_MODELLER_TONEX_PLUG:
                {
                    usb_tonex_plug_handle(&driver_obj);
                } break;

                case AMP_MODELLER_TONEX_ONE_PLUS:
                {
                    usb_tonex_one_plus_handle(&driver_obj);    
                } break;

                default:
                {
                    // nothing needed
                } break;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }

    usb_host_client_deregister(driver_obj.client_hdl);
    ESP_LOGI(TAG, "USB thread exit");
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
#ifdef ENABLE_ENUM_FILTER_CALLBACK
static bool set_config_cb(const usb_device_desc_t *dev_desc, uint8_t *bConfigurationValue)
{
    // If the USB device has more than one configuration, set the second configuration
    if (dev_desc->bNumConfigurations > 1) {
        *bConfigurationValue = 2;
    } else {
        *bConfigurationValue = 1;
    }

    // Return true to enumerate the USB device
    return true;
}
#endif // ENABLE_ENUM_FILTER_CALLBACK

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void host_lib_daemon_task(void *arg)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Installing USB Host Library");

    SemaphoreHandle_t signaling_sem = (SemaphoreHandle_t)arg;

    // delay here, as with the big Tonex (being self-powered) we need to give the ESP32 enough time
    // to initialise the USB port before we try to enumerate the bus
    vTaskDelay(500); 

    //Create the USB class driver task
    xTaskCreatePinnedToCore(class_driver_task,
                            "class",
                            (3 * 1024), 
                            (void*)signaling_sem,
                            USB_CLASS_TASK_PRIORITY,
                            &class_driver_task_hdl,
                            0);

    usb_host_config_t host_config = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LEVEL2,
#ifdef ENABLE_ENUM_FILTER_CALLBACK
        .enum_filter_cb = set_config_cb,
#endif // ENABLE_ENUM_FILTER_CALLBACK
#if USB_HOST_CONFIG_HAS_FSLS_ONLY 
        // force high speed mode for better hub compatibility
        .fsls_only = true,        
#endif
    };

    err = usb_host_install(&host_config);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "usb_host_install() failed!");   
    }

    // Signal to the class driver task that the host library is installed
    xSemaphoreGive(signaling_sem);

    //Short delay to let client task spin up
    vTaskDelay(10); 

    while (1) 
    {
        uint32_t event_flags;
        
        err = usb_host_lib_handle_events(portMAX_DELAY, &event_flags);

        if (err != ESP_OK)
        {
            // error
            ESP_LOGI(TAG, "usb_host_lib_handle_events not OK");
        }
        else
        {
            if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) 
            {
                //has_clients = false;
            }
            
            if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) 
            {
                //has_devices = false;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void usb_set_preset(uint32_t preset)
{
    tUSBMessage message;

    if (usb_input_queue == NULL)
    {
        ESP_LOGE(TAG, "usb_set_preset queue null");            
    }
    else
    {
        message.Command = USB_COMMAND_SET_PRESET;
        message.Payload = preset;

        // send to queue
        if (xQueueSend(usb_input_queue, (void*)&message, 0) != pdPASS)
        {
            ESP_LOGE(TAG, "usb_set_preset queue send failed!");            
        }
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void usb_request_tuner(uint8_t state)
{
    tUSBMessage message;

    if (usb_input_queue == NULL)
    {
        ESP_LOGE(TAG, "usb_request_tuner queue null");            
    }
    else
    {
        message.Command = USB_COMMAND_REQUEST_TUNER;
        message.Payload = state;

        // send to queue
        if (xQueueSend(usb_input_queue, (void*)&message, 0) != pdPASS)
        {
            ESP_LOGE(TAG, "usb_request_tuner queue send failed!");            
        }
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void usb_modify_parameter(uint16_t index, float value)
{
    tUSBMessage message;

    ESP_LOGI(TAG, "usb_modify_parameter: %d, %f", (int)index, value);            

    if (usb_input_queue == NULL)
    {
        ESP_LOGE(TAG, "usb_modify_parameter queue null");            
    }
    else
    {
        message.Command = USB_COMMAND_MODIFY_PARAMETER;
        message.Payload = index;
        message.PayloadFloat = value;

        // send to queue
        if (xQueueSend(usb_input_queue, (void*)&message, 0) != pdPASS)
        {
            ESP_LOGE(TAG, "usb_modify_parameter queue send failed!");            
        }
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void usb_load_preset_to_slot_a(uint32_t preset)
{
    tUSBMessage message;

    if (usb_input_queue == NULL)
    {
        ESP_LOGE(TAG, "usb_load_preset_to_slot_a queue null");            
    }
    else
    {
        message.Command = USB_COMMAND_LOAD_PRESET_TO_SLOT_A;
        message.Payload = preset;

        // send to queue
        if (xQueueSend(usb_input_queue, (void*)&message, 0) != pdPASS)
        {
            ESP_LOGE(TAG, "usb_load_preset_to_slot_a queue send failed!");            
        }
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void usb_load_preset_to_slot_b(uint32_t preset)
{
    tUSBMessage message;

    if (usb_input_queue == NULL)
    {
        ESP_LOGE(TAG, "usb_load_preset_to_slot_b queue null");            
    }
    else
    {
        message.Command = USB_COMMAND_LOAD_PRESET_TO_SLOT_B;
        message.Payload = preset;

        // send to queue
        if (xQueueSend(usb_input_queue, (void*)&message, 0) != pdPASS)
        {
            ESP_LOGE(TAG, "usb_load_preset_to_slot_b queue send failed!");            
        }
    }
}

/****************************************************************************
* NAME:
* DESCRIPTION:
* PARAMETERS:
* RETURN:
* NOTES:
*****************************************************************************/  
void usb_set_ab_slots(uint32_t preset_a, uint32_t preset_b)
{
    tUSBMessage message;

    if (usb_input_queue == NULL)
    {
        ESP_LOGE(TAG, "usb_set_ab_slots queue null");
    }
    else
    {
        message.Command = USB_COMMAND_SET_AB_SLOTS;
        message.Payload = ((preset_a & 0xFF) << 8) | (preset_b & 0xFF);

        // send to queue
        if (xQueueSend(usb_input_queue, (void*)&message, 0) != pdPASS)
        {
            ESP_LOGE(TAG, "usb_set_ab_slots queue send failed!");
        }
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void usb_save_preset(void)
{
    tUSBMessage message;

    if (usb_input_queue == NULL)
    {
        ESP_LOGE(TAG, "usb_save_preset queue null");            
    }
    else
    {
        message.Command = USB_COMMAND_SAVE_PRESET;
        message.Payload = 0;

        // send to queue
        if (xQueueSend(usb_input_queue, (void*)&message, 0) != pdPASS)
        {
            ESP_LOGE(TAG, "usb_save_preset queue send failed!");            
        }
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint8_t usb_get_max_presets_for_connected_modeller(void)
{
    uint8_t max = MAX_PRESETS_TONEX_ONE;
    switch (AmpModellerType)
    {
        case AMP_MODELLER_TONEX_ONE:
        {
            max = MAX_PRESETS_TONEX_ONE;
        } break;

        case AMP_MODELLER_TONEX:
        {
            max = MAX_PRESETS_TONEX;
        } break;
        
        case AMP_MODELLER_VALETON_GP5:
        {
            max = MAX_PRESETS_VALETON_GP5;
        } break;

        case AMP_MODELLER_TONEX_PLUG:
        {
            max = MAX_PRESETS_TONEX_PLUG;
        } break;

        case AMP_MODELLER_TONEX_ONE_PLUS:
        {
            max = MAX_PRESETS_TONEX_ONE_PLUS;
        } break;
    }

    return max;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint8_t usb_get_first_preset_index_for_connected_modeller(void)
{
    uint8_t first = 1;
    
    switch (AmpModellerType)
    {
        case AMP_MODELLER_TONEX_ONE:
        {
            first = 1;
        } break;

        case AMP_MODELLER_TONEX:
        {
            // big Tonex LCD uses 0-based indexing
            first = 0;
        } break;
        
        case AMP_MODELLER_VALETON_GP5:
        {
            first = 0;
        } break;
        
        case AMP_MODELLER_TONEX_PLUG:
        {
            first = 1;
        } break;

        case AMP_MODELLER_TONEX_ONE_PLUS:
        {
            first = 1;
        } break;
    }

    return first;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
uint8_t usb_get_connected_modeller_type(void)
{
    return AmpModellerType;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void init_usb_comms(void)
{
    // init USB
    SemaphoreHandle_t signaling_sem = xSemaphoreCreateBinary();

    // create queue for commands from other threads
    usb_input_queue = xQueueCreate(10, sizeof(tUSBMessage));
    if (usb_input_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create usb input queue!");
    }

    // reserve DMA capable large contiguous memory blocks
    tonex_common_preallocate_memory();

    //Create USB daemon task
    xTaskCreatePinnedToCore(host_lib_daemon_task,
                            "daemon",
                            (3 * 1024), 
                            (void*)signaling_sem,
                            USB_DAEMON_TASK_PRIORITY,
                            &daemon_task_hdl,
                            0);
}
