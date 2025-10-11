/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/queue.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_system.h"

#include "usb/usb_host.h"
#include "usb/midi_host.h"
#include "midi_host_common.h"
#include "midi_host_descriptor_parsing.h"

static const char *TAG = "midi";

// Control transfer constants
#define MIDI_CTRL_TRANSFER_SIZE (64)   // All standard CTRL requests and responses fit in this size
#define MIDI_CTRL_TIMEOUT_MS    (5000) // Every MIDI device should be able to respond to CTRL transfer in 5 seconds

// MIDI spinlock
static portMUX_TYPE midi_lock = portMUX_INITIALIZER_UNLOCKED;
#define MIDI_ENTER_CRITICAL()   portENTER_CRITICAL(&midi_lock)
#define MIDI_EXIT_CRITICAL()    portEXIT_CRITICAL(&midi_lock)

// MIDI events
#define MIDI_TEARDOWN          BIT0
#define MIDI_TEARDOWN_COMPLETE BIT1

// MIDI driver object
typedef struct {
    usb_host_client_handle_t midi_client_hdl;        /*!< USB Host handle reused for all MIDI devices in the system */
    SemaphoreHandle_t open_close_mutex;
    EventGroupHandle_t event_group;
    midi_new_dev_callback_t new_dev_cb;
    SLIST_HEAD(list_dev, midi_dev_s) midi_devices_list;   /*!< List of open pseudo devices */
} midi_obj_t;

static midi_obj_t *p_midi_obj = NULL;

/**
 * @brief Default MIDI driver configuration
 *
 * This configuration is used when user passes NULL to config pointer during device open.
 */
static const midi_host_driver_config_t midi_driver_config_default = {
    .driver_task_stack_size = 4096,
    .driver_task_priority = 10,
    .xCoreID = 0,
    .new_dev_cb = NULL,
};

/**
 * @brief Notification received callback
 *
 * Notification (interrupt) IN transfer is submitted at the end of this function to ensure periodic poll of IN endpoint.
 *
 * @param[in] transfer Transfer that triggered the callback
 */
static void notif_xfer_cb(usb_transfer_t *transfer);

/**
 * @brief Data received callback
 *
 * Data (bulk) IN transfer is submitted at the end of this function to ensure continuous poll of IN endpoint.
 *
 * @param[in] transfer Transfer that triggered the callback
 */
static void in_xfer_cb(usb_transfer_t *transfer);

/**
 * @brief Data send callback
 *
 * Reused for bulk OUT and CTRL transfers
 *
 * @param[in] transfer Transfer that triggered the callback
 */
static void out_xfer_cb(usb_transfer_t *transfer);

/**
 * @brief USB Host Client event callback
 *
 * Handling of USB device connection/disconnection to/from root HUB.
 *
 * @param[in] event_msg Event message type
 * @param[in] arg Caller's argument (not used in this driver)
 */
static void usb_event_cb(const usb_host_client_event_msg_t *event_msg, void *arg);

/**
 * @brief Reset IN transfer
 *
 * In in_xfer_cb() we can modify IN transfer parameters, this function resets the transfer to its defaults
 *
 * @param[in] midi_dev Pointer to MIDI device
 */
static void midi_reset_in_transfer(midi_dev_t *midi_dev)
{
    assert(midi_dev->data.in_xfer);
    usb_transfer_t *transfer = midi_dev->data.in_xfer;
    uint8_t **ptr = (uint8_t **)(&(transfer->data_buffer));
    *ptr = midi_dev->data.in_data_buffer_base;
    transfer->num_bytes = transfer->data_buffer_size;
    // This is a hotfix for IDF changes, where 'transfer->data_buffer_size' does not contain actual buffer length,
    // but *allocated* buffer length, which can be larger if CONFIG_HEAP_POISONING_COMPREHENSIVE is enabled
    transfer->num_bytes -= transfer->data_buffer_size % midi_dev->data.in_mps;
}

/**
 * @brief MIDI driver handling task
 *
 * USB host client registration and deregistration is handled here.
 *
 * @param[in] arg User's argument. Handle of a task that started this task.
 */
static void midi_client_task(void *arg)
{
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    midi_obj_t *midi_obj = p_midi_obj; // Make local copy of the driver's handle
    assert(midi_obj->midi_client_hdl);

    // Start handling client's events
    while (1) {
        usb_host_client_handle_events(midi_obj->midi_client_hdl, portMAX_DELAY);
        EventBits_t events = xEventGroupGetBits(midi_obj->event_group);
        if (events & MIDI_TEARDOWN) {
            break;
        }
    }

    ESP_LOGD(TAG, "Deregistering client");
    ESP_ERROR_CHECK(usb_host_client_deregister(midi_obj->midi_client_hdl));
    xEventGroupSetBits(midi_obj->event_group, MIDI_TEARDOWN_COMPLETE);
    vTaskDelete(NULL);
}

/**
 * @brief Cancel transfer and reset endpoint
 *
 * This function will cancel ongoing transfer a reset its endpoint to ready state.
 *
 * @param[in] dev_hdl USB device handle
 * @param[in] transfer Transfer to be cancelled
 * @return esp_err_t
 */
static esp_err_t midi_reset_transfer_endpoint(usb_device_handle_t dev_hdl, usb_transfer_t *transfer)
{
    assert(dev_hdl);
    assert(transfer);

    ESP_RETURN_ON_ERROR(usb_host_endpoint_halt(dev_hdl, transfer->bEndpointAddress), TAG,);
    ESP_RETURN_ON_ERROR(usb_host_endpoint_flush(dev_hdl, transfer->bEndpointAddress), TAG,);
    usb_host_endpoint_clear(dev_hdl, transfer->bEndpointAddress);
    return ESP_OK;
}

/**
 * @brief Start MIDI device
 *
 * After this call, USB host peripheral will continuously poll IN endpoints.
 *
 * @param midi_dev
 * @param[in] event_cb  Device event callback
 * @param[in] in_cb     Data received callback
 * @param[in] user_arg  Optional user's argument, that will be passed to the callbacks
 * @return esp_err_t
 */
static esp_err_t midi_start(midi_dev_t *midi_dev, midi_host_dev_callback_t event_cb, midi_data_callback_t in_cb, void *user_arg)
{
    esp_err_t ret = ESP_OK;
    assert(midi_dev);

    MIDI_ENTER_CRITICAL();
    midi_dev->notif.cb = event_cb;
    midi_dev->data.in_cb = in_cb;
    midi_dev->cb_arg = user_arg;
    MIDI_EXIT_CRITICAL();

    // Claim data interface and start polling its IN endpoint
    ESP_GOTO_ON_ERROR(
        usb_host_interface_claim(
            p_midi_obj->midi_client_hdl,
            midi_dev->dev_hdl,
            midi_dev->data.intf_desc->bInterfaceNumber,
            midi_dev->data.intf_desc->bAlternateSetting),
        err, TAG, "Could not claim interface");
    if (midi_dev->data.in_xfer) {
        ESP_LOGD(TAG, "Submitting poll for BULK IN transfer");
        ESP_ERROR_CHECK(usb_host_transfer_submit(midi_dev->data.in_xfer));
    }

    // If notification are supported, claim its interface and start polling its IN endpoint
    if (midi_dev->notif.xfer) {
        if (midi_dev->notif.intf_desc != midi_dev->data.intf_desc) {
            ESP_GOTO_ON_ERROR(
                usb_host_interface_claim(
                    p_midi_obj->midi_client_hdl,
                    midi_dev->dev_hdl,
                    midi_dev->notif.intf_desc->bInterfaceNumber,
                    midi_dev->notif.intf_desc->bAlternateSetting),
                err, TAG, "Could not claim interface");
        }
        ESP_LOGD(TAG, "Submitting poll for INTR IN transfer");
        ESP_ERROR_CHECK(usb_host_transfer_submit(midi_dev->notif.xfer));
    }

    // Everything OK, add the device into list and return
    MIDI_ENTER_CRITICAL();
    SLIST_INSERT_HEAD(&p_midi_obj->midi_devices_list, midi_dev, list_entry);
    MIDI_EXIT_CRITICAL();
    return ret;

err:
    usb_host_interface_release(p_midi_obj->midi_client_hdl, midi_dev->dev_hdl, midi_dev->data.intf_desc->bInterfaceNumber);
    if (midi_dev->notif.xfer && (midi_dev->notif.intf_desc != midi_dev->data.intf_desc)) {
        usb_host_interface_release(p_midi_obj->midi_client_hdl, midi_dev->dev_hdl, midi_dev->notif.intf_desc->bInterfaceNumber);
    }
    return ret;
}

static void midi_transfers_free(midi_dev_t *midi_dev);
/**
 * @brief Helper function that releases resources claimed by MIDI device
 *
 * Close underlying USB device, free device driver memory
 *
 * @note All interfaces claimed by this device must be release before calling this function
 * @param midi_dev MIDI device handle to be removed
 */
static void midi_device_remove(midi_dev_t *midi_dev)
{
    assert(midi_dev);
    midi_transfers_free(midi_dev);
    free(midi_dev->midi_func_desc);
    // We don't check the error code of usb_host_device_close, as the close might fail, if someone else is still using the device (not all interfaces are released)
    usb_host_device_close(p_midi_obj->midi_client_hdl, midi_dev->dev_hdl); // Gracefully continue on error
    free(midi_dev);
}

/**
 * @brief Open USB device with requested VID/PID
 *
 * This function has two regular return paths:
 * 1. USB device with matching VID/PID is already opened by this driver: allocate new MIDI device on top of the already opened USB device.
 * 2. USB device with matching VID/PID is NOT opened by this driver yet: poll USB connected devices until it is found.
 *
 * @note This function will block for timeout_ms, if the device is not enumerated at the moment of calling this function.
 * @param[in] vid Vendor ID
 * @param[in] pid Product ID
 * @param[in] timeout_ms Connection timeout [ms]
 * @param[out] dev MIDI device
 * @return esp_err_t
 */
static esp_err_t midi_find_and_open_usb_device(uint16_t vid, uint16_t pid, int timeout_ms, midi_dev_t **dev)
{
    assert(p_midi_obj);
    assert(dev);

    *dev = calloc(1, sizeof(midi_dev_t));
    if (*dev == NULL) {
        return ESP_ERR_NO_MEM;
    }

    // First, check list of already opened MIDI devices
    ESP_LOGD(TAG, "Checking list of opened USB devices");
    midi_dev_t *midi_dev;
    SLIST_FOREACH(midi_dev, &p_midi_obj->midi_devices_list, list_entry) {
        const usb_device_desc_t *device_desc;
        ESP_ERROR_CHECK(usb_host_get_device_descriptor(midi_dev->dev_hdl, &device_desc));
        if ((vid == device_desc->idVendor || vid == MIDI_HOST_ANY_VID) &&
                (pid == device_desc->idProduct || pid == MIDI_HOST_ANY_PID)) {
            // Return path 1:
            (*dev)->dev_hdl = midi_dev->dev_hdl;
            return ESP_OK;
        }
    }

    // Second, poll connected devices until new device is connected or timeout
    TickType_t timeout_ticks = (timeout_ms == 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    TimeOut_t connection_timeout;
    vTaskSetTimeOutState(&connection_timeout);

    do {
        ESP_LOGD(TAG, "Checking list of connected USB devices");
        uint8_t dev_addr_list[10];
        int num_of_devices;
        ESP_ERROR_CHECK(usb_host_device_addr_list_fill(sizeof(dev_addr_list), dev_addr_list, &num_of_devices));

        // Go through device address list and find the one we are looking for
        for (int i = 0; i < num_of_devices; i++) {
            usb_device_handle_t current_device;
            // Open USB device
            if (usb_host_device_open(p_midi_obj->midi_client_hdl, dev_addr_list[i], &current_device) != ESP_OK) {
                continue; // In case we failed to open this device, continue with next one in the list
            }
            assert(current_device);
            const usb_device_desc_t *device_desc;
            ESP_ERROR_CHECK(usb_host_get_device_descriptor(current_device, &device_desc));
            if ((vid == device_desc->idVendor || vid == MIDI_HOST_ANY_VID) &&
                    (pid == device_desc->idProduct || pid == MIDI_HOST_ANY_PID)) {
                // Return path 2:
                (*dev)->dev_hdl = current_device;
                return ESP_OK;
            }
            usb_host_device_close(p_midi_obj->midi_client_hdl, current_device);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    } while (xTaskCheckForTimeOut(&connection_timeout, &timeout_ticks) == pdFALSE);

    // Timeout was reached, clean-up
    free(*dev);
    *dev = NULL;
    return ESP_ERR_NOT_FOUND;
}

esp_err_t midi_host_install(const midi_host_driver_config_t *driver_config)
{
    MIDI_CHECK(!p_midi_obj, ESP_ERR_INVALID_STATE);

    // Check driver configuration, use default if NULL is passed
    if (driver_config == NULL) {
        driver_config = &midi_driver_config_default;
    }

    // Allocate all we need for this driver
    esp_err_t ret;
    midi_obj_t *midi_obj = heap_caps_calloc(1, sizeof(midi_obj_t), MALLOC_CAP_DEFAULT);
    EventGroupHandle_t event_group = xEventGroupCreate();
    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    TaskHandle_t driver_task_h = NULL;
    xTaskCreatePinnedToCore(
        midi_client_task, "USB-MIDI", driver_config->driver_task_stack_size, NULL,
        driver_config->driver_task_priority, &driver_task_h, driver_config->xCoreID);

    if (midi_obj == NULL || driver_task_h == NULL || event_group == NULL || mutex == NULL) {
        ret = ESP_ERR_NO_MEM;

        ESP_LOGE(TAG, "midi_host_install error");
        goto err;
    }

    // Register USB Host client
    usb_host_client_handle_t usb_client = NULL;
    const usb_host_client_config_t client_config = {
        .is_synchronous = false,
        .max_num_event_msg = 3,
        .async.client_event_callback = usb_event_cb,
        .async.callback_arg = NULL
    };
    ESP_GOTO_ON_ERROR(usb_host_client_register(&client_config, &usb_client), err, TAG, "Failed to register USB host client");

    // Initialize MIDI driver structure
    SLIST_INIT(&(midi_obj->midi_devices_list));
    midi_obj->event_group = event_group;
    midi_obj->open_close_mutex = mutex;
    midi_obj->midi_client_hdl = usb_client;
    midi_obj->new_dev_cb = driver_config->new_dev_cb;

    // Between 1st call of this function and following section, another task might try to install this driver:
    // Make sure that there is only one instance of this driver in the system
    MIDI_ENTER_CRITICAL();
    if (p_midi_obj) {
        // Already created
        ret = ESP_ERR_INVALID_STATE;
        MIDI_EXIT_CRITICAL();
        goto client_err;
    } else {
        p_midi_obj = midi_obj;
    }
    MIDI_EXIT_CRITICAL();

    // Everything OK: Start MIDI-Driver task and return
    xTaskNotifyGive(driver_task_h);

    ESP_LOGI(TAG, "midi_host_installed OK");
    return ESP_OK;

client_err:
    usb_host_client_deregister(usb_client);
err: // Clean-up
    free(midi_obj);
    if (event_group) {
        vEventGroupDelete(event_group);
    }
    if (driver_task_h) {
        vTaskDelete(driver_task_h);
    }
    if (mutex) {
        vSemaphoreDelete(mutex);
    }
    return ret;
}

esp_err_t midi_host_uninstall()
{
    esp_err_t ret;

    MIDI_ENTER_CRITICAL();
    MIDI_CHECK_FROM_CRIT(p_midi_obj, ESP_ERR_INVALID_STATE);
    midi_obj_t *midi_obj = p_midi_obj; // Save Driver's handle to temporary handle
    MIDI_EXIT_CRITICAL();

    xSemaphoreTake(p_midi_obj->open_close_mutex, portMAX_DELAY); // Wait for all open/close calls to finish

    MIDI_ENTER_CRITICAL();
    if (SLIST_EMPTY(&p_midi_obj->midi_devices_list)) { // Check that device list is empty (all devices closed)
        p_midi_obj = NULL; // NULL static driver pointer: No open/close calls form this point
    } else {
        ret = ESP_ERR_INVALID_STATE;
        MIDI_EXIT_CRITICAL();
        goto unblock;
    }
    MIDI_EXIT_CRITICAL();

    // Signal to MIDI task to stop, unblock it and wait for its deletion
    xEventGroupSetBits(midi_obj->event_group, MIDI_TEARDOWN);
    usb_host_client_unblock(midi_obj->midi_client_hdl);
    ESP_GOTO_ON_FALSE(
        xEventGroupWaitBits(midi_obj->event_group, MIDI_TEARDOWN_COMPLETE, pdFALSE, pdFALSE, pdMS_TO_TICKS(100)),
        ESP_ERR_NOT_FINISHED, unblock, TAG,);

    // Free remaining resources and return
    vEventGroupDelete(midi_obj->event_group);
    xSemaphoreGive(midi_obj->open_close_mutex);
    vSemaphoreDelete(midi_obj->open_close_mutex);
    free(midi_obj);
    return ESP_OK;

unblock:
    xSemaphoreGive(midi_obj->open_close_mutex);
    return ret;
}

esp_err_t midi_host_register_new_dev_callback(midi_new_dev_callback_t new_dev_cb)
{
    MIDI_ENTER_CRITICAL();
    p_midi_obj->new_dev_cb = new_dev_cb;
    MIDI_EXIT_CRITICAL();
    return ESP_OK;
}

/**
 * @brief Free USB transfers used by this device
 *
 * @note There can be no transfers in flight, at the moment of calling this function.
 * @param[in] midi_dev Pointer to MIDI device
 */
static void midi_transfers_free(midi_dev_t *midi_dev)
{
    assert(midi_dev);
    if (midi_dev->notif.xfer != NULL) {
        usb_host_transfer_free(midi_dev->notif.xfer);
    }
    if (midi_dev->data.in_xfer != NULL) {
        midi_reset_in_transfer(midi_dev);
        usb_host_transfer_free(midi_dev->data.in_xfer);
    }
    if (midi_dev->data.out_xfer != NULL) {
        if (midi_dev->data.out_xfer->context != NULL) {
            vSemaphoreDelete((SemaphoreHandle_t)midi_dev->data.out_xfer->context);
        }
        if (midi_dev->data.out_mux != NULL) {
            vSemaphoreDelete(midi_dev->data.out_mux);
        }
        usb_host_transfer_free(midi_dev->data.out_xfer);
    }
    if (midi_dev->ctrl_transfer != NULL) {
        if (midi_dev->ctrl_transfer->context != NULL) {
            vSemaphoreDelete((SemaphoreHandle_t)midi_dev->ctrl_transfer->context);
        }
        if (midi_dev->ctrl_mux != NULL) {
            vSemaphoreDelete(midi_dev->ctrl_mux);
        }
        usb_host_transfer_free(midi_dev->ctrl_transfer);
    }
}

/**
 * @brief Allocate MIDI transfers
 *
 * @param[in] midi_dev       Pointer to MIDI device
 * @param[in] notif_ep_desc Pointer to notification EP descriptor
 * @param[in] in_ep_desc-   Pointer to data IN EP descriptor
 * @param[in] in_buf_len    Length of data IN buffer
 * @param[in] out_ep_desc   Pointer to data OUT EP descriptor
 * @param[in] out_buf_len   Length of data OUT buffer
 * @return
 *     - ESP_OK:            Success
 *     - ESP_ERR_NO_MEM:    Not enough memory for transfers and semaphores allocation
 *     - ESP_ERR_NOT_FOUND: IN or OUT endpoints were not found in the selected interface
 */
static esp_err_t midi_transfers_allocate(midi_dev_t *midi_dev, const usb_ep_desc_t *notif_ep_desc, const usb_ep_desc_t *in_ep_desc, size_t in_buf_len, const usb_ep_desc_t *out_ep_desc, size_t out_buf_len)
{
    assert(in_ep_desc);
    assert(out_ep_desc);
    esp_err_t ret;

    // 1. Setup notification transfer if it is supported
    if (notif_ep_desc) {
        ESP_GOTO_ON_ERROR(
            usb_host_transfer_alloc(USB_EP_DESC_GET_MPS(notif_ep_desc), 0, &midi_dev->notif.xfer),
            err, TAG,);
        midi_dev->notif.xfer->device_handle = midi_dev->dev_hdl;
        midi_dev->notif.xfer->bEndpointAddress = notif_ep_desc->bEndpointAddress;
        midi_dev->notif.xfer->callback = notif_xfer_cb;
        midi_dev->notif.xfer->context = midi_dev;
        midi_dev->notif.xfer->num_bytes = USB_EP_DESC_GET_MPS(notif_ep_desc);
    }

    // 2. Setup control transfer
    ESP_GOTO_ON_ERROR(
        usb_host_transfer_alloc(MIDI_CTRL_TRANSFER_SIZE, 0, &midi_dev->ctrl_transfer),
        err, TAG,);
    midi_dev->ctrl_transfer->timeout_ms = 1000;
    midi_dev->ctrl_transfer->bEndpointAddress = 0;
    midi_dev->ctrl_transfer->device_handle = midi_dev->dev_hdl;
    midi_dev->ctrl_transfer->callback = out_xfer_cb;
    midi_dev->ctrl_transfer->context = xSemaphoreCreateBinary();
    ESP_GOTO_ON_FALSE(midi_dev->ctrl_transfer->context, ESP_ERR_NO_MEM, err, TAG,);
    midi_dev->ctrl_mux = xSemaphoreCreateMutex();
    ESP_GOTO_ON_FALSE(midi_dev->ctrl_mux, ESP_ERR_NO_MEM, err, TAG,);

    // 3. Setup IN data transfer (if it is required (in_buf_len > 0))
    if (in_buf_len != 0) {
        ESP_GOTO_ON_ERROR(
            usb_host_transfer_alloc(in_buf_len, 0, &midi_dev->data.in_xfer),
            err, TAG,
        );
        assert(midi_dev->data.in_xfer);
        midi_dev->data.in_xfer->callback = in_xfer_cb;
        midi_dev->data.in_xfer->num_bytes = in_buf_len;
        midi_dev->data.in_xfer->bEndpointAddress = in_ep_desc->bEndpointAddress;
        midi_dev->data.in_xfer->device_handle = midi_dev->dev_hdl;
        midi_dev->data.in_xfer->context = midi_dev;
        midi_dev->data.in_mps = USB_EP_DESC_GET_MPS(in_ep_desc);
        midi_dev->data.in_data_buffer_base = midi_dev->data.in_xfer->data_buffer;
    }

    // 4. Setup OUT bulk transfer (if it is required (out_buf_len > 0))
    if (out_buf_len != 0) {
        ESP_GOTO_ON_ERROR(
            usb_host_transfer_alloc(out_buf_len, 0, &midi_dev->data.out_xfer),
            err, TAG,
        );
        assert(midi_dev->data.out_xfer);
        midi_dev->data.out_xfer->device_handle = midi_dev->dev_hdl;
        midi_dev->data.out_xfer->context = xSemaphoreCreateBinary();
        ESP_GOTO_ON_FALSE(midi_dev->data.out_xfer->context, ESP_ERR_NO_MEM, err, TAG,);
        midi_dev->data.out_mux = xSemaphoreCreateMutex();
        ESP_GOTO_ON_FALSE(midi_dev->data.out_mux, ESP_ERR_NO_MEM, err, TAG,);
        midi_dev->data.out_xfer->bEndpointAddress = out_ep_desc->bEndpointAddress;
        midi_dev->data.out_xfer->callback = out_xfer_cb;
    }
    return ESP_OK;

err:
    midi_transfers_free(midi_dev);
    return ret;
}

esp_err_t midi_host_open(uint16_t vid, uint16_t pid, uint8_t interface_idx, const midi_host_device_config_t *dev_config, midi_dev_hdl_t *midi_hdl_ret)
{
    esp_err_t ret;
    MIDI_CHECK(p_midi_obj, ESP_ERR_INVALID_STATE);
    MIDI_CHECK(dev_config, ESP_ERR_INVALID_ARG);
    MIDI_CHECK(midi_hdl_ret, ESP_ERR_INVALID_ARG);

    xSemaphoreTake(p_midi_obj->open_close_mutex, portMAX_DELAY);
    
    // Find underlying USB device
    midi_dev_t *midi_dev;
    ret =  midi_find_and_open_usb_device(vid, pid, dev_config->connection_timeout_ms, &midi_dev);
    
    if (ESP_OK != ret) 
    {
        ESP_LOGE(TAG, "Failed to open USB device");
        goto exit;
    }

    // Get Device and Configuration descriptors
    const usb_config_desc_t *config_desc;
    const usb_device_desc_t *device_desc;
    ESP_ERROR_CHECK(usb_host_get_device_descriptor(midi_dev->dev_hdl, &device_desc));
    ESP_ERROR_CHECK(usb_host_get_active_config_descriptor(midi_dev->dev_hdl, &config_desc));

    // Parse the required interface descriptor
    midi_parsed_info_t midi_info;
    ESP_GOTO_ON_ERROR(midi_parse_interface_descriptor(device_desc, config_desc, interface_idx, &midi_info), err, TAG, "Could not open required interface as MIDI");

    // Save all members of midi_dev
    midi_dev->data.intf_desc = midi_info.data_intf;
    midi_dev->notif.intf_desc = midi_info.notif_intf;
    midi_dev->midi_func_desc = midi_info.func;
    midi_dev->midi_func_desc_cnt = midi_info.func_cnt;

    // The following line is here for backward compatibility with v1.0.*
    // where fixed size of IN buffer (equal to IN Maximum Packet Size) was used
    const size_t in_buf_size = (dev_config->data_cb && (dev_config->in_buffer_size == 0)) ? USB_EP_DESC_GET_MPS(midi_info.in_ep) : dev_config->in_buffer_size;

    // Allocate USB transfers, claim MIDI interfaces and return MIDI handle
    ESP_GOTO_ON_ERROR(
        midi_transfers_allocate(midi_dev, midi_info.notif_ep, midi_info.in_ep, in_buf_size, midi_info.out_ep, dev_config->out_buffer_size),
        err, TAG,);
    ESP_GOTO_ON_ERROR(midi_start(midi_dev, dev_config->event_cb, dev_config->data_cb, dev_config->user_arg), err, TAG,);
    *midi_hdl_ret = (midi_dev_hdl_t)midi_dev;
    xSemaphoreGive(p_midi_obj->open_close_mutex);

    ESP_LOGI(TAG, "Midi device opened OK");

    return ESP_OK;

err:
    midi_device_remove(midi_dev);
exit:
    xSemaphoreGive(p_midi_obj->open_close_mutex);
    *midi_hdl_ret = NULL;
    return ret;
}

esp_err_t midi_host_close(midi_dev_hdl_t midi_hdl)
{
    MIDI_CHECK(p_midi_obj, ESP_ERR_INVALID_STATE);
    MIDI_CHECK(midi_hdl, ESP_ERR_INVALID_ARG);

    xSemaphoreTake(p_midi_obj->open_close_mutex, portMAX_DELAY);

    // Make sure that the device is in the devices list (that it is not already closed)
    midi_dev_t *midi_dev;
    bool device_found = false;
    MIDI_ENTER_CRITICAL();
    SLIST_FOREACH(midi_dev, &p_midi_obj->midi_devices_list, list_entry) {
        if (midi_dev == (midi_dev_t *)midi_hdl) {
            device_found = true;
            break;
        }
    }

    // Device was not found in the midi_devices_list; it was already closed, return OK
    if (!device_found) {
        MIDI_EXIT_CRITICAL();
        xSemaphoreGive(p_midi_obj->open_close_mutex);
        return ESP_OK;
    }

    // No user callbacks from this point
    midi_dev->notif.cb = NULL;
    midi_dev->data.in_cb = NULL;
    MIDI_EXIT_CRITICAL();

    // Cancel polling of BULK IN and INTERRUPT IN
    if (midi_dev->data.in_xfer) {
        ESP_ERROR_CHECK(midi_reset_transfer_endpoint(midi_dev->dev_hdl, midi_dev->data.in_xfer));
    }
    if (midi_dev->notif.xfer != NULL) {
        ESP_ERROR_CHECK(midi_reset_transfer_endpoint(midi_dev->dev_hdl, midi_dev->notif.xfer));
    }

    // Release all interfaces
    ESP_ERROR_CHECK(usb_host_interface_release(p_midi_obj->midi_client_hdl, midi_dev->dev_hdl, midi_dev->data.intf_desc->bInterfaceNumber));
    if ((midi_dev->notif.intf_desc != NULL) && (midi_dev->notif.intf_desc != midi_dev->data.intf_desc)) {
        ESP_ERROR_CHECK(usb_host_interface_release(p_midi_obj->midi_client_hdl, midi_dev->dev_hdl, midi_dev->notif.intf_desc->bInterfaceNumber));
    }

    MIDI_ENTER_CRITICAL();
    SLIST_REMOVE(&p_midi_obj->midi_devices_list, midi_dev, midi_dev_s, list_entry);
    MIDI_EXIT_CRITICAL();

    midi_device_remove(midi_dev);
    xSemaphoreGive(p_midi_obj->open_close_mutex);
    return ESP_OK;
}

void midi_host_desc_print(midi_dev_hdl_t midi_hdl)
{
    assert(midi_hdl);
    midi_dev_t *midi_dev = (midi_dev_t *)midi_hdl;

    const usb_device_desc_t *device_desc;
    const usb_config_desc_t *config_desc;
    ESP_ERROR_CHECK_WITHOUT_ABORT(usb_host_get_device_descriptor(midi_dev->dev_hdl, &device_desc));
    ESP_ERROR_CHECK_WITHOUT_ABORT(usb_host_get_active_config_descriptor(midi_dev->dev_hdl, &config_desc));
    usb_print_device_descriptor(device_desc);
    usb_print_config_descriptor(config_desc, midi_print_desc);
}

/**
 * @brief Check finished transfer status
 *
 * Return to on transfer completed OK.
 * Cancel the transfer and issue user's callback in case of an error.
 *
 * @param[in] transfer Transfer to be checked
 * @return true Transfer completed
 * @return false Transfer NOT completed
 */
static bool midi_is_transfer_completed(usb_transfer_t *transfer)
{
    midi_dev_t *midi_dev = (midi_dev_t *)transfer->context;
    bool completed = false;

    switch (transfer->status) {
    case USB_TRANSFER_STATUS_COMPLETED:
        completed = true;
        break;
    case USB_TRANSFER_STATUS_NO_DEVICE: // User is notified about device disconnection from usb_event_cb
    case USB_TRANSFER_STATUS_CANCELED:
        break;
    case USB_TRANSFER_STATUS_ERROR:
    case USB_TRANSFER_STATUS_TIMED_OUT:
    case USB_TRANSFER_STATUS_STALL:
    case USB_TRANSFER_STATUS_OVERFLOW:
    case USB_TRANSFER_STATUS_SKIPPED:
    default:
        // Transfer was not completed or cancelled by user. Inform user about this
        if (midi_dev->notif.cb) {
            const midi_host_dev_event_data_t error_event = {
                .type = MIDI_HOST_ERROR,
                .data.error = (int) transfer->status
            };
            midi_dev->notif.cb(&error_event, midi_dev->cb_arg);
        }
    }
    return completed;
}

static void in_xfer_cb(usb_transfer_t *transfer)
{
    ESP_LOGD(TAG, "in xfer cb");
    midi_dev_t *midi_dev = (midi_dev_t *)transfer->context;

    if (!midi_is_transfer_completed(transfer)) {
        return;
    }

    if (midi_dev->data.in_cb) {
        const bool data_processed = midi_dev->data.in_cb(transfer->data_buffer, transfer->actual_num_bytes, midi_dev->cb_arg);

        // Information for developers:
        // In order to save RAM and CPU time, the application can indicate that the received data was not processed and that the application expects more data.
        // In this case, the next received data must be appended to the existing buffer.
        // Since the data_buffer in usb_transfer_t is a constant pointer, we must cast away to const qualifier.
        if (!data_processed) {
#if !SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
            // In case the received data was not processed, the next RX data must be appended to current buffer
            uint8_t **ptr = (uint8_t **)(&(transfer->data_buffer));
            *ptr += transfer->actual_num_bytes;

            // Calculate remaining space in the buffer. Attention: pointer arithmetic!
            size_t space_left = transfer->data_buffer_size - (transfer->data_buffer - midi_dev->data.in_data_buffer_base);
            uint16_t mps = midi_dev->data.in_mps;
            transfer->num_bytes = (space_left / mps) * mps; // Round down to MPS for next transfer

            if (transfer->num_bytes == 0) {
                // The IN buffer cannot accept more data, inform the user and reset the buffer
                ESP_LOGW(TAG, "IN buffer overflow");
                midi_reset_in_transfer(midi_dev);
            }
#else
            // For targets that must sync internal memory through L1CACHE, we cannot change the data_buffer
            // because it would lead to unaligned cache sync, which is not allowed
            ESP_LOGW(TAG, "RX buffer append is not yet supported on ESP32-P4!");
#endif
        } else {
            midi_reset_in_transfer(midi_dev);
        }
    }

    ESP_LOGD(TAG, "Submitting poll for BULK IN transfer");
    usb_host_transfer_submit(midi_dev->data.in_xfer);
}

static void notif_xfer_cb(usb_transfer_t *transfer)
{
    ESP_LOGD(TAG, "notif xfer cb");
    midi_dev_t *midi_dev = (midi_dev_t *)transfer->context;

    if (midi_is_transfer_completed(transfer)) {
#if 0        
        midi_notification_t *notif = (midi_notification_t *)transfer->data_buffer;
        switch (notif->bNotificationCode) {
        case USB_MIDI_NOTIF_NETWORK_CONNECTION: {
            if (midi_dev->notif.cb) {
                const midi_host_dev_event_data_t net_conn_event = {
                    .type = MIDI_HOST_NETWORK_CONNECTION,
                    .data.network_connected = (bool) notif->wValue
                };
                midi_dev->notif.cb(&net_conn_event, midi_dev->cb_arg);
            }
            break;
        }
        case USB_MIDI_NOTIF_SERIAL_STATE: {
            midi_dev->serial_state.val = *((uint16_t *)notif->Data);
            if (midi_dev->notif.cb) {
                const midi_host_dev_event_data_t serial_state_event = {
                    .type = MIDI_HOST_SERIAL_STATE,
                    .data.serial_state = midi_dev->serial_state
                };
                midi_dev->notif.cb(&serial_state_event, midi_dev->cb_arg);
            }
            break;
        }
        case USB_MIDI_NOTIF_RESPONSE_AVAILABLE: // Encapsulated commands not implemented - fallthrough
        default:
            ESP_LOGW(TAG, "Unsupported notification type 0x%02X", notif->bNotificationCode);
            ESP_LOG_BUFFER_HEX(TAG, transfer->data_buffer, transfer->actual_num_bytes);
            break;
        }
#endif
        // Start polling for new data again
        ESP_LOGD(TAG, "Submitting poll for INTR IN transfer");
        usb_host_transfer_submit(midi_dev->notif.xfer);
    }
}

static void out_xfer_cb(usb_transfer_t *transfer)
{
    ESP_LOGD(TAG, "out/ctrl xfer cb");
    assert(transfer->context);
    xSemaphoreGive((SemaphoreHandle_t)transfer->context);
}

static void usb_event_cb(const usb_host_client_event_msg_t *event_msg, void *arg)
{
    switch (event_msg->event) {
    case USB_HOST_CLIENT_EVENT_NEW_DEV:
        // Guard p_midi_obj->new_dev_cb from concurrent access
        ESP_LOGI(TAG, "New device connected");
        MIDI_ENTER_CRITICAL();
        midi_new_dev_callback_t _new_dev_cb = p_midi_obj->new_dev_cb;
        MIDI_EXIT_CRITICAL();

        if (_new_dev_cb) {
            usb_device_handle_t new_dev;
            if (usb_host_device_open(p_midi_obj->midi_client_hdl, event_msg->new_dev.address, &new_dev) != ESP_OK) {
                break;
            }
            assert(new_dev);
            _new_dev_cb(new_dev);
            usb_host_device_close(p_midi_obj->midi_client_hdl, new_dev);
        }

        break;
    case USB_HOST_CLIENT_EVENT_DEV_GONE: {
        ESP_LOGI(TAG, "Device suddenly disconnected");
        // Find MIDI pseudo-devices associated with this USB device and close them
        midi_dev_t *midi_dev;
        midi_dev_t *tmidi_dev;
        // We are using 'SAFE' version of 'SLIST_FOREACH' which enables user to close the disconnected device in the callback
        SLIST_FOREACH_SAFE(midi_dev, &p_midi_obj->midi_devices_list, list_entry, tmidi_dev) {
            if (midi_dev->dev_hdl == event_msg->dev_gone.dev_hdl && midi_dev->notif.cb) {
                // The suddenly disconnected device was opened by this driver: inform user about this
                const midi_host_dev_event_data_t disconn_event = {
                    .type = MIDI_HOST_DEVICE_DISCONNECTED,
                    .data.midi_hdl = (midi_dev_hdl_t) midi_dev,
                };
                midi_dev->notif.cb(&disconn_event, midi_dev->cb_arg);
            }
        }
        break;
    }
    default:
        assert(false);
        break;
    }
}

esp_err_t midi_host_data_tx_blocking(midi_dev_hdl_t midi_hdl, const uint8_t *data, size_t data_len, uint32_t timeout_ms)
{
    esp_err_t ret;
    MIDI_CHECK(midi_hdl, ESP_ERR_INVALID_ARG);
    midi_dev_t *midi_dev = (midi_dev_t *)midi_hdl;
    MIDI_CHECK(data && (data_len > 0), ESP_ERR_INVALID_ARG);
    MIDI_CHECK(midi_dev->data.out_xfer, ESP_ERR_NOT_SUPPORTED); // Device was opened as read-only.
    MIDI_CHECK(data_len <= midi_dev->data.out_xfer->data_buffer_size, ESP_ERR_INVALID_SIZE);

    // Take OUT mutex and fill the OUT transfer
    BaseType_t taken = xSemaphoreTake(midi_dev->data.out_mux, pdMS_TO_TICKS(timeout_ms));
    if (taken != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    ESP_LOGI(TAG, "Submitting BULK OUT transfer");
    SemaphoreHandle_t transfer_finished_semaphore = (SemaphoreHandle_t)midi_dev->data.out_xfer->context;
    xSemaphoreTake(transfer_finished_semaphore, 0); // Make sure the semaphore is taken before we submit new transfer

    memcpy(midi_dev->data.out_xfer->data_buffer, data, data_len);
    midi_dev->data.out_xfer->num_bytes = data_len;
    midi_dev->data.out_xfer->timeout_ms = timeout_ms;
    ESP_GOTO_ON_ERROR(usb_host_transfer_submit(midi_dev->data.out_xfer), unblock, TAG,);

    // Wait for OUT transfer completion
    taken = xSemaphoreTake(transfer_finished_semaphore, pdMS_TO_TICKS(timeout_ms));
    if (!taken) {
        midi_reset_transfer_endpoint(midi_dev->dev_hdl, midi_dev->data.out_xfer); // Resetting the endpoint will cause all in-progress transfers to complete
        ESP_LOGW(TAG, "TX transfer timeout");
        ret = ESP_ERR_TIMEOUT;
        goto unblock;
    }

    ESP_GOTO_ON_FALSE(midi_dev->data.out_xfer->status == USB_TRANSFER_STATUS_COMPLETED, ESP_ERR_INVALID_RESPONSE, unblock, TAG, "Bulk OUT transfer error");
    ESP_GOTO_ON_FALSE(midi_dev->data.out_xfer->actual_num_bytes == data_len, ESP_ERR_INVALID_RESPONSE, unblock, TAG, "Incorrect number of bytes transferred");
    ret = ESP_OK;

unblock:
    xSemaphoreGive(midi_dev->data.out_mux);
    return ret;
}

esp_err_t midi_host_midi_desc_get(midi_dev_hdl_t midi_hdl, const usb_standard_desc_t **desc_out)
{
    MIDI_CHECK(midi_hdl, ESP_ERR_INVALID_ARG);
    midi_dev_t *midi_dev = (midi_dev_t *)midi_hdl;
    esp_err_t ret = ESP_ERR_NOT_FOUND;
    *desc_out = NULL;

#if 0    
    for (int i = 0; i < midi_dev->midi_func_desc_cnt; i++) {
        const midi_header_desc_t *_desc = (const midi_header_desc_t *)((*(midi_dev->midi_func_desc))[i]);

        ret = ESP_OK;
        *desc_out = (const usb_standard_desc_t *)_desc;
        break;
    }
#endif        
    return ret;
}
