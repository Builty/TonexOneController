/*
 Copyright (C) 2025  Greg Smith

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

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

#include "blemidi.h"

#define FOOTSWITCH_SAMPLE_COUNT             5       // 20 msec per sample
#define BANK_MODE_BUTTONS                   4
#define MAX_PRESETS                         20      // suit Tonex One
#define BANK_MAXIMUM                        (MAX_PRESETS / BANK_MODE_BUTTONS)

#define FOOTSWITCH_1		GPIO_NUM_4
#define FOOTSWITCH_2		GPIO_NUM_6
#define FOOTSWITCH_3		GPIO_NUM_2
#define FOOTSWITCH_4		GPIO_NUM_1 

enum FootswitchStates
{
    FOOTSWITCH_IDLE,
    FOOTSWITCH_WAIT_RELEASE_1,
    FOOTSWITCH_WAIT_RELEASE_2
};

static const char *TAG = "minimidi_ctrl";

typedef struct
{
    uint8_t state;
    uint32_t sample_counter;
    uint8_t index;
    uint8_t current_bank;
    uint8_t index_pending;
} tFootswitchControl;

static tFootswitchControl FootswitchControl;

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void send_program_change(uint8_t index)
{
    uint8_t message[2];

    // send Program change
    message[0] = 0xC0;
    message[1] = index;

    blemidi_send_message(0, message, sizeof(message));
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static uint8_t read_footswitch_input(uint8_t number, uint8_t* switch_state)
{
    *switch_state = gpio_get_level(number);
    return true;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void footswitch_handle_dual_mode(void)
{
    uint8_t value;   

    switch (FootswitchControl.state)
    {
        case FOOTSWITCH_IDLE:
        default:
        {
            // read footswitches
            if (read_footswitch_input(FOOTSWITCH_1, &value)) 
            {
                if (value == 0)
                {
                    ESP_LOGI(TAG, "Footswitch 1 pressed");

                    // foot switch 1 pressed
                    if (FootswitchControl.index > 0)
                    {
                        FootswitchControl.index--;
                        send_program_change(FootswitchControl.index);
                    }

                    // wait release	
                    FootswitchControl.sample_counter = 0;
                    FootswitchControl.state = FOOTSWITCH_WAIT_RELEASE_1;
                }
            }

            if (FootswitchControl.state == FOOTSWITCH_IDLE)
            {
                if (read_footswitch_input(FOOTSWITCH_2, &value))
                {
                    if (value == 0)
                    {
                        ESP_LOGI(TAG, "Footswitch 2 pressed");

                        // foot switch 2 pressed, send event
                        if (FootswitchControl.index < 20)
                        {
                            FootswitchControl.index++;
                            send_program_change(FootswitchControl.index);
                        }

                        // wait release	
                        FootswitchControl.sample_counter = 0;
                        FootswitchControl.state = FOOTSWITCH_WAIT_RELEASE_2;
                    }
                }
            }
        } break;

        case FOOTSWITCH_WAIT_RELEASE_1:
        {
            // read footswitch 1
            if (read_footswitch_input(FOOTSWITCH_1, &value))
            {
                if (value != 0)
                {
                    FootswitchControl.sample_counter++;
                    if (FootswitchControl.sample_counter == FOOTSWITCH_SAMPLE_COUNT)
                    {
                        // foot switch released
                        FootswitchControl.state = FOOTSWITCH_IDLE;		
                    }
                }
                else
                {
                    // reset counter
                    FootswitchControl.sample_counter = 0;
                }
            }
        } break;

        case FOOTSWITCH_WAIT_RELEASE_2:
        {
            // read footswitch 2
            if (read_footswitch_input(FOOTSWITCH_2, &value))
            {
                if (value != 0)
                {
                    FootswitchControl.sample_counter++;
                    if (FootswitchControl.sample_counter == FOOTSWITCH_SAMPLE_COUNT)
                    {
                        // foot switch released
                        FootswitchControl.state = FOOTSWITCH_IDLE;
                    }                 
                }
                else
                {
                    // reset counter
                    FootswitchControl.sample_counter = 0;
                }
            }
        } break;
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void footswitch_handle_quad_banked(void)
{
    uint8_t value;
    uint8_t binary_val = 0;    

    // read all 4 switches (and swap so 1 is pressed)
    read_footswitch_input(FOOTSWITCH_1, &value);
    if (value == 0)
    {
        binary_val |= 1;
    }

    read_footswitch_input(FOOTSWITCH_2, &value);
    if (value == 0)
    {
        binary_val |= 2;
    }

    read_footswitch_input(FOOTSWITCH_3, &value);
    if (value == 0)
    {
        binary_val |= 4;
    }

    read_footswitch_input(FOOTSWITCH_4, &value);
    if (value == 0)
    {
        binary_val |= 8;
    }
    
    // handle state
    switch (FootswitchControl.state)
    {
        case FOOTSWITCH_IDLE:
        {
            // any buttons pressed?
            if (binary_val != 0)
            {
                // check if A+B is pressed
                if (binary_val == 0x03)
                {
                    if (FootswitchControl.current_bank > 0)
                    {
                        // bank down
                        FootswitchControl.current_bank--;   
                        ESP_LOGI(TAG, "Footswitch banked down %d", FootswitchControl.current_bank);
                    }

                    FootswitchControl.state = FOOTSWITCH_WAIT_RELEASE_1;
                }
                // check if C+D is pressed
                else if (binary_val == 0x0C)
                {
                    if (FootswitchControl.current_bank < BANK_MAXIMUM)
                    {
                        // bank up
                        FootswitchControl.current_bank++;
                        ESP_LOGI(TAG, "Footswitch banked up %d", FootswitchControl.current_bank);
                    }

                    FootswitchControl.state = FOOTSWITCH_WAIT_RELEASE_1;
                }
                else
                {
                    // single button pressed, just store it. Preset select only happens on button release
                    FootswitchControl.index_pending = binary_val;
                }
            }
            else
            {
                if (FootswitchControl.index_pending != 0)
                {
                    uint8_t new_preset = FootswitchControl.current_bank * BANK_MODE_BUTTONS;

                    // get the index from the bit set
                    if ((FootswitchControl.index_pending & 0x01) != 0)
                    {
                        // nothing needed
                    }
                    else if ((FootswitchControl.index_pending & 0x02) != 0)
                    {
                        new_preset += 1;
                    }
                    else if ((FootswitchControl.index_pending & 0x04) != 0)
                    {
                        new_preset += 2;
                    }
                    else if ((FootswitchControl.index_pending & 0x08) != 0)
                    {
                        new_preset += 3;
                    }
                    
                    // set the change command
                    send_program_change(new_preset);
                    FootswitchControl.index_pending = 0;

                    // give a little debounce time
                    vTaskDelay(pdMS_TO_TICKS(20));
                }
            }
        } break;

        case FOOTSWITCH_WAIT_RELEASE_1:
        {
            // check if all buttons released
            if (binary_val == 0)
            {
                FootswitchControl.state = FOOTSWITCH_IDLE;
                FootswitchControl.index_pending = 0;

                // give a little debounce time
                vTaskDelay(pdMS_TO_TICKS(20));
            }
        } break;
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
static void task_midi(void *pvParameters)
{
    while (1) 
    {
        blemidi_tick();

        // select one mode TODO via web config
        //footswitch_handle_dual_mode();
        footswitch_handle_quad_banked();
        
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void callback_midi_message_received(uint8_t blemidi_port, uint16_t timestamp, uint8_t midi_status, uint8_t *remaining_message, size_t len, size_t continued_sysex_pos)
{
    // ignore
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void app_main()
{
    // init GPIO
    gpio_config_t gpio_config_struct;

    gpio_config_struct.pin_bit_mask = (((uint64_t)1 << FOOTSWITCH_1) | ((uint64_t)1 << FOOTSWITCH_2) | ((uint64_t)1 << FOOTSWITCH_3) | ((uint64_t)1 << FOOTSWITCH_4));
    gpio_config_struct.mode = GPIO_MODE_INPUT;
    gpio_config_struct.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config_struct.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&gpio_config_struct);

    memset((void*)&FootswitchControl, 0, sizeof(FootswitchControl));
    FootswitchControl.state = FOOTSWITCH_IDLE;

    // install BLE MIDI service
    int status = blemidi_init(callback_midi_message_received);
  
    if (status < 0) 
    {
        ESP_LOGE(TAG, "BLE MIDI Driver returned status=%d", status);
    } 
    else 
    {
        ESP_LOGI(TAG, "BLE MIDI Driver initialized successfully");
        xTaskCreate(task_midi, "task_midi", 4096, NULL, 8, NULL);
    }

    esp_log_level_set(TAG, ESP_LOG_WARN);
}
