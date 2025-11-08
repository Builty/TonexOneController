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
// Data2
// Data3
static tModellerParameter ValetonParameters[VALETON_GLOBAL_LAST] = 
{
    //value, Min,                       Max,                        Name           Type                        Data1                    Data2                 Data 3
    {0,      0,                         1,                          "NG EN",       MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_NR, VALETON_BLOCK_ENABLE,  0},           //VALETON_PARAM_NR_ENABLE   
    {0,      0,                         1,                          "PRE EN",      MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_PRE, VALETON_BLOCK_ENABLE,  0},           //VALETON_PARAM_PRE_ENABLE,
    {0,      0,                         1,                          "DST EN",      MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_DST, VALETON_BLOCK_ENABLE, 0},           //VALETON_PARAM_DIST_ENABLE,
    {0,      0,                         1,                          "AMP EN",      MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_AMP, VALETON_BLOCK_ENABLE, 0},                    //VALETON_PARAM_AMP_ENABLE
    {0,      0,                         1,                          "CAB EN",      MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_CAB, VALETON_BLOCK_ENABLE, 0},                  //VALETON_PARAM_CAB_ENABLE
    {0,      0,                         1,                          "EQ EN",       MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_EQ, VALETON_BLOCK_ENABLE, 0},            //VALETON_PARAM_EQ_ENABLE,
    {0,      0,                         1,                          "MOD EN",      MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_MOD, VALETON_BLOCK_ENABLE, 0},            // VALETON_PARAM_MOD_ENABLE,
    {0,      0,                         1,                          "DLY EN",      MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_DLY, VALETON_BLOCK_ENABLE, 0},            // VALETON_PARAM_DLY_ENABLE,
    {0,      0,                         1,                          "RVB EN",      MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_RVB, VALETON_BLOCK_ENABLE, 0},            // VALETON_PARAM_RVB_ENABLE,
    {0,      0,                         1,                          "NS EN",       MODELLER_PARAM_TYPE_SWITCH, VALETON_EFFECT_BLOCK_NS, VALETON_BLOCK_ENABLE, 0},                   //VALETON_PARAM_NS_ENABLE
     
    {0,      VALETON_EFFECT_PRE_COMP,   VALETON_EFFECT_PRE_DETUNE,  "PRE TP",      MODELLER_PARAM_TYPE_SELECT, VALETON_EFFECT_BLOCK_PRE, VALETON_BLOCK_SELECTION, 0},           //VALETON_PARAM_PRE_TYPE,    
    {0,      VALETON_EFFECT_DIST_GREEN_OD,VALETON_EFFECT_DIST_BASS_OD,"DST TP",    MODELLER_PARAM_TYPE_SELECT, VALETON_EFFECT_BLOCK_DST, VALETON_BLOCK_SELECTION, 0},           //VALETON_PARAM_DIST_TYPE 
    {0,      VALETON_EFFECT_AMP_TWEEDY, VALETON_EFFECT_AMP_AC_PRE2, "AMP TP",      MODELLER_PARAM_TYPE_SELECT, VALETON_EFFECT_BLOCK_AMP, VALETON_BLOCK_SELECTION, 0},               //VALETON_PARAM_AMP_TYPE
    {0,      VALETON_EFFECT_CAB_TWD_CP, VALETON_EFFECT_CAB_AMPG_4X10,"CAB TP",     MODELLER_PARAM_TYPE_SELECT, VALETON_EFFECT_BLOCK_CAB, VALETON_BLOCK_SELECTION, 0},               //VALETON_PARAM_CAB_TYPE
    {0,      VALETON_EFFECT_EQ_GUITAR_EQ1, VALETON_EFFECT_EQ_MESS_EQ,"EQ TP",      MODELLER_PARAM_TYPE_SELECT, VALETON_EFFECT_BLOCK_EQ, VALETON_BLOCK_SELECTION, 0},            //VALETON_PARAM_EQ_TYPE
    {0,      VALETON_EFFECT_MOD_A_CHORUS, VALETON_EFFECT_MOD_BIAS_TREM,"MOD TP",   MODELLER_PARAM_TYPE_SELECT,VALETON_EFFECT_BLOCK_MOD, VALETON_BLOCK_SELECTION, 0},            //VALETON_PARAM_MOD_TYPE,
    {0,      VALETON_EFFECT_DLY_PURE,   VALETON_EFFECT_DLY_PING_PONG,"DL TP",      MODELLER_PARAM_TYPE_SELECT, VALETON_EFFECT_BLOCK_DLY, VALETON_BLOCK_SELECTION, 0},            //VALETON_PARAM_DLY_TYPE,
    {0,      VALETON_EFFECT_RVB_AIR,     VALETON_EFFECT_RVB_SWEET_SPACE, "RVB TP", MODELLER_PARAM_TYPE_SELECT, VALETON_EFFECT_BLOCK_RVB, VALETON_BLOCK_SELECTION, 0},            //VALETON_PARAM_RVB_TYPE,
    {0,      0,                         1,                          "NS TP",       MODELLER_PARAM_TYPE_SELECT, VALETON_EFFECT_BLOCK_NS, VALETON_BLOCK_SELECTION, 0},                //VALETON_PARAM_NS_TYPE
 
    {0,      0,                         100,                        "NR 0",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NR, 0, 0},            //VALETON_PARAM_NR_PARAM_0
    {0,      0,                         100,                        "NR 1",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NR, 0, 1},            //VALETON_PARAM_NR_PARAM_1
    {0,      0,                         100,                        "NR 2",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NR, 0, 2},            //VALETON_PARAM_NR_PARAM_2
    {0,      0,                         100,                        "NR 3",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NR, 0, 3},            //VALETON_PARAM_NR_PARAM_3
    {0,      0,                         100,                        "NR 4",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NR, 0, 4},            //VALETON_PARAM_NR_PARAM_4
    {0,      0,                         100,                        "NR 5",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NR, 0, 5},            //VALETON_PARAM_NR_PARAM_5
    {0,      0,                         100,                        "NR 6",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NR, 0, 6},            //VALETON_PARAM_NR_PARAM_6
    {0,      0,                         100,                        "NR 7",      MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NR, 0, 7},            //VALETON_PARAM_NR_PARAM_7
    
    {0,      0,                         100,                        "PRE 0",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, 0, 0},            //VALETON_PARAM_PRE_PARAM_0,
    {0,      0,                         100,                        "PRE 1",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, 0, 1},            //VALETON_PARAM_PRE_PARAM_1,
    {0,      0,                         100,                        "PRE 2",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, 0, 2},            //VALETON_PARAM_PRE_PARAM_2,
    {0,      0,                         100,                        "PRE 3",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, 0, 3},            //VALETON_PARAM_PRE_PARAM_3,
    {0,      0,                         100,                        "PRE 4",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, 0, 4},            //VALETON_PARAM_PRE_PARAM_4,
    {0,      0,                         100,                        "PRE 5",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, 0, 5},            //VALETON_PARAM_PRE_PARAM_5,
    {0,      0,                         100,                        "PRE 6",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, 0, 6},            //VALETON_PARAM_PRE_PARAM_6,
    {0,      0,                         100,                        "PRE 7",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_PRE, 0, 7},            //VALETON_PARAM_PRE_PARAM_7,
    
    {0,      0,                         100,                        "DST 0",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, 0, 0},            //VALETON_PARAM_DIST_PARAM_0,
    {0,      0,                         100,                        "DST 1",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, 0, 1},            //VALETON_PARAM_DIST_PARAM_1,
    {0,      0,                         100,                        "DST 2",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, 0, 2},            //VALETON_PARAM_DIST_PARAM_2,
    {0,      0,                         100,                        "DST 3",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, 0, 3},            //VALETON_PARAM_DIST_PARAM_3,
    {0,      0,                         100,                        "DST 4",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, 0, 4},            //VALETON_PARAM_DIST_PARAM_4,
    {0,      0,                         100,                        "DST 5",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, 0, 5},            //VALETON_PARAM_DIST_PARAM_5,
    {0,      0,                         100,                        "DST 6",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, 0, 6},            //VALETON_PARAM_DIST_PARAM_6,
    {0,      0,                         100,                        "DST 7",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DST, 0, 7},            //VALETON_PARAM_DIST_PARAM_7,

    {0,      0,                         100,                        "AMP 0",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_AMP, 0, 0},            //VALETON_PARAM_AMP_PARAM_0,
    {0,      0,                         100,                        "AMP 1",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_AMP, 0, 1},            //VALETON_PARAM_AMP_PARAM_1,
    {0,      0,                         100,                        "AMP 2",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_AMP, 0, 2},            //VALETON_PARAM_AMP_PARAM_2,
    {0,      0,                         100,                        "AMP 3",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_AMP, 0, 3},            //VALETON_PARAM_AMP_PARAM_3,
    {0,      0,                         100,                        "AMP 4",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_AMP, 0, 4},            //VALETON_PARAM_AMP_PARAM_4,
    {0,      0,                         100,                        "AMP 5",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_AMP, 0, 5},            //VALETON_PARAM_AMP_PARAM_5,
    {0,      0,                         100,                        "AMP 6",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_AMP, 0, 6},            //VALETON_PARAM_AMP_PARAM_6,
    {0,      0,                         100,                        "AMP 7",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_AMP, 0, 7},            //VALETON_PARAM_AMP_PARAM_7,

    {0,      0,                         100,                        "CAB 0",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_CAB, 0, 0},            //VALETON_PARAM_CAB_PARAM_0,
    {0,      0,                         100,                        "CAB 1",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_CAB, 0, 1},            //VALETON_PARAM_CAB_PARAM_1,
    {0,      0,                         100,                        "CAB 2",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_CAB, 0, 2},            //VALETON_PARAM_CAB_PARAM_2,
    {0,      0,                         100,                        "CAB 3",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_CAB, 0, 3},            //VALETON_PARAM_CAB_PARAM_3,
    {0,      0,                         100,                        "CAB 4",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_CAB, 0, 4},            //VALETON_PARAM_CAB_PARAM_4,
    {0,      0,                         100,                        "CAB 5",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_CAB, 0, 5},            //VALETON_PARAM_CAB_PARAM_5,
    {0,      0,                         100,                        "CAB 6",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_CAB, 0, 6},            //VALETON_PARAM_CAB_PARAM_6,
    {0,      0,                         100,                        "CAB 7",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_CAB, 0, 7},            //VALETON_PARAM_CAB_PARAM_7,

    {0,      0,                         100,                        "EQ 0",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, 0, 0},            //VALETON_PARAM_EQ_PARAM_0,
    {0,      0,                         100,                        "EQ 1",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, 0, 1},            //VALETON_PARAM_EQ_PARAM_1,
    {0,      0,                         100,                        "EQ 2",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, 0, 2},            //VALETON_PARAM_EQ_PARAM_2,
    {0,      0,                         100,                        "EQ 3",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, 0, 3},            //VALETON_PARAM_EQ_PARAM_3,
    {0,      0,                         100,                        "EQ 4",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, 0, 4},            //VALETON_PARAM_EQ_PARAM_4,
    {0,      0,                         100,                        "EQ 5",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, 0, 5},            //VALETON_PARAM_EQ_PARAM_5,
    {0,      0,                         100,                        "EQ 6",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, 0, 6},            //VALETON_PARAM_EQ_PARAM_6,
    {0,      0,                         100,                        "EQ 7",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_EQ, 0, 7},            //VALETON_PARAM_EQ_PARAM_7,

    {0,      0,                         100,                        "MOD 0",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, 0, 0},            //VALETON_PARAM_MOD_PARAM_0,
    {0,      0,                         100,                        "MOD 1",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, 0, 1},            //VALETON_PARAM_MOD_PARAM_1,
    {0,      0,                         100,                        "MOD 2",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, 0, 2},            //VALETON_PARAM_MOD_PARAM_2,
    {0,      0,                         100,                        "MOD 3",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, 0, 3},            //VALETON_PARAM_MOD_PARAM_3,
    {0,      0,                         100,                        "MOD 4",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, 0, 4},            //VALETON_PARAM_MOD_PARAM_4,
    {0,      0,                         100,                        "MOD 5",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, 0, 5},            //VALETON_PARAM_MOD_PARAM_5,
    {0,      0,                         100,                        "MOD 6",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, 0, 6},            //VALETON_PARAM_MOD_PARAM_6,
    {0,      0,                         100,                        "MOD 7",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_MOD, 0, 7},            //VALETON_PARAM_MOD_PARAM_7,

    {0,      0,                         100,                        "DLY 0",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, 0, 0},            //VALETON_PARAM_DLY_PARAM_0,
    {0,      0,                         100,                        "DLY 1",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, 0, 1},            //VALETON_PARAM_DLY_PARAM_1,
    {0,      0,                         100,                        "DLY 2",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, 0, 2},            //VALETON_PARAM_DLY_PARAM_2,
    {0,      0,                         100,                        "DLY 3",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, 0, 3},            //VALETON_PARAM_DLY_PARAM_3,
    {0,      0,                         100,                        "DLY 4",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, 0, 4},            //VALETON_PARAM_DLY_PARAM_4,
    {0,      0,                         100,                        "DLY 5",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, 0, 5},            //VALETON_PARAM_DLY_PARAM_5,
    {0,      0,                         100,                        "DLY 6",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, 0, 6},            //VALETON_PARAM_DLY_PARAM_6,
    {0,      0,                         100,                        "DLY 7",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_DLY, 0, 7},            //VALETON_PARAM_DLY_PARAM_7,

    {0,      0,                         100,                        "RVB 0",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, 0, 0},            //VALETON_PARAM_RVB_PARAM_0,
    {0,      0,                         100,                        "RVB 1",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, 0, 1},            //VALETON_PARAM_RVB_PARAM_1,
    {0,      0,                         100,                        "RVB 2",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, 0, 2},            //VALETON_PARAM_RVB_PARAM_2,
    {0,      0,                         100,                        "RVB 3",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, 0, 3},            //VALETON_PARAM_RVB_PARAM_3,
    {0,      0,                         100,                        "RVB 4",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, 0, 4},            //VALETON_PARAM_RVB_PARAM_4,
    {0,      0,                         100,                        "RVB 5",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, 0, 5},            //VALETON_PARAM_RVB_PARAM_5,
    {0,      0,                         100,                        "RVB 6",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, 0, 6},            //VALETON_PARAM_RVB_PARAM_6,
    {0,      0,                         100,                        "RVB 7",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_RVB, 0, 7},            //VALETON_PARAM_RVB_PARAM_7,

    {0,      0,                         100,                        "NS 0",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NS, 0, 0},            //VALETON_PARAM_NS_PARAM_0,
    {0,      0,                         100,                        "NS 1",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NS, 0, 1},            //VALETON_PARAM_NS_PARAM_1,
    {0,      0,                         100,                        "NS 2",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NS, 0, 2},            //VALETON_PARAM_NS_PARAM_2,
    {0,      0,                         100,                        "NS 3",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NS, 0, 3},            //VALETON_PARAM_NS_PARAM_3,
    {0,      0,                         100,                        "NS 4",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NS, 0, 4},            //VALETON_PARAM_NS_PARAM_4,
    {0,      0,                         100,                        "NS 5",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NS, 0, 5},            //VALETON_PARAM_NS_PARAM_5,
    {0,      0,                         100,                        "NS 6",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NS, 0, 6},            //VALETON_PARAM_NS_PARAM_6,
    {0,      0,                         100,                        "NS 7",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NS, 0, 7},            //VALETON_PARAM_NS_PARAM_7,
    {0,      0,                         100,                        "NS 8",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NS, 0, 8},            //VALETON_PARAM_NS_PARAM_8,
    {0,      0,                         100,                        "NS 9",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NS, 0, 9},            //VALETON_PARAM_NS_PARAM_9,
    {0,      0,                         100,                        "NS 10",     MODELLER_PARAM_TYPE_RANGE, VALETON_EFFECT_BLOCK_NS, 0, 10},            //VALETON_PARAM_NS_PARAM_10,

    // dummy end of params marker
    {0,      0,                         0,                          "LAST",        MODELLER_PARAM_TYPE_RANGE, 0, 0, 0},                // VALETON_PARAM_LAST,

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