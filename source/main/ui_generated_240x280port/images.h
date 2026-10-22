#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_arrow_left;
extern const lv_img_dsc_t img_arrow_right;
extern const lv_img_dsc_t img_bt_conn;
extern const lv_img_dsc_t img_bt_disconn;
extern const lv_img_dsc_t img_smythbuilt;
extern const lv_img_dsc_t img_usb_fail;
extern const lv_img_dsc_t img_usb_ok;
extern const lv_img_dsc_t img_wifi_conn;
extern const lv_img_dsc_t img_wifi_disconn;
extern const lv_img_dsc_t img_effect_icon_cab_off;
extern const lv_img_dsc_t img_effect_icon_cab_on;
extern const lv_img_dsc_t img_effect_icon_delay_off;
extern const lv_img_dsc_t img_effect_icon_delay_on_d;
extern const lv_img_dsc_t img_effect_icon_delay_on_t;
extern const lv_img_dsc_t img_effect_icon_amp_off;
extern const lv_img_dsc_t img_effect_icon_amp_on;
extern const lv_img_dsc_t img_effect_icon_comp_off;
extern const lv_img_dsc_t img_effect_icon_comp_on;
extern const lv_img_dsc_t img_effect_icon_gate_off;
extern const lv_img_dsc_t img_effect_icon_gate_on;
extern const lv_img_dsc_t img_effect_icon_mod_off;
extern const lv_img_dsc_t img_effect_icon_mod_on_chorus;
extern const lv_img_dsc_t img_effect_icon_mod_on_flanger;
extern const lv_img_dsc_t img_effect_icon_mod_on_phaser;
extern const lv_img_dsc_t img_effect_icon_mod_on_rotary;
extern const lv_img_dsc_t img_effect_icon_mod_on_tremolo;
extern const lv_img_dsc_t img_effect_icon_reverb_off;
extern const lv_img_dsc_t img_effect_icon_reverb_on_p;
extern const lv_img_dsc_t img_effect_icon_reverb_on_r;
extern const lv_img_dsc_t img_effect_icon_reverb_on_s1;
extern const lv_img_dsc_t img_effect_icon_reverb_on_s2;
extern const lv_img_dsc_t img_effect_icon_reverb_on_s3;
extern const lv_img_dsc_t img_effect_icon_reverb_on_s4;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[33];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/