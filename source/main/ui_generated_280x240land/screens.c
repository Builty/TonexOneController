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
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

void create_screen_screen1() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.screen1 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 280);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_set_size(obj, 280, 48);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2a2a2a), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_pos(obj, 199, -13);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_usb_fail);
                            lv_img_set_zoom(obj, 220);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                        }
                        {
                            // ui_WiFiStatusDisconn
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_wi_fi_status_disconn = obj;
                            lv_obj_set_pos(obj, 121, -11);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_wifi_disconn);
                            lv_img_set_zoom(obj, 220);
                        }
                        {
                            // ui_BTStatusDisconn
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_bt_status_disconn = obj;
                            lv_obj_set_pos(obj, 175, -13);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_bt_disconn);
                            lv_img_set_zoom(obj, 220);
                        }
                        {
                            // ui_IKLogo
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_ik_logo = obj;
                            lv_obj_set_pos(obj, -23, -11);
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
                    // ui_CStatus
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_cstatus = obj;
                    lv_obj_set_pos(obj, 46, 246);
                    lv_obj_set_size(obj, 28, 28);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff563f2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_x(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_y(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "C");
                }
                {
                    // ui_MStatus
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_mstatus = obj;
                    lv_obj_set_pos(obj, 78, 246);
                    lv_obj_set_size(obj, 28, 28);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff563f2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_x(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_y(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "M");
                }
                {
                    // ui_DStatus
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_dstatus = obj;
                    lv_obj_set_pos(obj, 46, 215);
                    lv_obj_set_size(obj, 28, 28);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff563f2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_x(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_y(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "D");
                }
                {
                    // ui_RStatus
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_rstatus = obj;
                    lv_obj_set_pos(obj, 78, 215);
                    lv_obj_set_size(obj, 28, 28);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff563f2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_x(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_y(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "R");
                }
                {
                    // ui_PresetHeadingLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_preset_heading_label = obj;
                    lv_obj_set_pos(obj, 22, 101);
                    lv_obj_set_size(obj, 101, 76);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_SCROLLED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffd1a60c), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_x(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_pivot_y(obj, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "?");
                }
                {
                    // ui_BPMLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_bpm_label = obj;
                    lv_obj_set_pos(obj, 47, 60);
                    lv_obj_set_size(obj, 53, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "BPM");
                }
                {
                    // ui_BPM
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_bpm = obj;
                    lv_obj_set_pos(obj, 74, 60);
                    lv_obj_set_size(obj, 53, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_transform_angle(obj, -900, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "128");
                }
                {
                    // BottomContainer
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.bottom_container = obj;
                    lv_obj_set_pos(obj, 63, 90);
                    lv_obj_set_size(obj, 270, 130);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff262525), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff563f2a), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_pos(obj, 1, 1);
                            lv_obj_set_size(obj, 245, 110);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Preset Name");
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
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_screen1();
}
