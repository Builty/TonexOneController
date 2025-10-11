/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "usb/usb_host.h"
#include "usb/midi_host_types.h"

// Pass these to midi_host_open() to signal that you don't care about VID/PID of the opened device
#define MIDI_HOST_ANY_VID (0)
#define MIDI_HOST_ANY_PID (0)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief New USB device callback
 *
 * Provides already opened usb_dev, that will be closed after this callback returns.
 * This is useful for peeking device's descriptors, e.g. peeking VID/PID and loading proper driver.
 *
 * @attention This callback is called from USB Host context, so the MIDI device can't be opened here.
 */
typedef void (*midi_new_dev_callback_t)(usb_device_handle_t usb_dev);

/**
 * @brief Configuration structure of USB Host MIDI-ACM driver
 *
 */
typedef struct {
    size_t driver_task_stack_size;         /**< Stack size of the driver's task */
    unsigned driver_task_priority;         /**< Priority of the driver's task */
    int  xCoreID;                          /**< Core affinity of the driver's task */
    midi_new_dev_callback_t new_dev_cb; /**< New USB device connected callback. Can be NULL. */
} midi_host_driver_config_t;

/**
 * @brief Install MIDI driver
 *
 * - USB Host Library must already be installed before calling this function (via usb_host_install())
 * - This function should be called before calling any other MIDI driver functions
 *
 * @param[in] driver_config Driver configuration structure. If set to NULL, a default configuration will be used.
 * @return
 *   - ESP_OK: Success
 *   - ESP_ERR_INVALID_STATE: The MIDI driver is already installed or USB host library is not installed
 *   - ESP_ERR_NO_MEM: Not enough memory for installing the driver
 */
esp_err_t midi_host_install(const midi_host_driver_config_t *driver_config);

/**
 * @brief Uninstall MIDI driver
 *
 * - Users must ensure that all MIDI devices must be closed via midi_host_close() before calling this function
 *
 * @return
 *   - ESP_OK: Success
 *   - ESP_ERR_INVALID_STATE: The MIDI driver is not installed or not all MIDI devices are closed
 *   - ESP_ERR_NOT_FINISHED: The MIDI driver failed to uninstall completely
 */
esp_err_t midi_host_uninstall(void);

/**
 * @brief Register new USB device callback
 *
 * The callback will be called for every new USB device, not just MIDI-ACM class.
 *
 * @param[in] new_dev_cb New device callback function
 * @return
 *   - ESP_OK: Success
 */
esp_err_t midi_host_register_new_dev_callback(midi_new_dev_callback_t new_dev_cb);

/**
 * @brief Open MIDI device
 *
 * The driver first looks for MIDI compliant descriptor, if it is not found the driver checks if the interface has 2 Bulk endpoints that can be used for data
 *
 * Use MIDI_HOST_ANY_* macros to signal that you don't care about the device's VID and PID. In this case, first USB device will be opened.
 * It is recommended to use this feature if only one device can ever be in the system (there is no USB HUB connected).
 *
 * @param[in] vid           Device's Vendor ID, set to MIDI_HOST_ANY_VID for any
 * @param[in] pid           Device's Product ID, set to MIDI_HOST_ANY_PID for any
 * @param[in] interface_idx Index of device's interface used for MIDI-ACM communication
 * @param[in] dev_config    Configuration structure of the device
 * @param[out] midi_hdl_ret  MIDI device handle
 * @return
 *   - ESP_OK: Success
 *   - ESP_ERR_INVALID_STATE: The MIDI driver is not installed
 *   - ESP_ERR_INVALID_ARG: dev_config or midi_hdl_ret is NULL
 *   - ESP_ERR_NO_MEM: Not enough memory for opening the device
 *   - ESP_ERR_NOT_FOUND: USB device with specified VID/PID is not connected or does not have specified interface
 */
esp_err_t midi_host_open(uint16_t vid, uint16_t pid, uint8_t interface_idx, const midi_host_device_config_t *dev_config, midi_dev_hdl_t *midi_hdl_ret);

/**
 * @brief Close MIDI device and release its resources
 *
 * @note All in-flight transfers will be prematurely canceled.
 * @param[in] midi_hdl MIDI handle obtained from midi_host_open()
 * @return
 *   - ESP_OK: Success - device closed
 *   - ESP_ERR_INVALID_STATE: midi_hdl is NULL or the MIDI driver is not installed
 */
esp_err_t midi_host_close(midi_dev_hdl_t midi_hdl);

/**
 * @brief Transmit data - blocking mode
 *
 * @param midi_hdl MIDI handle obtained from midi_host_open()
 * @param[in] data       Data to be sent
 * @param[in] data_len   Data length
 * @param[in] timeout_ms Timeout in [ms]
 * @return esp_err_t
 */
esp_err_t midi_host_data_tx_blocking(midi_dev_hdl_t midi_hdl, const uint8_t *data, size_t data_len, uint32_t timeout_ms);

/**
 * @brief Print device's descriptors
 *
 * Device and full Configuration descriptors are printed in human readable format to stdout.
 *
 * @param midi_hdl MIDI handle obtained from midi_host_open()
 */
void midi_host_desc_print(midi_dev_hdl_t midi_hdl);

/**
 * @brief Get MIDI functional descriptor
 *
 * @param midi_hdl       MIDI handle obtained from midi_host_open()
 * @param[in] desc_type Type of functional descriptor
 * @param[out] desc_out Pointer to the required descriptor
 * @return
 *   - ESP_OK: Success
 *   - ESP_ERR_INVALID_ARG: Invalid device or descriptor type
 *   - ESP_ERR_NOT_FOUND: The required descriptor is not present in the device
 */
esp_err_t midi_host_midi_desc_get(midi_dev_hdl_t midi_hdl, const usb_standard_desc_t **desc_out);

#ifdef __cplusplus
}
#endif
