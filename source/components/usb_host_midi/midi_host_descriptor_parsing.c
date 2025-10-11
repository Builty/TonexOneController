/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <string.h>
#include "esp_check.h"
#include "esp_log.h"
#include "usb/usb_helpers.h"
#include "midi_host_descriptor_parsing.h"

#define USB_AUDIO_SUBCLASS_MIDI_STREAMING   3
static const char *TAG = "midi_parsing";

/**
 * @brief Searches interface by index and verifies its Midi-compliance
 *
 * @param[in] device_desc Pointer to Device descriptor
 * @param[in] config_desc Pointer do Configuration descriptor
 * @param[in] intf_idx    Index of the required interface
 * @return true  The required interface is Midi compliant
 * @return false The required interface is NOT Midi compliant
 */
static bool midi_parse_is_midi_compliant(const usb_device_desc_t *device_desc, const usb_config_desc_t *config_desc, uint8_t intf_idx)
{
    //if ((device_desc->bDeviceClass == USB_CLASS_PER_INTERFACE) || (device_desc->bDeviceClass == USB_CLASS_AUDIO)) 
    {
        const usb_intf_desc_t *intf_desc = usb_parse_interface_descriptor(config_desc, intf_idx, 0, NULL);

        if ((intf_desc->bInterfaceClass == USB_CLASS_AUDIO) && (intf_desc->bInterfaceSubClass == USB_AUDIO_SUBCLASS_MIDI_STREAMING)) 
        {
            ESP_LOGI(TAG, "Found Midi interface %d", intf_idx);
            return true;
        }                
    }

#if 0    
    if (((device_desc->bDeviceClass == USB_CLASS_MISC) && (device_desc->bDeviceSubClass == USB_SUBCLASS_COMMON) && (device_desc->bDeviceProtocol == USB_DEVICE_PROTOCOL_IAD)) 
       || ((device_desc->bDeviceClass == USB_CLASS_PER_INTERFACE) && (device_desc->bDeviceSubClass == USB_SUBCLASS_NULL) && (device_desc->bDeviceProtocol == USB_PROTOCOL_NULL))) 
    {
        const usb_standard_desc_t *this_desc = (const usb_standard_desc_t *)config_desc;
    
        while ((this_desc = usb_parse_next_descriptor_of_type(this_desc, config_desc->wTotalLength, USB_B_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION, &desc_offset))) 
        {
            const usb_iad_desc_t *iad_desc = (const usb_iad_desc_t *)this_desc;
            if ((iad_desc->bFirstInterface == intf_idx) && (iad_desc->bInterfaceCount == 2) && (iad_desc->bFunctionClass == USB_CLASS_AUDIO)) 
            {
                // 2. This is a composite device, that uses Interface Association Descriptor
                ESP_LOGI(TAG, "Found Midi interface %d", intf_idx);
                return true;
            }
        };
    }
#endif        

    return false;
}

/**
 * @brief Parse MIDI functional descriptors
 *
 * @attention The driver must take care of memory freeing
 * @param[in] intf_desc   Pointer to Notification interface descriptor
 * @param[in] total_len   wTotalLength of the Configuration descriptor
 * @param[in] desc_offset Offset of the intf_desc in the Configuration descriptor in bytes
 * @param[out] desc_cnt   Number of Functional descriptors found
 * @return Pointer to array of pointers to Functional descriptors
 */
static midi_func_array_t *midi_parse_functional_descriptors(const usb_intf_desc_t *intf_desc, uint16_t total_len, int desc_offset, int *desc_cnt)
{
    // MIDI specific descriptors should be right after MIDI-Communication interface descriptor
    // Note: That's why we use usb_parse_next_descriptor instead of usb_parse_next_descriptor_of_type.
    // The latter could return MIDI specific descriptors that don't belong to this interface
    int func_desc_cnt = 0;
    int intf_offset = desc_offset;
    const usb_standard_desc_t *midi_desc = (const usb_standard_desc_t *)intf_desc;
    
    while ((midi_desc = usb_parse_next_descriptor(midi_desc, total_len, &intf_offset))) 
    {
        if (midi_desc->bDescriptorType != ((USB_CLASS_AUDIO << 4) | USB_B_DESCRIPTOR_TYPE_INTERFACE )) 
        {
            if (func_desc_cnt == 0) 
            {
                return NULL; // There are no MIDI specific descriptors
            } 
            else 
            {
                break;       // We found all MIDI specific descriptors
            }
        }
        func_desc_cnt++;
    }

    // Allocate memory for the functional descriptors pointers
    midi_func_array_t *func_desc = malloc(func_desc_cnt * (sizeof(usb_standard_desc_t *)));
    if (!func_desc) 
    {
        ESP_LOGD(TAG, "Out of mem for functional descriptors");
        return NULL;
    }

    // Save the descriptors
    intf_offset = desc_offset; // Reset the offset counter
    midi_desc = (const usb_standard_desc_t *)intf_desc;
    
    for (int i = 0; i < func_desc_cnt; i++) 
    {
        midi_desc = (const usb_standard_desc_t *)usb_parse_next_descriptor(midi_desc, total_len, &intf_offset);
        (*func_desc)[i] = midi_desc;
    }
    *desc_cnt = func_desc_cnt;
    return func_desc;
}

esp_err_t midi_parse_interface_descriptor(const usb_device_desc_t *device_desc, const usb_config_desc_t *config_desc, uint8_t intf_idx, midi_parsed_info_t *info_ret)
{
    int desc_offset = 0;

    memset(info_ret, 0, sizeof(midi_parsed_info_t));
    const usb_intf_desc_t *first_intf_desc = usb_parse_interface_descriptor(config_desc, intf_idx, 0, &desc_offset);
    ESP_RETURN_ON_FALSE(
        first_intf_desc,
        ESP_ERR_NOT_FOUND, TAG, "Required interface no %d was not found.", intf_idx);

    int temp_offset = desc_offset;
    
    for (int i = 0; i < first_intf_desc->bNumEndpoints; i++) 
    {
        const usb_ep_desc_t *this_ep = usb_parse_endpoint_descriptor_by_index(first_intf_desc, i, config_desc->wTotalLength, &desc_offset);
        assert(this_ep);

        if (USB_EP_DESC_GET_XFERTYPE(this_ep) == USB_TRANSFER_TYPE_INTR) 
        {
            info_ret->notif_intf = first_intf_desc;
            info_ret->notif_ep = this_ep;
        } 
        else if (USB_EP_DESC_GET_XFERTYPE(this_ep) == USB_TRANSFER_TYPE_BULK) 
        {
            info_ret->data_intf = first_intf_desc;
            if (USB_EP_DESC_GET_EP_DIR(this_ep)) 
            {
                info_ret->in_ep = this_ep;
            } 
            else 
            {
                info_ret->out_ep = this_ep;
            }
        }
        desc_offset = temp_offset;
    }

    const bool midi_compliant = midi_parse_is_midi_compliant(device_desc, config_desc, intf_idx);
    if (midi_compliant) 
    {
        info_ret->notif_intf = first_intf_desc; // We make sure that intf_desc is set for MIDI compliant devices that use EP0 as notification element
        info_ret->func = midi_parse_functional_descriptors(first_intf_desc, config_desc->wTotalLength, desc_offset, &info_ret->func_cnt);
    }

    if (!info_ret->data_intf && midi_compliant) 
    {
        // MIDI compliant devices have data endpoints in the second interface
        // Some devices offer alternate settings for data interface:
        // First interface with 0 endpoints (default control pipe only) and second with standard 2 endpoints for full-duplex data
        // We always select interface with 2 bulk endpoints
        const int num_of_alternate = usb_parse_interface_number_of_alternate(config_desc, intf_idx + 1);
        for (int i = 0; i < num_of_alternate + 1; i++) 
        {
            const usb_intf_desc_t *second_intf_desc = usb_parse_interface_descriptor(config_desc, intf_idx + 1, i, &desc_offset);
            temp_offset = desc_offset;
        
            if (second_intf_desc && second_intf_desc->bNumEndpoints == 2) 
            {
                for (int i = 0; i < second_intf_desc->bNumEndpoints; i++) 
                {
                    const usb_ep_desc_t *this_ep = usb_parse_endpoint_descriptor_by_index(second_intf_desc, i, config_desc->wTotalLength, &desc_offset);
                    assert(this_ep);
                    if (USB_EP_DESC_GET_XFERTYPE(this_ep) == USB_TRANSFER_TYPE_BULK) 
                    {
                        info_ret->data_intf = second_intf_desc;
                        if (USB_EP_DESC_GET_EP_DIR(this_ep)) {
                            info_ret->in_ep = this_ep;
                        } 
                        else 
                        {
                            info_ret->out_ep = this_ep;
                        }
                    }
                    desc_offset = temp_offset;
                }
                break;
            }
        }
    }

    // If we did not find IN and OUT data endpoints, the device cannot be used
    return (info_ret->in_ep && info_ret->out_ep) ? ESP_OK : ESP_ERR_NOT_FOUND;
}

void midi_print_desc(const usb_standard_desc_t *_desc)
{
    // todo
}
