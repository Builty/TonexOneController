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
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sys/param.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "main.h"
#include "midi_serial.h"
#include "control.h"
#include "task_priorities.h"
#include "midi_helper.h"

#define MIDI_SERIAL_TASK_STACK_SIZE             (3 * 1024)
#define MIDI_SERIAL_BUFFER_SIZE                 128

#define UART_PORT_NUM                           UART_NUM_1

static const char *TAG = "app_midi_serial";

// Note: based on https://github.com/vit3k/tonex_controller/blob/main/main/midi.cpp

static uint8_t midi_serial_buffer[MIDI_SERIAL_BUFFER_SIZE];
static uint8_t midi_serial_channel = 0;

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void __attribute__((unused)) midi_serial_uart_rx_purge(void)
{
    int length = 0;
    uart_get_buffered_data_len(UART_PORT_NUM, (size_t*)&length);

    if (length != 0)
    {
        // read and discard remaining data
        uart_read_bytes(UART_PORT_NUM, midi_serial_buffer, length, pdMS_TO_TICKS(1));
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void midi_serial_task(void *arg)
{
    int rx_length;

    ESP_LOGI(TAG, "Midi Serial task start");

    // init UART
    uart_config_t uart_config = {
        .baud_rate = 31250,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    int intr_alloc_flags = 0;
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, MIDI_SERIAL_BUFFER_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // enable pullup on RX line, to help if pin is floating and Midi serial is enabled
    gpio_pullup_en(UART_RX_PIN);

    // purge any rubbish read during init
    vTaskDelay(pdMS_TO_TICKS(5));
    midi_serial_uart_rx_purge();

    while (1) 
    {
        // try to read data from UART
        rx_length = uart_read_bytes(UART_PORT_NUM, midi_serial_buffer, (MIDI_SERIAL_BUFFER_SIZE - 1), pdMS_TO_TICKS(5));
        
        if (rx_length != 0)
        {
            // process data
            midi_helper_process_incoming_data(midi_serial_buffer, rx_length, midi_serial_channel, 1);

            if (UART_TX_PIN != -1)
            {
                // send it back out the TX (midi through)
                uart_write_bytes(UART_PORT_NUM, midi_serial_buffer, rx_length);
            }

            // don't hog the CPU
            vTaskDelay(pdMS_TO_TICKS(1));
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
void midi_serial_init(void)
{	
    memset((void*)midi_serial_buffer, 0, sizeof(midi_serial_buffer));

    // get the channel to use
    midi_serial_channel = control_get_config_item_int(CONFIG_ITEM_MIDI_CHANNEL);

    // adjust to zero based indexing
    if (midi_serial_channel > 0)
    {
        midi_serial_channel--;
    }

    // debug code to test data processor
    //                       header       PC            ts      CC    first       second      third         ts      PC   
    //uint8_t test_data[] = {0x80, 0x80, 0xC0, 0x00,   0x80,   0xB0, 0x22, 0x7F, 0x43, 0x00, 0x0C, 0x21,   0x80,   0xC0, 0x02};
    //                       header      PC
    //uint8_t test_data[] = {0x80, 0x80, 0xC0, 0x03};
    //                      header      CC
    //uint8_t test_data[] = {0x80, 0x80, 0xB0, 0x03, 0x00};
    //                       CC
    //uint8_t test_data[] = {0xB0, 0x03, 0x00};
    //                       PC
    //uint8_t test_data[] = {0xC0, 0x00};
    //                       PC          CC 
    //uint8_t test_data[] = {0xC0, 0x00, 0xB0, 116, 64};
    //midi_helper_process_incoming_data(test_data, sizeof(test_data), midi_serial_channel, 1);

    xTaskCreatePinnedToCore(midi_serial_task, "MIDIS", MIDI_SERIAL_TASK_STACK_SIZE, NULL, MIDI_SERIAL_TASK_PRIORITY, NULL, 1);
}
