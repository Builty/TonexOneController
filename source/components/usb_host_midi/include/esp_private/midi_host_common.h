/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <sys/queue.h>                  // For singly linked list

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"            // For mutexes and semaphores

#include "usb/usb_host.h"               // For USB device handle and transfers

// MIDI check macros
#define MIDI_CHECK(cond, ret_val) ({                             \
    if (!(cond)) {                                                  \
        return (ret_val);                                           \
    }                                                               \
})

#define MIDI_CHECK_FROM_CRIT(cond, ret_val) ({                   \
    if (!(cond)) {                                                  \
        MIDI_EXIT_CRITICAL();                                    \
        return ret_val;                                             \
    }                                                               \
})

typedef struct midi_dev_s midi_dev_t;
struct midi_dev_s {
    usb_device_handle_t dev_hdl;          // USB device handle
    void *cb_arg;                         // Common argument for user's callbacks (data IN and Notification)
    struct {
        usb_transfer_t *out_xfer;         // OUT data transfer
        usb_transfer_t *in_xfer;          // IN data transfer
        midi_data_callback_t in_cb;       // User's callback for async (non-blocking) data IN
        uint16_t in_mps;                  // IN endpoint Maximum Packet Size
        uint8_t *in_data_buffer_base;     // Pointer to IN data buffer in usb_transfer_t
        const usb_intf_desc_t *intf_desc; // Pointer to data interface descriptor
        SemaphoreHandle_t out_mux;        // OUT mutex
    } data;

    struct {
        usb_transfer_t *xfer;             // IN notification transfer
        const usb_intf_desc_t *intf_desc; // Pointer to notification interface descriptor, can be NULL if there is no notification channel in the device
        midi_host_dev_callback_t cb;   // User's callback for device events
    } notif;                              // Structure with Notif pipe data

    usb_transfer_t *ctrl_transfer;        // CTRL (endpoint 0) transfer
    SemaphoreHandle_t ctrl_mux;           // CTRL mutex
    int midi_func_desc_cnt;                // Number of Midi Functional descriptors in following array
    const usb_standard_desc_t *(*midi_func_desc)[]; // Pointer to array of pointers to const usb_standard_desc_t
    SLIST_ENTRY(midi_dev_s) list_entry;
};
