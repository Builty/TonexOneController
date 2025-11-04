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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "usb/usb_host.h"
#include "usb/cdc_acm_host.h"
#include "driver/i2c.h"
#include "usb_comms.h"
#include "control.h"
#include "valeton_params.h"


#define PARAM_MUTEX_TIMEOUT         2000        // msec

static const char *TAG = "app_ValetonParams";


/*
** Static vars
*/
static SemaphoreHandle_t ParamMutex;
//MODELLER_PARAM_TYPE_SWITCH
// MODELLER_PARAM_TYPE_RANGE
// "value" below is just a default, is overridden by the preset on load
// Data1 = block
// Data2 = pedal
// Data3 = param
static tModellerParameter ValetonParameters[VALETON_GLOBAL_LAST] = 
{
    //value, Min,                       Max,                        Name           Type                        Data1                    Data2                 Data 3
    {0,      0,                         1,                          "NG EN",       MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_NR, VALETON_BLOCK_ENABLE,  0},           //VALETON_PARAM_NR_ENABLE   
    {0,      0,                         100,                        "NG THRESH",   MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NR, VALETON_EFFECT_NR_GATE, 0},            //VALETON_PARAM_NR_THRESHOLD   
    
    {0,      0,                         1,                          "PRE EN",      MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_PRE, VALETON_BLOCK_ENABLE,  0},           //VALETON_PARAM_PRE_ENABLE,
    {0,      VALETON_EFFECT_PRE_COMP,   VALETON_EFFECT_PRE_DETUNE,  "PRE TP",      MODELLER_PARAM_TYPE_SELECT, VALETON_EFFECT_BLOCK_PRE, VALETON_BLOCK_SELECTION, 0},           //VALETON_PARAM_PRE_TYPE,    
    {0,      0,                         100,                        "PRE COS",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_COMP, 0},            //VALETON_PARAM_PRE_COMP_SUSTAIN,
    {0,      0,                         100,                        "PRE COV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_COMP, 1},            //VALETON_PARAM_PRE_COMP_VOL,
    {0,      0,                         100,                        "PRE C4S",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_COMP4, 0},            //VALETON_PARAM_PRE_COMP4_SUSTAIN,
    {0,      0,                         100,                        "PRE C4A",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_COMP4, 1},            //VALETON_PARAM_PRE_COMP4_ATTACK,
    {0,      0,                         100,                        "PRE C4V",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_COMP4, 2},            //VALETON_PARAM_PRE_COMP4_VOL,
    {0,      0,                         100,                        "PRE C4C",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_COMP4, 3},            //VALETON_PARAM_PRE_COMP4_CLIP,
    {0,      0,                         100,                        "PRE BG",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_BOOST, 0},            //VALETON_PARAM_PRE_BOOST_GAIN,
    {0,      0,                         1,                          "PRE B3S",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_BOOST, 1},           //VALETON_PARAM_PRE_BOOST_3DB,
    {0,      0,                         1,                          "PRE BBR",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_BOOST, 2},           //VALETON_PARAM_PRE_BOOST_BRIGHT,
    {0,      0,                         100,                        "PRE MBG",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_MICRO_BOOST, 0},      //VALETON_PARAM_PRE_MICRO_BOOST_GAIN,
    {0,      0,                         100,                        "PRE BBG",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_B_BOOST, 0},            //VALETON_PARAM_PRE_B_BOOST_GAIN,
    {0,      0,                         100,                        "PRE BBV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_B_BOOST, 1},            //VALETON_PARAM_PRE_B_BOOST_VOL,
    {0,      0,                         100,                        "PRE BBB",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_B_BOOST, 2},            //VALETON_PARAM_PRE_B_BOOST_BASS,
    {0,      0,                         100,                        "PRE BBT",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_B_BOOST, 3},            //VALETON_PARAM_PRE_B_BOOST_TREBLE,
    {0,      0,                         100,                        "PRE TOS",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_TOUCHER, 0},            //VALETON_PARAM_PRE_TOUCHER_SENSE,
    {0,      0,                         100,                        "PRE TOR",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_TOUCHER, 1},            //VALETON_PARAM_PRE_TOUCHER_RANGE,
    {0,      0,                         100,                        "PRE TOQ",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_TOUCHER, 2},            //VALETON_PARAM_PRE_TOUCHER_Q,
    {0,      0,                         100,                        "PRE TOM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_TOUCHER, 3},            //VALETON_PARAM_PRE_TOUCHER_MIX,
    {0,      0,                         1,                          "PRE TOO",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_TOUCHER, 4},           //VALETON_PARAM_PRE_TOUCHER_MODE
    {0,      0,                         100,                        "PRE CRD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_CRIER, 0},            //VALETON_PARAM_PRE_CRIER_DEPTH,
    {0,      0,                         100,                        "PRE CRR",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_CRIER, 1},            //VALETON_PARAM_PRE_CRIER_RATE,
    {0,      0,                         100,                        "PRE CRV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_CRIER, 2},            //VALETON_PARAM_PRE_CRIER_VOL,
    {0,      0,                         100,                        "PRE CRL",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_CRIER, 3},            //VALETON_PARAM_PRE_CRIER_LOW,
    {0,      0,                         100,                        "PRE CRQ",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_CRIER, 4},            //VALETON_PARAM_PRE_CRIER_Q,
    {0,      0,                         100,                        "PRE OCL",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_OCTA, 0},            //VALETON_PARAM_PRE_OCTA_LOW,
    {0,      0,                         100,                        "PRE OCH",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_OCTA, 1},            //VALETON_PARAM_PRE_OCTA_HIGH,
    {0,      0,                         100,                        "PRE OCD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_OCTA, 2},            //VALETON_PARAM_PRE_OCTA_DRY,
    {0,      0,                         100,                        "PRE PIH",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_PITCH, 0},            //VALETON_PARAM_PRE_PITCH_HIGH,
    {0,      0,                         100,                        "PRE PIL",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_PITCH, 1},            //VALETON_PARAM_PRE_PITCH_LOW,
    {0,      0,                         100,                        "PRE PID",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_PITCH, 2},            //VALETON_PARAM_PRE_PITCH_DRY,
    {0,      0,                         100,                        "PRE PIHV",    MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_PITCH, 3},            //VALETON_PARAM_PRE_PITCH_H_VOL,
    {0,      0,                         100,                        "PRE PILV",    MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_PITCH, 4},            //VALETON_PARAM_PRE_PITCH_L_VOL,
    {0,      -50,                       50,                         "PRE DED",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_DETUNE, 0},            //VALETON_PARAM_PRE_DETUNE_DETUNE,
    {0,      0,                         100,                        "PRE DEW",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_DETUNE, 1},            //VALETON_PARAM_PRE_DETUNE_WET,
    {0,      0,                         100,                        "PRE DER",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, VALETON_EFFECT_PRE_DETUNE, 2},            //VALETON_PARAM_PRE_DETUNE_DRY,
    
    {0,      0,                         1,                          "DST EN",      MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_DST, VALETON_BLOCK_ENABLE, 0},           //VALETON_PARAM_DIST_ENABLE,
    {0,      VALETON_EFFECT_DIST_GREEN_OD,VALETON_EFFECT_DIST_BASS_OD,"DST TP",    MODELLER_PARAM_TYPE_SELECT, VALETON_EFFECT_BLOCK_DST, VALETON_BLOCK_SELECTION, 0},           //VALETON_PARAM_DIST_TYPE
    {0,      0,                         100,                        "DST GRG",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_GREEN_OD, 0},            //VALETON_PARAM_DIST_GREEN_OD_GAIN,
    {0,      0,                         100,                        "DST GRT",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_GREEN_OD, 1},            //VALETON_PARAM_DIST_GREEN_OD_TONE,
    {0,      0,                         100,                        "DST GRV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_GREEN_OD, 2},            //VALETON_PARAM_DIST_GREEN_OD_VOL,
    {0,      0,                         100,                        "DST YEG",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_YELLOW_OD, 0},            //VALETON_PARAM_DIST_YELLOW_OD_GAIN,
    {0,      0,                         100,                        "DST YEV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_YELLOW_OD, 1},            //VALETON_PARAM_DIST_YELLOW_OD_VOL,
    {0,      0,                         100,                        "DST SUG",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_SUPER_OD, 0},            //VALETON_PARAM_DIST_SUPER_OD_GAIN,
    {0,      0,                         100,                        "DST SUT",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_SUPER_OD, 1},            //VALETON_PARAM_DIST_SUPER_OD_TONE,
    {0,      0,                         100,                        "DST SUV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_SUPER_OD, 2},            //VALETON_PARAM_DIST_SUPER_OD_VOL,
    {0,      0,                         100,                        "DST SMG",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_SM_DIST, 0},            //VALETON_PARAM_DIST_SM_DIST_GAIN,
    {0,      0,                         100,                        "DST SMT",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_SM_DIST, 1},            //VALETON_PARAM_DIST_SM_DIST_TONE,
    {0,      0,                         100,                        "DST SMV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_SM_DIST, 2},            //VALETON_PARAM_DIST_SM_DIST_VOL,
    {0,      0,                         100,                        "DST PLG",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_PLUSTORTION, 0},            //VALETON_PARAM_DIST_PLUSTORTION_GAIN,
    {0,      0,                         100,                        "DST PLV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_PLUSTORTION, 1},            //VALETON_PARAM_DIST_PLUSTORTION_VOL,
    {0,      0,                         100,                        "DST LAG",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_LA_CHARGER, 0},            //VALETON_PARAM_DIST_LA_CHARGER_GAIN,
    {0,      0,                         100,                        "DST LAT",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_LA_CHARGER, 1},            //VALETON_PARAM_DIST_LA_CHARGER_TONE,
    {0,      0,                         100,                        "DST LAV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_LA_CHARGER, 2},            //VALETON_PARAM_DIST_LA_CHARGER_VOL,
    {0,      0,                         100,                        "DST DKG",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_DARKTALE, 0},            //VALETON_PARAM_DIST_DARKTALE_GAIN,
    {0,      0,                         100,                        "DST DKF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_DARKTALE, 1},            //VALETON_PARAM_DIST_DARKTALE_FILTER,
    {0,      0,                         100,                        "DST DKV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_DARKTALE, 2},            //VALETON_PARAM_DIST_DARKTALE_VOL,
    {0,      0,                         100,                        "DST SOF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_SORA_FUZZ, 0},            //VALETON_PARAM_DIST_SORA_FUZZ_FUZZ,
    {0,      0,                         100,                        "DST SOV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_SORA_FUZZ, 1},            //VALETON_PARAM_DIST_SORA_FUZZ_VOL,
    {0,      0,                         100,                        "DST RHF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_RED_HAZE, 0},            //VALETON_PARAM_DIST_RED_HAZE_FUZZ,
    {0,      0,                         100,                        "DST RHV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_RED_HAZE, 1},            //VALETON_PARAM_DIST_RED_HAZE_VOL,
    {0,      0,                         100,                        "DST BOG",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_BASS_OD, 0},            //VALETON_PARAM_DIST_BASS_OD_GAIN,
    {0,      0,                         100,                        "DST BOB",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_BASS_OD, 1},            //VALETON_PARAM_DIST_BASS_OD_BLEND,
    {0,      0,                         100,                        "DST BOV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_BASS_OD, 2},            //VALETON_PARAM_DIST_BASS_OD_VOL,
    {0,      0,                         100,                        "DST BOA",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_BASS_OD, 3},            //VALETON_PARAM_DIST_BASS_OD_BASS,
    {0,      0,                         100,                        "DST BOT",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, VALETON_EFFECT_DIST_BASS_OD, 4},            //VALETON_PARAM_DIST_BASS_OD_TREBLE,

    {0,      0,                         1,                          "EQ EN",       MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_EQ, VALETON_BLOCK_ENABLE, 0},            //VALETON_PARAM_EQ_ENABLE,
    {0,      VALETON_EFFECT_EQ_GUITAR_EQ1, VALETON_EFFECT_EQ_MESS_EQ,"EQ TP",      MODELLER_PARAM_TYPE_SELECT, VALETON_EFFECT_BLOCK_EQ, VALETON_BLOCK_SELECTION, 0},            //VALETON_PARAM_EQ_TYPE
    {0,      0,                         100,                        "EQ G11",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_GUITAR_EQ1, 0},            //VALETON_PARAM_EQ_GUITAR_EQ1_125HZ,
    {0,      0,                         100,                        "EQ G14",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_GUITAR_EQ1, 1},            //VALETON_PARAM_EQ_GUITAR_EQ1_400HZ,
    {0,      0,                         100,                        "EQ G18",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_GUITAR_EQ1, 2},            //VALETON_PARAM_EQ_GUITAR_EQ1_800HZ,
    {0,      0,                         100,                        "EQ G16",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_GUITAR_EQ1, 3},            //VALETON_PARAM_EQ_GUITAR_EQ1_1600HZ,
    {0,      0,                         100,                        "EQ G10",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_GUITAR_EQ1, 4},            //VALETON_PARAM_EQ_GUITAR_EQ1_4000HZ,
    {0,      0,                         100,                        "EQ G1V",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_GUITAR_EQ1, 5},            //VALETON_PARAM_EQ_GUITAR_EQ1_VOL,
    {0,      0,                         100,                        "EQ G21",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_GUITAR_EQ2, 0},            //VALETON_PARAM_EQ_GUITAR_EQ2_100HZ,
    {0,      0,                         100,                        "EQ G25",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_GUITAR_EQ2, 1},            //VALETON_PARAM_EQ_GUITAR_EQ2_500HZ,
    {0,      0,                         100,                        "EQ G20",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_GUITAR_EQ2, 2},            //VALETON_PARAM_EQ_GUITAR_EQ2_1000HZ,
    {0,      0,                         100,                        "EQ G23",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_GUITAR_EQ2, 3},            //VALETON_PARAM_EQ_GUITAR_EQ2_3000HZ,
    {0,      0,                         100,                        "EQ G26",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_GUITAR_EQ2, 4},            //VALETON_PARAM_EQ_GUITAR_EQ2_6000HZ,
    {0,      0,                         100,                        "EQ G2V",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_GUITAR_EQ2, 5},            //VALETON_PARAM_EQ_GUITAR_EQ2_VOL,
    {0,      0,                         100,                        "EQ B13",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_BASS_EQ1, 0},            //VALETON_PARAM_EQ_BASS_EQ1_33HZ,
    {0,      0,                         100,                        "EQ B11",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_BASS_EQ1, 1},            //VALETON_PARAM_EQ_BASS_EQ1_150HZ,
    {0,      0,                         100,                        "EQ B16",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_BASS_EQ1, 2},            //VALETON_PARAM_EQ_BASS_EQ1_600HZ,
    {0,      0,                         100,                        "EQ B12",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_BASS_EQ1, 3},            //VALETON_PARAM_EQ_BASS_EQ1_2000HZ,
    {0,      0,                         100,                        "EQ B18",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_BASS_EQ1, 4},            //VALETON_PARAM_EQ_BASS_EQ1_8000HZ,
    {0,      0,                         100,                        "EQ B1V",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_BASS_EQ1, 5},            //VALETON_PARAM_EQ_BASS_EQ1_VOL,
    {0,      0,                         100,                        "EQ B25",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_BASS_EQ2, 0},            //VALETON_PARAM_EQ_BASS_EQ2_50HZ,
    {0,      0,                         100,                        "EQ B21",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_BASS_EQ2, 1},            //VALETON_PARAM_EQ_BASS_EQ2_120HZ,
    {0,      0,                         100,                        "EQ B24",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_BASS_EQ2, 2},            //VALETON_PARAM_EQ_BASS_EQ2_400HZ,
    {0,      0,                         100,                        "EQ B28",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_BASS_EQ2, 3},            //VALETON_PARAM_EQ_BASS_EQ2_800HZ,
    {0,      0,                         100,                        "EQ B25",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_BASS_EQ2, 4},            //VALETON_PARAM_EQ_BASS_EQ2_4500HZ,
    {0,      0,                         100,                        "EQ B2V",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_BASS_EQ2, 5},            //VALETON_PARAM_EQ_BASS_EQ2_VOL,
    {0,      0,                         100,                        "EQ ME8",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_MESS_EQ, 0},            //VALETON_PARAM_EQ_MESS_EQ_80HZ,
    {0,      0,                         100,                        "EQ ME2",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_MESS_EQ, 1},            //VALETON_PARAM_EQ_MESS_EQ_240HZ,
    {0,      0,                         100,                        "EQ ME7",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_MESS_EQ, 2},            //VALETON_PARAM_EQ_MESS_EQ_750HZ,
    {0,      0,                         100,                        "EQ ME0",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_MESS_EQ, 3},            //VALETON_PARAM_EQ_MESS_EQ_2200HZ,
    {0,      0,                         100,                        "EQ ME6",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, VALETON_EFFECT_EQ_MESS_EQ, 4},            //VALETON_PARAM_EQ_MESS_EQ_6600HZ,

    {0,      0,                         1,                          "MOD EN",      MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_MOD, VALETON_BLOCK_ENABLE, 0},            // VALETON_PARAM_MOD_ENABLE,
    {0,      VALETON_EFFECT_MOD_A_CHORUS, VALETON_EFFECT_MOD_BIAS_TREM,"MOD TP",   MODELLER_PARAM_TYPE_SELECT,VALETON_EFFECT_BLOCK_MOD, VALETON_BLOCK_SELECTION, 0},            //VALETON_PARAM_MOD_TYPE,
    {0,      0,                         100,                        "MOD ACD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_A_CHORUS, 0},            //VALETON_PARAM_MOD_A_CHORUS_DEPTH,
    {0,      0,                         10,                         "MOD ACR",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_A_CHORUS, 1},            //VALETON_PARAM_MOD_A_CHORUS_RATE,
    {0,      0,                         100,                        "MOD ACT",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_A_CHORUS, 2},            //VALETON_PARAM_MOD_A_CHORUS_TONE,
    {0,      0,                         100,                        "MOD BCD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_B_CHORUS, 0},            //VALETON_PARAM_MOD_B_CHORUS_DEPTH,
    {0,      0,                         10,                         "MOD BCR",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_B_CHORUS, 1},            //VALETON_PARAM_MOD_B_CHORUS_RATE,
    {0,      0,                         100,                        "MOD BCV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_B_CHORUS, 2},            //VALETON_PARAM_MOD_B_CHORUS_VOL,
    {0,      0,                         100,                        "MOD JED",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_JET, 0},            //VALETON_PARAM_MOD_JET_DEPTH,
    {0,      0,                         10,                         "MOD JER",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_JET, 1},            //VALETON_PARAM_MOD_JET_RATE,
    {0,      0,                         100,                        "MOD JEP",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_JET, 2},            //VALETON_PARAM_MOD_JET_PDELAY,
    {0,      0,                         100,                        "MOD JEF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_JET, 3},            //VALETON_PARAM_MOD_JET_FBACK,
    {0,      0,                         100,                        "MOD NJD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_N_JET, 0},            //VALETON_PARAM_MOD_N_JET_DEPTH,
    {0,      0,                         10,                         "MOD NJR",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_N_JET, 1},            //VALETON_PARAM_MOD_N_JET_RATE,
    {0,      0,                         100,                        "MOD NJP",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_N_JET, 2},            //VALETON_PARAM_MOD_N_JET_PDELAY,
    {0,      0,                         100,                        "MOD NJF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_N_JET, 3},            //VALETON_PARAM_MOD_N_JET_FBACK,
    {0,      0,                         10,                         "MOD OPR",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_O_PHASE, 0},            //VALETON_PARAM_MOD_O_PHASE_RATE,
    {0,      0,                         100,                        "MOD MVD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_M_VIBE, 0},            //VALETON_PARAM_MOD_M_VIBE_DEPTH,
    {0,      0,                         10,                         "MOD MVR",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_M_VIBE, 1},            //VALETON_PARAM_MOD_M_VIBE_RATE,
    {0,      0,                         100,                        "MOD VRD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_V_ROTO, 0},            //VALETON_PARAM_MOD_V_ROTO_DEPTH,
    {0,      0,                         10,                         "MOD VRR",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_V_ROTO, 1},            //VALETON_PARAM_MOD_V_ROTO_RATE,
    {0,      0,                         100,                        "MOD VID",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_VIBRATOR, 0},            //VALETON_PARAM_MOD_VIBRATO_DEPTH,
    {0,      0,                         10,                         "MOD VIR",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_VIBRATOR, 1},            //VALETON_PARAM_MOD_VIBRATO_RATE,
    {0,      0,                         100,                        "MOD VIV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_VIBRATOR, 2},            //VALETON_PARAM_MOD_VIBRATO_VOL,
    {0,      0,                         100,                        "MOD OTD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_O_TREM, 0},            //VALETON_PARAM_MOD_O_TREM_DEPTH,
    {0,      0,                         10,                         "MOD OTR",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_O_TREM, 1},            //VALETON_PARAM_MOD_O_TREM_RATE,
    {0,      0,                         100,                        "MOD SID",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_SINE_TREM, 0},            //VALETON_PARAM_MOD_SINE_TREM_DEPTH,
    {0,      0,                         10,                         "MOD SIR",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_SINE_TREM, 1},            //VALETON_PARAM_MOD_SINE_TREM_RATE,
    {0,      0,                         100,                        "MOD SIV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_SINE_TREM, 2},            //VALETON_PARAM_MOD_SINE_TREM_VOL,
    {0,      0,                         100,                        "MOD ODD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_BIAS_TREM, 0},            //VALETON_PARAM_MOD_BIAS_TREM_DEPTH,
    {0,      0,                         10,                         "MOD ODR",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_BIAS_TREM, 1},            //VALETON_PARAM_MOD_BIAS_TREM_RATE,
    {0,      0,                         100,                        "MOD ODV",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_BIAS_TREM, 2},            //VALETON_PARAM_MOD_BIAS_TREM_VOL,
    {0,      0,                         100,                        "MOD ODB",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, VALETON_EFFECT_MOD_BIAS_TREM, 3},            //VALETON_PARAM_MOD_BIAS_TREM_BIAS,

    {0,      0,                         1,                          "DLY EN",      MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_DLY, VALETON_BLOCK_ENABLE, 0},            // VALETON_PARAM_DLY_ENABLE,
    {0,      VALETON_EFFECT_DLY_PURE,   VALETON_EFFECT_DLY_PING_PONG,"DL TP",      MODELLER_PARAM_TYPE_SELECT, VALETON_EFFECT_BLOCK_DLY, VALETON_BLOCK_SELECTION, 0},            //VALETON_PARAM_DLY_TYPE,
    {0,      0,                         100,                        "DLY PUM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_PURE, 0},            //VALETON_PARAM_DLY_PURE_MIX,
    {0,      0,                         1000,                       "DLY PUT",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_PURE, 1},            //VALETON_PARAM_DLY_PURE_TIME,
    {0,      0,                         100,                        "DLY PUF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_PURE, 2},            //VALETON_PARAM_DLY_PURE_FEEDBACK,
    {0,      0,                         1,                          "DLY PUT",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_PURE, 3},            // VALETON_PARAM_DLY_PURE_TRAIL,
    {0,      0,                         100,                        "DLY ANM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_ANALOG, 0},            //VALETON_PARAM_DLY_ANALOG_MIX,
    {0,      0,                         1000,                       "DLY ANT",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_ANALOG, 1},            //VALETON_PARAM_DLY_ANALOG_TIME,
    {0,      0,                         100,                        "DLY ANF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_ANALOG, 2},            //VALETON_PARAM_DLY_ANALOG_FEEDBACK,
    {0,      0,                         1,                          "DLY ANT",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_ANALOG, 3},            // VALETON_PARAM_DLY_ANALOG_TRAIL,
    {0,      0,                         100,                        "DLY SLM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_SLAPBACK, 0},            //VALETON_PARAM_DLY_SLAPBACK_MIX,
    {0,      0,                         1000,                       "DLY SLT",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_SLAPBACK, 1},            //VALETON_PARAM_DLY_SLAPBACK_TIME,
    {0,      0,                         100,                        "DLY SLF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_SLAPBACK, 2},            //VALETON_PARAM_DLY_SLAPBACK_FEEDBACK,
    {0,      0,                         1,                          "DLY STT",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_SLAPBACK, 3},            // VALETON_PARAM_DLY_SLAPBACK_TRAIL,
    {0,      0,                         100,                        "DLY SEM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_SWEET_ECHO, 0},            //VALETON_PARAM_DLY_SWEET_ECHO_MIX,
    {0,      0,                         1000,                       "DLY SET",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_SWEET_ECHO, 1},            //VALETON_PARAM_DLY_SWEET_ECHO_TIME,
    {0,      0,                         100,                        "DLY SEF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_SWEET_ECHO, 2},            //VALETON_PARAM_DLY_SWEET_ECHO_FEEDBACK,
    {0,      0,                         1,                          "DLY SET",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_SWEET_ECHO, 3},            // VALETON_PARAM_DLY_SWEET_ECHO_TRAIL,
    {0,      0,                         100,                        "DLY TPM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_TAPE, 0},            //VALETON_PARAM_DLY_TAPE_MIX,
    {0,      0,                         1000,                       "DLY TPT",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_TAPE, 1},            //VALETON_PARAM_DLY_TAPE_TIME,
    {0,      0,                         100,                        "DLY TPF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_TAPE, 2},            //VALETON_PARAM_DLY_TAPE_FEEDBACK,
    {0,      0,                         1,                          "DLY TPT",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_TAPE, 3},            // VALETON_PARAM_DLY_TAPE_TRAIL,
    {0,      0,                         100,                        "DLY TUM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_TUBE, 0},            //VALETON_PARAM_DLY_TUBE_MIX,
    {0,      0,                         1000,                       "DLY TUT",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_TUBE, 1},            //VALETON_PARAM_DLY_TUBE_TIME,
    {0,      0,                         100,                        "DLY TUF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_TUBE, 2},            //VALETON_PARAM_DLY_TUBE_FEEDBACK,
    {0,      0,                         1,                          "DLY TUT",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_TUBE, 3},            // VALETON_PARAM_DLY_TUBE_TRAIL,
    {0,      0,                         100,                        "DLY ECM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_REV_ECHO, 0},            //VALETON_PARAM_DLY_REV_ECHO_MIX,
    {0,      0,                         1000,                       "DLY ECT",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_REV_ECHO, 1},            //VALETON_PARAM_DLY_REV_ECHO_TIME,
    {0,      0,                         100,                        "DLY ECF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_REV_ECHO, 2},            //VALETON_PARAM_DLY_REV_ECHO_FEEDBACK,
    {0,      0,                         1,                          "DLY ECT",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_REV_ECHO, 3},            // VALETON_PARAM_DLY_REV_ECHO_TRAIL,
    {0,      0,                         100,                        "DLY REM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_RING_ECHO, 0},            //VALETON_PARAM_DLY_RING_ECHO_MIX,
    {0,      0,                         1000,                       "DLY REI",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_RING_ECHO, 1},            //VALETON_PARAM_DLY_RING_ECHO_TIME,
    {0,      0,                         100,                        "DLY REF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_RING_ECHO, 2},            //VALETON_PARAM_DLY_RING_ECHO_FEEDBACK,
    {0,      0,                         100,                        "DLY REM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_RING_ECHO, 3},            //VALETON_PARAM_DLY_RING_ECHO_R_MIX,
    {0,      0,                         100,                        "DLY REF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_RING_ECHO, 4},            //VALETON_PARAM_DLY_RING_ECHO_FREQ,
    {0,      0,                         100,                        "DLY REN",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_RING_ECHO, 5},            //VALETON_PARAM_DLY_RING_ECHO_TONE,
    {0,      0,                         1,                          "DLY RET",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_RING_ECHO, 6},            // VALETON_PARAM_DLY_RING_ECHO_TRAIL,
    {0,      0,                         100,                        "DLY SEM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_SWEEP_ECHO, 0},            //VALETON_PARAM_DLY_SWEEP_ECHO_MIX,
    {0,      0,                         1000,                       "DLY SET",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_SWEEP_ECHO, 1},            //VALETON_PARAM_DLY_SWEEP_ECHO_TIME,
    {0,      0,                         100,                        "DLY SEF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_SWEEP_ECHO, 2},            //VALETON_PARAM_DLY_SWEEP_ECHO_FBACK,
    {0,      0,                         100,                        "DLY SES",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_SWEEP_ECHO, 3},            //VALETON_PARAM_DLY_SWEEP_ECHO_S_DEPTH,
    {0,      0,                         100,                        "DLY SER",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_SWEEP_ECHO, 4},            //VALETON_PARAM_DLY_SWEEP_ECHO_S_RATE,
    {0,      0,                         1,                          "DLY SET",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_SWEEP_ECHO, 5},            // VALETON_PARAM_DLY_SWEEP_ECHO_TRAIL,
    {0,      0,                         100,                        "DLY PPM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_PING_PONG, 0},            //VALETON_PARAM_DLY_PING_PONG_MIX,
    {0,      0,                         1000,                       "DLY PPI",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_PING_PONG, 1},            //VALETON_PARAM_DLY_PING_PONG_TIME,
    {0,      0,                         100,                        "DLY PPF",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_PING_PONG, 2},            //VALETON_PARAM_DLY_PING_PONG_FEEDBACK,
    {0,      0,                         1,                          "DLY PPT",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_DLY, VALETON_EFFECT_DLY_PING_PONG, 3},            // VALETON_PARAM_DLY_PING_PONG_TRAIL,

    {0,      0,                         1,                          "RVB EN",      MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_RVB, VALETON_BLOCK_ENABLE, 0},            // VALETON_PARAM_RVB_ENABLE,
    {0,      VALETON_EFFECT_RVB_AIR,     VALETON_EFFECT_RVB_SWEET_SPACE, "RVB TP", MODELLER_PARAM_TYPE_SELECT, VALETON_EFFECT_BLOCK_RVB, VALETON_BLOCK_SELECTION, 0},            //VALETON_PARAM_RVB_TYPE,
    {0,      0,                         100,                        "RVB AIM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_AIR, 0},            //VALETON_PARAM_RVB_AIR_MIX,
    {0,      0,                         100,                        "RVB AID",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_AIR, 1},            //VALETON_PARAM_RVB_AIR_DECAY,
    {0,      0,                         100,                        "RVB AID",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_AIR, 2},            //VALETON_PARAM_RVB_AIR_DAMP,
    {0,      0,                         1,                          "RVB AIT",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_AIR, 3},          //VALETON_PARAM_RVB_AIR_TRAIL,
    {0,      0,                         100,                        "RVB RMT",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_ROOM, 0},            //VALETON_PARAM_RVB_ROOM_MIX,
    {0,      0,                         100,                        "RVB RMD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_ROOM, 1},            //VALETON_PARAM_RVB_ROOM_DECAY,
    {0,      0,                         1,                          "RVB RMT",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_ROOM, 2},          //VALETON_PARAM_RVB_ROOM_TRAIL,
    {0,      0,                         100,                        "RVB HAM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_HALL, 0},            //VALETON_PARAM_RVB_HALL_MIX,
    {0,      0,                         100,                        "RVB HAD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_HALL, 1},            //VALETON_PARAM_RVB_HALL_DECAY,
    {0,      0,                         1,                          "RVB HAT",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_HALL, 2},          //VALETON_PARAM_RVB_HALL_TRAIL,
    {0,      0,                         100,                        "RVB CHM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_CHURCH, 0},            //VALETON_PARAM_RVB_CHURCH_MIX,
    {0,      0,                         100,                        "RVB CHD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_CHURCH, 1},            //VALETON_PARAM_RVB_CHURCH_DECAY,
    {0,      0,                         1,                          "RVB CHT",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_CHURCH, 2},          //VALETON_PARAM_RVB_CHURCH_TRAIL,
    {0,      0,                         100,                        "RVB P1M",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_PLATE_L, 0},            //VALETON_PARAM_RVB_PLATE_L_MIX,
    {0,      0,                         100,                        "RVB P1D",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_PLATE_L, 1},            //VALETON_PARAM_RVB_PLATE_L_DECAY,
    {0,      0,                         1,                          "RVB P1T",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_PLATE_L,2},          //VALETON_PARAM_RVB_PLATE_L_TRAIL,
    {0,      0,                         100,                        "RVB PLM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_PLATE, 3},            //VALETON_PARAM_RVB_PLATE_MIX,
    {0,      0,                         100,                        "RVB PLD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_PLATE, 4},            //VALETON_PARAM_RVB_PLATE_DECAY,
    {0,      0,                         100,                        "RVB PLR",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_PLATE, 5},            //VALETON_PARAM_RVB_PLATE_DAMP,
    {0,      0,                         1,                          "RVB PLT",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_PLATE, 6},          //VALETON_PARAM_RVB_PLATE_TRAIL,
    {0,      0,                         100,                        "RVB SPM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_SPRING, 0},            //VALETON_PARAM_RVB_SPRING_MIX,
    {0,      0,                         100,                        "RVB SPD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_SPRING, 1},            //VALETON_PARAM_RVB_SPRING_DECAY,
    {0,      0,                         1,                          "RVB ST",      MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_SPRING, 2},          //VALETON_PARAM_RVB_SPRING_TRAIL,
    {0,      0,                         100,                        "RVB NSM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_N_STAR, 0},            //VALETON_PARAM_RVB_N_STAR_MIX,
    {0,      0,                         100,                        "RVB NSD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_N_STAR, 1},            //VALETON_PARAM_RVB_N_STAR_DECAY,
    {0,      0,                         1,                          "RVB NST",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_N_STAR, 2},          //VALETON_PARAM_RVB_N_STAR_TRAIL,
    {0,      0,                         100,                        "RVB DSM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_DEEPSEA, 0},            //VALETON_PARAM_RVB_DEEPSEA_MIX,
    {0,      0,                         100,                        "RVB DSD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_DEEPSEA, 1},            //VALETON_PARAM_RVB_DEEPSEA_DECAY,
    {0,      0,                         1,                          "RVB DST",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_DEEPSEA, 2},          //VALETON_PARAM_RVB_DEEPSEA_TRAIL,
    {0,      0,                         100,                        "RVB SPM",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_SWEET_SPACE, 0},            //VALETON_PARAM_RVB_SWEET_SPACE_MIX,
    {0,      0,                         100,                        "RVB SPD",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_SWEET_SPACE, 1},            //VALETON_PARAM_RVB_SWEET_SPACE_DECAY,
    {0,      0,                         100,                        "RVB SPA",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_SWEET_SPACE, 2},            //VALETON_PARAM_RVB_SWEET_SPACE_DAMP,
    {0,      0,                         100,                        "RVB SPO",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_SWEET_SPACE, 3},            //VALETON_PARAM_RVB_SWEET_SPACE_MOD,
    {0,      0,                         1,                          "RVB SPT",     MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_RVB, VALETON_EFFECT_RVB_SWEET_SPACE, 4},          //VALETON_PARAM_RVB_SWEET_SPACE_TRAIL,

    // dummy end of params marker
    {0,      0,                         0,                          "LAST",        MODELLER_PARAM_TYPE_RANGE, 0, 0, 0},                // TONEX_PARAM_LAST,

    //************* Global params *****************
    {80,     40,                        240,                        "BPM",         MODELLER_PARAM_TYPE_RANGE, 0, 0, 0},                 // VALETON_GLOBAL_BPM
    {0,      -15,                       15,                         "TRIM",        MODELLER_PARAM_TYPE_RANGE, 0, 0, 0},                // VALETON_GLOBAL_INPUT_TRIM
    {0,      0,                         1,                          "CABSIM",      MODELLER_PARAM_TYPE_SWITCH, 0, 0, 0},              // VALETON_GLOBAL_CABSIM_BYPASS
    {440,    415,                       465,                        "TUNEREF",     MODELLER_PARAM_TYPE_RANGE, 0, 0, 0},              // VALETON_GLOBAL_TUNING_REFERENCE
    {0,      0,                         1,                          "BYPASS",      MODELLER_PARAM_TYPE_SWITCH, 0, 0, 0},             // VALETON_GLOBAL_BYPASS
    {0,     -40,                        3,                          "MVOL",        MODELLER_PARAM_TYPE_RANGE, 0, 0, 0},                 // VALETON_GLOBAL_MASTER_VOLUME
};


/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t valeton_params_get_locked_access(tModellerParameter** param_ptr)
{
    // take mutex
    if (xSemaphoreTake(ParamMutex, pdMS_TO_TICKS(PARAM_MUTEX_TIMEOUT)) == pdTRUE)
    {		
        *param_ptr = ValetonParameters;
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "valeton_params_get_locked_access Mutex timeout!");   
    }

    return ESP_FAIL;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t valeton_params_release_locked_access(void)
{
    // release mutex
    xSemaphoreGive(ParamMutex);

    return ESP_OK;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t valeton_params_get_min_max(uint16_t param_index, float* min, float* max)
{
    if (param_index >= VALETON_GLOBAL_LAST)
    {
        // invalid
        return ESP_FAIL;
    }

    // take mutex
    if (xSemaphoreTake(ParamMutex, pdMS_TO_TICKS(PARAM_MUTEX_TIMEOUT)) == pdTRUE)
    {		
        *min = ValetonParameters[param_index].Min;
        *max = ValetonParameters[param_index].Max;

        // release mutex
        xSemaphoreGive(ParamMutex);

        return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "valeton_params_get_min_max Mutex timeout!");   
    }

    return ESP_FAIL;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
float valeton_params_clamp_value(uint16_t param_index, float value)
{
    if (param_index >= VALETON_GLOBAL_LAST)
    {
        // invalid
        return 0;
    }

    // take mutex
    if (xSemaphoreTake(ParamMutex, pdMS_TO_TICKS(PARAM_MUTEX_TIMEOUT)) == pdTRUE)
    {		
        if (value < ValetonParameters[param_index].Min)
        {
            value = ValetonParameters[param_index].Min;
        }
        else if (value > ValetonParameters[param_index].Max)
        {
            value = ValetonParameters[param_index].Max;
        }

        // release mutex
        xSemaphoreGive(ParamMutex);

        return value;
    }
    else
    {
        ESP_LOGE(TAG, "valeton_params_clamp_value Mutex timeout!");   
    }

    return 0;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t __attribute__((unused)) valeton_dump_parameters(void)
{
    // take mutex
    if (xSemaphoreTake(ParamMutex, pdMS_TO_TICKS(PARAM_MUTEX_TIMEOUT)) == pdTRUE)
    {	
        // dump all the param values and names
        for (uint32_t loop = 0; loop < VALETON_GLOBAL_LAST; loop++)
        {
            ESP_LOGI(TAG, "Param Dump: %s = %0.2f", ValetonParameters[loop].Name, ValetonParameters[loop].Value);
        }

        // release mutex
        xSemaphoreGive(ParamMutex);

         return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "valeton_dump_parameters Mutex timeout!");   
    }

    return ESP_FAIL;
}

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
esp_err_t valeton_params_init(void)
{
    // create mutex to protect interprocess issues with memory sharing
    ParamMutex = xSemaphoreCreateMutex();
    if (ParamMutex == NULL)
    {
        ESP_LOGE(TAG, "ParamMutex Mutex create failed!");
        return ESP_FAIL;
    }

    return ESP_OK;
}