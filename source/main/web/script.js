const AMP_MODELLER_NONE = 0;
const AMP_MODELLER_TONEX_ONE = 1;
const AMP_MODELLER_TONEX = 2;
const AMP_MODELLER_VALETON_GP5 = 3;

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
var pageTimer = 0;
var SocketConnected = 0;   
var maxPresets = 20;
var startPreset = 1;
var modellerType = AMP_MODELLER_NONE;
var isSyncDone = 0;
var preset_order_cfg;
var preset_color_cfg;

let userIsDragging = null; 
let dragTimeout = null; 

var curent_model_0 = 0xFF;
var curent_model_1 = 0xFF;
var curent_model_2 = 0xFF;
var curent_model_3 = 0xFF;
var curent_model_4 = 0xFF;
var curent_model_5 = 0xFF;
var curent_model_6 = 0xFF;
var curent_model_7 = 0xFF;
var curent_model_8 = 0xFF;
var curent_model_9 = 0xFF;

var effect_icon_order = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];

// Tonex param models
const TONEX_REVERB_SPRING_1 = 0;
const TONEX_REVERB_SPRING_2 = 1;
const TONEX_REVERB_SPRING_3 = 2;
const TONEX_REVERB_SPRING_4 = 3;
const TONEX_REVERB_ROOM = 4;
const TONEX_REVERB_PLATE = 5;

const TONEX_MODULATION_CHORUS = 0;
const TONEX_MODULATION_TREMOLO = 1;
const TONEX_MODULATION_PHASER = 2;
const TONEX_MODULATION_FLANGER = 3;
const TONEX_MODULATION_ROTARY = 4;

const TONEX_DELAY_DIGITAL = 0;
const TONEX_DELAY_TAPE = 1;


// Tonex params
// noise gate
const TONEX_PARAM_NOISE_GATE_POST = 0;
const TONEX_PARAM_NOISE_GATE_ENABLE = 1;
const TONEX_PARAM_NOISE_GATE_THRESHOLD = 2;
const TONEX_PARAM_NOISE_GATE_RELEASE = 3;
const TONEX_PARAM_NOISE_GATE_DEPTH = 4;

// Compressor
const TONEX_PARAM_COMP_POST = 5;
const TONEX_PARAM_COMP_ENABLE = 6;
const TONEX_PARAM_COMP_THRESHOLD = 7;
const TONEX_PARAM_COMP_MAKE_UP = 8;
const TONEX_PARAM_COMP_ATTACK = 9;

// EQ    
const TONEX_PARAM_EQ_POST = 10;     
const TONEX_PARAM_EQ_BASS = 11;
const TONEX_PARAM_EQ_BASS_FREQ = 12;
const TONEX_PARAM_EQ_MID = 13;
const TONEX_PARAM_EQ_MIDQ = 14;
const TONEX_PARAM_EQ_MID_FREQ = 15;
const TONEX_PARAM_EQ_TREBLE = 16;
const TONEX_PARAM_EQ_TREBLE_FREQ = 17;

//Model and VIR params
const TONEX_PARAM_MODEL_AMP_ENABLE = 18;  
const TONEX_PARAM_MODEL_SW1 = 19;
const TONEX_PARAM_MODEL_GAIN = 20;
const TONEX_PARAM_MODEL_VOLUME = 21;
const TONEX_PARAM_MODEX_MIX = 22;
const TONEX_PARAM_CABINET_UNKNOWN = 23;
const TONEX_PARAM_CABINET_TYPE = 24;
const TONEX_PARAM_VIR_CABINET_MODEL = 25;
const TONEX_PARAM_VIR_RESO = 26;
const TONEX_PARAM_VIR_MIC_1 = 27;
const TONEX_PARAM_VIR_MIC_1_X = 28;
const TONEX_PARAM_VIR_MIC_1_Z = 29;
const TONEX_PARAM_VIR_MIC_2 = 30;
const TONEX_PARAM_VIR_MIC_2_X = 31;
const TONEX_PARAM_VIR_MIC_2_Z = 32;
const TONEX_PARAM_VIR_BLEND = 33;
const TONEX_PARAM_MODEL_PRESENCE = 34;
const TONEX_PARAM_MODEL_DEPTH = 35;

// Reverb
const TONEX_PARAM_REVERB_POSITION = 36;
const TONEX_PARAM_REVERB_ENABLE = 37;
const TONEX_PARAM_REVERB_MODEL = 38;
const TONEX_PARAM_REVERB_SPRING1_TIME = 39
const TONEX_PARAM_REVERB_SPRING1_PREDELAY = 40;
const TONEX_PARAM_REVERB_SPRING1_COLOR = 41;
const TONEX_PARAM_REVERB_SPRING1_MIX = 42;
const TONEX_PARAM_REVERB_SPRING2_TIME = 43;
const TONEX_PARAM_REVERB_SPRING2_PREDELAY = 44;
const TONEX_PARAM_REVERB_SPRING2_COLOR = 45;
const TONEX_PARAM_REVERB_SPRING2_MIX = 46;
const TONEX_PARAM_REVERB_SPRING3_TIME = 47;
const TONEX_PARAM_REVERB_SPRING3_PREDELAY = 48;
const TONEX_PARAM_REVERB_SPRING3_COLOR = 49;
const TONEX_PARAM_REVERB_SPRING3_MIX = 50;
const TONEX_PARAM_REVERB_SPRING4_TIME = 51;
const TONEX_PARAM_REVERB_SPRING4_PREDELAY = 52;
const TONEX_PARAM_REVERB_SPRING4_COLOR = 53;
const TONEX_PARAM_REVERB_SPRING4_MIX = 54;
const TONEX_PARAM_REVERB_ROOM_TIME = 55;
const TONEX_PARAM_REVERB_ROOM_PREDELAY = 56;
const TONEX_PARAM_REVERB_ROOM_COLOR = 57;
const TONEX_PARAM_REVERB_ROOM_MIX = 58;
const TONEX_PARAM_REVERB_PLATE_TIME = 59;
const TONEX_PARAM_REVERB_PLATE_PREDELAY = 60;
const TONEX_PARAM_REVERB_PLATE_COLOR = 61;
const TONEX_PARAM_REVERB_PLATE_MIX = 62;

// Modulation
const TONEX_PARAM_MODULATION_POST = 63;
const TONEX_PARAM_MODULATION_ENABLE = 64;
const TONEX_PARAM_MODULATION_MODEL = 65;
const TONEX_PARAM_MODULATION_CHORUS_SYNC = 66;
const TONEX_PARAM_MODULATION_CHORUS_TS = 67;
const TONEX_PARAM_MODULATION_CHORUS_RATE = 68;
const TONEX_PARAM_MODULATION_CHORUS_DEPTH = 69;
const TONEX_PARAM_MODULATION_CHORUS_LEVEL = 70;
const TONEX_PARAM_MODULATION_TREMOLO_SYNC = 71;
const TONEX_PARAM_MODULATION_TREMOLO_TS = 72;
const TONEX_PARAM_MODULATION_TREMOLO_RATE = 73;
const TONEX_PARAM_MODULATION_TREMOLO_SHAPE = 74;
const TONEX_PARAM_MODULATION_TREMOLO_SPREAD = 75;
const TONEX_PARAM_MODULATION_TREMOLO_LEVEL = 76;
const TONEX_PARAM_MODULATION_PHASER_SYNC = 77;
const TONEX_PARAM_MODULATION_PHASER_TS = 78;
const TONEX_PARAM_MODULATION_PHASER_RATE = 79;
const TONEX_PARAM_MODULATION_PHASER_DEPTH = 80;
const TONEX_PARAM_MODULATION_PHASER_LEVEL = 81;
const TONEX_PARAM_MODULATION_FLANGER_SYNC = 82;
const TONEX_PARAM_MODULATION_FLANGER_TS = 83;
const TONEX_PARAM_MODULATION_FLANGER_RATE = 84;
const TONEX_PARAM_MODULATION_FLANGER_DEPTH = 85;
const TONEX_PARAM_MODULATION_FLANGER_FEEDBACK = 86;
const TONEX_PARAM_MODULATION_FLANGER_LEVEL = 87;
const TONEX_PARAM_MODULATION_ROTARY_SYNC = 88;
const TONEX_PARAM_MODULATION_ROTARY_TS = 89;
const TONEX_PARAM_MODULATION_ROTARY_SPEED = 90;
const TONEX_PARAM_MODULATION_ROTARY_RADIUS = 91;
const TONEX_PARAM_MODULATION_ROTARY_SPREAD = 92;
const TONEX_PARAM_MODULATION_ROTARY_LEVEL = 93;

// Delay
const TONEX_PARAM_DELAY_POST = 94;
const TONEX_PARAM_DELAY_ENABLE = 95;
const TONEX_PARAM_DELAY_MODEL = 96;
const TONEX_PARAM_DELAY_DIGITAL_SYNC = 97;
const TONEX_PARAM_DELAY_DIGITAL_TS = 98;
const TONEX_PARAM_DELAY_DIGITAL_TIME = 99;
const TONEX_PARAM_DELAY_DIGITAL_FEEDBACK = 100;
const TONEX_PARAM_DELAY_DIGITAL_MODE = 101;
const TONEX_PARAM_DELAY_DIGITAL_MIX = 102;
const TONEX_PARAM_DELAY_TAPE_SYNC = 103;
const TONEX_PARAM_DELAY_TAPE_TS = 104;
const TONEX_PARAM_DELAY_TAPE_TIME = 105;
const TONEX_PARAM_DELAY_TAPE_FEEDBACK = 106;
const TONEX_PARAM_DELAY_TAPE_MODE = 107;
const TONEX_PARAM_DELAY_TAPE_MIX = 108;        
const TONEX_PARAM_LAST = 109;           // last actual parameter

// globals 
const TONEX_GLOBAL_BPM = 110;                 
const TONEX_GLOBAL_INPUT_TRIM = 111;        
const TONEX_GLOBAL_CABSIM_BYPASS = 112;
const TONEX_GLOBAL_TEMPO_SOURCE = 113;
const TONEX_GLOBAL_TUNING_REFERENCE = 114;
const TONEX_GLOBAL_BYPASS = 115;
const TONEX_GLOBAL_MASTER_VOLUME = 116;

// valeton params
const ValetonParameters = {
    // block enables
    VALETON_PARAM_NR_ENABLE: 0,
    VALETON_PARAM_PRE_ENABLE: 1,
    VALETON_PARAM_DIST_ENABLE: 2,
    VALETON_PARAM_AMP_ENABLE: 3,
    VALETON_PARAM_CAB_ENABLE: 4,
    VALETON_PARAM_EQ_ENABLE: 5,
    VALETON_PARAM_MOD_ENABLE: 6,
    VALETON_PARAM_DLY_ENABLE: 7,
    VALETON_PARAM_RVB_ENABLE: 8,
    VALETON_PARAM_NS_ENABLE: 9,

    // models
    VALETON_PARAM_NR_TYPE: 10,
    VALETON_PARAM_PRE_TYPE: 11,
    VALETON_PARAM_DIST_TYPE: 12,
    VALETON_PARAM_AMP_TYPE: 13,
    VALETON_PARAM_CAB_TYPE: 14,
    VALETON_PARAM_EQ_TYPE: 15,
    VALETON_PARAM_MOD_TYPE: 16,
    VALETON_PARAM_DLY_TYPE: 17,
    VALETON_PARAM_RVB_TYPE: 18,
    VALETON_PARAM_NS_TYPE: 19,

    // general stuff
    VALETON_PARAM_PATCH_VOLUME: 20,
    VALETON_PARAM_EFFECT_SLOT_0: 21,
    VALETON_PARAM_EFFECT_SLOT_1: 22,
    VALETON_PARAM_EFFECT_SLOT_2: 23,
    VALETON_PARAM_EFFECT_SLOT_3: 24,
    VALETON_PARAM_EFFECT_SLOT_4: 25,
    VALETON_PARAM_EFFECT_SLOT_5: 26,
    VALETON_PARAM_EFFECT_SLOT_6: 27,
    VALETON_PARAM_EFFECT_SLOT_7: 28,
    VALETON_PARAM_EFFECT_SLOT_8: 29,
    VALETON_PARAM_EFFECT_SLOT_9: 30,

    // params
    //**** NR **** 
    VALETON_PARAM_NR_PARAM_0: 31,
    VALETON_PARAM_NR_PARAM_1: 32,
    VALETON_PARAM_NR_PARAM_2: 33,
    VALETON_PARAM_NR_PARAM_3: 34,
    VALETON_PARAM_NR_PARAM_4: 35,
    VALETON_PARAM_NR_PARAM_5: 36,
    VALETON_PARAM_NR_PARAM_6: 37,
    VALETON_PARAM_NR_PARAM_7: 38,

    //**** Pre ****
    VALETON_PARAM_PRE_PARAM_0: 39,
    VALETON_PARAM_PRE_PARAM_1: 40,
    VALETON_PARAM_PRE_PARAM_2: 41,
    VALETON_PARAM_PRE_PARAM_3: 42,
    VALETON_PARAM_PRE_PARAM_4: 43,
    VALETON_PARAM_PRE_PARAM_5: 44,
    VALETON_PARAM_PRE_PARAM_6: 45,
    VALETON_PARAM_PRE_PARAM_7: 46,
    
    //**** Dist ****
    VALETON_PARAM_DIST_PARAM_0: 47,
    VALETON_PARAM_DIST_PARAM_1: 48,
    VALETON_PARAM_DIST_PARAM_2: 49,
    VALETON_PARAM_DIST_PARAM_3: 50,
    VALETON_PARAM_DIST_PARAM_4: 51,
    VALETON_PARAM_DIST_PARAM_5: 52,
    VALETON_PARAM_DIST_PARAM_6: 53,
    VALETON_PARAM_DIST_PARAM_7: 54,

    //**** Amp ****
    VALETON_PARAM_AMP_PARAM_0: 55,
    VALETON_PARAM_AMP_PARAM_1: 56,
    VALETON_PARAM_AMP_PARAM_2: 57,
    VALETON_PARAM_AMP_PARAM_3: 58,
    VALETON_PARAM_AMP_PARAM_4: 59,
    VALETON_PARAM_AMP_PARAM_5: 60,
    VALETON_PARAM_AMP_PARAM_6: 61,
    VALETON_PARAM_AMP_PARAM_7: 62,

    //**** Cab ****
    VALETON_PARAM_CAB_PARAM_0: 63,
    VALETON_PARAM_CAB_PARAM_1: 64,
    VALETON_PARAM_CAB_PARAM_2: 65,
    VALETON_PARAM_CAB_PARAM_3: 66,
    VALETON_PARAM_CAB_PARAM_4: 67,
    VALETON_PARAM_CAB_PARAM_5: 68,
    VALETON_PARAM_CAB_PARAM_6: 69,
    VALETON_PARAM_CAB_PARAM_7: 70,

    //**** EQ ****  
    VALETON_PARAM_EQ_PARAM_0: 71,
    VALETON_PARAM_EQ_PARAM_1: 72,
    VALETON_PARAM_EQ_PARAM_2: 73,
    VALETON_PARAM_EQ_PARAM_3: 74,
    VALETON_PARAM_EQ_PARAM_4: 75,
    VALETON_PARAM_EQ_PARAM_5: 76,
    VALETON_PARAM_EQ_PARAM_6: 77,
    VALETON_PARAM_EQ_PARAM_7: 78,

    //**** Mod ****
    VALETON_PARAM_MOD_PARAM_0: 79,
    VALETON_PARAM_MOD_PARAM_1: 80,
    VALETON_PARAM_MOD_PARAM_2: 81,
    VALETON_PARAM_MOD_PARAM_3: 82,
    VALETON_PARAM_MOD_PARAM_4: 83,
    VALETON_PARAM_MOD_PARAM_5: 84,
    VALETON_PARAM_MOD_PARAM_6: 85,
    VALETON_PARAM_MOD_PARAM_7: 86,

    //**** Delay ****
    VALETON_PARAM_DLY_PARAM_0: 87,
    VALETON_PARAM_DLY_PARAM_1: 88,
    VALETON_PARAM_DLY_PARAM_2: 89,
    VALETON_PARAM_DLY_PARAM_3: 90,
    VALETON_PARAM_DLY_PARAM_4: 91,
    VALETON_PARAM_DLY_PARAM_5: 92,
    VALETON_PARAM_DLY_PARAM_6: 93,
    VALETON_PARAM_DLY_PARAM_7: 94,

    //**** Reverb ****
    VALETON_PARAM_RVB_PARAM_0: 95,
    VALETON_PARAM_RVB_PARAM_1: 96,
    VALETON_PARAM_RVB_PARAM_2: 97,
    VALETON_PARAM_RVB_PARAM_3: 98,
    VALETON_PARAM_RVB_PARAM_4: 99,
    VALETON_PARAM_RVB_PARAM_5: 100,
    VALETON_PARAM_RVB_PARAM_6: 101,
    VALETON_PARAM_RVB_PARAM_7: 102,

    //**** NS ****
    VALETON_PARAM_NS_PARAM_0: 103,
    VALETON_PARAM_NS_PARAM_1: 104,
    VALETON_PARAM_NS_PARAM_2: 105,
    VALETON_PARAM_NS_PARAM_3: 106,
    VALETON_PARAM_NS_PARAM_4: 107,
    VALETON_PARAM_NS_PARAM_5: 108,
    VALETON_PARAM_NS_PARAM_6: 109,
    VALETON_PARAM_NS_PARAM_7: 110,
    VALETON_PARAM_NS_PARAM_8: 111,
    VALETON_PARAM_NS_PARAM_9: 112,
    VALETON_PARAM_NS_PARAM_10: 113,

    // must be last actual parameter
    VALETON_PARAM_LAST: 114,

    // these are not parameters as such, but globals/special
    VALETON_GLOBAL_BPM: 115,
    VALETON_GLOBAL_INPUT_TRIM: 116,
    VALETON_GLOBAL_CABSIM_BYPASS: 117,
    VALETON_GLOBAL_MASTER_VOLUME: 118,
    VALETON_GLOBAL_RECORD_LEVEL: 119,
    VALETON_GLOBAL_MONITOR_LEVEL: 120,
    VALETON_GLOBAL_BT_LEVEL: 121,

    // must be last actual global
    VALETON_GLOBAL_LAST: 122
};

const ValetonEffectBlocks = {
    VALETON_EFFECT_BLOCK_NR:    0,
    VALETON_EFFECT_BLOCK_PRE:   1,
    VALETON_EFFECT_BLOCK_DST:   2,
    VALETON_EFFECT_BLOCK_AMP:   3,
    VALETON_EFFECT_BLOCK_CAB:   4,
    VALETON_EFFECT_BLOCK_EQ:    5,
    VALETON_EFFECT_BLOCK_MOD:   6,
    VALETON_EFFECT_BLOCK_DLY:   7,
    VALETON_EFFECT_BLOCK_RVB:   8,
    VALETON_EFFECT_BLOCK_NS:    9,
    VALETON_EFFECT_BLOCK_LAST:  10
};

const ValetonEffectPre = {
    VALETON_EFFECT_PRE_COMP:        0,
    VALETON_EFFECT_PRE_COMP4:       1,
    VALETON_EFFECT_PRE_BOOST:       2,
    VALETON_EFFECT_PRE_MICRO_BOOST: 3,
    VALETON_EFFECT_PRE_B_BOOST:     4,
    VALETON_EFFECT_PRE_TOUCHER:     5,
    VALETON_EFFECT_PRE_CRIER:       6,
    VALETON_EFFECT_PRE_OCTA:        7,
    VALETON_EFFECT_PRE_PITCH:       8,
    VALETON_EFFECT_PRE_DETUNE:      9
};

const ValetonEffectDist = {
    VALETON_EFFECT_DIST_GREEN_OD:     0,
    VALETON_EFFECT_DIST_YELLOW_OD:    1,
    VALETON_EFFECT_DIST_SUPER_OD:     2,
    VALETON_EFFECT_DIST_SM_DIST:      3,
    VALETON_EFFECT_DIST_PLUSTORTION:  4,
    VALETON_EFFECT_DIST_LA_CHARGER:   5,
    VALETON_EFFECT_DIST_DARKTALE:     6,
    VALETON_EFFECT_DIST_SORA_FUZZ:    7,
    VALETON_EFFECT_DIST_RED_HAZE:     8,
    VALETON_EFFECT_DIST_BASS_OD:      9
};

const ValetonEffectEQ = {
    VALETON_EFFECT_EQ_GUITAR_EQ1: 0,
    VALETON_EFFECT_EQ_GUITAR_EQ2: 1,
    VALETON_EFFECT_EQ_BASS_EQ1:   2,
    VALETON_EFFECT_EQ_BASS_EQ2:   3,
    VALETON_EFFECT_EQ_MESS_EQ:    4
};

const ValetonEffectMod = {
    VALETON_EFFECT_MOD_A_CHORUS:     0,
    VALETON_EFFECT_MOD_B_CHORUS:     1,
    VALETON_EFFECT_MOD_JET:          2,
    VALETON_EFFECT_MOD_N_JET:        3,
    VALETON_EFFECT_MOD_O_PHASE:      4,
    VALETON_EFFECT_MOD_M_VIBE:       5,
    VALETON_EFFECT_MOD_V_ROTO:       6,
    VALETON_EFFECT_MOD_VIBRATO:      7,
    VALETON_EFFECT_MOD_O_TREM:       8,
    VALETON_EFFECT_MOD_SINE_TREM:    9,
    VALETON_EFFECT_MOD_BIAS_TREM:   10
};

const ValetonEffectDly = {
    VALETON_EFFECT_DLY_PURE:        0,
    VALETON_EFFECT_DLY_ANALOG:      1,
    VALETON_EFFECT_DLY_SLAPBACK:    2,
    VALETON_EFFECT_DLY_SWEET_ECHO:  3,
    VALETON_EFFECT_DLY_TAPE:        4,
    VALETON_EFFECT_DLY_TUBE:        5,
    VALETON_EFFECT_DLY_REV_ECHO:    6,
    VALETON_EFFECT_DLY_RING_ECHO:   7,
    VALETON_EFFECT_DLY_SWEEP_ECHO:  8,
    VALETON_EFFECT_DLY_PING_PONG:   9
};

const ValetonEffectRvb = {
    VALETON_EFFECT_RVB_AIR:         0,
    VALETON_EFFECT_RVB_ROOM:        1,
    VALETON_EFFECT_RVB_HALL:        2,
    VALETON_EFFECT_RVB_CHURCH:      3,
    VALETON_EFFECT_RVB_PLATE_L:     4,
    VALETON_EFFECT_RVB_PLATE:       5,
    VALETON_EFFECT_RVB_SPRING:      6,
    VALETON_EFFECT_RVB_N_STAR:      7,
    VALETON_EFFECT_RVB_DEEPSEA:     8,
    VALETON_EFFECT_RVB_SWEET_SPACE: 9
};


const ValetonEffectCab = {
    VALETON_EFFECT_CAB_TWD_CP:         0,
    VALETON_EFFECT_CAB_DARK_VIT:       1,
    VALETON_EFFECT_CAB_FOXY_1X12:      2,
    VALETON_EFFECT_CAB_L_STAR_1X12:    3,
    VALETON_EFFECT_CAB_DARK_CS_2X12:   4,
    VALETON_EFFECT_CAB_DARK_TWIN_2X12: 5,
    VALETON_EFFECT_CAB_SUP_STAR_2X12:  6,
    VALETON_EFFECT_CAB_J_120_2X12:     7,
    VALETON_EFFECT_CAB_FOXY_2X12:      8,
    VALETON_EFFECT_CAB_UK_GRN_2X12:    9,
    VALETON_EFFECT_CAB_UK_GRN_4X12:   10,
    VALETON_EFFECT_CAB_BOG_4X12:      11,
    VALETON_EFFECT_CAB_DIZZ_4X12:     12,
    VALETON_EFFECT_CAB_EV_4X12:       13,
    VALETON_EFFECT_CAB_SOLO_4X12:     14,
    VALETON_EFFECT_CAB_MESS_4X12:     15,
    VALETON_EFFECT_CAB_EAGLE_4X12:    16,
    VALETON_EFFECT_CAB_JUICE_4X12:    17,
    VALETON_EFFECT_CAB_BELLMAN_2X12:  18,
    VALETON_EFFECT_CAB_AMPG_4X10:     19
};

const ValetonEffectAmp = {
    VALETON_EFFECT_AMP_TWEEDY:          0,
    VALETON_EFFECT_AMP_BELLMAN_59N:     1,
    VALETON_EFFECT_AMP_DARK_TWIN:       2,
    VALETON_EFFECT_AMP_FOXY_30N:        3,
    VALETON_EFFECT_AMP_J_120_CL:        4,
    VALETON_EFFECT_AMP_MATCH_CL:        5,
    VALETON_EFFECT_AMP_L_STAR_CL:       6,
    VALETON_EFFECT_AMP_UK_45:           7,
    VALETON_EFFECT_AMP_UK_50JP:         8,
    VALETON_EFFECT_AMP_UK_800:          9,
    VALETON_EFFECT_AMP_BELLMAN_59B:    10,
    VALETON_EFFECT_AMP_FOXY_30TB:      11,
    VALETON_EFFECT_AMP_SUPDUAL_OD:     12,
    VALETON_EFFECT_AMP_SOLO100_OD:     13,
    VALETON_EFFECT_AMP_Z38_OD:         14,
    VALETON_EFFECT_AMP_BAD_KT_OD:      15,
    VALETON_EFFECT_AMP_JUICE_R100:     16,
    VALETON_EFFECT_AMP_DIZZ_VH:        17,
    VALETON_EFFECT_AMP_DIZZ_VH_PLUS:   18,
    VALETON_EFFECT_AMP_EAGLE_120:      19,
    VALETON_EFFECT_AMP_EV_51:          20,
    VALETON_EFFECT_AMP_SOLO100_LD:     21,
    VALETON_EFFECT_AMP_MESS_DUALV:     22,
    VALETON_EFFECT_AMP_MESS_DUALM:     23,
    VALETON_EFFECT_AMP_POWER_LD:       24,
    VALETON_EFFECT_AMP_FLAGMAN_PLUS:   25,
    VALETON_EFFECT_AMP_BOG_REDV:       26,
    VALETON_EFFECT_AMP_CLASSIC_BASS:   27,
    VALETON_EFFECT_AMP_FOXY_BASS:      28,
    VALETON_EFFECT_AMP_MESS_BASS:      29,
    VALETON_EFFECT_AMP_AC_PRE1:        30,
    VALETON_EFFECT_AMP_AC_PRE2:        31
};

// External Footswitch midi associations
const midiControlChangeAssociations = new Map();

// MAIN
midiControlChangeAssociations.set("MAIN", null);
midiControlChangeAssociations.set(102, { param: "GAIN", value: { type: "RANGE" } })
midiControlChangeAssociations.set(103, { param: "VOLUME", value: { type: "RANGE" } })
midiControlChangeAssociations.set(104, { param: "MODEL MIX", value: { type: "RANGE" } })
midiControlChangeAssociations.set(23, { param: "BASS", value: { type: "RANGE" } })
midiControlChangeAssociations.set(24, { param: "BASS HZ", value: { type: "RANGE" } })
midiControlChangeAssociations.set(25, { param: "MID", value: { type: "RANGE" } })
midiControlChangeAssociations.set(26, { param: "MID Q", value: { type: "RANGE" } })
midiControlChangeAssociations.set(27, { param: "MID HZ", value: { type: "RANGE" } })
midiControlChangeAssociations.set(28, { param: "TREBLE", value: { type: "RANGE" } })
midiControlChangeAssociations.set(29, { param: "TREBLE HZ", value: { type: "RANGE" } })
midiControlChangeAssociations.set(30, { param: "EQ POSITION", value: { type: "RANGE" } })
midiControlChangeAssociations.set(106, { param: "PRESENCE", value: { type: "RANGE" } })
midiControlChangeAssociations.set(107, { param: "DEPTH", value: { type: "RANGE" } })

// GATE
midiControlChangeAssociations.set("GATE", null);
midiControlChangeAssociations.set(14, { param: "GATE POWER", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(15, { param: "GATE THRESHOLD", value: { type: "RANGE" } })
midiControlChangeAssociations.set(16, { param: "GATE RELEASE", value: { type: "RANGE" } })
midiControlChangeAssociations.set(17, { param: "GATE DEPTH", value: { type: "RANGE" } })
midiControlChangeAssociations.set(13, { param: "GATE POSITION", value: { type: "CHOICE", values: { 0: "FIRST", 127: "POST AMP" } } })

// COMP
midiControlChangeAssociations.set("COMP", null);
midiControlChangeAssociations.set(18, { param: "COMP POWER", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(19, { param: "COMP THRESHOLD", value: { type: "RANGE" } })
midiControlChangeAssociations.set(20, { param: "COMP GAIN", value: { type: "RANGE" } })
midiControlChangeAssociations.set(21, { param: "COMP ATTACK", value: { type: "RANGE" } })
midiControlChangeAssociations.set(22, { param: "COMP POSITION", value: { type: "CHOICE", values: { 0: "PRE AMP", 127: "POST AMP" } } })

// VIR
midiControlChangeAssociations.set("VIR", null);
midiControlChangeAssociations.set(108, { param: "VIR RESO", value: { type: "RANGE" } })
midiControlChangeAssociations.set(109, { param: "VIR MIC 1", value: { type: "CHOICE", values: { 0: "COND", 1: "DYN", 2: "RBN" } } })
midiControlChangeAssociations.set(110, { param: "VIR MIC 1 X", value: { type: "RANGE" } })
midiControlChangeAssociations.set(111, { param: "VIR MIC 1 Z", value: { type: "RANGE" } })
midiControlChangeAssociations.set(112, { param: "VIR MIC 2", value: { type: "CHOICE", values: { 0: "COND", 1: "DYN", 2: "RBN" } } })
midiControlChangeAssociations.set(113, { param: "VIR MIC 2 X", value: { type: "RANGE" } })
midiControlChangeAssociations.set(114, { param: "VIR MIC 2 Z", value: { type: "RANGE" } })
midiControlChangeAssociations.set(115, { param: "VIR BLEND", value: { type: "RANGE" } })

// MOD
midiControlChangeAssociations.set("MOD", null);
midiControlChangeAssociations.set(32, { param: "MOD POWER", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(33, { param: "MOD TYPE", value: { type: "CHOICE", values: { 0: "CHORUS", 1: "TREMOLO", 2: "PHASER", 3: "FLANGER", 4: "ROTARY" } } })
midiControlChangeAssociations.set(31, { param: "MOD POSITION", value: { type: "CHOICE", values: { 0: "PRE AMP", 127: "POST AMP" } } })

// MOD / CHORUS
midiControlChangeAssociations.set("MOD / CHORUS", null);
midiControlChangeAssociations.set(34, { param: "MOD CHORUS SYNC", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(35, { param: "MOD CHORUS RATE (TIMESIGN)", value: { type: "RANGE" } })
midiControlChangeAssociations.set(36, { param: "MOD CHORUS DEPTH", value: { type: "RANGE" } })
midiControlChangeAssociations.set(37, { param: "MOD CHORUS LEVEL", value: { type: "RANGE" } })

// MOD / TREMOLO
midiControlChangeAssociations.set("MOD / TREMOLO", null);
midiControlChangeAssociations.set(38, { param: "MOD TREMOLO SYNC", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(39, { param: "MOD TREMOLO RATE (TIMESIGN)", value: { type: "RANGE" } })
midiControlChangeAssociations.set(40, { param: "MOD TREMOLO SHAPE", value: { type: "RANGE" } })
midiControlChangeAssociations.set(41, { param: "MOD TREMOLO SPREAD", value: { type: "RANGE" } })
midiControlChangeAssociations.set(42, { param: "MOD TREMOLO LEVEL", value: { type: "RANGE" } })

// MOD / TREMOLO
midiControlChangeAssociations.set("MOD / TREMOLO", null);
midiControlChangeAssociations.set(43, { param: "MOD PHASER SYNC", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(44, { param: "MOD PHASER RATE (TIMESIGN)", value: { type: "RANGE" } })
midiControlChangeAssociations.set(45, { param: "MOD PHASER DEPTH", value: { type: "RANGE" } })
midiControlChangeAssociations.set(46, { param: "MOD PHASER LEVEL", value: { type: "RANGE" } })

// MOD / FLANGER
midiControlChangeAssociations.set("MOD / FLANGER", null);
midiControlChangeAssociations.set(47, { param: "MOD FLANGER SYNC", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(48, { param: "MOD FLANGER RATE (TIMESIGN)", value: { type: "RANGE" } })
midiControlChangeAssociations.set(49, { param: "MOD FLANGER DEPTH", value: { type: "RANGE" } })
midiControlChangeAssociations.set(50, { param: "MOD FLANGER FEEDBACK", value: { type: "RANGE" } })
midiControlChangeAssociations.set(51, { param: "MOD FLANGER LEVEL", value: { type: "RANGE" } })

// MOD / ROTARY
midiControlChangeAssociations.set("MOD / ROTARY", null);
midiControlChangeAssociations.set(52, { param: "MOD ROTARY SYNC", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(53, { param: "MOD ROTARY SPEED (TIMESIGN)", value: { type: "RANGE" } })
midiControlChangeAssociations.set(54, { param: "MOD ROTARY RADIUS", value: { type: "RANGE" } })
midiControlChangeAssociations.set(55, { param: "MOD ROTARY SPREAD", value: { type: "RANGE" } })
midiControlChangeAssociations.set(56, { param: "MOD ROTARY LEVEL", value: { type: "RANGE" } })

// DELAY
midiControlChangeAssociations.set("DELAY", null);
midiControlChangeAssociations.set(2, { param: "DELAY POWER", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(3, { param: "DELAY TYPE", value: { type: "CHOICE", values: { 0: "DIGITAL", 1: "TAPE" } } })
midiControlChangeAssociations.set(1, { param: "DELAY POSITION", value: { type: "CHOICE", values: { 0: "PRE AMP", 127: "POST AMP" } } })

// DELAY / DIGITAL
midiControlChangeAssociations.set("DELAY / DIGITAL", null);
midiControlChangeAssociations.set(4, { param: "DELAY DIGITAL SYNC", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(5, { param: "DELAY DIGITAL TIME (TIMESIGN)", value: { type: "RANGE" } })
midiControlChangeAssociations.set(6, { param: "DELAY DIGITAL FEEDBACK", value: { type: "RANGE" } })
midiControlChangeAssociations.set(7, { param: "DELAY DIGITAL MODE", value: { type: "CHOICE", values: { 0: "NORMAL", 64: "PING.PONG" } } })
midiControlChangeAssociations.set(8, { param: "DELAY DIGITAL MIX", value: { type: "RANGE" } })

// DELAY / TAPE
midiControlChangeAssociations.set("DELAY / TAPE", null);
midiControlChangeAssociations.set(91, { param: "DELAY TAPE SYNC", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(92, { param: "DELAY TAPE TIME (TIMESIGN)", value: { type: "RANGE" } })
midiControlChangeAssociations.set(93, { param: "DELAY TAPE FEEDBACK", value: { type: "RANGE" } })
midiControlChangeAssociations.set(94, { param: "DELAY TAPE MODE", value: { type: "CHOICE", values: { 0: "NORMAL", 64: "PING.PONG" } } })
midiControlChangeAssociations.set(95, { param: "DELAY TAPE MIX", value: { type: "RANGE" } })

// REVERB
midiControlChangeAssociations.set("REVERB", null);
midiControlChangeAssociations.set(75, { param: "REVERB POWER", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(85, { param: "REVERB TYPE", value: { type: "CHOICE", values: { 0: "SPRING 1", 1: "SPRING 2", 2: "SPRING 3", 3: "SPRING 4", 4: "ROOM", 5: "PLATE" } } })
midiControlChangeAssociations.set(84, { param: "REVERB POSITION", value: { type: "CHOICE", values: { 0: "POST AMP", 127: "LAST" } } })

// REVERB / SPRING 1
midiControlChangeAssociations.set("REVERB / SPRING 1", null);
midiControlChangeAssociations.set(59, { param: "REVERB SPRING 1 TIME", value: { type: "RANGE" } })
midiControlChangeAssociations.set(60, { param: "REVERB SPRING 1 PRE.DELAY", value: { type: "RANGE" } })
midiControlChangeAssociations.set(61, { param: "REVERB SPRING 1 COLOR", value: { type: "RANGE" } })
midiControlChangeAssociations.set(62, { param: "REVERB SPRING 1 MIX", value: { type: "RANGE" } })

// REVERB / SPRING 2
midiControlChangeAssociations.set("REVERB / SPRING 2", null);
midiControlChangeAssociations.set(63, { param: "REVERB SPRING 2 TIME", value: { type: "RANGE" } })
midiControlChangeAssociations.set(64, { param: "REVERB SPRING 2 PRE.DELAY", value: { type: "RANGE" } })
midiControlChangeAssociations.set(65, { param: "REVERB SPRING 2 COLOR", value: { type: "RANGE" } })
midiControlChangeAssociations.set(66, { param: "REVERB SPRING 2 MIX", value: { type: "RANGE" } })

// REVERB / SPRING 3
midiControlChangeAssociations.set("REVERB / SPRING 3", null);
midiControlChangeAssociations.set(67, { param: "REVERB SPRING 3 TIME", value: { type: "RANGE" } })
midiControlChangeAssociations.set(68, { param: "REVERB SPRING 3 PRE.DELAY", value: { type: "RANGE" } })
midiControlChangeAssociations.set(69, { param: "REVERB SPRING 3 COLOR", value: { type: "RANGE" } })
midiControlChangeAssociations.set(70, { param: "REVERB SPRING 3 MIX", value: { type: "RANGE" } })

// REVERB / SPRING 4
midiControlChangeAssociations.set("REVERB / SPRING 4", null);
midiControlChangeAssociations.set(80, { param: "REVERB SPRING 4 TIME", value: { type: "RANGE" } })
midiControlChangeAssociations.set(81, { param: "REVERB SPRING 4 PRE.DELAY", value: { type: "RANGE" } })
midiControlChangeAssociations.set(82, { param: "REVERB SPRING 4 COLOR", value: { type: "RANGE" } })
midiControlChangeAssociations.set(83, { param: "REVERB SPRING 4 MIX", value: { type: "RANGE" } })

// REVERB / ROOM
midiControlChangeAssociations.set("REVERB / ROOM", null);
midiControlChangeAssociations.set(71, { param: "REVERB ROOM TIME", value: { type: "RANGE" } })
midiControlChangeAssociations.set(72, { param: "REVERB ROOM PRE.DELAY", value: { type: "RANGE" } })
midiControlChangeAssociations.set(73, { param: "REVERB ROOM COLOR", value: { type: "RANGE" } })
midiControlChangeAssociations.set(74, { param: "REVERB ROOM MIX", value: { type: "RANGE" } })

// REVERB / PLATE
midiControlChangeAssociations.set("REVERB / PLATE", null);
midiControlChangeAssociations.set(76, { param: "REVERB PLATE TIME", value: { type: "RANGE" } })
midiControlChangeAssociations.set(77, { param: "REVERB PLATE PRE.DELAY", value: { type: "RANGE" } })
midiControlChangeAssociations.set(78, { param: "REVERB PLATE COLOR", value: { type: "RANGE" } })
midiControlChangeAssociations.set(79, { param: "REVERB PLATE MIX", value: { type: "RANGE" } })

// GLOBAL
midiControlChangeAssociations.set("GLOBAL", null);
// not supported midiControlChangeAssociations.set(12, { param: "PRESET ON/OFF", value: { type: "TOGGLE" } })
// not supported midiControlChangeAssociations.set(11, { param: "EXPRESSION PEDAL", value: { type: "RANGE" } })
midiControlChangeAssociations.set(0, { param: "MIDI PATCH BANK", value: { type: "CHOICE", values: { 0: "000", 1: "001" } } })
midiControlChangeAssociations.set(86, { param: "PRESET DOWN", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(87, { param: "PRESET UP", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(88, { param: "BPM", value: { type: "RANGE" } })
// not supported midiControlChangeAssociations.set(89, { param: "BANK DOWN", value: { type: "TOGGLE" } })
// not supported midiControlChangeAssociations.set(90, { param: "BANK UP", value: { type: "TOGGLE" } })
//not supported midiControlChangeAssociations.set(9, { param: "TUNER", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(10, { param: "TAP TEMPO", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(116, { param: "INPUT TRIM", value: { type: "RANGE" } })
midiControlChangeAssociations.set(117, { param: "CABSIM BYPASS", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(118, { param: "TEMPO SOURCE", value: { type: "TOGGLE" } })
midiControlChangeAssociations.set(119, { param: "TUNING REFERENCE", value: { type: "RANGE" } })
midiControlChangeAssociations.set(122, { param: "GLOBAL VOLUME", value: { type: "RANGE" } })
midiControlChangeAssociations.set(127, { param: "SET PRESET", value: { type: "RANGE" } })

// tap tempo
let globalTapTempo = new TapTempo();

// Init web socket when the page loads
window.addEventListener('load', onload);

const THROTTLE_DELAY = 200; // ms between WebSocket sends
let lastSendTime = 0;
let throttleTimeout = null;

function onload(event) {
    // Get the element with id="defaultOpen" and click on it
    document.getElementById("defaultOpen").click();
    
    document.getElementById("defaultFX").click();
    document.getElementById("defaultIFX").click();

    initWebSocket();
    pageTimer = setInterval("pageTimerEvent();", 750);

    inputs = document.getElementsByTagName('input');
    for (i = 0; i < inputs.length; i++) {
        if (inputs[i].type == "range" && inputs[i].oninput == null) {
            inputs[i].oninput = function(e){showValue(e.target, true);};
        }
    }

    // initialize animations
    bpmBlink();
    delayBlink();
    modulationAnimate();
    populateExternalFootswitches();
    populateInternalFootswitches();
    setEffectIcons();    
    
    document.addEventListener('mouseup', () => {
        if (dragTimeout) clearTimeout(dragTimeout);
        setTimeout(() => { 
            userIsDragging = null; 
            //console.log("Slider stopped");
        }, 250);
    });

    document.addEventListener('touchend', () => {
        if (dragTimeout) clearTimeout(dragTimeout);
        setTimeout(() => { 
            userIsDragging = null; 
            //console.log("Slider stopped");
        }, 250);
    });
}

function setEffectIcons() {
    // make sure the correct icons are showing to suit the modeller
    updateEffectsChainIcons();

    // set effect icons
    switch (modellerType)
    {
        case AMP_MODELLER_TONEX_ONE:
        case AMP_MODELLER_TONEX:
        default:
        {    
            // noise gate
            var fxImage = document.getElementById("fx_image_gate");
            var fxState = document.getElementById("ng_enable");

            if (fxState.checked) {
                fxImage.src = "img/effect_icon_gate_on.png";
            } 
            else {
                fxImage.src = "img/effect_icon_gate_off.png";
            }

            // eq
            fxImage = document.getElementById("fx_image_eq");
            fxImage.src = "img/effect_icon_eq.png"; 

            // amp enable
            fxImage = document.getElementById("fx_image_amp");
            fxState = document.getElementById("am_ampen");
            if (fxState.checked) {
                fxImage.src = "img/effect_icon_amp_on.png";
            } 
            else {
                fxImage.src = "img/effect_icon_amp_off.png";
            }

            // cab enable
            fxImage = document.getElementById("fx_image_cab");
            fxState = document.getElementById("am_cabtype");
            if (fxState.value == 2) {
                fxImage.src = "img/effect_icon_cab_off.png";
            } 
            else {
                fxImage.src = "img/effect_icon_cab_on.png";
            }
            
            // compressor    
            fxImage = document.getElementById("fx_image_comp");
            fxState = document.getElementById("cp_enable");
            if (fxState.checked) {
                fxImage.src = "img/effect_icon_comp_on.png";
            } 
            else {
                fxImage.src = "img/effect_icon_comp_off.png";
            }

            // modulation
            fxImage = document.getElementById("fx_image_mod");
            fxState = document.getElementById("mo_enable");
            if (fxState.checked) {
                // check the model
                var model = getSelectValue("mo_model");
                
                if (model == TONEX_MODULATION_CHORUS) {
                    fxImage.src = "img/effect_icon_mod_on_chorus.png";
                }
                else if (model == TONEX_MODULATION_TREMOLO) {
                    fxImage.src = "img/effect_icon_mod_on_tremolo.png";
                }
                else if (model == TONEX_MODULATION_PHASER) {
                    fxImage.src = "img/effect_icon_mod_on_phaser.png";
                }
                else if (model == TONEX_MODULATION_FLANGER) {
                    fxImage.src = "img/effect_icon_mod_on_flanger.png";
                }
                else if (model == TONEX_MODULATION_ROTARY) {
                    fxImage.src = "img/effect_icon_mod_on_rotary.png";
                }
            } 
            else {
                fxImage.src = "img/effect_icon_mod_off.png";
            }

            // delay
            fxImage = document.getElementById("fx_image_dly");
            fxState = document.getElementById("dl_enable");
            if (fxState.checked) {
                if (getSelectValue("dl_model") == TONEX_DELAY_DIGITAL) {
                    fxImage.src = "img/effect_icon_delay_on_d.png";
                } 
                else if (getSelectValue("dl_model") == TONEX_DELAY_TAPE) {
                    fxImage.src = "img/effect_icon_delay_on_t.png";
                }
            } 
            else {
                fxImage.src = "img/effect_icon_delay_off.png";
            }

            // reverb
            fxImage = document.getElementById("fx_image_rvb");
            fxState = document.getElementById("rv_enable");
            if (fxState.checked) {
                // check the model
                var model = getSelectValue("rv_model");

                if (model == TONEX_REVERB_SPRING_1) {
                    fxImage.src = "img/effect_icon_reverb_on_s1.png";
                }
                else if (model == TONEX_REVERB_SPRING_2) {
                    fxImage.src = "img/effect_icon_reverb_on_s2.png";
                }
                else if (model == TONEX_REVERB_SPRING_3) {
                    fxImage.src = "img/effect_icon_reverb_on_s3.png";
                }
                else if (model == TONEX_REVERB_SPRING_4) {
                    fxImage.src = "img/effect_icon_reverb_on_s4.png";
                }
                else if (model == TONEX_REVERB_PLATE) {
                    fxImage.src = "img/effect_icon_reverb_on_p.png";
                }
                else if (model == TONEX_REVERB_ROOM) {
                    fxImage.src = "img/effect_icon_reverb_on_r.png";
                }
            } 
            else {
                fxImage.src = "img/effect_icon_reverb_off.png";
            }

            // set the order
            var gatePost = document.getElementById("ng_post");
            var compPost = document.getElementById("cp_post");
            var modPost = document.getElementById("mo_post");
            var delayPost = document.getElementById("dl_post");
            var eqPost = document.getElementById("eq_post");
            var rvbPost = document.getElementById("rv_post");

            const container = document.querySelector('.fx_icon_container');
            const images = Array.from(container.querySelectorAll('img'));
            var newOrderIds = [];
            var index = 0;

            if (!gatePost.checked)
            {
                newOrderIds[index] = 'fx_image_gate';
                index++;
            }
            
            if (!compPost.checked)
            {
                newOrderIds[index] = 'fx_image_comp';
                index++;
            }
            
            if (!modPost.checked)
            {
                newOrderIds[index] = 'fx_image_mod';
                index++;
            }
            
            if (!delayPost.checked)
            {
                newOrderIds[index] = 'fx_image_dly';
                index++;
            }

            if (!eqPost.checked)
            {
                newOrderIds[index] = 'fx_image_eq';
                index++;
            }

            newOrderIds[index] = 'fx_image_amp';
            index++;
            newOrderIds[index] = 'fx_image_cab';
            index++;

            if (eqPost.checked)
            {
                newOrderIds[index] = 'fx_image_eq';
                index++;
            }

            if (gatePost.checked)
            {
                newOrderIds[index] = 'fx_image_gate';
                index++;
            }
            
            if (compPost.checked)
            {
                newOrderIds[index] = 'fx_image_comp';
                index++;
            }
            
            if (!rvbPost.checked)
            {
                newOrderIds[index] = 'fx_image_rvb';
                index++;
            }

            if (modPost.checked)
            {
                newOrderIds[index] = 'fx_image_mod';
                index++;
            }
            
            if (delayPost.checked)
            {
                newOrderIds[index] = 'fx_image_dly';
                index++;
            }

            if (rvbPost.checked)
            {
                newOrderIds[index] = 'fx_image_rvb';
                index++;
            }

            const orderedImages = newOrderIds.map(id => images.find(img => img.id === id));
            container.innerHTML = ''; // Clear the container
            orderedImages.forEach(img => container.appendChild(img));
        } break;

        case AMP_MODELLER_VALETON_GP5:
        {
            // nr
            var fxImage = document.getElementById("fx_image_nr");
            var fxState = document.getElementById("val_nr_enable");
            if (fxState.checked) {
                fxImage.src = "img/nr_on.png";
            } 
            else {
                fxImage.src = "img/nr_off.png";
            }

            // pre
            fxImage = document.getElementById("fx_image_pre");
            fxState = document.getElementById("val_pre_enable");        
            if (fxState.checked) {
                fxImage.src = "img/pre_on.png";
            } 
            else {
                fxImage.src = "img/pre_off.png";
            }
            
            // dst
            fxImage = document.getElementById("fx_image_dst");
            fxState = document.getElementById("val_dst_enable");

            if (fxState.checked) {
                fxImage.src = "img/dst_on.png";
            } 
            else {
                fxImage.src = "img/dst_off.png";
            }

            // amp 
            fxImage = document.getElementById("fx_image_amp");
            fxState = document.getElementById("val_amp_enable");
            nsState = document.getElementById("val_ns_enable");
            if (nsState.checked) {
                if (fxState.checked) {
                    fxImage.src = "img/amp_disabled.png";
                } 
                else {
                    fxImage.src = "img/amp_off.png";
                }
            }
            else {
                if (fxState.checked) {
                    fxImage.src = "img/amp_on.png";
                } 
                else {
                    fxImage.src = "img/amp_off.png";
                }
            }

            // cab
            fxImage = document.getElementById("fx_image_cab");
            fxState = document.getElementById("val_cab_enable");
            nsState = document.getElementById("val_ns_enable");
            if (nsState.checked) {
                if (fxState.checked) {
                    fxImage.src = "img/cab_disabled.png";
                } 
                else {
                    fxImage.src = "img/cab_off.png";
                }
            }
            else {
                if (fxState.checked) {
                    fxImage.src = "img/cab_on.png";
                } 
                else {
                    fxImage.src = "img/cab_off.png";
                }
            }

            // eq    
            fxImage = document.getElementById("fx_image_eq");
            fxState = document.getElementById("val_eq_enable");
            if (fxState.checked) {
                fxImage.src = "img/eq_on.png";
            } 
            else {
                fxImage.src = "img/eq_off.png";
            }

            // mod
            fxImage = document.getElementById("fx_image_mod");
            fxState = document.getElementById("val_mod_enable");
            if (fxState.checked) {
               fxImage.src = "img/mod_on.png";
            } 
            else {
                fxImage.src = "img/mod_off.png";
            }

            // dly
            fxImage = document.getElementById("fx_image_dly");
            fxState = document.getElementById("val_dly_enable");
            if (fxState.checked) {
                fxImage.src = "img/dly_on.png";
            } 
            else {
                fxImage.src = "img/dly_off.png";
            }

            // rvb
            fxImage = document.getElementById("fx_image_rvb");
            fxState = document.getElementById("val_rvb_enable");
            if (fxState.checked) {
                fxImage.src = "img/rvb_on.png";
            } 
            else {
                fxImage.src = "img/rvb_off.png";
            }

            // ns
            fxImage = document.getElementById("fx_image_ns");
            fxState = document.getElementById("val_ns_enable");
            if (fxState.checked) {
                fxImage.src = "img/tc_on.png";
            } 
            else {
                fxImage.src = "img/tc_off.png";
            }

            // set effect icon order
            const container = document.querySelector('.fx_icon_container');
            const images = Array.from(container.querySelectorAll('img'));
            var newOrderIds = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];

            for (var loop = ValetonEffectBlocks.VALETON_EFFECT_BLOCK_NR; loop < ValetonEffectBlocks.VALETON_EFFECT_BLOCK_LAST; loop++)
            {
                switch (effect_icon_order[loop])
                {
                    case ValetonEffectBlocks.VALETON_EFFECT_BLOCK_NR:
                    {
                        newOrderIds[loop] = 'fx_image_nr';
                    } break;

                    case ValetonEffectBlocks.VALETON_EFFECT_BLOCK_PRE:
                    {
                        newOrderIds[loop] = 'fx_image_pre';
                    } break;

                    case ValetonEffectBlocks.VALETON_EFFECT_BLOCK_DST:
                    {
                        newOrderIds[loop] = 'fx_image_dst';
                    } break;

                    case ValetonEffectBlocks.VALETON_EFFECT_BLOCK_AMP:
                    {
                        newOrderIds[loop] = 'fx_image_amp';
                    } break;

                    case ValetonEffectBlocks.VALETON_EFFECT_BLOCK_CAB:
                    {
                        newOrderIds[loop] = 'fx_image_cab';
                    } break;

                    case ValetonEffectBlocks.VALETON_EFFECT_BLOCK_EQ:
                    {
                        newOrderIds[loop] = 'fx_image_eq';
                    } break;

                    case ValetonEffectBlocks.VALETON_EFFECT_BLOCK_MOD:
                    {
                        newOrderIds[loop] = 'fx_image_mod';
                    } break;
                    
                    case ValetonEffectBlocks.VALETON_EFFECT_BLOCK_DLY:
                    {
                        newOrderIds[loop] = 'fx_image_dly';
                    } break;

                    case ValetonEffectBlocks.VALETON_EFFECT_BLOCK_RVB:
                    {
                        newOrderIds[loop] = 'fx_image_rvb';
                    } break;

                    case ValetonEffectBlocks.VALETON_EFFECT_BLOCK_NS:
                    {
                        newOrderIds[loop] = 'fx_image_ns';
                    } break;
                }                
            }

            const orderedImages = newOrderIds.map(id => images.find(img => img.id === id));
            container.innerHTML = ''; // Clear the container
            orderedImages.forEach(img => container.appendChild(img));
        } break;
    }
}

function pageTimerEvent(){
    let modal = bootstrap.Modal.getOrCreateInstance(document.getElementById('modConnectionFault')) 

    if (SocketConnected === 0) {
        if (!modal._isShown) {
            console.log("Not connected");
            modal.show();
        }
    }
    else {
        if (isSyncDone == 0) {
            sendWS({"CMD": "GETSYNCCOMPLETE"});
        }

        if (modal._isShown) {
            console.log('Connected');
            modal.hide();            
        }
    }
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    SocketConnected = 1;
    
    // request config, params, and current preset
    // Get modeller data first so we now how to display the presets
    sendWS({"CMD": "GETMODELLERDATA"});
    sendWS({"CMD": "GETSYNCCOMPLETE"});
    sendWS({"CMD": "GETCONFIG"});                      
}

function onClose(event) {
    console.log('Connection closed');
    SocketConnected = 0;
    setTimeout(initWebSocket, 2000);
}

function sendWS(data) {
    if (SocketConnected === 1) {
        websocket.send(JSON.stringify(data));
    }
}

function createPresetOrderRow(parent, preset, enable_up, enable_down) {
    // Create the outer div with class "row" and attributes
    const rowDiv = document.createElement('div');
    rowDiv.className = 'row';
    rowDiv.id = 'preset-order-' + preset;
    rowDiv.setAttribute('order_value', preset);

    // Create the inner div with class "col d-flex align-items-center preset-order-item"
    const colDiv = document.createElement('div');
    colDiv.className = 'col d-flex align-items-center preset-order-item';

    // Create the first span with the bullet symbol
    const bulletSpan = document.createElement('span');
    bulletSpan.textContent = '⬤';

    // Create a non-breaking space
    const space = document.createTextNode('\u00A0');

    // Create the second span with class "flex-grow-1" and text "1"
    const textSpan = document.createElement('span');
    textSpan.className = 'flex-grow-1';
    textSpan.textContent = (preset + 1).toString();

    let upButton = document.createElement('button');
    upButton.className = 'btn btn-dark';
    if (enable_up) {
        // Create the up button with onclick event                
        upButton.textContent = '↑';
        upButton.onclick = () => presetOrderUp(preset); 
    } else {
        // Create the disabled up button
        upButton.style.opacity = '0%';
        upButton.disabled = true;
        upButton.textContent = '↑';
    }

    let downButton = document.createElement('button');
    downButton.className = 'btn btn-dark';
    if (enable_down) {
        // Create the down button with onclick event
        downButton.textContent = '↓';
        downButton.onclick = () => presetOrderDown(preset); 
    } else {
         // Create the disabled down button
        downButton.style.opacity = '0%';
        downButton.disabled = true;
        downButton.textContent = '↓';
    }

    //console.log("createPresetOrderRow");
    //console.log(upButton);
    //console.log(downButton);

    // Append elements to the inner div
    colDiv.appendChild(bulletSpan);
    colDiv.appendChild(space);
    colDiv.appendChild(textSpan);
    colDiv.appendChild(upButton);
    colDiv.appendChild(downButton);

    // Append inner div to outer div
    rowDiv.appendChild(colDiv);

    parent.appendChild(rowDiv); 
}

// Function that receives the message from the ESP32
function onMessage(event) {
    var received_data = JSON.parse(event.data);
    //console.log(received_data);
    processReturnCmd(received_data);        
}

function configureParamRange(objname, value, min, max, step=1) {
    //console.log("configureParamRange");
    //console.log(objname);
    var paramrange = document.getElementById(objname);
    paramrange.step = step;
    paramrange.min = min;
    paramrange.max = max;

    // only update the value if not being currently dragged
    if (userIsDragging !== paramrange.id) {
        paramrange.value = value;            
        showValue(paramrange, false);
    } else {
        //console.log("Slider value change suppressed");
    }
}

function configureParamSwitch(objname, value) {
    //console.log("configureParamSwitch");

    var paramswitch = document.getElementById(objname);
    if (value == 0) {
        paramswitch.checked = false;
    } else {
        paramswitch.checked = true;
    }
}

function configureParamSelect(objname, value) {
    //console.log("configureParamSelect");
    var paramrange = document.getElementById(objname);
    paramrange.value = value;
}

function getSelectValue(objname) {
    var sel = document.getElementById(objname);
    return sel.value;
}

function setParamLabel(objname, text) {
    var sel = document.getElementById(objname);
    sel.innerHTML = text;
}

function setParamVisible(objname, visible) {
    var elem = document.getElementById(objname);

    if (visible) {
        elem.style.display = "block";
    } else {
        elem.style.display = "none";
    }
}

function setParamValue(objname, value) {
    var obj = document.getElementById(objname);
    obj.value = value;
}

function setParamData(objname, param_index) {
    // add param index, for future use
    var obj = document.getElementById(objname);
    //obj.dataset.indexNumber = param_index;
    obj.setAttribute("data-indexnumber", param_index);
}

function showDelayTimeAndTimesign(sync) {
    document.getElementById("dl_time").parentElement.style.display = sync == 0 ? "block" : "none";
    document.getElementById("dl_timesign").parentElement.style.display = sync == 0 ? "none" : "block";
}

function showModTimeAndTimesign(sync) {
    document.getElementById("mo_param1").parentElement.style.display = sync == 0 ? "block" : "none";
    document.getElementById("mo_timesign").parentElement.style.display = sync == 0 ? "none" : "block";
}

function updateVisibleTabs(currentDevice) {
    document.querySelectorAll('.tablinks[data-show-for]').forEach(btn => {
        const allowed = btn.getAttribute('data-show-for').split(' ');
        btn.classList.toggle('hidden', !allowed.includes(currentDevice));
    });
}

function configureParam(id, visible, labelText = null) {
    const input = document.getElementById(id);
    if (!input) return; // safety

    const container = input.closest('.container');
    if (container) {
        container.style.display = visible ? '' : 'none';
    }
    if (labelText !== null) {
        const label = document.querySelector(`label[for="${id}"]`);
        if (label) label.textContent = labelText;
    }
}

function processReturnCmd(data) {
    switch (data['CMD'])
    {
        case 'GETPARAMS':
            console.log("Got Params");

            switch (modellerType)
            {
                case AMP_MODELLER_TONEX_ONE:
                case AMP_MODELLER_TONEX:
                default:
                {    
                    for (var key in data['PARAMS']) {
                        //console.log(data['PARAMS'][key]);
                        var min = data['PARAMS'][key]['Min'];
                        var max = data['PARAMS'][key]['Max'];
                        var value = data['PARAMS'][key]['Val'];

                        switch (parseInt(key))
                        {
                            case TONEX_PARAM_NOISE_GATE_POST:  
                                configureParamSwitch("ng_post", value);
                                setParamData("ng_post", parseInt(key));
                                break;

                            case TONEX_PARAM_NOISE_GATE_ENABLE: 
                                configureParamSwitch("ng_enable", value);     
                                setParamData("ng_enable", parseInt(key));                       
                                break;

                            case TONEX_PARAM_NOISE_GATE_THRESHOLD: 
                                configureParamRange("ng_thresh", value, min, max);
                                setParamData("ng_thresh", parseInt(key));
                                break;

                            case TONEX_PARAM_NOISE_GATE_RELEASE: 
                                configureParamRange("ng_release", value, min, max);
                                setParamData("ng_release", parseInt(key));
                                break;

                            case TONEX_PARAM_NOISE_GATE_DEPTH: 
                                configureParamRange("ng_depth", value, min, max);
                                setParamData("ng_depth", parseInt(key));
                                break;

                            // Compressor
                            case TONEX_PARAM_COMP_POST: 
                                configureParamSwitch("cp_post", value);
                                setParamData("cp_post", parseInt(key));
                                break;

                            case TONEX_PARAM_COMP_ENABLE:
                                configureParamSwitch("cp_enable", value);
                                setParamData("cp_enable", parseInt(key));
                                break;

                            case TONEX_PARAM_COMP_THRESHOLD:
                                configureParamRange("cp_thresh", value, min, max, 0.5);
                                setParamData("cp_thresh", parseInt(key));
                                break;

                            case TONEX_PARAM_COMP_MAKE_UP: 
                                configureParamRange("cp_gain", value, min, max);
                                setParamData("cp_gain", parseInt(key));
                                break;

                            case TONEX_PARAM_COMP_ATTACK:
                                configureParamRange("cp_attack", value, min, max);
                                setParamData("cp_attack", parseInt(key));
                                break;

                            // EQ    
                            case TONEX_PARAM_EQ_POST:
                                configureParamSwitch("eq_post", value);
                                setParamData("eq_post", parseInt(key));
                                break;

                            case TONEX_PARAM_EQ_BASS:
                                configureParamRange("eq_bass", value, min, max, 0.1);
                                setParamData("eq_bass", parseInt(key));
                                break;

                            case TONEX_PARAM_EQ_BASS_FREQ:
                                configureParamRange("eq_bassf", value, min, max, 15);
                                setParamData("eq_bassf", parseInt(key));                                
                                break;

                            case TONEX_PARAM_EQ_MID:
                                configureParamRange("eq_mid", value, min, max, 0.1);
                                setParamData("eq_mid", parseInt(key));
                                break;

                            case TONEX_PARAM_EQ_MIDQ:
                                configureParamRange("eq_midq", value, min, max, 0.05);
                                setParamData("eq_midq", parseInt(key));                            
                                break;

                            case TONEX_PARAM_EQ_MID_FREQ:
                                configureParamRange("eq_midf", value, min, max, 50);
                                setParamData("eq_midf", parseInt(key));
                                break;

                            case TONEX_PARAM_EQ_TREBLE:
                                configureParamRange("eq_treble", value, min, max, 0.1);
                                setParamData("eq_treble", parseInt(key));
                                break;

                            case TONEX_PARAM_EQ_TREBLE_FREQ:
                                configureParamRange("eq_treblef", value, min, max, 100);
                                setParamData("eq_treblef", parseInt(key));
                                break;
                            
                            //Model and VIR params
                            case TONEX_PARAM_MODEL_AMP_ENABLE:
                                configureParamSwitch("am_ampen", value);
                                setParamData("am_ampen", parseInt(key));
                                break;

                            case TONEX_PARAM_MODEL_GAIN:
                                configureParamRange("am_gain", value, min, max, 0.1);
                                setParamData("am_gain", parseInt(key));
                                break;

                            case TONEX_PARAM_MODEL_VOLUME:
                                configureParamRange("am_volume", value, min, max, 0.1);
                                setParamData("am_volume", parseInt(key));
                                break;

                            case TONEX_PARAM_MODEX_MIX:
                                configureParamRange("am_mix", value, min, max);
                                setParamData("am_mix", parseInt(key));                            
                                break;

                            case TONEX_PARAM_CABINET_TYPE:
                                configureParamSelect("am_cabtype", value);
                                setParamData("am_cabtype", parseInt(key));
                                break;
                        
                            case TONEX_PARAM_VIR_CABINET_MODEL:
                                break;

                            case TONEX_PARAM_VIR_RESO:
                                break;

                            case TONEX_PARAM_VIR_MIC_1:
                                break;

                            case TONEX_PARAM_VIR_MIC_1_X:
                                break;

                            case TONEX_PARAM_VIR_MIC_1_Z:
                                break;

                            case TONEX_PARAM_VIR_MIC_2:
                                break;

                            case TONEX_PARAM_VIR_MIC_2_X:
                                break;

                            case TONEX_PARAM_VIR_MIC_2_Z:
                                break;

                            case TONEX_PARAM_VIR_BLEND:
                                break;
                            
                            case TONEX_PARAM_MODEL_PRESENCE:
                                configureParamRange("am_presence", value, min, max,  0.1);
                                setParamData("am_presence", parseInt(key));
                                break;

                            case TONEX_PARAM_MODEL_DEPTH:
                                configureParamRange("am_depth", value, min, max, 0.1);
                                setParamData("am_depth", parseInt(key));                            
                                break;

                            // Reverb
                            case TONEX_PARAM_REVERB_POSITION:
                                configureParamSwitch("rv_post", value);
                                setParamData("rv_post", parseInt(key));
                                break;

                            case TONEX_PARAM_REVERB_ENABLE:
                                configureParamSwitch("rv_enable", value);
                                setParamData("rv_enable", parseInt(key));
                                break;

                            case TONEX_PARAM_REVERB_MODEL:
                                configureParamSelect("rv_model", value);
                                setParamData("rv_model", parseInt(key));
                                break;

                            case TONEX_PARAM_REVERB_SPRING1_TIME:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_1) {
                                    configureParamRange("rv_time", value, min, max, 0.5);
                                    setParamData("rv_time", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_SPRING1_PREDELAY:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_1) {
                                    configureParamRange("rv_predelay", value, min, max, 10);
                                    setParamData("rv_predelay", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_SPRING1_COLOR:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_1) {
                                    configureParamRange("rv_color", value, min, max);
                                    setParamData("rv_color", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_SPRING1_MIX:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_1) {
                                    configureParamRange("rv_mix", value, min, max);
                                    setParamData("rv_mix", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_SPRING2_TIME:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_2) {
                                    configureParamRange("rv_time", value, min, max, 0.5);
                                    setParamData("rv_time", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_SPRING2_PREDELAY:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_2) {
                                    configureParamRange("rv_predelay", value, min, max, 10);
                                    setParamData("rv_predelay", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_SPRING2_COLOR:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_2) {
                                    configureParamRange("rv_color", value, min, max);
                                    setParamData("rv_color", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_SPRING2_MIX:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_2) {
                                    configureParamRange("rv_mix", value, min, max);
                                    setParamData("rv_mix", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_SPRING3_TIME:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_3) {
                                    configureParamRange("rv_time", value, min, max, 0.5);
                                    setParamData("rv_time", parseInt(key));
                                }
                                break;
                            
                            case TONEX_PARAM_REVERB_SPRING3_PREDELAY:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_3) {
                                    configureParamRange("rv_predelay", value, min, max, 10);
                                    setParamData("rv_predelay", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_SPRING3_COLOR:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_3) {
                                    configureParamRange("rv_color", value, min, max);
                                    setParamData("rv_color", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_SPRING3_MIX:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_3) {
                                    configureParamRange("rv_mix", value, min, max);
                                    setParamData("rv_mix", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_SPRING4_TIME:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_4) {
                                    configureParamRange("rv_time", value, min, max, 0.5);
                                    setParamData("rv_time", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_SPRING4_PREDELAY:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_4) {
                                    configureParamRange("rv_predelay", value, min, max, 10);
                                    setParamData("rv_predelay", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_SPRING4_COLOR:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_4) {
                                    configureParamRange("rv_color", value, min, max);
                                    setParamData("rv_color", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_SPRING4_MIX:
                                if (getSelectValue("rv_model") == TONEX_REVERB_SPRING_4) {
                                    configureParamRange("rv_mix", value, min, max);
                                    setParamData("rv_mix", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_ROOM_TIME:
                                if (getSelectValue("rv_model") == TONEX_REVERB_ROOM) {
                                    configureParamRange("rv_time", value, min, max, 0.5);
                                    setParamData("rv_time", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_ROOM_PREDELAY:
                                if (getSelectValue("rv_model") == TONEX_REVERB_ROOM) {
                                    configureParamRange("rv_predelay", value, min, max, 10);
                                    setParamData("rv_predelay", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_ROOM_COLOR:
                                if (getSelectValue("rv_model") == TONEX_REVERB_ROOM) {
                                    configureParamRange("rv_color", value, min, max);
                                    setParamData("rv_color", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_ROOM_MIX:
                                if (getSelectValue("rv_model") == TONEX_REVERB_ROOM) {
                                    configureParamRange("rv_mix", value, min, max);
                                    setParamData("rv_mix", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_PLATE_TIME:
                                if (getSelectValue("rv_model") == TONEX_REVERB_PLATE) {
                                    configureParamRange("rv_time", value, min, max, 0.5);
                                    setParamData("rv_time", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_PLATE_PREDELAY:
                                if (getSelectValue("rv_model") == TONEX_REVERB_PLATE) {
                                    configureParamRange("rv_predelay", value, min, max, 10);
                                    setParamData("rv_predelay", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_PLATE_COLOR:
                                if (getSelectValue("rv_model") == TONEX_REVERB_PLATE) {
                                    configureParamRange("rv_color", value, min, max);
                                    setParamData("rv_color", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_REVERB_PLATE_MIX:
                                if (getSelectValue("rv_model") == TONEX_REVERB_PLATE) {
                                    configureParamRange("rv_mix", value, min, max);
                                    setParamData("rv_mix", parseInt(key));
                                }
                                break;  

                            // Modulation
                            case TONEX_PARAM_MODULATION_POST:
                                configureParamSwitch("mo_post", value);
                                setParamData("mo_post", parseInt(key));
                                break;

                            case TONEX_PARAM_MODULATION_ENABLE:
                                configureParamSwitch("mo_enable", value);
                                setParamData("mo_enable", parseInt(key));
                                break;

                            case TONEX_PARAM_MODULATION_MODEL:
                                configureParamSelect("mo_model", value);
                                setParamData("mo_model", parseInt(key));

                                // has model changed?
                                if (value !== curent_model_0)
                                {
                                    curent_model_0 = value;

                                    // set param labels
                                    if (getSelectValue("mo_model") == TONEX_MODULATION_CHORUS) {
                                        setParamLabel("mo_param1l", "Rate");
                                        setParamLabel("mo_param2l", "Depth");
                                        setParamLabel("mo_param3l", "Level");

                                        setParamVisible("mo_param4", false);
                                        setParamVisible("mo_param4l", false);
                                    }
                                    else if (getSelectValue("mo_model") == TONEX_MODULATION_TREMOLO) {
                                        setParamLabel("mo_param1l", "Rate");
                                        setParamLabel("mo_param2l", "Shape");
                                        setParamLabel("mo_param3l", "Spread");
                                        setParamLabel("mo_param4l", "Level");

                                        setParamVisible("mo_param4", true);
                                        setParamVisible("mo_param4l", true);
                                    }
                                    else if (getSelectValue("mo_model") == TONEX_MODULATION_PHASER) {
                                        setParamLabel("mo_param1l", "Rate");
                                        setParamLabel("mo_param2l", "Depth");
                                        setParamLabel("mo_param3l", "Level");

                                        setParamVisible("mo_param4", false);
                                        setParamVisible("mo_param4l", false);
                                    }
                                    else if (getSelectValue("mo_model") == TONEX_MODULATION_FLANGER) {
                                        setParamLabel("mo_param1l", "Rate");
                                        setParamLabel("mo_param2l", "Depth");
                                        setParamLabel("mo_param3l", "Feedback");
                                        setParamLabel("mo_param4l", "Level");

                                        setParamVisible("mo_param4", true);
                                        setParamVisible("mo_param4l", true);
                                    }
                                    else if (getSelectValue("mo_model") == TONEX_MODULATION_ROTARY) {
                                        setParamLabel("mo_param1l", "Speed");
                                        setParamLabel("mo_param2l", "Radius");
                                        setParamLabel("mo_param3l", "Spread");
                                        setParamLabel("mo_param4l", "Level");

                                        setParamVisible("mo_param4", true);
                                        setParamVisible("mo_param4l", true);
                                    }
                                }
                                break;

                            case TONEX_PARAM_MODULATION_CHORUS_SYNC:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_CHORUS) {
                                    configureParamSwitch("mo_sync", value);   
                                    setParamData("mo_sync", parseInt(key));
                                    showModTimeAndTimesign(value);
                                }
                                break;

                            case TONEX_PARAM_MODULATION_CHORUS_TS:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_CHORUS) {
                                    configureParamSelect("mo_timesign", value);
                                    setParamData("mo_timesign", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_CHORUS_RATE:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_CHORUS) {
                                    configureParamRange("mo_param1", value, min, max, 0.02);
                                    setParamData("mo_param1", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_CHORUS_DEPTH:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_CHORUS) {
                                    configureParamRange("mo_param2", value, min, max);
                                    setParamData("mo_param2", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_CHORUS_LEVEL:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_CHORUS) {
                                    configureParamRange("mo_param3", value, min, max, 0.1);
                                    setParamData("mo_param3", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_TREMOLO_SYNC:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_TREMOLO) {
                                    configureParamSwitch("mo_sync", value);
                                    setParamData("mo_sync", parseInt(key));
                                    showModTimeAndTimesign(value);
                                }
                                break;

                            case TONEX_PARAM_MODULATION_TREMOLO_TS:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_TREMOLO) {
                                    configureParamSelect("mo_timesign", value);
                                    setParamData("mo_timesign", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_TREMOLO_RATE:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_TREMOLO) {
                                    configureParamRange("mo_param1", value, min, max, 0.1);
                                    setParamData("mo_param1", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_TREMOLO_SHAPE:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_TREMOLO) {
                                    configureParamRange("mo_param2", value, min, max, 0.1);
                                    setParamData("mo_param2", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_TREMOLO_SPREAD:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_TREMOLO) {
                                    configureParamRange("mo_param3", value, min, max);
                                    setParamData("mo_param3", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_TREMOLO_LEVEL:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_TREMOLO) {
                                    configureParamRange("mo_param4", value, min, max, 0.1);
                                    setParamData("mo_param4", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_PHASER_SYNC:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_PHASER) {
                                    configureParamSwitch("mo_sync", value);
                                    setParamData("mo_sync", parseInt(key));
                                    showModTimeAndTimesign(value);
                                }
                                break;

                            case TONEX_PARAM_MODULATION_PHASER_TS:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_PHASER) {
                                    configureParamSelect("mo_timesign", value);
                                    setParamData("mo_timesign", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_PHASER_RATE:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_PHASER) {
                                    configureParamRange("mo_param1", value, min, max, 0.02);
                                    setParamData("mo_param1", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_PHASER_DEPTH:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_PHASER) {
                                    configureParamRange("mo_param2", value, min, max);
                                    setParamData("mo_param2", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_PHASER_LEVEL:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_PHASER) {
                                    configureParamRange("mo_param3", value, min, max, 0.1);
                                    setParamData("mo_param3", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_FLANGER_SYNC:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_FLANGER) {
                                    configureParamSwitch("mo_sync", value);
                                    setParamData("mo_sync", parseInt(key));
                                    showModTimeAndTimesign(value);
                                }
                                break;

                            case TONEX_PARAM_MODULATION_FLANGER_TS:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_FLANGER) {
                                    configureParamSelect("mo_timesign", value);
                                    setParamData("mo_timesign", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_FLANGER_RATE:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_FLANGER) {
                                    configureParamRange("mo_param1", value, min, max, 0.02);
                                    setParamData("mo_param1", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_FLANGER_DEPTH:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_FLANGER) {
                                    configureParamRange("mo_param2", value, min, max);
                                    setParamData("mo_param2", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_FLANGER_FEEDBACK:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_FLANGER) {
                                    configureParamRange("mo_param3", value, min, max);
                                    setParamData("mo_param3", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_FLANGER_LEVEL:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_FLANGER) {
                                    configureParamRange("mo_param4", value, min, max, 0.1);
                                    setParamData("mo_param4", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_ROTARY_SYNC:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_ROTARY) {
                                    configureParamSwitch("mo_sync", value);
                                    setParamData("mo_sync", parseInt(key));
                                    showModTimeAndTimesign(value);
                                }
                                break;

                            case TONEX_PARAM_MODULATION_ROTARY_TS:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_ROTARY) {
                                    configureParamSelect("mo_timesign", value);
                                    setParamData("mo_timesign", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_ROTARY_SPEED:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_ROTARY) {
                                    configureParamRange("mo_param1", value, min, max, 5);
                                    setParamData("mo_param1", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_ROTARY_RADIUS:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_ROTARY) {
                                    configureParamRange("mo_param2", value, min, max, 5);
                                    setParamData("mo_param2", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_ROTARY_SPREAD:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_ROTARY) {
                                    configureParamRange("mo_param3", value, min, max);
                                    setParamData("mo_param3", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_MODULATION_ROTARY_LEVEL:
                                if (getSelectValue("mo_model") == TONEX_MODULATION_ROTARY) {
                                    configureParamRange("mo_param4", value, min, max, 0.1);
                                    setParamData("mo_param4", parseInt(key));
                                }
                                break;
                            
                            // Delay
                            case TONEX_PARAM_DELAY_POST:
                                configureParamSwitch("dl_post", value);
                                setParamData("dl_post", parseInt(key));
                                break;

                            case TONEX_PARAM_DELAY_ENABLE:
                                configureParamSwitch("dl_enable", value);
                                setParamData("dl_enable", parseInt(key));
                                break;

                            case TONEX_PARAM_DELAY_MODEL:
                                configureParamSelect("dl_model", value);
                                setParamData("dl_model", parseInt(key));
                                break;

                            case TONEX_PARAM_DELAY_DIGITAL_SYNC:
                                if (getSelectValue("dl_model") == TONEX_DELAY_DIGITAL) {
                                    configureParamSwitch("dl_sync", value);
                                    setParamData("dl_sync", parseInt(key));
                                    showDelayTimeAndTimesign(value);
                                }
                                break;

                            case TONEX_PARAM_DELAY_DIGITAL_TS:
                                if (getSelectValue("dl_model") == TONEX_DELAY_DIGITAL) {
                                    configureParamSelect("dl_timesign", value);
                                    setParamData("dl_timesign", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_DELAY_DIGITAL_TIME:
                                if (getSelectValue("dl_model") == TONEX_DELAY_DIGITAL) {
                                    configureParamRange("dl_time", value, min, max, 10);
                                    setParamData("dl_time", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_DELAY_DIGITAL_FEEDBACK:
                                if (getSelectValue("dl_model") == TONEX_DELAY_DIGITAL) {
                                    configureParamRange("dl_feedback", value, min, max);
                                    setParamData("dl_feedback", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_DELAY_DIGITAL_MODE:
                                if (getSelectValue("dl_model") == TONEX_DELAY_DIGITAL) {
                                    configureParamSwitch("dl_pingpong", value);
                                    setParamData("dl_pingpong", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_DELAY_DIGITAL_MIX:
                                if (getSelectValue("dl_model") == TONEX_DELAY_DIGITAL) {
                                    configureParamRange("dl_mix", value, min, max);
                                    setParamData("dl_mix", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_DELAY_TAPE_SYNC:
                                if (getSelectValue("dl_model") == TONEX_DELAY_TAPE) {
                                    configureParamSwitch("dl_sync", value);
                                    setParamData("dl_sync", parseInt(key));
                                    showDelayTimeAndTimesign(value);
                                }
                                break;
                                
                            case TONEX_PARAM_DELAY_TAPE_TS:
                                if (getSelectValue("dl_model") == TONEX_DELAY_TAPE) {
                                    configureParamSelect("dl_timesign", value);
                                    setParamData("dl_timesign", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_DELAY_TAPE_TIME:
                                if (getSelectValue("dl_model") == TONEX_DELAY_TAPE) {
                                    configureParamRange("dl_time", value, min, max, 10);
                                    setParamData("dl_time", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_DELAY_TAPE_FEEDBACK:
                                if (getSelectValue("dl_model") == TONEX_DELAY_TAPE) {
                                    configureParamRange("dl_feedback", value, min, max);
                                    setParamData("dl_feedback", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_DELAY_TAPE_MODE:
                                if (getSelectValue("dl_model") == TONEX_DELAY_TAPE) {
                                    configureParamSwitch("dl_pingpong", value);
                                    setParamData("dl_pingpong", parseInt(key));
                                }
                                break;

                            case TONEX_PARAM_DELAY_TAPE_MIX:
                                if (getSelectValue("dl_model") == TONEX_DELAY_TAPE) {
                                    configureParamRange("dl_mix", value, min, max);
                                    setParamData("dl_mix", parseInt(key));
                                }
                                break;

                            case TONEX_GLOBAL_BPM:
                                configureParamRange("gb_bpm", value, min, max);
                                setParamData("gb_bpm", parseInt(key));
                                break;

                            case TONEX_GLOBAL_INPUT_TRIM:
                                configureParamRange("gb_trim", value, min, max, 0.1);
                                setParamData("gb_trim", parseInt(key));
                                break;

                            case TONEX_GLOBAL_CABSIM_BYPASS:
                                configureParamSwitch("gb_cabsim", value);
                                setParamData("gb_cabsim", parseInt(key));
                                break;

                            case TONEX_GLOBAL_TEMPO_SOURCE: 
                                configureParamSwitch("gb_tempos", value);
                                setParamData("gb_tempos", parseInt(key));
                                break;

                            case TONEX_GLOBAL_TUNING_REFERENCE:
                                configureParamRange("gb_tref", value, min, max, 1);
                                setParamData("gb_tref", parseInt(key));
                                break;

                            case TONEX_GLOBAL_BYPASS:
                                configureParamSwitch("gb_bypass", value);
                                setParamData("gb_bypass", parseInt(key));
                                break;

                            case TONEX_GLOBAL_MASTER_VOLUME:
                                configureParamRange("gb_vol", value, min, max, 0.1);
                                setParamData("gb_vol", parseInt(key));
                                break;
                        }
                    } 
                } break;

                case AMP_MODELLER_VALETON_GP5:
                {    
                    for (var key in data['PARAMS']) {
                        //console.log(data['PARAMS'][key]);
                        var min = data['PARAMS'][key]['Min'];
                        var max = data['PARAMS'][key]['Max'];
                        var value = data['PARAMS'][key]['Val'];

                        switch (parseInt(key))
                        {
                            // Block enables
                            case ValetonParameters.VALETON_PARAM_NR_ENABLE: {
                                configureParamSwitch("val_nr_enable", value);
                                setParamData("val_nr_enable", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_PRE_ENABLE: {
                                configureParamSwitch("val_pre_enable", value);
                                setParamData("val_pre_enable", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_DIST_ENABLE: {
                                configureParamSwitch("val_dst_enable", value);
                                setParamData("val_dst_enable", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_AMP_ENABLE: {
                                configureParamSwitch("val_amp_enable", value);
                                setParamData("val_amp_enable", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_CAB_ENABLE: {
                                configureParamSwitch("val_cab_enable", value);
                                setParamData("val_cab_enable", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_EQ_ENABLE: {
                                configureParamSwitch("val_eq_enable", value);
                                setParamData("val_eq_enable", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_MOD_ENABLE: {
                                configureParamSwitch("val_mod_enable", value);
                                setParamData("val_mod_enable", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_DLY_ENABLE: {
                                configureParamSwitch("val_dly_enable", value);
                                setParamData("val_dly_enable", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_RVB_ENABLE: {
                                configureParamSwitch("val_rvb_enable", value);
                                setParamData("val_rvb_enable", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_NS_ENABLE: {
                                configureParamSwitch("val_ns_enable", value);
                                setParamData("val_ns_enable", parseInt(key));
                                break;
                            }

                            // Models
                            case ValetonParameters.VALETON_PARAM_NR_TYPE: {
                                configureParamSelect("val_nr_type", value);
                                setParamData("val_nr_type", parseInt(key));

                                // has model changed?
                                if (value !== curent_model_0)
                                {
                                    curent_model_0 = value;
                                
                                    // set params
                                    configureParam('val_nr_param0', true,  "Threshold");
                                }
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_PRE_TYPE: {
                                configureParamSelect("val_pre_type", value);
                                setParamData("val_pre_type", parseInt(key));

                                // has model changed?
                                if (value !== curent_model_1)
                                {
                                    curent_model_1 = value;

                                    // set params
                                    switch (value)
                                    {
                                        case ValetonEffectPre.VALETON_EFFECT_PRE_COMP:
                                        {
                                            configureParam('val_pre_param0', true,  "Sustain");
                                            configureParam('val_pre_param1', true,  "Volume");
                                            configureParam('val_pre_param2', false);
                                            configureParam('val_pre_param3', false);
                                            configureParam('val_pre_param4', false);
                                        } break;

                                        case ValetonEffectPre.VALETON_EFFECT_PRE_COMP4:
                                        {
                                            configureParam('val_pre_param0', true,  "Sustain");
                                            configureParam('val_pre_param1', true,  "Attack");
                                            configureParam('val_pre_param2', true,  "Volume");
                                            configureParam('val_pre_param3', true,  "Clip");
                                            configureParam('val_pre_param4', false);
                                        } break;

                                        case ValetonEffectPre.VALETON_EFFECT_PRE_BOOST:
                                        {
                                            configureParam('val_pre_param0', true,  "Gain");
                                            configureParam('val_pre_param1', false);
                                            configureParam('val_pre_param2', false);
                                            configureParam('val_pre_param3', false);
                                            configureParam('val_pre_param4', false);
                                            // TODO: 2 switches
                                        } break;

                                        case ValetonEffectPre.VALETON_EFFECT_PRE_MICRO_BOOST:
                                        {
                                            configureParam('val_pre_param0', true,  "Gain");
                                            configureParam('val_pre_param1', false);
                                            configureParam('val_pre_param2', false);
                                            configureParam('val_pre_param3', false);
                                            configureParam('val_pre_param4', false);
                                        } break;

                                        case ValetonEffectPre.VALETON_EFFECT_PRE_B_BOOST:
                                        {
                                            configureParam('val_pre_param0', true,  "Gain");
                                            configureParam('val_pre_param1', true,  "Volume");
                                            configureParam('val_pre_param2', true,  "Bass");
                                            configureParam('val_pre_param3', true,  "Treble");
                                            configureParam('val_pre_param4', false);
                                        } break;

                                        case ValetonEffectPre.VALETON_EFFECT_PRE_TOUCHER:
                                        {
                                            configureParam('val_pre_param0', true,  "Sense");
                                            configureParam('val_pre_param1', true,  "Range");
                                            configureParam('val_pre_param2', true,  "Q");
                                            configureParam('val_pre_param3', true,  "Mix");
                                            configureParam('val_pre_param4', false);
                                            // TODO: switch here
                                        } break;

                                        case ValetonEffectPre.VALETON_EFFECT_PRE_CRIER:
                                        {
                                            configureParam('val_pre_param0', true,  "Depth");
                                            configureParam('val_pre_param1', true,  "Rate");
                                            configureParam('val_pre_param2', true,  "Volume");
                                            configureParam('val_pre_param3', true,  "Low");
                                            configureParam('val_pre_param4', true,  "Q");
                                        } break;

                                        case ValetonEffectPre.VALETON_EFFECT_PRE_OCTA:
                                        {
                                            configureParam('val_pre_param0', true,  "Low");
                                            configureParam('val_pre_param1', true,  "High");
                                            configureParam('val_pre_param2', true,  "Dry");
                                            configureParam('val_pre_param3', false);
                                            configureParam('val_pre_param4', false);
                                        } break;

                                        case ValetonEffectPre.VALETON_EFFECT_PRE_PITCH:
                                        {
                                            configureParam('val_pre_param0', true,  "High");
                                            configureParam('val_pre_param1', true,  "Low");
                                            configureParam('val_pre_param2', true,  "Dry");
                                            configureParam('val_pre_param3', true,  "H-Vol");
                                            configureParam('val_pre_param4', true,  "L-Vol");
                                        } break;

                                        case ValetonEffectPre.VALETON_EFFECT_PRE_DETUNE:
                                        {
                                            configureParam('val_pre_param0', true,  "Detune");
                                            configureParam('val_pre_param1', true,  "Wet");
                                            configureParam('val_pre_param2', true,  "Dry");
                                            configureParam('val_pre_param3', false);
                                            configureParam('val_pre_param4', false);
                                        } break;
                                    }
                                } 
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_DIST_TYPE: {
                                configureParamSelect("val_dst_type", value);
                                setParamData("val_dst_type", parseInt(key));

                                // has model changed?
                                if (value !== curent_model_2)
                                {
                                    curent_model_2 = value;

                                    switch (value) {
                                        case ValetonEffectDist.VALETON_EFFECT_DIST_GREEN_OD:
                                        case ValetonEffectDist.VALETON_EFFECT_DIST_SUPER_OD:
                                        case ValetonEffectDist.VALETON_EFFECT_DIST_SM_DIST:
                                        case ValetonEffectDist.VALETON_EFFECT_DIST_LA_CHARGER:
                                        {
                                            configureParam('val_dst_param0', true,  "Gain");
                                            configureParam('val_dst_param1', true,  "Tone");
                                            configureParam('val_dst_param2', true,  "Volume");
                                            configureParam('val_dst_param3', false);
                                            configureParam('val_dst_param4', false);
                                        } break;

                                        case ValetonEffectDist.VALETON_EFFECT_DIST_YELLOW_OD:
                                        case ValetonEffectDist.VALETON_EFFECT_DIST_PLUSTORTION:
                                        {
                                            configureParam('val_dst_param0', true,  "Gain");
                                            configureParam('val_dst_param1', true,  "Volume");
                                            configureParam('val_dst_param2', false);
                                            configureParam('val_dst_param3', false);
                                            configureParam('val_dst_param4', false);
                                        } break;

                                        case ValetonEffectDist.VALETON_EFFECT_DIST_DARKTALE:
                                        {
                                            configureParam('val_dst_param0', true,  "Gain");
                                            configureParam('val_dst_param1', true,  "Filter");
                                            configureParam('val_dst_param2', true,  "Volume");
                                            configureParam('val_dst_param3', false);
                                            configureParam('val_dst_param4', false);
                                        } break;

                                        case ValetonEffectDist.VALETON_EFFECT_DIST_SORA_FUZZ:
                                        case ValetonEffectDist.VALETON_EFFECT_DIST_RED_HAZE:
                                        {
                                            configureParam('val_dst_param0', true,  "Fuzz");
                                            configureParam('val_dst_param1', true,  "Volume");
                                            configureParam('val_dst_param2', false);
                                            configureParam('val_dst_param3', false);
                                            configureParam('val_dst_param4', false);
                                        } break;

                                        case ValetonEffectDist.VALETON_EFFECT_DIST_BASS_OD:
                                        {
                                            configureParam('val_dst_param0', true,  "Gain");
                                            configureParam('val_dst_param1', true,  "Blend");
                                            configureParam('val_dst_param2', true,  "Volume");
                                            configureParam('val_dst_param3', true,  "Bass");
                                            configureParam('val_dst_param4', true,  "Treble");
                                        } break;

                                        default:
                                        {
                                            configureParam('val_dst_param0', false);
                                            configureParam('val_dst_param1', false);
                                            configureParam('val_dst_param2', false);
                                            configureParam('val_dst_param3', false);
                                            configureParam('val_dst_param4', false);
                                        } break;
                                    }
                                }
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_AMP_TYPE: {
                                configureParamSelect("val_amp_type", value);
                                setParamData("val_amp_type", parseInt(key));

                                // has model changed?
                                if (value !== curent_model_3)
                                {
                                    curent_model_3 = value;

                                    switch (value) {

                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_TWEEDY:
                                        {
                                            configureParam('val_amp_param0', true,  "Gain");
                                            configureParam('val_amp_param1', true,  "Tone");
                                            configureParam('val_amp_param2', true,  "Volume");
                                            configureParam('val_amp_param3', false);
                                            configureParam('val_amp_param4', false);
                                            configureParam('val_amp_param5', false);
                                            configureParam('val_amp_param6', false);
                                        } break;

                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_BELLMAN_59N:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_MATCH_CL:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_L_STAR_CL:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_UK_45:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_UK_800:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_BELLMAN_59B:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_SOLO100_OD:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_BAD_KT_OD:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_DIZZ_VH:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_DIZZ_VH_PLUS:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_EAGLE_120:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_SOLO100_LD:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_MESS_DUALV:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_MESS_DUALM:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_POWER_LD:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_FLAGMAN_PLUS:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_BOG_REDV:
                                        {
                                            configureParam('val_amp_param0', true,  "Gain");
                                            configureParam('val_amp_param1', true,  "Presence");
                                            configureParam('val_amp_param2', true,  "Volume");
                                            configureParam('val_amp_param3', true,  "Bass");
                                            configureParam('val_amp_param4', true,  "Middle");
                                            configureParam('val_amp_param5', true,  "Treble");
                                            configureParam('val_amp_param6', false);
                                        } break;

                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_DARK_TWIN:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_JUICE_R100:
                                        {
                                            configureParam('val_amp_param0', true,  "Gain");
                                            configureParam('val_amp_param1', true,  "Volume");
                                            configureParam('val_amp_param2', true,  "Bass");
                                            configureParam('val_amp_param3', true,  "Middle");
                                            configureParam('val_amp_param4', true,  "Treble");
                                            // TODO: Bright switch
                                            configureParam('val_amp_param5', false);
                                            configureParam('val_amp_param6', false);
                                        } break;

                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_FOXY_30N:
                                        {
                                            configureParam('val_amp_param0', true,  "Gain");
                                            configureParam('val_amp_param1', true,  "Tone Cut");
                                            configureParam('val_amp_param2', true,  "Volume");
                                            //todo bright switch
                                            configureParam('val_amp_param3', false);
                                            configureParam('val_amp_param4', false);
                                            configureParam('val_amp_param5', false);
                                            configureParam('val_amp_param6', false);
                                        } break;

                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_J_120_CL:
                                        {
                                            configureParam('val_amp_param0', true,  "Volume");
                                            configureParam('val_amp_param1', true,  "Bass");
                                            configureParam('val_amp_param2', true,  "Middle");
                                            configureParam('val_amp_param3', true,  "Treble");                                        
                                            //todo bright switch                                            
                                            configureParam('val_amp_param4', false);
                                            configureParam('val_amp_param5', false);
                                            configureParam('val_amp_param6', false);
                                        } break;
                                        
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_UK_50JP:
                                        {
                                            configureParam('val_amp_param0', true,  "Gain 1");
                                            configureParam('val_amp_param1', true,  "Presence");
                                            configureParam('val_amp_param2', true,  "Volume");
                                            configureParam('val_amp_param3', true,  "Bass");
                                            configureParam('val_amp_param4', true,  "Middle");
                                            configureParam('val_amp_param5', true,  "Treble");
                                            configureParam('val_amp_param6', true,  "Gain 2");
                                        } break;

                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_FOXY_30TB:
                                        {
                                            configureParam('val_amp_param0', true,  "Gain");
                                            configureParam('val_amp_param1', true,  "Tone Cut");
                                            configureParam('val_amp_param2', true,  "Volume");
                                            configureParam('val_amp_param3', true,  "Bass");
                                            configureParam('val_amp_param4', true,  "Treble");
                                            //todo character switch
                                            configureParam('val_amp_param5', false);
                                            configureParam('val_amp_param6', false);
                                        } break;

                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_SUPDUAL_OD:
                                        {
                                            configureParam('val_amp_param0', true,  "Gain 1");
                                            configureParam('val_amp_param1', true,  "Tone 1");
                                            configureParam('val_amp_param2', true,  "Gain 2");
                                            configureParam('val_amp_param3', true,  "Tone 2");
                                            configureParam('val_amp_param4', true,  "Volume");
                                            configureParam('val_amp_param5', false);
                                            configureParam('val_amp_param6', false);
                                        } break;

                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_Z38_OD:
                                        {
                                            configureParam('val_amp_param0', true,  "Gain");
                                            configureParam('val_amp_param1', true,  "Tone Cut");
                                            configureParam('val_amp_param2', true,  "Volume");
                                            configureParam('val_amp_param3', true,  "Bass");
                                            configureParam('val_amp_param4', true,  "Middle");
                                            configureParam('val_amp_param5', false);
                                            configureParam('val_amp_param6', false);
                                        } break;

                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_EV_51:
                                        {
                                            configureParam('val_amp_param0', true,  "Gain");
                                            configureParam('val_amp_param1', true,  "Volume");
                                            configureParam('val_amp_param2', true,  "Bass");
                                            configureParam('val_amp_param3', true,  "Mid");
                                            configureParam('val_amp_param4', true,  "Middle");
                                            configureParam('val_amp_param5', false);
                                            configureParam('val_amp_param6', false);
                                        } break;

                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_CLASSIC_BASS:
                                        {
                                            configureParam('val_amp_param0', true,  "Gain");
                                            configureParam('val_amp_param1', true,  "Bass");
                                            configureParam('val_amp_param2', true,  "Middle");
                                            configureParam('val_amp_param3', true,  "Mid Freq");
                                            configureParam('val_amp_param4', true,  "Treble");
                                            configureParam('val_amp_param5', true,  "Volume");
                                            configureParam('val_amp_param6', false);
                                        } break;

                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_FOXY_BASS:
                                        {
                                            configureParam('val_amp_param0', true,  "Volume");
                                            configureParam('val_amp_param1', true,  "Bass");
                                            configureParam('val_amp_param2', true,  "Treble");
                                            configureParam('val_amp_param3', false);
                                            configureParam('val_amp_param4', false);
                                            configureParam('val_amp_param5', false);
                                            configureParam('val_amp_param6', false);
                                        } break;

                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_MESS_BASS:
                                        {
                                            configureParam('val_amp_param0', true,  "Gain");
                                            configureParam('val_amp_param1', true,  "Volume");
                                            configureParam('val_amp_param2', true,  "Bass");
                                            configureParam('val_amp_param3', true,  "Mid");
                                            configureParam('val_amp_param4', true,  "Treble");
                                            configureParam('val_amp_param5', false);
                                            configureParam('val_amp_param6', false);
                                        } break;  
                                        
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_AC_PRE1:
                                        case ValetonEffectAmp.VALETON_EFFECT_AMP_AC_PRE2:
                                        {
                                            configureParam('val_amp_param0', true,  "Volume");
                                            configureParam('val_amp_param1', true,  "Tone");
                                            configureParam('val_amp_param2', true,  "Balance");
                                            configureParam('val_amp_param3', true,  "EQ Freq");
                                            configureParam('val_amp_param4', true,  "EQ Q");
                                            configureParam('val_amp_param5', true,  "EQ Gain");
                                            configureParam('val_amp_param6', false);
                                        } break;  
                                    }
                                }
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_CAB_TYPE: {
                                configureParamSelect("val_cab_type", value);
                                setParamData("val_cab_type", parseInt(key));

                                // has model changed?
                                if (value !== curent_model_4)
                                {
                                    curent_model_4 = value;

                                    configureParam('val_cab_param0', true,  "Volume");
                                    configureParam('val_cab_param1', false);
                                    configureParam('val_cab_param2', false);
                                    configureParam('val_cab_param3', false);
                                    configureParam('val_cab_param4', false);
                                }
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_EQ_TYPE: {
                                configureParamSelect("val_eq_type", value);
                                setParamData("val_eq_type", parseInt(key));

                                // has model changed?
                                if (value !== curent_model_5)
                                {
                                    curent_model_5 = value;

                                    switch (value) {
                                        case ValetonEffectEQ.VALETON_EFFECT_EQ_GUITAR_EQ1:
                                        {
                                            configureParam('val_eq_param0', true, "125 Hz");
                                            configureParam('val_eq_param1', true, "400 Hz");
                                            configureParam('val_eq_param2', true, "800 Hz");
                                            configureParam('val_eq_param3', true, "1.6 kHz");
                                            configureParam('val_eq_param4', true, "4 kHz");
                                            configureParam('val_eq_param5', true, "Volume");
                                        } break;

                                        case ValetonEffectEQ.VALETON_EFFECT_EQ_GUITAR_EQ2:
                                        {
                                            configureParam('val_eq_param0', true, "100 Hz");
                                            configureParam('val_eq_param1', true, "500 Hz");
                                            configureParam('val_eq_param2', true, "1 kHz");
                                            configureParam('val_eq_param3', true, "3 kHz");
                                            configureParam('val_eq_param4', true, "6 kHz");
                                            configureParam('val_eq_param5', true, "Volume");
                                        } break;

                                        case ValetonEffectEQ.VALETON_EFFECT_EQ_BASS_EQ1:
                                        {
                                            configureParam('val_eq_param0', true, "33 Hz");
                                            configureParam('val_eq_param1', true, "150 Hz");
                                            configureParam('val_eq_param2', true, "600 Hz");
                                            configureParam('val_eq_param3', true, "2 kHz");
                                            configureParam('val_eq_param4', true, "8 kHz");
                                            configureParam('val_eq_param5', true, "Volume");
                                        } break;

                                        case ValetonEffectEQ.VALETON_EFFECT_EQ_BASS_EQ2:
                                        {
                                            configureParam('val_eq_param0', true, "50 Hz");
                                            configureParam('val_eq_param1', true, "120 Hz");
                                            configureParam('val_eq_param2', true, "400 Hz");
                                            configureParam('val_eq_param3', true, "800 Hz");
                                            configureParam('val_eq_param4', true, "4.5 kHz");
                                            configureParam('val_eq_param5', true, "Volume");
                                        } break;

                                        case ValetonEffectEQ.VALETON_EFFECT_EQ_MESS_EQ:
                                        {
                                            configureParam('val_eq_param0', true, "80 Hz");
                                            configureParam('val_eq_param1', true, "240 Hz");
                                            configureParam('val_eq_param2', true, "750 Hz");
                                            configureParam('val_eq_param3', true, "2.2 kHz");
                                            configureParam('val_eq_param4', true, "6.6 kHz");
                                            configureParam('val_eq_param5', false);
                                        } break;

                                        default:
                                        {
                                            configureParam('val_eq_param0', false);
                                            configureParam('val_eq_param1', false);
                                            configureParam('val_eq_param2', false);
                                            configureParam('val_eq_param3', false);
                                            configureParam('val_eq_param4', false);
                                            configureParam('val_eq_param5', false);
                                        } break;
                                    }
                                }
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_MOD_TYPE: {
                                configureParamSelect("val_mod_type", value);
                                setParamData("val_mod_type", parseInt(key));

                                // has model changed?
                                if (value !== curent_model_6)
                                {
                                    curent_model_6 = value;

                                    switch (value) {
                                        case ValetonEffectMod.VALETON_EFFECT_MOD_A_CHORUS:
                                        case ValetonEffectMod.VALETON_EFFECT_MOD_B_CHORUS:
                                        {
                                            configureParam('val_mod_param0', true,  "Depth");
                                            configureParam('val_mod_param1', true,  "Rate");
                                            configureParam('val_mod_param2', true,  "Tone");
                                            configureParam('val_mod_param3', false);
                                            configureParam('val_mod_param4', false);
                                        } break;

                                        case ValetonEffectMod.VALETON_EFFECT_MOD_JET:
                                        case ValetonEffectMod.VALETON_EFFECT_MOD_N_JET:
                                        {
                                            configureParam('val_mod_param0', true,  "Depth");
                                            configureParam('val_mod_param1', true,  "Rate");
                                            configureParam('val_mod_param2', true,  "P.Delay");
                                            configureParam('val_mod_param3', true,  "F.Back");
                                            configureParam('val_mod_param4', false);
                                        } break;

                                        case ValetonEffectMod.VALETON_EFFECT_MOD_O_PHASE:
                                        {
                                            configureParam('val_mod_param0', true,  "Rate");
                                            configureParam('val_mod_param1', false);
                                            configureParam('val_mod_param2', false);
                                            configureParam('val_mod_param3', false);
                                            configureParam('val_mod_param4', false);
                                        } break;

                                        case ValetonEffectMod.VALETON_EFFECT_MOD_M_VIBE:
                                        case ValetonEffectMod.VALETON_EFFECT_MOD_V_ROTO:
                                        case ValetonEffectMod.VALETON_EFFECT_MOD_O_TREM:
                                        {
                                            configureParam('val_mod_param0', true,  "Depth");
                                            configureParam('val_mod_param1', true,  "Rate");
                                            configureParam('val_mod_param2', false);
                                            configureParam('val_mod_param3', false);
                                            configureParam('val_mod_param4', false);
                                        } break;

                                        case ValetonEffectMod.VALETON_EFFECT_MOD_VIBRATO:
                                        case ValetonEffectMod.VALETON_EFFECT_MOD_SINE_TREM:
                                        {
                                            configureParam('val_mod_param0', true,  "Depth");
                                            configureParam('val_mod_param1', true,  "Rate");
                                            configureParam('val_mod_param2', true,  "Volume");
                                            configureParam('val_mod_param3', false);
                                            configureParam('val_mod_param4', false);
                                        } break;

                                        case ValetonEffectMod.VALETON_EFFECT_MOD_BIAS_TREM:
                                        {
                                            configureParam('val_mod_param0', true,  "Depth");
                                            configureParam('val_mod_param1', true,  "Rate");
                                            configureParam('val_mod_param2', true,  "Volume");
                                            configureParam('val_mod_param3', true,  "Bias");
                                            configureParam('val_mod_param4', false);
                                        } break;

                                        default:
                                        {
                                            configureParam('val_mod_param0', false);
                                            configureParam('val_mod_param1', false);
                                            configureParam('val_mod_param2', false);
                                            configureParam('val_mod_param3', false);
                                            configureParam('val_mod_param4', false);
                                        } break;
                                    }
                                }
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_DLY_TYPE: {
                                configureParamSelect("val_dly_type", value);
                                setParamData("val_dly_type", parseInt(key));

                                // has model changed?
                                if (value !== curent_model_7)
                                {
                                    curent_model_7 = value;

                                    switch (value) {
                                        case ValetonEffectDly.VALETON_EFFECT_DLY_PURE:
                                        case ValetonEffectDly.VALETON_EFFECT_DLY_SWEET_ECHO:
                                        case ValetonEffectDly.VALETON_EFFECT_DLY_TAPE:
                                        case ValetonEffectDly.VALETON_EFFECT_DLY_TUBE:
                                        case ValetonEffectDly.VALETON_EFFECT_DLY_ANALOG:
                                        case ValetonEffectDly.VALETON_EFFECT_DLY_SLAPBACK:
                                        case ValetonEffectDly.VALETON_EFFECT_DLY_REV_ECHO:
                                        case ValetonEffectDly.VALETON_EFFECT_DLY_PING_PONG:
                                        {
                                            configureParam('val_dly_param0', true,  "Mix");
                                            configureParam('val_dly_param1', true,  "Time");
                                            configureParam('val_dly_param2', true,  "F.Back");
                                            configureParam('val_dly_param3', false);
                                            configureParam('val_dly_param4', false);
                                            configureParam('val_dly_param5', false);
                                            //todo configureParam('val_dly_param6', true,  "Trail");
                                            configureParam('val_dly_param6', false);
                                        } break;

                                        case ValetonEffectDly.VALETON_EFFECT_DLY_RING_ECHO:
                                        {
                                            configureParam('val_dly_param0', true,  "Mix");
                                            configureParam('val_dly_param1', true,  "Time");
                                            configureParam('val_dly_param2', true,  "F.Back");
                                            configureParam('val_dly_param3', true,  "R-Mix");
                                            configureParam('val_dly_param4', true,  "Freq");
                                            configureParam('val_dly_param5', true,  "Tone");
                                            //todo configureParam('val_dly_param6', true,  "Trail");
                                            configureParam('val_dly_param6', false);
                                        } break;

                                        case ValetonEffectDly.VALETON_EFFECT_DLY_SWEEP_ECHO:
                                        {
                                            configureParam('val_dly_param0', true,  "Mix");
                                            configureParam('val_dly_param1', true,  "Time");
                                            configureParam('val_dly_param2', true,  "F.Back");
                                            configureParam('val_dly_param3', true,  "S-Depth");
                                            configureParam('val_dly_param4', true,  "S-Rate");
                                            //to do trail switch on param 5
                                            configureParam('val_dly_param5', false);
                                            configureParam('val_dly_param6', false);
                                        } break;

                                        default:
                                        {
                                            configureParam('val_dly_param0', false);
                                            configureParam('val_dly_param1', false);
                                            configureParam('val_dly_param2', false);
                                            configureParam('val_dly_param3', false);
                                            configureParam('val_dly_param4', false);
                                            configureParam('val_dly_param5', false);
                                            configureParam('val_dly_param6', false);
                                        } break;
                                    }
                                }
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_RVB_TYPE: {
                                configureParamSelect("val_rvb_type", value);
                                setParamData("val_rvb_type", parseInt(key));

                                // has model changed?
                                if (value !== curent_model_8)
                                {
                                    curent_model_8 = value;

                                    switch (value) {
                                        case ValetonEffectRvb.VALETON_EFFECT_RVB_AIR:
                                        case ValetonEffectRvb.VALETON_EFFECT_RVB_PLATE:
                                        {
                                            configureParam('val_rvb_param0', true,  "Mix");
                                            configureParam('val_rvb_param1', true,  "Decay");
                                            configureParam('val_rvb_param2', true,  "Damp");
                                            configureParam('val_rvb_param3', false);
                                            configureParam('val_rvb_param4', false);
                                            // TODO: Trail switch
                                        } break;

                                        case ValetonEffectRvb.VALETON_EFFECT_RVB_ROOM:
                                        case ValetonEffectRvb.VALETON_EFFECT_RVB_HALL:
                                        case ValetonEffectRvb.VALETON_EFFECT_RVB_CHURCH:
                                        case ValetonEffectRvb.VALETON_EFFECT_RVB_PLATE_L:
                                        case ValetonEffectRvb.VALETON_EFFECT_RVB_SPRING:
                                        case ValetonEffectRvb.VALETON_EFFECT_RVB_N_STAR:
                                        case ValetonEffectRvb.VALETON_EFFECT_RVB_DEEPSEA:
                                        {
                                            configureParam('val_rvb_param0', true,  "Mix");
                                            configureParam('val_rvb_param1', true,  "Decay");
                                            configureParam('val_rvb_param2', false);
                                            configureParam('val_rvb_param3', false);
                                            configureParam('val_rvb_param4', false);
                                            // TODO: Trail switch
                                        } break;

                                        case ValetonEffectRvb.VALETON_EFFECT_RVB_SWEET_SPACE:
                                        {
                                            configureParam('val_rvb_param0', true,  "Mix");
                                            configureParam('val_rvb_param1', true,  "Decay");
                                            configureParam('val_rvb_param2', true,  "Damp");
                                            configureParam('val_rvb_param3', true,  "Mod");
                                            configureParam('val_rvb_param4', false);
                                            // TODO: Trail switch
                                        } break;

                                        default:
                                        {
                                            configureParam('val_rvb_param0', false);
                                            configureParam('val_rvb_param1', false);
                                            configureParam('val_rvb_param2', false);
                                            configureParam('val_rvb_param3', false);
                                            configureParam('val_rvb_param4', false);
                                        } break;
                                    }
                                }
                               break;
                            }
                            case ValetonParameters.VALETON_PARAM_NS_TYPE: {
                                configureParamSelect("val_ns_type", value);
                                setParamData("val_ns_type", parseInt(key));

                                // has model changed?
                                if (value !== curent_model_9)
                                {
                                    curent_model_9 = value;

                                    configureParam('val_ns_param0', true,  "Gain");
                                    configureParam('val_ns_param1', true,  "Volume");
                                    configureParam('val_ns_param2', true,  "Bass");
                                    configureParam('val_ns_param3', true,  "Middle");
                                    configureParam('val_ns_param4', true,  "Treble");
                                }
                                break;
                            }

                            // General
                            case ValetonParameters.VALETON_PARAM_PATCH_VOLUME: {
                                //todo
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_EFFECT_SLOT_0: {
                                effect_icon_order[0] = value;
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_EFFECT_SLOT_1: {
                                effect_icon_order[1] = value;
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_EFFECT_SLOT_2: {
                                effect_icon_order[2] = value;
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_EFFECT_SLOT_3: {
                                effect_icon_order[3] = value;
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_EFFECT_SLOT_4: {
                                effect_icon_order[4] = value;
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_EFFECT_SLOT_5: {
                                effect_icon_order[5] = value;
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_EFFECT_SLOT_6: {
                                effect_icon_order[6] = value;
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_EFFECT_SLOT_7: {
                                effect_icon_order[7] = value;
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_EFFECT_SLOT_8: {
                                effect_icon_order[8] = value;
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_EFFECT_SLOT_9: {
                                effect_icon_order[9] = value;
                                break;
                            }

                            // NR parameters
                            case ValetonParameters.VALETON_PARAM_NR_PARAM_0: {
                                configureParamRange("val_nr_param0", value, min, max);
                                setParamData("val_nr_param0", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_PARAM_NR_PARAM_1: 
                            case ValetonParameters.VALETON_PARAM_NR_PARAM_2: 
                            case ValetonParameters.VALETON_PARAM_NR_PARAM_3: 
                            case ValetonParameters.VALETON_PARAM_NR_PARAM_4: 
                            case ValetonParameters.VALETON_PARAM_NR_PARAM_5:
                            case ValetonParameters.VALETON_PARAM_NR_PARAM_6: 
                            case ValetonParameters.VALETON_PARAM_NR_PARAM_7: {
                                // not used
                                break;
                            }

                            // Pre parameters
                            case ValetonParameters.VALETON_PARAM_PRE_PARAM_0:
                                configureParamRange("val_pre_param0", value, min, max);
                                setParamData("val_pre_param0", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_PRE_PARAM_1:
                                configureParamRange("val_pre_param1", value, min, max);
                                setParamData("val_pre_param1", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_PRE_PARAM_2:
                                configureParamRange("val_pre_param2", value, min, max);
                                setParamData("val_pre_param2", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_PRE_PARAM_3:
                                configureParamRange("val_pre_param3", value, min, max);
                                setParamData("val_pre_param3", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_PRE_PARAM_4:
                                configureParamRange("val_pre_param4", value, min, max);
                                setParamData("val_pre_param4", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_PRE_PARAM_5:
                                //configureParamRange("val_pre_param5", value, min, max);
                                //setParamData("val_pre_param5", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_PRE_PARAM_6:
                                //configureParamRange("val_pre_param6", value, min, max);
                                //setParamData("val_pre_param6", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_PRE_PARAM_7:
                                //configureParamRange("val_pre_param7", value, min, max);
                                //setParamData("val_pre_param7", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DIST_PARAM_0:
                                configureParamRange("val_dst_param0", value, min, max);
                                setParamData("val_dst_param0", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DIST_PARAM_1:
                                configureParamRange("val_dst_param1", value, min, max);
                                setParamData("val_dst_param1", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DIST_PARAM_2:
                                configureParamRange("val_dst_param2", value, min, max);
                                setParamData("val_dst_param2", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DIST_PARAM_3:
                                configureParamRange("val_dst_param3", value, min, max);
                                setParamData("val_dst_param3", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DIST_PARAM_4:
                                configureParamRange("val_dst_param4", value, min, max);
                                setParamData("val_dst_param4", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DIST_PARAM_5:
                                //configureParamRange("val_dst_param5", value, min, max);
                                //setParamData("val_dst_param5", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DIST_PARAM_6:
                                //configureParamRange("val_dst_param6", value, min, max);
                                //setParamData("val_dst_param6", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DIST_PARAM_7:
                                //configureParamRange("val_dst_param7", value, min, max);
                                //setParamData("val_dst_param7", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_AMP_PARAM_0:
                                configureParamRange("val_amp_param0", value, min, max);
                                setParamData("val_amp_param0", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_AMP_PARAM_1:
                                configureParamRange("val_amp_param1", value, min, max);
                                setParamData("val_amp_param1", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_AMP_PARAM_2:
                                configureParamRange("val_amp_param2", value, min, max);
                                setParamData("val_amp_param2", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_AMP_PARAM_3:
                                configureParamRange("val_amp_param3", value, min, max);
                                setParamData("val_amp_param3", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_AMP_PARAM_4:
                                configureParamRange("val_amp_param4", value, min, max);
                                setParamData("val_amp_param4", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_AMP_PARAM_5:
                                configureParamRange("val_amp_param5", value, min, max);
                                setParamData("val_amp_param5", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_AMP_PARAM_6:
                                configureParamRange("val_amp_param6", value, min, max);
                                setParamData("val_amp_param6", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_AMP_PARAM_7:
                                //configureParamRange("val_amp_param7", value, min, max);
                                //setParamData("val_amp_param7", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_CAB_PARAM_0:
                                configureParamRange("val_cab_param0", value, min, max);
                                setParamData("val_cab_param0", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_CAB_PARAM_1:
                                configureParamRange("val_cab_param1", value, min, max);
                                setParamData("val_cab_param1", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_CAB_PARAM_2:
                                configureParamRange("val_cab_param2", value, min, max);
                                setParamData("val_cab_param2", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_CAB_PARAM_3:
                                configureParamRange("val_cab_param3", value, min, max);
                                setParamData("val_cab_param3", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_CAB_PARAM_4:
                                configureParamRange("val_cab_param4", value, min, max);
                                setParamData("val_cab_param4", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_CAB_PARAM_5:
                                //configureParamRange("val_cab_param5", value, min, max);
                                //setParamData("val_cab_param5", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_CAB_PARAM_6:
                                //configureParamRange("val_cab_param6", value, min, max);
                                //setParamData("val_cab_param6", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_CAB_PARAM_7:
                                //configureParamRange("val_cab_param7", value, min, max);
                                //setParamData("val_cab_param7", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_EQ_PARAM_0:
                                configureParamRange("val_eq_param0", value, min, max);
                                setParamData("val_eq_param0", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_EQ_PARAM_1:
                                configureParamRange("val_eq_param1", value, min, max);
                                setParamData("val_eq_param1", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_EQ_PARAM_2:
                                configureParamRange("val_eq_param2", value, min, max);
                                setParamData("val_eq_param2", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_EQ_PARAM_3:
                                configureParamRange("val_eq_param3", value, min, max);
                                setParamData("val_eq_param3", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_EQ_PARAM_4:
                                configureParamRange("val_eq_param4", value, min, max);
                                setParamData("val_eq_param4", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_EQ_PARAM_5:
                                configureParamRange("val_eq_param5", value, min, max);
                                setParamData("val_eq_param5", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_EQ_PARAM_6:
                                //configureParamRange("val_eq_param6", value, min, max);
                                //setParamData("val_eq_param6", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_EQ_PARAM_7:
                                //configureParamRange("val_eq_param7", value, min, max);
                                //setParamData("val_eq_param7", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_MOD_PARAM_0:
                                configureParamRange("val_mod_param0", value, min, max);
                                setParamData("val_mod_param0", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_MOD_PARAM_1:
                                configureParamRange("val_mod_param1", value, min, max);
                                setParamData("val_mod_param1", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_MOD_PARAM_2:
                                configureParamRange("val_mod_param2", value, min, max);
                                setParamData("val_mod_param2", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_MOD_PARAM_3:
                                configureParamRange("val_mod_param3", value, min, max);
                                setParamData("val_mod_param3", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_MOD_PARAM_4:
                                configureParamRange("val_mod_param4", value, min, max);
                                setParamData("val_mod_param4", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_MOD_PARAM_5:
                                //configureParamRange("val_mod_param5", value, min, max);
                                //setParamData("val_mod_param5", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_MOD_PARAM_6:
                                //configureParamRange("val_mod_param6", value, min, max);
                                //setParamData("val_mod_param6", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_MOD_PARAM_7:
                                //configureParamRange("val_mod_param7", value, min, max);
                                //setParamData("val_mod_param7", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DLY_PARAM_0:
                                configureParamRange("val_dly_param0", value, min, max);
                                setParamData("val_dly_param0", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DLY_PARAM_1:
                                configureParamRange("val_dly_param1", value, min, max);
                                setParamData("val_dly_param1", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DLY_PARAM_2:
                                configureParamRange("val_dly_param2", value, min, max);
                                setParamData("val_dly_param2", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DLY_PARAM_3:
                                configureParamRange("val_dly_param3", value, min, max);
                                setParamData("val_dly_param3", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DLY_PARAM_4:
                                configureParamRange("val_dly_param4", value, min, max);
                                setParamData("val_dly_param4", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DLY_PARAM_5:
                                configureParamRange("val_dly_param5", value, min, max);
                                setParamData("val_dly_param5", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DLY_PARAM_6:
                                //configureParamSwitch("val_dly_param6", value);
                                //setParamData("val_dly_param6", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_DLY_PARAM_7:
                                //configureParamRange("val_dly_param7", value, min, max);
                                //setParamData("val_dly_param7", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_RVB_PARAM_0:
                                configureParamRange("val_rvb_param0", value, min, max);
                                setParamData("val_rvb_param0", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_RVB_PARAM_1:
                                configureParamRange("val_rvb_param1", value, min, max);
                                setParamData("val_rvb_param1", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_RVB_PARAM_2:
                                configureParamRange("val_rvb_param2", value, min, max);
                                setParamData("val_rvb_param2", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_RVB_PARAM_3:
                                configureParamRange("val_rvb_param3", value, min, max);
                                setParamData("val_rvb_param3", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_RVB_PARAM_4:
                                configureParamRange("val_rvb_param4", value, min, max);
                                setParamData("val_rvb_param4", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_RVB_PARAM_5:
                                //configureParamRange("val_rvb_param5", value, min, max);
                                //setParamData("val_rvb_param5", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_RVB_PARAM_6:
                                //configureParamRange("val_rvb_param6", value, min, max);
                                //setParamData("val_rvb_param6", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_RVB_PARAM_7:
                                //configureParamRange("val_rvb_param7", value, min, max);
                                //setParamData("val_rvb_param7", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_NS_PARAM_0:
                                configureParamRange("val_ns_param0", value, min, max);
                                setParamData("val_ns_param0", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_NS_PARAM_1:
                                configureParamRange("val_ns_param1", value, min, max);
                                setParamData("val_ns_param1", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_NS_PARAM_2:
                                configureParamRange("val_ns_param2", value, min, max);
                                setParamData("val_ns_param2", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_NS_PARAM_3:
                                configureParamRange("val_ns_param3", value, min, max);
                                setParamData("val_ns_param3", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_NS_PARAM_4:
                                configureParamRange("val_ns_param4", value, min, max);
                                setParamData("val_ns_param4", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_NS_PARAM_5:
                                //configureParamRange("val_ns_param5", value, min, max);
                                //setParamData("val_ns_param5", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_NS_PARAM_6:
                                //configureParamRange("val_ns_param6", value, min, max);
                                //setParamData("val_ns_param6", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_NS_PARAM_7:
                                //configureParamRange("val_ns_param7", value, min, max);
                                //setParamData("val_ns_param7", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_NS_PARAM_8:
                                //configureParamRange("val_ns_param8", value, min, max);
                                //setParamData("val_ns_param8", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_NS_PARAM_9:
                                //configureParamRange("val_ns_param9", value, min, max);
                                //setParamData("val_ns_param9", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_NS_PARAM_10:
                                //configureParamRange("val_ns_param10", value, min, max);
                                //setParamData("val_ns_param10", parseInt(key));
                                break;

                            case ValetonParameters.VALETON_PARAM_LAST: {
                                // End of regular parameters
                                break;
                            }

                            // Globals
                            case ValetonParameters.VALETON_GLOBAL_BPM: {
                                configureParamRange("val_gb_bpm", value, min, max, 1);
                                setParamData("val_gb_bpm", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_GLOBAL_INPUT_TRIM: {
                                configureParamRange("val_gb_trim", value, min, max, 1);
                                setParamData("val_gb_trim", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_GLOBAL_CABSIM_BYPASS: {
                                configureParamSwitch("val_gb_cabsim", value);
                                setParamData("val_gb_cabsim", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_GLOBAL_MASTER_VOLUME: {
                                configureParamRange("val_gb_vol", value, min, max, 1);
                                setParamData("val_gb_vol", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_GLOBAL_RECORD_LEVEL: {
                                configureParamRange("val_gb_rec", value, min, max, 1);
                                setParamData("val_gb_rec", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_GLOBAL_MONITOR_LEVEL: {
                                configureParamRange("val_gb_mon", value, min, max, 1);
                                setParamData("val_gb_mon", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_GLOBAL_BT_LEVEL: {
                                configureParamRange("val_gb_bt", value, min, max, 1);
                                setParamData("val_gb_bt", parseInt(key));
                                break;
                            }
                            case ValetonParameters.VALETON_GLOBAL_LAST: {
                                // End of globals
                                break;
                            }
                        }
                    }
                } break;
            } 

            // update the effect icons to the current states
            setEffectIcons();
            break;
            
        case 'GETMODELLERDATA':  
            console.log("Got Modeller Data");

            // get the maximum presets for the connected pedal
            maxPresets = data['MAX_PRESETS'];
            startPreset = data['START_PRESET'];

            // set modeller type
            modellerType = data['MODELLER_TYPE'];

            // enable the relevant menu tabs
            document.querySelectorAll('.tab-group').forEach(group => {
                group.classList.remove('active');
            });

            switch (modellerType) {
                case AMP_MODELLER_TONEX_ONE:
                case AMP_MODELLER_TONEX:
                default:
                    updateVisibleTabs("Tonex");
                    break;

                case AMP_MODELLER_VALETON_GP5:
                    updateVisibleTabs("Valeton");
                    break;
            }

        case 'GETSYNCCOMPLETE':
            var syncState = isSyncDone;
            isSyncDone = data['SYNC'];

            // check for state changed
            if (isSyncDone != syncState) {
                if (isSyncDone == 1) {
                    console.log("Sync done");

                    // request all the stuff                            
                    sendWS({"CMD": "GETPRESETNAMES"});
                    sendWS({"CMD": "GETPARAMS"});   
                    sendWS({"CMD": "GETPRESET"});                                   
                }
            }
            break;

        case 'GETCONFIG':  
            console.log("Got Config");

            // set all config items              
            configureParamSelect("btmode", data['BT_MODE']);
            configureParamSwitch("mvavechoc", data['BT_CHOC_EN']);
            configureParamSwitch("xvivemd1", data['BT_MD1_EN']);
            configureParamSwitch("custombte", data['BT_CUST_EN']);
            setParamLabel("custombt", data['BT_CUST_NAME']);
            configureParamSwitch("togglebypass", data['TOGGLE_BYPASS']);
            configureParamSwitch("looparound", data['LOOP_AROUND']);
            configureParamSwitch("midienabled", data['S_MIDI_EN']);
            configureParamSelect("midichannel", data['S_MIDI_CH']);                    
            configureParamSwitch("btmidicc", data['BT_MIDI_CC']);
            configureParamSelect("wifimode", data['WIFI_MODE']);                    
            configureParamSelect("wifitxpower", data['WIFI_POWER']);                
            setParamValue("wifiname", data['WIFI_SSID']);
            setParamValue("wifipassword", data['WIFI_PW']);                    
            setParamValue("mdnsname", data['MDNS_NAME']);

            configureParamSelect("screenrot", data['SCREEN_ROT']);
            configureParamSelect("presetslot", data['PRESET_SLOT']);                
            configureParamSwitch("hitchsense", data['HIGH_TCH_SNS']);

            configureParamSelect("extfslay", data['EXTFS_PS_LAYOUT']);
                                
            configureParamSelect("extfx1sw", data['EXTFS_ES1_SW']);
            setParamValue("extfx1cc", data['EXTFS_ES1_CC']);
            extFSChanged("extfx1");
            setExtFSValue("extfx1v1", data['EXTFS_ES1_V1']);
            setExtFSValue("extfx1v2", data['EXTFS_ES1_V2']);

            configureParamSelect("extfx2sw", data['EXTFS_ES2_SW']);
            setParamValue("extfx2cc", data['EXTFS_ES2_CC']);
            extFSChanged("extfx2");
            setExtFSValue("extfx2v1", data['EXTFS_ES2_V1']);
            setExtFSValue("extfx2v2", data['EXTFS_ES2_V2']);

            configureParamSelect("extfx3sw", data['EXTFS_ES3_SW']);
            setParamValue("extfx3cc", data['EXTFS_ES3_CC']);
            extFSChanged("extfx3");
            setExtFSValue("extfx3v1", data['EXTFS_ES3_V1']);
            setExtFSValue("extfx3v2", data['EXTFS_ES3_V2']);

            configureParamSelect("extfx4sw", data['EXTFS_ES4_SW']);
            setParamValue("extfx4cc", data['EXTFS_ES4_CC']);
            extFSChanged("extfx4");
            setExtFSValue("extfx4v1", data['EXTFS_ES4_V1']);
            setExtFSValue("extfx4v2", data['EXTFS_ES4_V2']);

            configureParamSelect("extfx5sw", data['EXTFS_ES5_SW']);
            setParamValue("extfx5cc", data['EXTFS_ES5_CC']);
            extFSChanged("extfx5");
            setExtFSValue("extfx5v1", data['EXTFS_ES5_V1']);
            setExtFSValue("extfx5v2", data['EXTFS_ES5_V2']);

            configureParamSelect("extfx6sw", data['EXTFS_ES6_SW']);                    
            setParamValue("extfx6cc", data['EXTFS_ES6_CC']);
            extFSChanged("extfx6");
            setExtFSValue("extfx6v1", data['EXTFS_ES6_V1']);
            setExtFSValue("extfx6v2", data['EXTFS_ES6_V2']);

            configureParamSelect("extfx7sw", data['EXTFS_ES7_SW']);
            setParamValue("extfx7cc", data['EXTFS_ES7_CC']);
            extFSChanged("extfx7");
            setExtFSValue("extfx7v1", data['EXTFS_ES7_V1']);
            setExtFSValue("extfx7v2", data['EXTFS_ES7_V2']);

            configureParamSelect("extfx8sw", data['EXTFS_ES8_SW']);
            setParamValue("extfx8cc", data['EXTFS_ES8_CC']);
            extFSChanged("extfx8");
            setExtFSValue("extfx8v1", data['EXTFS_ES8_V1']);
            setExtFSValue("extfx8v2", data['EXTFS_ES8_V2']);

            configureParamSelect("intfslay", data['FOOTSW_MODE']);

            configureParamSelect("intfx1sw", data['INTFS_ES1_SW']);
            setParamValue("intfx1cc", data['INTFS_ES1_CC']);
            extFSChanged("intfx1");
            setExtFSValue("intfx1v1", data['INTFS_ES1_V1']);
            setExtFSValue("intfx1v2", data['INTFS_ES1_V2']);

            configureParamSelect("intfx2sw", data['INTFS_ES2_SW']);
            setParamValue("intfx2cc", data['INTFS_ES2_CC']);
            extFSChanged("intfx2");
            setExtFSValue("intfx2v1", data['INTFS_ES2_V1']);
            setExtFSValue("intfx2v2", data['INTFS_ES2_V2']);

            configureParamSelect("intfx3sw", data['INTFS_ES3_SW']);
            setParamValue("intfx3cc", data['INTFS_ES3_CC']);
            extFSChanged("intfx3");
            setExtFSValue("intfx3v1", data['INTFS_ES3_V1']);
            setExtFSValue("intfx3v2", data['INTFS_ES3_V2']);

            configureParamSelect("intfx4sw", data['INTFS_ES4_SW']);
            setParamValue("intfx4cc", data['INTFS_ES4_CC']);
            extFSChanged("intfx4");
            setExtFSValue("intfx4v1", data['INTFS_ES4_V1']);
            setExtFSValue("intfx4v2", data['INTFS_ES4_V2']);

            // save these for later use (after we get the preset details)
            preset_order_cfg = data['PRESET_ORDER'];
            preset_color_cfg = data['PRESET_COLORS'];
            break;   
        
        case 'GETPRESET':
            configureParamSelect("set_preset", data['INDEX']);
            break;  
        
        case 'GETPRESETNAMES':
            console.log("Got Preset Names");
            var preset_list = document.getElementById("set_preset");

            var item_count = Object.keys(data['PRESET_NAMES']).length;

            //console.log(data);
            //console.log(item_count);
            //console.log(preset_list.options.length);

            if (preset_list.options.length != item_count) {
                console.log("Init GetPresetNames");

                // delete existing options so we don't get double-ups
                preset_list.options.length = 0;

                // init select options
                for (var index = 0; index < item_count; index++) {
                    const option = new Option((index + startPreset).toString(), index.toString());
                    preset_list.add(option);
                }

                //console.log(preset_list);

                // build preset order list to suit max presets
                var preset_order_div = document.getElementById("preset-order-list");

                // delete existing items s we don't get double-ups
                preset_order_div.innerHTML = "";

                // create top row
                createPresetOrderRow(preset_order_div, 0, false, true);

                // create other rows except last
                for (var index = 1; index < item_count - 1; index++) {
                    createPresetOrderRow(preset_order_div, index, true, true);
                }

                // create last row
                createPresetOrderRow(preset_order_div, item_count - 1, true, false);

                // create bank indicators
                var banks_div = document.getElementById("preset-orderbank-column");
                
                // delete existing items s we don't get double-ups
                //banks_div.innerHTML = "";

                for (var bank = 0; bank < (item_count / 3); bank++) {
                    var bankDiv = document.createElement('div');
                    bankDiv.className = 'row preset-orderbank';
                    bankDiv.setAttribute('style', 'padding-bottom: -2px;');
                    bankDiv.id = 'preset-orderbank-' + bank;
                    bankDiv.setAttribute('order_value', bank);

                    var bankCol = document.createElement('div');
                    bankCol.className = 'col';

                    var bankSpan = document.createElement('span');
                    bankSpan.textContent = 'Bank ' + (bank + 1);

                    bankCol.appendChild(bankSpan);
                    bankDiv.appendChild(bankCol);
                    banks_div.appendChild(bankDiv);

                    //console.log('preset-orderbank');
                    //console.log(bank);
                }
            }
           
            // update the list to have the preset names
            for (var index in data['PRESET_NAMES']) {
                let name = data['PRESET_NAMES'][index];
                var option_entry = document.getElementById("set_preset").options[index]

                if (option_entry == null) {
                    console.log("null option_entry:")
                    console.log(index);
                    console.log(data);
                } 
                else {
                    option_entry.text = name;
                    var element = document.querySelector('[id^="preset-order-"][order_value="' + index + '"]');

                    //console.log('preset names');
                    //console.log(index);
                    //console.log(element);
                    if (element != null) {
                        presetOrderElementName(element).innerHTML = name;
                    }
                }                       
            }

            setPresetOrder(preset_order_cfg, preset_color_cfg);
            break;                  
    }
}

function openTab(evt, tabName) {
    var i, tabcontent, tablinks;
    tabcontent = document.getElementsByClassName("tabcontent");
    
    for (i = 0; i < tabcontent.length; i++) {
        tabcontent[i].style.display = "none";
    }
    tablinks = document.getElementsByClassName("tablinks");
    
    for (i = 0; i < tablinks.length; i++) {
        tablinks[i].className = tablinks[i].className.replace(" active", "");
    }
    document.getElementById(tabName).style.display = "block";
    evt.currentTarget.className += " active";
}

function openFX(evt, tabName) {
    var i, tabcontent, tablinks;
    tabcontent = document.getElementsByClassName("fxtabcontent");
    
    for (i = 0; i < tabcontent.length; i++) {
        tabcontent[i].style.display = "none";
    }
    tablinks = document.getElementsByClassName("fxtablink");
    
    for (i = 0; i < tablinks.length; i++) {
        tablinks[i].className = tablinks[i].className.replace(" active", "");
    }
    document.getElementById(tabName).style.display = "block";
    evt.currentTarget.className += " active";
}

function openIFX(evt, tabName) {
    var i, tabcontent, tablinks;
    tabcontent = document.getElementsByClassName("ifxtabcontent");
    
    for (i = 0; i < tabcontent.length; i++) {
        tabcontent[i].style.display = "none";
    }
    tablinks = document.getElementsByClassName("ifxtablink");
    
    for (i = 0; i < tablinks.length; i++) {
        tablinks[i].className = tablinks[i].className.replace(" active", "");
    }
    document.getElementById(tabName).style.display = "block";
    evt.currentTarget.className += " active";
}

function setToast(msg) {
    var x = $('#toast');
    x.html('<div class="toast-container"><div id="toast-text">' + msg + '</div></div>');
    x.addClass('show');
    setTimeout(function(){ x.removeClass('show'); }, 2750);
}

function saveSettings() {
    if (SocketConnected === 1) {
        // get values
        var btmode = document.getElementById("btmode").value;

        var mvavechoc = document.getElementById("mvavechoc").checked;
        var mvavechocen = mvavechoc ? 1 : 0;

        var xvivemd1 = document.getElementById("xvivemd1").checked;
        var xvivemd1en = xvivemd1 ? 1 : 0;

        var custombte = document.getElementById("custombte").checked;
        var custombteen = custombte ? 1 : 0;

        var custombt = document.getElementById("custombt").value;

        var togglebypass = document.getElementById("togglebypass").checked;
        var togglebypassen = togglebypass ? 1 : 0;

        var looparound = document.getElementById("looparound").checked;
        var looparounden = looparound ? 1 : 0;

        var midienabled = document.getElementById("midienabled").checked;
        var midienableden = midienabled ? 1 : 0;
        
        var midichannel = document.getElementById("midichannel").value;
        var footmode = document.getElementById("intfslay").value;
        
        var btmidicc = document.getElementById("btmidicc").checked;
        var btmidiccen = btmidicc ? 1 : 0;
                           
        var screenrot = document.getElementById("screenrot").value;       
        var presetslot = document.getElementById("presetslot").value;         
        
        var hitchsensechk = document.getElementById("hitchsense").checked;         
        var hitchsense = hitchsensechk ? 1 : 0;

        var extfslay = document.getElementById("extfslay").value;  

        var extfx1sw = document.getElementById("extfx1sw").value;  
        var extfx1cc = document.getElementById("extfx1cc").value;  
        var extfx1v1 = getExtFSValue("1", "1");
        var extfx1v2 = getExtFSValue("1", "2");

        var extfx2sw = document.getElementById("extfx2sw").value;  
        var extfx2cc = document.getElementById("extfx2cc").value;  
        var extfx2v1 = getExtFSValue("2", "1");
        var extfx2v2 = getExtFSValue("2", "2");

        var extfx3sw = document.getElementById("extfx3sw").value;  
        var extfx3cc = document.getElementById("extfx3cc").value;  
        var extfx3v1 = getExtFSValue("3", "1");
        var extfx3v2 = getExtFSValue("3", "2");

        var extfx4sw = document.getElementById("extfx4sw").value;  
        var extfx4cc = document.getElementById("extfx4cc").value;  
        var extfx4v1 = getExtFSValue("4", "1");
        var extfx4v2 = getExtFSValue("4", "2");

        var extfx5sw = document.getElementById("extfx5sw").value;  
        var extfx5cc = document.getElementById("extfx5cc").value;  
        var extfx5v1 = getExtFSValue("5", "1");
        var extfx5v2 = getExtFSValue("5", "2");

        var extfx6sw = document.getElementById("extfx6sw").value;  
        var extfx6cc = document.getElementById("extfx6cc").value;  
        var extfx6v1 = getExtFSValue("6", "1");
        var extfx6v2 = getExtFSValue("6", "2");
        
        var extfx7sw = document.getElementById("extfx7sw").value;  
        var extfx7cc = document.getElementById("extfx7cc").value;  
        var extfx7v1 = getExtFSValue("7", "1");
        var extfx7v2 = getExtFSValue("7", "2");
        
        var extfx8sw = document.getElementById("extfx8sw").value;  
        var extfx8cc = document.getElementById("extfx8cc").value;  
        var extfx8v1 = getExtFSValue("8", "1");
        var extfx8v2 = getExtFSValue("8", "2");


        var intfx1sw = document.getElementById("intfx1sw").value;  
        var intfx1cc = document.getElementById("intfx1cc").value;  
        var intfx1v1 = getIntFSValue("1", "1");
        var intfx1v2 = getIntFSValue("1", "2");

        var intfx2sw = document.getElementById("intfx2sw").value;  
        var intfx2cc = document.getElementById("intfx2cc").value;  
        var intfx2v1 = getIntFSValue("2", "1");
        var intfx2v2 = getIntFSValue("2", "2");

        var intfx3sw = document.getElementById("intfx3sw").value;  
        var intfx3cc = document.getElementById("intfx3cc").value;  
        var intfx3v1 = getIntFSValue("3", "1");
        var intfx3v2 = getIntFSValue("3", "2");

        var intfx4sw = document.getElementById("intfx4sw").value;  
        var intfx4cc = document.getElementById("intfx4cc").value;  
        var intfx4v1 = getIntFSValue("4", "1"); 
        var intfx4v2 = getIntFSValue("4", "2");

        sendWS({"CMD": "SETCONFIG", 
                "BT_MODE": parseInt(btmode),
                "BT_CHOC_EN": mvavechocen, 
                "BT_MD1_EN": xvivemd1en,
                "BT_CUST_EN": custombteen,
                "BT_CUST_NAME": custombt,
                "TOGGLE_BYPASS": togglebypassen,
                "LOOP_AROUND": looparounden,
                "S_MIDI_EN": midienableden,
                "S_MIDI_CH": parseInt(midichannel),
                "FOOTSW_MODE": parseInt(footmode),
                "BT_MIDI_CC": btmidiccen,
                "SCREEN_ROT": parseInt(screenrot),
                "PRESET_SLOT": parseInt(presetslot),
                "HIGH_TCH_SNS": parseInt(hitchsense),
                "EXTFS_PS_LAYOUT": parseInt(extfslay),                        
                "EXTFS_ES1_SW": parseInt(extfx1sw),
                "EXTFS_ES1_CC": parseInt(extfx1cc),
                "EXTFS_ES1_V1": parseInt(extfx1v1),
                "EXTFS_ES1_V2": parseInt(extfx1v2),
                "EXTFS_ES2_SW": parseInt(extfx2sw),
                "EXTFS_ES2_CC": parseInt(extfx2cc),
                "EXTFS_ES2_V1": parseInt(extfx2v1),
                "EXTFS_ES2_V2": parseInt(extfx2v2),
                "EXTFS_ES3_SW": parseInt(extfx3sw),
                "EXTFS_ES3_CC": parseInt(extfx3cc),
                "EXTFS_ES3_V1": parseInt(extfx3v1),
                "EXTFS_ES3_V2": parseInt(extfx3v2),
                "EXTFS_ES4_SW": parseInt(extfx4sw),
                "EXTFS_ES4_CC": parseInt(extfx4cc),
                "EXTFS_ES4_V1": parseInt(extfx4v1),
                "EXTFS_ES4_V2": parseInt(extfx4v2),
                "EXTFS_ES5_SW": parseInt(extfx5sw),
                "EXTFS_ES5_CC": parseInt(extfx5cc),
                "EXTFS_ES5_V1": parseInt(extfx5v1),
                "EXTFS_ES5_V2": parseInt(extfx5v2),
                "EXTFS_ES6_SW": parseInt(extfx6sw),
                "EXTFS_ES6_CC": parseInt(extfx6cc),
                "EXTFS_ES6_V1": parseInt(extfx6v1),
                "EXTFS_ES6_V2": parseInt(extfx6v2),
                "EXTFS_ES7_SW": parseInt(extfx7sw),
                "EXTFS_ES7_CC": parseInt(extfx7cc),
                "EXTFS_ES7_V1": parseInt(extfx7v1),
                "EXTFS_ES7_V2": parseInt(extfx7v2),
                "EXTFS_ES8_SW": parseInt(extfx8sw),
                "EXTFS_ES8_CC": parseInt(extfx8cc),
                "EXTFS_ES8_V1": parseInt(extfx8v1),
                "EXTFS_ES8_V2": parseInt(extfx8v2),
                "INTFS_ES1_SW": parseInt(intfx1sw),
                "INTFS_ES1_CC": parseInt(intfx1cc),
                "INTFS_ES1_V1": parseInt(intfx1v1),
                "INTFS_ES1_V2": parseInt(intfx1v2),
                "INTFS_ES2_SW": parseInt(intfx2sw),
                "INTFS_ES2_CC": parseInt(intfx2cc),
                "INTFS_ES2_V1": parseInt(intfx2v1),
                "INTFS_ES2_V2": parseInt(intfx2v2),
                "INTFS_ES3_SW": parseInt(intfx3sw),
                "INTFS_ES3_CC": parseInt(intfx3cc),
                "INTFS_ES3_V1": parseInt(intfx3v1),
                "INTFS_ES3_V2": parseInt(intfx3v2),
                "INTFS_ES4_SW": parseInt(intfx4sw),
                "INTFS_ES4_CC": parseInt(intfx4cc),
                "INTFS_ES4_V1": parseInt(intfx4v1),
                "INTFS_ES4_V2": parseInt(intfx4v2),
            });  

        setToast('Settings saved.<br>Rebooting now');
    }
}

function SaveWifi() {
    if (SocketConnected === 1) {
        // get values
        var wifi_mode = document.getElementById("wifimode").value;
        var wifitxpower = document.getElementById("wifitxpower").value;                
        var wifi_ssid = document.getElementById("wifiname").value;
        var wifi_pw = document.getElementById("wifipassword").value;
        var mdnsname = document.getElementById("mdnsname").value;                
        
        console.log('WiFi config send');
        
        sendWS({"CMD": "SETWIFI", 
                "WIFI_MODE": parseInt(wifi_mode),
                "WIFI_POWER": parseInt(wifitxpower),
                "WIFI_SSID": wifi_ssid,
                "WIFI_PW": wifi_pw,
                "MDNS_NAME": mdnsname});
                
        setToast('WiFi Config Saved');
    }
}

function onPresetChange() {
    if (SocketConnected === 1) {
        // get values
        var preset_index = document.getElementById("set_preset").value;
        console.log('Preset change: ');
        console.log(preset_index);
        
        sendWS({"CMD": "SETPRESET", 
                "PRESET": parseInt(preset_index)});                        
    }
}

function onParamChange(e) {
    console.log('Param changed');            
    var param_index = e.target.getAttribute("data-indexnumber");
    var param_value;

    if (e.target.type && e.target.type === 'checkbox') {
        var checked = e.target.checked;
        param_value = checked ? 1.0 : 0.0;
    } else {
        param_value = parseFloat(e.target.value);
    }

    // range changes sent already in other function
    if (e.target.type !== 'range') {
        if (SocketConnected === 1) {                
            sendWS({"CMD": "SETPARAM", 
                    "INDEX": parseInt(param_index),
                    "VALUE": param_value});                        
        }
    } 
}

function showValue(e, send_value) {
    var param_index = e.getAttribute("data-indexnumber");
    const label = document.querySelector(`label[for="${e.id}"]`);
    if (!label) {
        console.error(`Label for input ${e.id} not found`);
        return;
    }
    
    // Update label text
    if (send_value !== false || userIsDragging !== e.id) {
        label.innerText = label.innerText.replace(/ :.*/, "") + " : " + parseFloat(e.value);
        const unit = label.getAttribute("unit");
        if (unit) {
            label.innerText += unit;
        }
    }

    if (send_value) {
        // save the slider control being dragged
        userIsDragging = e.id;
        
        // check for web socket send, with rate limit
        const now = Date.now();
        if (now - lastSendTime >= THROTTLE_DELAY) {
            if (SocketConnected === 1) {                
                sendWS({"CMD": "SETPARAM", 
                    "INDEX": parseInt(param_index),
                    "VALUE": parseFloat(e.value)});                        
            }
            lastSendTime = now;
        } else {
            // If within delay, queue the next send
            clearTimeout(throttleTimeout);
            
            throttleTimeout = setTimeout(() => {
                if (SocketConnected === 1) {                
                    sendWS({"CMD": "SETPARAM", 
                    "INDEX": parseInt(param_index),
                    "VALUE": parseFloat(e.value)});                        
                }

                lastSendTime = Date.now();
            }, THROTTLE_DELAY - (now - lastSendTime));
        }
    }

    // Handle label click
    label.onclick = function(event) {
        try {
            const tmp_id = event.target.getAttribute('for');
            if (!tmp_id) throw new Error("No 'for' attribute on label");
            
            const modalElement = document.getElementById('value_input_bg');
            if (!modalElement) throw new Error("Modal element 'value_input_bg' not found");
            
            const value_input_modal = bootstrap.Modal.getOrCreateInstance(modalElement);
            value_input_modal.show();

            const valueLabel = document.getElementById("value_input_lbl");
            const valueInput = document.getElementById("value_input_val");
            const targetInput = document.getElementById(tmp_id);
            const button = document.getElementById("value_input_btn");
            
            if (!valueLabel || !valueInput || !targetInput || !button) {
                throw new Error("Required element not found");
            }

            // Update modal label
            valueLabel.innerText = label.innerText.replace(/:.*/, "");
            const unit = label.getAttribute("unit");
            if (unit) {
                valueLabel.innerText += `(${unit})`;
            }

            // Set input values
            valueInput.value = targetInput.value;
            valueInput.step = targetInput.step;

            // Remove existing listeners to avoid duplicates
            const newButton = button.cloneNode(true);
            button.parentNode.replaceChild(newButton, button);

            // Add click handler
            newButton.addEventListener("click", () => {
                console.log("Param change via number event");
                targetInput.value = valueInput.value;
                console.log("Dispatching input event on", tmp_id);
                targetInput.dispatchEvent(new InputEvent('change', { bubbles: true }));
                value_input_modal.hide();
            });
        } catch (err) {
            console.error("Error in label click handler:", err);
        }
    };
}
 
// Tap tempo
function TapTempo() {
    this.prevTime = 0;
    this.countClicks = 0;
    this.currentTime = 0;
    this.bpm = 0;
    this.timeDifference = 0;
    this.bpmTotal = 0;
    this.bpmFinal = 0;

    this.tap = (input) => {
        if (this.prevTime === 0) {
            this.prevTime = Date.now();
        } else if ((Date.now()-this.prevTime) > 2200) {
            this.prevTime = 0;
            this.bpm = 0;
            this.bpmTotal = 0;
            this.bpmFinal = 0;
            this.countClicks = 0;
        } else {
            this.currentTime = Date.now();
            this.timeDifference = this.currentTime - this.prevTime;
            this.prevTime = this.currentTime;
            this.bpm = 60 / this.timeDifference;
            this.bpmTotal = this.bpmTotal + this.bpm;
            this.countClicks++;
            this.bpmFinal = (this.bpmTotal / this.countClicks) * 1000;

            if (this.bpmFinal < input.min) {
                this.bpmFinal = input.min;
            }
            if (this.bpmFinal > input.max) {
                this.bpmFinal = input.max;
            }
        }
        return this.bpmFinal;
    }
}

function globalTempoTap() {
    switch (modellerType)
    {
        case AMP_MODELLER_TONEX_ONE:
        case AMP_MODELLER_TONEX:
        default:
        {    
            let input = document.getElementById("gb_bpm");
            let bpm = globalTapTempo.tap(input);
            if (bpm > 0) {
                setParamValue("gb_bpm", bpm);
                showValue(input, false);

                var event = new Event('change');
                input.dispatchEvent(event);
            }
        } break;
        
        case AMP_MODELLER_VALETON_GP5:
        {    
            let input = document.getElementById("val_gb_bpm");
            let bpm = globalTapTempo.tap(input);
            if (bpm > 0) {
                setParamValue("val_gb_bpm", bpm);
                showValue(input, false);

                var event = new Event('change');
                input.dispatchEvent(event);
            }
        } break;
    }
}

function bpmBlink() {
    let input = document.getElementById("gb_bpm");
    var milliseconds = 1000*60/input.value;
    if (milliseconds <= 0) {
        setTimeout(() => {
            bpmBlink();
        }, 500);
        return;
    }

    var tap =  document.getElementById("gb_tap");
    tap.style.borderStyle = "solid";
    tap.style.boxShadow = "0 2px 15px #FB9230d0, inset 0 0 8px #FB9230d0";
    
    setTimeout(() => {
        tap.style.borderStyle = "none";
        tap.style.boxShadow = "0 2px 15px #00000030";
    }, milliseconds/4);

    setTimeout(() => {
        bpmBlink();
    }, milliseconds);
}

function delayBlink() {
    let input = document.getElementById("dl_time");
    var milliseconds = input.value;

    if (milliseconds <= 0) {
        setTimeout(() => {
            delayBlink();
        }, 500);
        return;
    }

    var indicator =  document.getElementById("dl_indicator");
    indicator.style.backgroundColor = "#FB9230d0";
    indicator.style.boxShadow = "0 2px 15px #FB9230d0";
    
    setTimeout(() => {
        indicator.style.backgroundColor = "#2A2A2A";
        indicator.style.boxShadow = "0 2px 15px #00000030";
    }, milliseconds/4);

    setTimeout(() => {
        delayBlink();
    }, milliseconds);
}

function modulationAnimate() {
    let inputId = "mo_param1";
    let input = document.getElementById(inputId);
    let label = document.querySelector(`label[for="${inputId}"]`);
    let isRate = label.innerHTML.startsWith("Rate");
    let milliseconds = input.value > 0 ? (isRate ? 1000/input.value : 1000*60/input.value) : 0;

    var indicator =  document.getElementById("mo_indicator");
    indicator.style.animationDuration = `${milliseconds}ms`;
    
    setTimeout(() => {
        modulationAnimate();
    }, 100);
}

// External footswitch control
function populateExternalFootswitches() {
    for (i=1; i<=8; i++) {
        var select = document.getElementById(`extfx${i}cc`);
        var optgroup = null;

        for (const [key, value] of midiControlChangeAssociations) {
            if (typeof key === "string") {
                optgroup = document.createElement("optgroup");
                optgroup.label = key;

                select.appendChild(optgroup);
            } else {
                var option = document.createElement("option");
                option.value = key;
                option.text = value.param;

                if (optgroup != null) {
                    optgroup.appendChild(option);
                } else {
                    select.appendChild(option);
                }
            }
        }
    }
}

function extFSChanged(fx) {
    var controlChange = document.getElementById(`${fx}cc`);
    var association = midiControlChangeAssociations.get(parseInt(controlChange.value));

    var number1 = document.getElementById(`${fx}v1`);
    var number2 = document.getElementById(`${fx}v2`);
    var range1 = document.getElementById(`${fx}v1_r`);
    var range2 = document.getElementById(`${fx}v2_r`);
    var select1 = document.getElementById(`${fx}v1_s`);
    var select2 = document.getElementById(`${fx}v2_s`);
    
    switch (association.value.type) {
        case "RANGE":
            number1.parentElement.style.display = "block";
            number2.parentElement.style.display = "block";
            number1.style.display = "block";
            number2.style.display = "block";
            range1.style.display = "block";
            range2.style.display = "block";
            select1.style.display = "none";
            select2.style.display = "none";
            break;
        case "TOGGLE":
            number1.parentElement.style.display = "none";
            number2.parentElement.style.display = "none";
            break;
        case "CHOICE":
            number1.parentElement.style.display = "block";
            number2.parentElement.style.display = "block";
            number1.style.display = "none";
            number2.style.display = "none";
            range1.style.display = "none";
            range2.style.display = "none";
            select1.style.display = "block";
            select2.style.display = "block";
            break;
    }

    if (association.value.type == "CHOICE") {
        select1.innerHTML = null
        select2.innerHTML = null

        for (const key in association.value.values) {
            var value = association.value.values[key];

            var option1 = document.createElement("option");
            option1.value = key;
            option1.text = value;
            select1.appendChild(option1);

            var option2 = document.createElement("option");
            option2.value = key;
            option2.text = value;
            select2.appendChild(option2);
        }
    }
}

function setExtFSValue(objname, value) {
    document.getElementById(objname).value = value;
    document.getElementById(`${objname}_r`).value = value;
    document.getElementById(`${objname}_s`).value = value;
}


function getExtFSValue(fs, value) {
    var extfxcc = document.getElementById(`extfx${fs}cc`).value;
    var association = midiControlChangeAssociations.get(parseInt(extfxcc));

    switch (association.value.type) {
        case "RANGE":
            return document.getElementById(`extfx${fs}v${value}`).value;
        case "TOGGLE":
            switch (value) {
                case "1":
                    return 0;
                case "2":
                    return 1;
            }
            break;
        case "CHOICE":
            return document.getElementById(`extfx${fs}v${value}_s`).value;
    }
}

function populateInternalFootswitches() {
    for (i=1; i<=4; i++) {
        var select = document.getElementById(`intfx${i}cc`);
        var optgroup = null;

        for (const [key, value] of midiControlChangeAssociations) {
            if (typeof key === "string") {
                optgroup = document.createElement("optgroup");
                optgroup.label = key;

                select.appendChild(optgroup);
            } else {
                var option = document.createElement("option");
                option.value = key;
                option.text = value.param;

                if (optgroup != null) {
                    optgroup.appendChild(option);
                } else {
                    select.appendChild(option);
                }
            }
        }
    }
}

function getIntFSValue(fs, value) {
    var intfxcc = document.getElementById(`intfx${fs}cc`).value;
    var association = midiControlChangeAssociations.get(parseInt(intfxcc));

    switch (association.value.type) {
        case "RANGE":
            return document.getElementById(`intfx${fs}v${value}`).value;
        case "TOGGLE":
            switch (value) {
                case "1":
                    return 0;
                case "2":
                    return 1;
            }
            break;
        case "CHOICE":
            return document.getElementById(`intfx${fs}v${value}_s`).value;
    }
}

function setPresetOrder(presetOrder, presetColors) {
    var names = [];
    
    for (i = 0; i < maxPresets; i++) {
        var element = document.querySelector('[id^="preset-order-"][order_value="' + i + '"]');
        names.push(presetOrderElementName(element).innerHTML);
    }

    for (i = 0; i < maxPresets; i++) {
        let value = presetOrder[i];
        let name = names[value];
        var element = document.getElementById(`preset-order-${i}`);
        element.setAttribute("order_value", value);
        presetOrderElementColor(element).style.color = presetColors[value];
        presetOrderElementName(element).innerHTML = name;
    }

    updatePresetOrderBanks();
}

function updatePresetOrderBanks() {
    let column = document.getElementById('preset-orderbank-column');

    let internalLayout = document.querySelector('#intfslay option:checked').innerHTML;
    let externalLayout = document.querySelector('#extfslay option:checked').innerHTML;

    if (externalLayout != 'Disabled' && !externalLayout.startsWith("1x2")) {
        column.style.display = "block";
        updatePresetOrderBanksWithLayout(externalLayout);
    } else if (internalLayout != 'Disabled' && !internalLayout.startsWith("1x2")) {
        column.style.display = "block";
        updatePresetOrderBanksWithLayout(internalLayout);
    } else {
        column.style.display = "none";
    }
}

function updatePresetOrderBanksWithLayout(layout) {
    let layoutValues = /(\d)x(\d)(A|B)?.*/g.exec(layout);
    let presetsPerBank = layoutValues[1] * layoutValues[2] - (layoutValues[3] == 'B' ? 2 : 0);
    let banks = Math.ceil(maxPresets / presetsPerBank);
    let height = 100 * presetsPerBank / maxPresets;
    let heightLast = 100 * (maxPresets - (banks - 1) * presetsPerBank) / maxPresets;

    for (i=0; i< (maxPresets / 3); i++) {
        var element = document.getElementById(`preset-orderbank-${i}`);

        //console.log('updatePresetOrderBanksWithLayout');
        //console.log(i);
        //console.log(element);

        element.style.height = `${i<(banks-1) ? height : heightLast}%`;
        element.style.display = i>=banks ? "none" : "block";
    }
}

function presetOrderDown(index) {
    var element = document.getElementById(`preset-order-${index}`);
    var elementNext = document.getElementById(`preset-order-${index + 1}`);
    
    let orderValue = parseInt(elementNext.getAttribute("order_value"));
    let orderValueNext = parseInt(element.getAttribute("order_value"));
    element.setAttribute("order_value", orderValue);
    elementNext.setAttribute("order_value", orderValueNext);

    let name = presetOrderElementName(elementNext).innerHTML;
    let nameNext = presetOrderElementName(element).innerHTML;
    presetOrderElementName(element).innerHTML = name;
    presetOrderElementName(elementNext).innerHTML = nameNext;

    let color = presetOrderElementColor(elementNext).style.color;
    let colorNext = presetOrderElementColor(element).style.color;
    presetOrderElementColor(element).style.color = color;
    presetOrderElementColor(elementNext).style.color = colorNext;
}

function presetOrderUp(index) {
    var element = document.getElementById(`preset-order-${index}`);
    var elementPrevious = document.getElementById(`preset-order-${index - 1}`);

    let orderValue = parseInt(elementPrevious.getAttribute("order_value"));
    let orderValuePrevious = parseInt(element.getAttribute("order_value"));
    element.setAttribute("order_value", orderValue);
    elementPrevious.setAttribute("order_value", orderValuePrevious);

    let name = presetOrderElementName(elementPrevious).innerHTML;
    let namePrevious = presetOrderElementName(element).innerHTML;
    presetOrderElementName(element).innerHTML = name;
    presetOrderElementName(elementPrevious).innerHTML = namePrevious;

    let color = presetOrderElementColor(elementPrevious).style.color;
    let colorNext = presetOrderElementColor(element).style.color;
    presetOrderElementColor(element).style.color = color;
    presetOrderElementColor(elementPrevious).style.color = colorNext;
}

function presetOrderElementColor(element) {
    return element.children[0].children[0];
}

function presetOrderElementName(element) {
    return element.children[0].children[1];
}

function savePresetOrder() {
    var presetOrder = [];
    for (i = 0; i < maxPresets; i++) {
        var element = document.getElementById(`preset-order-${i}`);
        presetOrder.push(parseInt(element.getAttribute("order_value")));
    }

    sendWS({"CMD": "SETPRESETORDER", 
                "PRESET_ORDER": presetOrder});
}

function onWiFiStationSelected() {
    const selectElement = document.getElementById('wifimode');
    const selectedValue = selectElement.value;

    if (selectedValue == '1') {
        setToast('Ensure mDNS/Bonjour is<br>enabled on your router<br>before applying Station Mode!');
    }
}
        
function updateEffectsChainIcons() {
    const container = document.querySelector('.fx_icon_container');
    if (!container) return;

    // Clear existing content
    container.innerHTML = '';

    switch (modellerType)
    {
        case AMP_MODELLER_TONEX_ONE:
        case AMP_MODELLER_TONEX:
        default:
        {    
            const icons = [
                'gate', 'eq', 'amp', 'cab',
                'comp', 'mod', 'dly', 'rvb'
            ];
            icons.forEach(name => {
                const img = document.createElement('img');
                img.id = `fx_image_${name}`;
                container.appendChild(img);
            });
        } break;

        case AMP_MODELLER_VALETON_GP5: 
        {
            const icons = [
                'nr', 
                'pre',
                'dst',
                'amp',  
                'cab', 
                'eq',
                'mod',
                'dly',
                'rvb', 
                'ns',  
            ];
            icons.forEach(name => {
                const img = document.createElement('img');
                img.id = `fx_image_${name}`;
                container.appendChild(img);
            });
        } break;
    }
}
