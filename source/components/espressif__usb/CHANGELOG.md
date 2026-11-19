# Changelog for USB Host Library

All notable changes to this component will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.6.1] - 2026-11-13

### Added

- Added public API for resetting host channel data toggle. This is used together with ClearFeature(ENDPOINT_HALT) request from class drivers during device's EP recovery.

### Fixed

- Fixed a device object and HCD channel leak when the enumeration filter rejects a device connected through an external hub (https://github.com/espressif/esp-usb/pull/590).
- Fixed possible OOB read during string descriptor print
- Fixed an assertion in `_buffer_parse()` when a channel error interrupt arrives with no buffer executing, seen on device disconnect on ESP32-P4
- Fixed an assertion in `device_release()` when an external Hub is disconnected while one of its control transfers is in flight

## [1.6.0] - 2026-10-05

### Added

- Added DWC2 internal DMA access to the external RAM for ESP32-S31. The option already exists for the ESP32-P4, extending it for the ESP32-S31. The option is configurable via Kconfig.
- Added `usb_host_config_t.fsls_only` so High-Speed USB-OTG peripherals can operate as Full/Low-Speed only hosts. Set this field when calling `usb_host_install()`.
- Added check for maximum transfer size, per transfer type, when enqueuing an URB

### Fixed

- Fixed cache sync for IN data buffer for transfer lengths nearing to the transfer limits
- Fixed a race that could trigger an assertion when closing a device immediately after its control transfer completion callback.

## [1.5.0] - 2026-06-16

### Changed

- This component now requires ESP-IDF version >= 5.5.3

### Added

- Automatic root-port suspend before light sleep when `esp_light_sleep_start()` is called (https://github.com/espressif/esp-usb/pull/445), configurable in menuconfig via `CONFIG_ESP_SLEEP_EVENT_CALLBACKS`
- Added `USB_HOST_CLIENT_EVENT_DEV_REMOVED` and the `notify_dev_removed` client flag so clients can monitor removal of devices they have not opened.

## [1.4.1] - 2026-05-29

### Fixed

- Fixed race between root port disconnect handling and device recycle path that could abort with `ESP_ERR_NOT_FOUND` from `dev_tree_node_dev_gone()` (https://github.com/espressif/esp-idf/issues/18366)
- List also suspended devices in `usb_host_device_addr_list_fill()` function

## [1.4.0] - 2026-04-02

### Added

- Add ESP32-S31 to supported targets

### Fixed

- Fixed possible out-of-bounds read in `usb_parse_next_descriptor()` in case of malicious Configuration descriptor

## [1.3.0] - 2026-03-04

### Added

- USB Dual Host support on ESP32-P4

### Fixed

- Various security fixes

## [1.2.0] - 2026-02-05

### Added

- Device initiated remote wakeup (https://github.com/espressif/esp-usb/pull/298)

### Fixed

- Fixed deadlock that prevented closing devices from high priority tasks (https://github.com/espressif/esp-idf/issues/17707)

## [1.1.0] - 2025-12-04

### Added

- Global suspend/resume (https://github.com/espressif/esp-usb/pull/275)

### Fixed

- Hardware FIFO size biasing Kconfig now shows allocation formulas instead of chip-specific examples

## [1.0.1] - 2025-10-16

### Fixed

- Compatibility with esp-idf 5.5 and 5.4 releases (https://github.com/espressif/esp-usb/pull/273)
- Printing of High Speed periodic endpoint descriptor now also shows additional transactions in microframe (https://github.com/espressif/esp-usb/pull/282)

## [1.0.0] - 2025-09-18

- First release: taken from usb component present in esp-idf at commit [`166269f`](https://github.com/espressif/esp-idf/commit/166269fb9338607aa9726ecc4ea2d1763de31f0e)
