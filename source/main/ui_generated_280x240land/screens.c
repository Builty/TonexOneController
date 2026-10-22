#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;

//
// Event handlers
//

lv_obj_t *tick_value_change_obj;

//
// Screens
//

void create_screen_screen1() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.screen1 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 280);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ui_TouchGestureContainer
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.ui_touch_gesture_container = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 240, 280);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_add_event_cb(obj, action_gesture, LV_EVENT_GESTURE, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ui_TopPanel
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.ui_top_panel = obj;
                    lv_obj_set_pos(obj, -99, 135);
                    lv_obj_set_size(obj, 280, 42);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x2a2a2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_y(obj, 24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_x(obj, 120, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_USBStatusFail
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_usb_status_fail = obj;
                            lv_obj_set_pos(obj, 199, -14);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_usb_fail);
                            lv_img_set_zoom(obj, 220);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                        }
                        {
                            // ui_WiFiStatusDisconn
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_wi_fi_status_disconn = obj;
                            lv_obj_set_pos(obj, 121, -13);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_wifi_disconn);
                            lv_img_set_zoom(obj, 220);
                        }
                        {
                            // ui_BTStatusDisconn
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_bt_status_disconn = obj;
                            lv_obj_set_pos(obj, 175, -15);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_bt_disconn);
                            lv_img_set_zoom(obj, 220);
                        }
                        {
                            // ui_IKLogo
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_ik_logo = obj;
                            lv_obj_set_pos(obj, -23, -14);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_smythbuilt);
                            lv_img_set_zoom(obj, 200);
                        }
                        {
                            // ui_WiFiStatusConn
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_wi_fi_status_conn = obj;
                            lv_obj_set_pos(obj, 121, -11);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_wifi_conn);
                            lv_img_set_zoom(obj, 220);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                        }
                        {
                            // ui_BTStatusConn
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_bt_status_conn = obj;
                            lv_obj_set_pos(obj, 175, -13);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_bt_conn);
                            lv_img_set_zoom(obj, 220);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                        }
                        {
                            // ui_USBStatusOK
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_usb_status_ok = obj;
                            lv_obj_set_pos(obj, 199, -13);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_usb_ok);
                            lv_img_set_zoom(obj, 220);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                        }
                    }
                }
                {
                    // ui_IconGate
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_icon_gate = obj;
                    lv_obj_set_pos(obj, 40, 240);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_effect_icon_gate_off);
                    lv_img_set_angle(obj, -900);
                }
                {
                    // ui_IconAmp
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_icon_amp = obj;
                    lv_obj_set_pos(obj, 40, 203);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_effect_icon_amp_off);
                    lv_img_set_angle(obj, -900);
                }
                {
                    // ui_IconCab
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_icon_cab = obj;
                    lv_obj_set_pos(obj, 40, 165);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_effect_icon_cab_off);
                    lv_img_set_angle(obj, -900);
                }
                {
                    // ui_IconComp
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_icon_comp = obj;
                    lv_obj_set_pos(obj, 40, 127);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_effect_icon_comp_off);
                    lv_img_set_angle(obj, -900);
                }
                {
                    // ui_IconMod
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_icon_mod = obj;
                    lv_obj_set_pos(obj, 40, 89);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_effect_icon_mod_off);
                    lv_img_set_angle(obj, -900);
                }
                {
                    // ui_IconDelay
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_icon_delay = obj;
                    lv_obj_set_pos(obj, 40, 51);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_effect_icon_delay_off);
                    lv_img_set_angle(obj, -900);
                }
                {
                    // ui_IconReverb
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_icon_reverb = obj;
                    lv_obj_set_pos(obj, 40, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_effect_icon_reverb_off);
                    lv_img_set_angle(obj, -900);
                }
                {
                    // ui_CStatus
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_cstatus = obj;
                    lv_obj_set_pos(obj, 40, 244);
                    lv_obj_set_size(obj, 28, 28);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0x563f2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_x(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_y(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "C");
                }
                {
                    // ui_MStatus
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_mstatus = obj;
                    lv_obj_set_pos(obj, 40, 204);
                    lv_obj_set_size(obj, 28, 28);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0x563f2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_x(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_y(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "M");
                }
                {
                    // ui_DStatus
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_dstatus = obj;
                    lv_obj_set_pos(obj, 40, 166);
                    lv_obj_set_size(obj, 28, 28);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0x563f2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_x(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_y(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "D");
                }
                {
                    // ui_RStatus
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_rstatus = obj;
                    lv_obj_set_pos(obj, 40, 128);
                    lv_obj_set_size(obj, 28, 28);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0x563f2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_x(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_y(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "R");
                }
                {
                    // ui_PresetHeadingLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_preset_heading_label = obj;
                    lv_obj_set_pos(obj, 38, 101);
                    lv_obj_set_size(obj, 101, 76);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_SCROLLED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xd1a60c), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_x(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_y(obj, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "?");
                }
                {
                    // ui_BPMLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_bpm_label = obj;
                    lv_obj_set_pos(obj, 66, 277);
                    lv_obj_set_size(obj, 53, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "BPM");
                }
                {
                    // ui_BPM
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_bpm = obj;
                    lv_obj_set_pos(obj, 84, 278);
                    lv_obj_set_size(obj, 53, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "128");
                }
                {
                    // ui_BPMIndicator
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.ui_bpm_indicator = obj;
                    lv_obj_set_pos(obj, 77, 15);
                    lv_obj_set_size(obj, 24, 24);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xd1a60c), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xd1b44d), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // BottomContainer
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.bottom_container = obj;
                    lv_obj_set_pos(obj, 65, 90);
                    lv_obj_set_size(obj, 270, 128);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x262525), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0x563f2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_x(obj, 114, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_y(obj, 70, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_PresetHeadingLabel2
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_preset_heading_label2 = obj;
                            lv_obj_set_pos(obj, 1, -4);
                            lv_obj_set_size(obj, 270, 135);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "Preset text");
                        }
                    }
                }
            }
        }
    }
    
    tick_screen_screen1();
}

void tick_screen_screen1() {
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_screen1,
};
void tick_screen(int screen_index) {
    if (screen_index >= 0 && screen_index < 1) {
        tick_screen_funcs[screen_index]();
    }
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen(screenId - 1);
}

//
// Fonts
//

ext_font_desc_t fonts[] = {
#if LV_FONT_MONTSERRAT_8
    { "MONTSERRAT_8", &lv_font_montserrat_8 },
#endif
#if LV_FONT_MONTSERRAT_10
    { "MONTSERRAT_10", &lv_font_montserrat_10 },
#endif
#if LV_FONT_MONTSERRAT_12
    { "MONTSERRAT_12", &lv_font_montserrat_12 },
#endif
#if LV_FONT_MONTSERRAT_14
    { "MONTSERRAT_14", &lv_font_montserrat_14 },
#endif
#if LV_FONT_MONTSERRAT_16
    { "MONTSERRAT_16", &lv_font_montserrat_16 },
#endif
#if LV_FONT_MONTSERRAT_18
    { "MONTSERRAT_18", &lv_font_montserrat_18 },
#endif
#if LV_FONT_MONTSERRAT_20
    { "MONTSERRAT_20", &lv_font_montserrat_20 },
#endif
#if LV_FONT_MONTSERRAT_22
    { "MONTSERRAT_22", &lv_font_montserrat_22 },
#endif
#if LV_FONT_MONTSERRAT_24
    { "MONTSERRAT_24", &lv_font_montserrat_24 },
#endif
#if LV_FONT_MONTSERRAT_26
    { "MONTSERRAT_26", &lv_font_montserrat_26 },
#endif
#if LV_FONT_MONTSERRAT_28
    { "MONTSERRAT_28", &lv_font_montserrat_28 },
#endif
#if LV_FONT_MONTSERRAT_30
    { "MONTSERRAT_30", &lv_font_montserrat_30 },
#endif
#if LV_FONT_MONTSERRAT_32
    { "MONTSERRAT_32", &lv_font_montserrat_32 },
#endif
#if LV_FONT_MONTSERRAT_34
    { "MONTSERRAT_34", &lv_font_montserrat_34 },
#endif
#if LV_FONT_MONTSERRAT_36
    { "MONTSERRAT_36", &lv_font_montserrat_36 },
#endif
#if LV_FONT_MONTSERRAT_38
    { "MONTSERRAT_38", &lv_font_montserrat_38 },
#endif
#if LV_FONT_MONTSERRAT_40
    { "MONTSERRAT_40", &lv_font_montserrat_40 },
#endif
#if LV_FONT_MONTSERRAT_42
    { "MONTSERRAT_42", &lv_font_montserrat_42 },
#endif
#if LV_FONT_MONTSERRAT_44
    { "MONTSERRAT_44", &lv_font_montserrat_44 },
#endif
#if LV_FONT_MONTSERRAT_46
    { "MONTSERRAT_46", &lv_font_montserrat_46 },
#endif
#if LV_FONT_MONTSERRAT_48
    { "MONTSERRAT_48", &lv_font_montserrat_48 },
#endif
};

//
// Color themes
//

uint32_t active_theme_index = 0;

//
//
//

void create_screens() {

// Set default LVGL theme
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    // Initialize screens
    // Create screens
    create_screen_screen1();
}