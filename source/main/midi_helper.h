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

#pragma once

// for boolean values:
// 0 = off, 127 = on, and 64 to toggle the current state
#define MIDI_BOOL_DISABLE   0           // matches Tonex
#define MIDI_BOOL_TOGGLE    64          // custom for this controller, not natively supported by Tonex
#define MIDI_BOOL_ENABLE    127         // matches Tonex

esp_err_t midi_helper_adjust_param_via_midi(uint8_t change_num, uint8_t midi_value);
uint16_t midi_helper_get_param_for_change_num(uint8_t change_num, uint8_t midi_value_1, uint8_t midi_value_2);
float midi_helper_scale_midi_to_float(uint16_t param_index, uint8_t midi_value);
float midi_helper_boolean_midi_to_float(uint8_t midi_value);
uint8_t midi_helper_process_incoming_data(uint8_t* data, uint8_t length, uint8_t midi_channel, uint8_t enable_CC);
