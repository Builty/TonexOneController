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
#include "driver/i2c.h"
#include "esp_task_wdt.h"
#include "usb_comms.h"
#include "usb/cdc_acm_host.h"
#include "usb_tonex_common.h"
#include "usb_tonex_one.h"
#include "usb_tonex.h"
#include "control.h"
#include "task_priorities.h"

#ifdef CONFIG_USB_HOST_ENABLE_ENUM_FILTER_CALLBACK
#define ENABLE_ENUM_FILTER_CALLBACK
#endif // CONFIG_USB_HOST_ENABLE_ENUM_FILTER_CALLBACK

#define CLIENT_NUM_EVENT_MSG            5
#define CLASS_DRIVER_ACTION_NONE        0

// action bits
#define CLASS_DRIVER_ACTION_OPEN_DEV    1
#define CLASS_DRIVER_ACTION_READ_DEV    2
#define CLASS_DRIVER_ACTION_TRANSFER    4
#define CLASS_DRIVER_ACTION_CLOSE_DEV   8


static const char *TAG = "app_usb";
static TaskHandle_t daemon_task_hdl;
static TaskHandle_t class_driver_task_hdl;
static uint8_t AmpModellerType = AMP_MODELLER_NONE;
static QueueHandle_t usb_input_queue;

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void client_event_cb(const usb_host_client_event_msg_t *event_msg, void *arg)
{
    class_driver_t *driver_obj = (class_driver_t *)arg;

    switch (event_msg->event) 
    {
        case USB_HOST_CLIENT_EVENT_NEW_DEV:
            if (driver_obj->dev_addr == 0) 
            {
                driver_obj->dev_addr = event_msg->new_dev.address;

                // Open the device next
                driver_obj->actions |= CLASS_DRIVER_ACTION_OPEN_DEV;
            }
            break;

        case USB_HOST_CLIENT_EVENT_DEV_GONE:
            if (driver_obj->dev_hdl != NULL) 
            {
                // Cancel any other actions and close the device next
                driver_obj->actions |= CLASS_DRIVER_ACTION_CLOSE_DEV;
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
void class_driver_task(void *arg)
{
    esp_err_t err;
    SemaphoreHandle_t signaling_sem = (SemaphoreHandle_t)arg;
    class_driver_t driver_obj = {0};
    uint8_t exit = 0;
    const usb_device_desc_t* dev_desc;
    usb_device_info_t dev_info;    

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

    while (!exit) 
    {
        if (driver_obj.actions == CLASS_DRIVER_ACTION_NONE)
        {
            // Call the client event handler function - no waiting
            usb_host_client_handle_events(driver_obj.client_hdl, pdMS_TO_TICKS(1));
        }
        
        // Execute pending class driver actions
        if (driver_obj.actions & CLASS_DRIVER_ACTION_OPEN_DEV) 
        {
            ESP_LOGI(TAG, "Found USB device");

            // Open the device
            usb_host_device_open(driver_obj.client_hdl, driver_obj.dev_addr, &driver_obj.dev_hdl);

            // next read the device descriptor
            driver_obj.actions &= ~CLASS_DRIVER_ACTION_OPEN_DEV;
            driver_obj.actions |= CLASS_DRIVER_ACTION_READ_DEV;
        }

        if (driver_obj.actions & CLASS_DRIVER_ACTION_READ_DEV)
        {
            // read device info
            usb_host_device_info(driver_obj.dev_hdl, &dev_info);
            
            ESP_LOGI(TAG, "\t%s speed", (dev_info.speed == USB_SPEED_LOW) ? "Low" : "Full");
            ESP_LOGI(TAG, "\tbConfigurationValue %d", dev_info.bConfigurationValue);

            // read device descriptor
            ESP_ERROR_CHECK(usb_host_get_device_descriptor(driver_obj.dev_hdl, &dev_desc));
            usb_print_device_descriptor(dev_desc);

            // dump config descriptors
            //const usb_config_desc_t* config_desc;
            //ESP_ERROR_CHECK(usb_host_get_active_config_descriptor(driver_obj.dev_hdl, &config_desc));
            //usb_print_config_descriptor(config_desc, NULL);

            // check for IK Multimedia Vendor and Product ID 
            if ((dev_desc->idVendor == IK_MULTIMEDIA_USB_VENDOR) && (dev_desc->idProduct == TONEX_ONE_PRODUCT_ID))
            {
                // found Tonex One
                ESP_LOGI(TAG, "Found Tonex One");
                AmpModellerType = AMP_MODELLER_TONEX_ONE;

                usb_tonex_one_init(&driver_obj, usb_input_queue);
            }
            else if ((dev_desc->idVendor == IK_MULTIMEDIA_USB_VENDOR) && (dev_desc->idProduct == TONEX_PRODUCT_ID))
            {
                // found Tonex 
                ESP_LOGI(TAG, "Found Tonex");
                AmpModellerType = AMP_MODELLER_TONEX;

                usb_tonex_init(&driver_obj, usb_input_queue);
            }
            else
            {
                // check the device class
                if (dev_desc->bDeviceClass == 0x09)
                {
                    ESP_LOGI(TAG, "Found USB Hub");
                }
                else
                {
                    ESP_LOGI(TAG, "Found unexpected USB device");
                }

                usb_host_device_close(driver_obj.client_hdl, driver_obj.dev_hdl);
                driver_obj.dev_hdl = NULL;
                driver_obj.dev_addr = 0;
            }

            driver_obj.actions &= ~CLASS_DRIVER_ACTION_READ_DEV;
        }
        
        if (driver_obj.actions & CLASS_DRIVER_ACTION_CLOSE_DEV) 
        {
            ESP_LOGI(TAG, "USB close device");

            // Release the interface
            if (AmpModellerType != AMP_MODELLER_NONE)
            {
                usb_host_interface_release(driver_obj.client_hdl, driver_obj.dev_hdl, 1);
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

                default:
                {
                    // nothing needed
                } break;
            }

            AmpModellerType = AMP_MODELLER_NONE;

            // close device
            usb_host_device_close(driver_obj.client_hdl, driver_obj.dev_hdl);

            driver_obj.dev_hdl = NULL;
            driver_obj.dev_addr = 0;

            // update UI
            control_set_usb_status(0);

            //exit = 1;
            driver_obj.actions &= ~CLASS_DRIVER_ACTION_CLOSE_DEV;
        }

        // handle device
        switch (AmpModellerType)
        {
            case AMP_MODELLER_TONEX_ONE:        // fallthrough
            {
                usb_tonex_one_handle(&driver_obj);
            } break;

            case AMP_MODELLER_TONEX:
            {
                usb_tonex_handle(&driver_obj);
            } break;

            default:
            {
                // nothing needed
            } break;
        }
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
