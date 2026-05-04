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

#ifndef _LEDS_H
#define _LEDS_H

#ifdef __cplusplus
extern "C" {
#endif

void leds_init(void);
void leds_handle(void);

// threadsafe public API
void leds_set_colour(uint16_t led_flags, tLedColour* colour);

// weak function can be overwriiten in platform file
uint8_t leds_get_physical_index_for_virtual_index(uint8_t virtual_index);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif