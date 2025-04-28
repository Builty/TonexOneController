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


#ifndef _TONEX_PARAMS_H
#define _TONEX_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PARAM_NAME          12

typedef struct
{
    float Value;
    float Min;
    float Max;
    char Name[MAX_PARAM_NAME];
    uint8_t Type;
} tTonexParameter;

enum ParamTypes
{
    TONEX_PARAM_TYPE_SWITCH,        // on/off
    TONEX_PARAM_TYPE_SELECT,        // 0,1,2,3 etc
    TONEX_PARAM_TYPE_RANGE          // floating point range
};

enum TonexReverbModels
{
    TONEX_REVERB_SPRING_1,
    TONEX_REVERB_SPRING_2,
    TONEX_REVERB_SPRING_3,
    TONEX_REVERB_SPRING_4,
    TONEX_REVERB_ROOM,
    TONEX_REVERB_PLATE
};

enum TonexModulationModels
{
    TONEX_MODULATION_CHORUS,
    TONEX_MODULATION_TREMOLO,
    TONEX_MODULATION_PHASER,
    TONEX_MODULATION_FLANGER,
    TONEX_MODULATION_ROTARY
};

enum TonexDelayModels
{
    TONEX_DELAY_DIGITAL,
    TONEX_DELAY_TAPE
};

enum TonexCabinetTypes
{
    TONEX_CABINET_TONE_MODEL,
    TONEX_CABINET_VIR,
    TONEX_CABINET_DISABLED
};

// defined in the same order as they are sent by the Pedal
enum TonexParameters
{
    // noise gate
    TONEX_PARAM_NOISE_GATE_POST,        // Pre/Post
    TONEX_PARAM_NOISE_GATE_ENABLE,
    TONEX_PARAM_NOISE_GATE_THRESHOLD,
    TONEX_PARAM_NOISE_GATE_RELEASE,
    TONEX_PARAM_NOISE_GATE_DEPTH,

    // Compressor
    TONEX_PARAM_COMP_POST,              // Pre/Post
    TONEX_PARAM_COMP_ENABLE,
    TONEX_PARAM_COMP_THRESHOLD,
    TONEX_PARAM_COMP_MAKE_UP,
    TONEX_PARAM_COMP_ATTACK,            //9

    // EQ    
    TONEX_PARAM_EQ_POST,                // Pre/Post
    TONEX_PARAM_EQ_BASS,
    TONEX_PARAM_EQ_BASS_FREQ,
    TONEX_PARAM_EQ_MID,
    TONEX_PARAM_EQ_MIDQ,
    TONEX_PARAM_EQ_MID_FREQ,
    TONEX_PARAM_EQ_TREBLE,
    TONEX_PARAM_EQ_TREBLE_FREQ,         //17
    
    //Model and VIR params
    TONEX_PARAM_MODEL_AMP_ENABLE,
    TONEX_PARAM_MODEL_SW1,          // results in silence, unknown function
    TONEX_PARAM_MODEL_GAIN,
    TONEX_PARAM_MODEL_VOLUME,
    TONEX_PARAM_MODEX_MIX,
    TONEX_PARAM_CABINET_UNKNOWN,        // seems always set to 1 regardless of cabinet options
    TONEX_PARAM_CABINET_TYPE,           // 0 = disabled. 1 = VIR. 2 = Tone Model
    TONEX_PARAM_VIR_CABINET_MODEL,
    TONEX_PARAM_VIR_RESO,
    TONEX_PARAM_VIR_MIC_1,
    TONEX_PARAM_VIR_MIC_1_X,
    TONEX_PARAM_VIR_MIC_1_Z,                    
    TONEX_PARAM_VIR_MIC_2,
    TONEX_PARAM_VIR_MIC_2_X,
    TONEX_PARAM_VIR_MIC_2_Z,
    TONEX_PARAM_VIR_BLEND,
    TONEX_PARAM_MODEL_PRESENCE,
    TONEX_PARAM_MODEL_DEPTH,
    
    // Reverb
    TONEX_PARAM_REVERB_POSITION,
    TONEX_PARAM_REVERB_ENABLE,
    TONEX_PARAM_REVERB_MODEL,
    TONEX_PARAM_REVERB_SPRING1_TIME,
    TONEX_PARAM_REVERB_SPRING1_PREDELAY,
    TONEX_PARAM_REVERB_SPRING1_COLOR,
    TONEX_PARAM_REVERB_SPRING1_MIX,
    TONEX_PARAM_REVERB_SPRING2_TIME,
    TONEX_PARAM_REVERB_SPRING2_PREDELAY,
    TONEX_PARAM_REVERB_SPRING2_COLOR,
    TONEX_PARAM_REVERB_SPRING2_MIX,
    TONEX_PARAM_REVERB_SPRING3_TIME,
    TONEX_PARAM_REVERB_SPRING3_PREDELAY,
    TONEX_PARAM_REVERB_SPRING3_COLOR,
    TONEX_PARAM_REVERB_SPRING3_MIX,
    TONEX_PARAM_REVERB_SPRING4_TIME,
    TONEX_PARAM_REVERB_SPRING4_PREDELAY,
    TONEX_PARAM_REVERB_SPRING4_COLOR,
    TONEX_PARAM_REVERB_SPRING4_MIX,
    TONEX_PARAM_REVERB_ROOM_TIME,
    TONEX_PARAM_REVERB_ROOM_PREDELAY,
    TONEX_PARAM_REVERB_ROOM_COLOR,
    TONEX_PARAM_REVERB_ROOM_MIX,
    TONEX_PARAM_REVERB_PLATE_TIME,
    TONEX_PARAM_REVERB_PLATE_PREDELAY,
    TONEX_PARAM_REVERB_PLATE_COLOR,
    TONEX_PARAM_REVERB_PLATE_MIX,

    // Modulation
    TONEX_PARAM_MODULATION_POST,        // Pre/Post
    TONEX_PARAM_MODULATION_ENABLE,
    TONEX_PARAM_MODULATION_MODEL,
    TONEX_PARAM_MODULATION_CHORUS_SYNC,
    TONEX_PARAM_MODULATION_CHORUS_TS,
    TONEX_PARAM_MODULATION_CHORUS_RATE,
    TONEX_PARAM_MODULATION_CHORUS_DEPTH,
    TONEX_PARAM_MODULATION_CHORUS_LEVEL,
    TONEX_PARAM_MODULATION_TREMOLO_SYNC,
    TONEX_PARAM_MODULATION_TREMOLO_TS,
    TONEX_PARAM_MODULATION_TREMOLO_RATE,
    TONEX_PARAM_MODULATION_TREMOLO_SHAPE,
    TONEX_PARAM_MODULATION_TREMOLO_SPREAD,
    TONEX_PARAM_MODULATION_TREMOLO_LEVEL,
    TONEX_PARAM_MODULATION_PHASER_SYNC,
    TONEX_PARAM_MODULATION_PHASER_TS,
    TONEX_PARAM_MODULATION_PHASER_RATE,
    TONEX_PARAM_MODULATION_PHASER_DEPTH,
    TONEX_PARAM_MODULATION_PHASER_LEVEL,
    TONEX_PARAM_MODULATION_FLANGER_SYNC,
    TONEX_PARAM_MODULATION_FLANGER_TS,
    TONEX_PARAM_MODULATION_FLANGER_RATE,
    TONEX_PARAM_MODULATION_FLANGER_DEPTH,
    TONEX_PARAM_MODULATION_FLANGER_FEEDBACK,
    TONEX_PARAM_MODULATION_FLANGER_LEVEL,
    TONEX_PARAM_MODULATION_ROTARY_SYNC,
    TONEX_PARAM_MODULATION_ROTARY_TS,
    TONEX_PARAM_MODULATION_ROTARY_SPEED,
    TONEX_PARAM_MODULATION_ROTARY_RADIUS,
    TONEX_PARAM_MODULATION_ROTARY_SPREAD,
    TONEX_PARAM_MODULATION_ROTARY_LEVEL,
    
    // Delay
    TONEX_PARAM_DELAY_POST,             // Pre/Post
    TONEX_PARAM_DELAY_ENABLE,
    TONEX_PARAM_DELAY_MODEL,
    TONEX_PARAM_DELAY_DIGITAL_SYNC,
    TONEX_PARAM_DELAY_DIGITAL_TS,
    TONEX_PARAM_DELAY_DIGITAL_TIME,
    TONEX_PARAM_DELAY_DIGITAL_FEEDBACK,
    TONEX_PARAM_DELAY_DIGITAL_MODE,
    TONEX_PARAM_DELAY_DIGITAL_MIX,
    TONEX_PARAM_DELAY_TAPE_SYNC,
    TONEX_PARAM_DELAY_TAPE_TS,
    TONEX_PARAM_DELAY_TAPE_TIME,
    TONEX_PARAM_DELAY_TAPE_FEEDBACK,
    TONEX_PARAM_DELAY_TAPE_MODE,
    TONEX_PARAM_DELAY_TAPE_MIX,
    
    // must be last actual parameter
    TONEX_PARAM_LAST,

    // these are not parameters as such, but globals/special/set differently to the params above
    TONEX_GLOBAL_BPM,
    TONEX_GLOBAL_INPUT_TRIM,
    TONEX_GLOBAL_CABSIM_BYPASS,
    TONEX_GLOBAL_TEMPO_SOURCE,
    TONEX_GLOBAL_TUNING_REFERENCE,

    // must be last actual global
    TONEX_GLOBAL_LAST
};

// special cases for handling effect switches that use Midi but don't change a parameter
#define TONEX_UNKNOWN           0xFFFF

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} tTonexPresetColor;

esp_err_t tonex_params_init(void);
esp_err_t tonex_params_get_locked_access(tTonexParameter** param_ptr);
esp_err_t tonex_params_release_locked_access(void);
esp_err_t tonex_params_get_min_max(uint16_t param_index, float* min, float* max);
esp_err_t tonex_dump_parameters(void);
float tonex_params_clamp_value(uint16_t param_index, float value);

esp_err_t tonex_params_colors_get_locked_access(tTonexPresetColor** color_ptr);
esp_err_t tonex_params_colors_get_color(uint16_t preset_index, uint32_t* preset_color);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif