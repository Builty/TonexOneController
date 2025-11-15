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
    lv_obj_set_size(obj, 480, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ui_TouchGestureContainer
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.ui_touch_gesture_container = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 480, 320);
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
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 480, 39);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2a2a2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_USBStatusFail
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_usb_status_fail = obj;
                            lv_obj_set_pos(obj, 400, -21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_usb_fail);
                            lv_img_set_zoom(obj, 192);
                        }
                        {
                            // ui_ProjectHeadingLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_project_heading_label = obj;
                            lv_obj_set_pos(obj, 119, -12);
                            lv_obj_set_size(obj, 209, 28);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "TONEX Controller");
                        }
                        {
                            // ui_WiFiStatusDisconn
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_wi_fi_status_disconn = obj;
                            lv_obj_set_pos(obj, 333, -21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_wifi_disconn);
                            lv_img_set_zoom(obj, 192);
                        }
                        {
                            // ui_BTStatusDisconn
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_bt_status_disconn = obj;
                            lv_obj_set_pos(obj, 381, -21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_bt_disconn);
                            lv_img_set_zoom(obj, 192);
                        }
                        {
                            // ui_IKLogo
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_ik_logo = obj;
                            lv_obj_set_pos(obj, -30, -22);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_smythbuilt);
                            lv_img_set_zoom(obj, 192);
                        }
                        {
                            // ui_WiFiStatusConn
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_wi_fi_status_conn = obj;
                            lv_obj_set_pos(obj, 333, -21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_wifi_conn);
                            lv_img_set_zoom(obj, 192);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                        }
                        {
                            // ui_BTStatusConn
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_bt_status_conn = obj;
                            lv_obj_set_pos(obj, 381, -21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_bt_conn);
                            lv_img_set_zoom(obj, 192);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                        }
                        {
                            // ui_USBStatusOK
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_usb_status_ok = obj;
                            lv_obj_set_pos(obj, 400, -21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_usb_ok);
                            lv_img_set_zoom(obj, 192);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                        }
                    }
                }
                {
                    // ui_PresetHeadingLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_preset_heading_label = obj;
                    lv_obj_set_pos(obj, 1, 39);
                    lv_obj_set_size(obj, 480, 38);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    lv_obj_add_event_cb(obj, action_enable_skin_edit, LV_EVENT_LONG_PRESSED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_SCROLLED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffd1a60c), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Preset Name");
                }
                {
                    // ui_Skins
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.ui_skins = obj;
                    lv_obj_set_pos(obj, 0, -30);
                    lv_obj_set_size(obj, 480, 113);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_SkinImage
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_skin_image = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_skin_jcm);
                            lv_img_set_zoom(obj, 192);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // ui_BankTitleLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_bank_title_label = obj;
                    lv_obj_set_pos(obj, 10, 90);
                    lv_obj_set_size(obj, 84, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffd1a60c), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Bank");
                }
                {
                    // ui_BankValueLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_bank_value_label = obj;
                    lv_obj_set_pos(obj, 6, 117);
                    lv_obj_set_size(obj, 92, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "1");
                }
                {
                    // ui_BPMTitleLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_bpm_title_label = obj;
                    lv_obj_set_pos(obj, 395, 90);
                    lv_obj_set_size(obj, 73, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffd1a60c), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "BPM");
                }
                {
                    // ui_BPMValueLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_bpm_value_label = obj;
                    lv_obj_set_pos(obj, 384, 117);
                    lv_obj_set_size(obj, 92, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0");
                }
                {
                    // ui_BPMIndicator
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.ui_bpm_indicator = obj;
                    lv_obj_set_pos(obj, 410, 143);
                    lv_obj_set_size(obj, 40, 40);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffd1a60c), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xffd1b44d), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // ui_PresetDetailsTextArea
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.ui_preset_details_text_area = obj;
                    lv_obj_set_pos(obj, 8, 185);
                    lv_obj_set_size(obj, 464, 67);
                    lv_textarea_set_max_length(obj, 128);
                    lv_textarea_set_text(obj, "Preset Description...");
                    lv_textarea_set_one_line(obj, false);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_add_event_cb(obj, action_preset_description_pressed, LV_EVENT_PRESSED, (void *)0);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_add_state(obj, LV_STATE_DISABLED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff563f2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // ui_BottomPanelTonex
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.ui_bottom_panel_tonex = obj;
                    lv_obj_set_pos(obj, 0, 259);
                    lv_obj_set_size(obj, 480, 61);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2a2a2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_SettingsImage
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_settings_image = obj;
                            lv_obj_set_pos(obj, 412, -10);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_settings);
                            lv_img_set_zoom(obj, 240);
                            lv_obj_add_event_cb(obj, action_show_settings_page, LV_EVENT_PRESSED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_IconReverb
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_reverb = obj;
                            lv_obj_set_pos(obj, 356, -10);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_effect_icon_reverb_off);
                            lv_img_set_zoom(obj, 250);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_IconDelay
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_delay = obj;
                            lv_obj_set_pos(obj, 304, -10);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_effect_icon_delay_off);
                            lv_img_set_zoom(obj, 250);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_IconMod
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_mod = obj;
                            lv_obj_set_pos(obj, 252, -10);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_effect_icon_mod_off);
                            lv_img_set_zoom(obj, 250);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_IconComp
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_comp = obj;
                            lv_obj_set_pos(obj, 200, -10);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_effect_icon_comp_off);
                            lv_img_set_zoom(obj, 250);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_IconCab
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_cab = obj;
                            lv_obj_set_pos(obj, 148, -10);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_effect_icon_cab_off);
                            lv_img_set_zoom(obj, 250);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_IconAmp
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_amp = obj;
                            lv_obj_set_pos(obj, 96, -10);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_effect_icon_amp_off);
                            lv_img_set_zoom(obj, 250);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_IconGate
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_gate = obj;
                            lv_obj_set_pos(obj, 44, -10);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_effect_icon_gate_off);
                            lv_img_set_zoom(obj, 250);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_IconEQ
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_eq = obj;
                            lv_obj_set_pos(obj, -12, -10);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_effect_icon_eq);
                            lv_img_set_zoom(obj, 250);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                    }
                }
                {
                    // ui_BottomPanelValeton
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.ui_bottom_panel_valeton = obj;
                    lv_obj_set_pos(obj, 0, 259);
                    lv_obj_set_size(obj, 480, 61);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2a2a2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_icon_val_pre
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_val_pre = obj;
                            lv_obj_set_pos(obj, -19, -14);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_pre_off);
                            lv_img_set_zoom(obj, 200);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_icon_val_amp
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_val_amp = obj;
                            lv_obj_set_pos(obj, 24, -14);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_amp_off);
                            lv_img_set_zoom(obj, 200);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_icon_val_nr
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_val_nr = obj;
                            lv_obj_set_pos(obj, 67, -14);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_nr_off);
                            lv_img_set_zoom(obj, 200);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_icon_val_rvb
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_val_rvb = obj;
                            lv_obj_set_pos(obj, 110, -14);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_rvb_off);
                            lv_img_set_zoom(obj, 200);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_icon_val_cab
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_val_cab = obj;
                            lv_obj_set_pos(obj, 153, -14);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_cab_off);
                            lv_img_set_zoom(obj, 200);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_icon_val_tc
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_val_tc = obj;
                            lv_obj_set_pos(obj, 196, -14);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_tc_off);
                            lv_img_set_zoom(obj, 200);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_icon_val_eq
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_val_eq = obj;
                            lv_obj_set_pos(obj, 239, -14);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_eq_off);
                            lv_img_set_zoom(obj, 200);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_icon_val_mod
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_val_mod = obj;
                            lv_obj_set_pos(obj, 282, -14);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_mod_off);
                            lv_img_set_zoom(obj, 200);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_icon_val_dly
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_val_dly = obj;
                            lv_obj_set_pos(obj, 325, -14);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_dly_off);
                            lv_img_set_zoom(obj, 200);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_icon_val_dst
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_icon_val_dst = obj;
                            lv_obj_set_pos(obj, 368, -14);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_dst_off);
                            lv_img_set_zoom(obj, 200);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                            lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // ui_val_settings
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_val_settings = obj;
                            lv_obj_set_pos(obj, 417, -10);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_settings);
                            lv_img_set_zoom(obj, 220);
                            lv_obj_add_event_cb(obj, action_show_settings_page, LV_EVENT_PRESSED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                    }
                }
                {
                    // ui_LeftArrow
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_left_arrow = obj;
                    lv_obj_set_pos(obj, 6, 90);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_arrow_left);
                    lv_img_set_zoom(obj, 192);
                    lv_obj_add_event_cb(obj, action_amp_skin_previous, LV_EVENT_PRESSED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN|LV_OBJ_FLAG_CLICKABLE);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                }
                {
                    // ui_RightArrow
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_right_arrow = obj;
                    lv_obj_set_pos(obj, 397, 90);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_arrow_right);
                    lv_img_set_zoom(obj, 192);
                    lv_obj_add_event_cb(obj, action_amp_skin_next, LV_EVENT_PRESSED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN|LV_OBJ_FLAG_CLICKABLE);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                }
                {
                    // ui_EntryKeyboard
                    lv_obj_t *obj = lv_keyboard_create(parent_obj);
                    objects.ui_entry_keyboard = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 480, 183);
                    lv_obj_add_event_cb(obj, action_keyboard_ok, LV_EVENT_READY, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // ui_OKTick
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_ok_tick = obj;
                    lv_obj_set_pos(obj, 197, 258);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_tick);
                    lv_img_set_zoom(obj, 192);
                    lv_obj_add_event_cb(obj, action_save_skin_edit, LV_EVENT_PRESSED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN|LV_OBJ_FLAG_CLICKABLE);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                }
            }
        }
    }
    lv_keyboard_set_textarea(objects.ui_entry_keyboard, objects.ui_preset_details_text_area);
    
    tick_screen_screen1();
}

void tick_screen_screen1() {
}

void create_screen_settings() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ui_SettingsTabView
            lv_obj_t *obj = lv_tabview_create(parent_obj, LV_DIR_TOP, 60);
            objects.ui_settings_tab_view = obj;
            lv_obj_set_pos(obj, 5, 5);
            lv_obj_set_size(obj, 480, 320);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_color(obj, lv_color_hex(0xff563f2a), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_tabview_get_tab_btns(parent_obj);
                    objects.obj0 = obj;
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2a2a2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffc0c0c0), LV_PART_ITEMS | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_ITEMS | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_LEFT, LV_PART_ITEMS | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_ITEMS | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_ITEMS | LV_STATE_CHECKED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xfffb9230), LV_PART_ITEMS | LV_STATE_CHECKED);
                }
                {
                    // ui_GateTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Gate");
                    objects.ui_gate_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffafafa), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_NoiseGateEnableLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_noise_gate_enable_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_NoiseGateSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_noise_gate_switch = obj;
                            lv_obj_set_pos(obj, 106, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_NoiseGatePostLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_noise_gate_post_label = obj;
                            lv_obj_set_pos(obj, 309, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Post");
                        }
                        {
                            // ui_NoiseGatePostSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_noise_gate_post_switch = obj;
                            lv_obj_set_pos(obj, 393, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_NoiseGateThresholdLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_noise_gate_threshold_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Threshold");
                        }
                        {
                            // ui_NoiseGateThresholdSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_noise_gate_threshold_slider = obj;
                            lv_obj_set_pos(obj, 113, -72);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_NoiseGateThresholdValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_noise_gate_threshold_value = obj;
                            lv_obj_set_pos(obj, 417, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_NoiseGateReleaseLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_noise_gate_release_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Release");
                        }
                        {
                            // ui_NoiseGateReleaseSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_noise_gate_release_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 5, 500);
                            lv_slider_set_value(obj, 20, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_NoiseGateReleaseValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_noise_gate_release_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_NoiseGateDepthLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_noise_gate_depth_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Depth");
                        }
                        {
                            // ui_NoiseGateDepthSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_noise_gate_depth_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -60, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_NoiseGateDepthValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_noise_gate_depth_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_CompresssorTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Comp");
                    objects.ui_compresssor_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_CompressorEnableLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_compressor_enable_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_CompressorEnableSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_compressor_enable_switch = obj;
                            lv_obj_set_pos(obj, 106, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_CompressorPostLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_compressor_post_label = obj;
                            lv_obj_set_pos(obj, 309, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Post");
                        }
                        {
                            // ui_CompressorPostSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_compressor_post_switch = obj;
                            lv_obj_set_pos(obj, 393, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_CompressorThresholdLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_compressor_threshold_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Threshold");
                        }
                        {
                            // ui_CompressorThresholdSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_compressor_threshold_slider = obj;
                            lv_obj_set_pos(obj, 113, -72);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -40, 0);
                            lv_slider_set_value(obj, -14, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_CompressorThresholdValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_compressor_threshold_value = obj;
                            lv_obj_set_pos(obj, 417, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_CompressorAttackLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_compressor_attack_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Attack");
                        }
                        {
                            // ui_CompressorAttackSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_compressor_attack_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 1, 51);
                            lv_slider_set_value(obj, 14, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_CompressorAttackValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_compressor_attack_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_CompressorGainLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_compressor_gain_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Gain");
                        }
                        {
                            // ui_CompressorGainSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_compressor_gain_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -30, 10);
                            lv_slider_set_value(obj, -12, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_CompressorGainValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_compressor_gain_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_AmpTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Amp");
                    objects.ui_amp_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_AmpEnableLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_amp_enable_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_AmpEnableSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_amp_enable_switch = obj;
                            lv_obj_set_pos(obj, 106, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_AmpCabLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_amp_cab_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Cab");
                        }
                        {
                            // ui_CabinetModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_cabinet_model_dropdown = obj;
                            lv_obj_set_pos(obj, 106, 37);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "Tone Model\nVIR\nDisabled");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj1 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_AmplifierGainLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_amplifier_gain_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Gain");
                        }
                        {
                            // ui_AmplifierGainSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_amplifier_gain_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 0, 10);
                            lv_slider_set_value(obj, 5, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_AmplifierGainValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_amplifier_gain_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_AmplifierVolumeLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_amplifier_volume_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Volume");
                        }
                        {
                            // ui_AmplifierVolumeSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_amplifier_volume_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 0, 10);
                            lv_slider_set_value(obj, 5, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_AmplifierVolumeValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_amplifier_volume_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_AmplifierPresenseLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_amplifier_presense_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Presence");
                        }
                        {
                            // ui_AmplifierPresenseSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_amplifier_presense_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 0, 10);
                            lv_slider_set_value(obj, 5, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_AmplifierPresenseValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_amplifier_presense_value = obj;
                            lv_obj_set_pos(obj, 417, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_AmplifierDepthLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_amplifier_depth_label = obj;
                            lv_obj_set_pos(obj, 4, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Depth");
                        }
                        {
                            // ui_AmplifierDepthSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_amplifier_depth_slider = obj;
                            lv_obj_set_pos(obj, 113, 80);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 0, 10);
                            lv_slider_set_value(obj, 5, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_AmplifierDepthValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_amplifier_depth_value = obj;
                            lv_obj_set_pos(obj, 417, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_EQTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "EQ");
                    objects.ui_eq_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_EQPostLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_eq_post_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Post");
                        }
                        {
                            // ui_EQPostSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_eq_post_switch = obj;
                            lv_obj_set_pos(obj, 106, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_EQBassLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_eq_bass_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Bass");
                        }
                        {
                            // ui_EQBassSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_eq_bass_slider = obj;
                            lv_obj_set_pos(obj, 113, -72);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 0, 10);
                            lv_slider_set_value(obj, 5, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_EQBassValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_eq_bass_value = obj;
                            lv_obj_set_pos(obj, 417, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_EQMidLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_eq_mid_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Mid");
                        }
                        {
                            // ui_EQMidSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_eq_mid_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 0, 10);
                            lv_slider_set_value(obj, 5, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_EQMidValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_eq_mid_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_EQMidQLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_eq_mid_qlabel = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "MidQ");
                        }
                        {
                            // ui_EQMidQSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_eq_mid_qslider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 0, 10);
                            lv_slider_set_value(obj, 5, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_EQMidQValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_eq_mid_qvalue = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_EQTrebleLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_eq_treble_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Treble");
                        }
                        {
                            // ui_EQTrebleSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_eq_treble_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 0, 10);
                            lv_slider_set_value(obj, 5, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_EQTrebleValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_eq_treble_value = obj;
                            lv_obj_set_pos(obj, 417, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_ModulationTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Mod");
                    objects.ui_modulation_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_ModulationEnableLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_enable_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_ModulationEnableSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_modulation_enable_switch = obj;
                            lv_obj_set_pos(obj, 106, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ModulationPostLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_post_label = obj;
                            lv_obj_set_pos(obj, 309, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Post");
                        }
                        {
                            // ui_ModulationPostSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_modulation_post_switch = obj;
                            lv_obj_set_pos(obj, 393, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ModulationModeLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_mode_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Model");
                        }
                        {
                            // ui_ModulationModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_modulation_model_dropdown = obj;
                            lv_obj_set_pos(obj, 106, 37);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "Chorus\nTremolo\nPhaser\nFlanger\nRotary");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj2 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ModulationSyncLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_sync_label = obj;
                            lv_obj_set_pos(obj, 309, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Sync");
                        }
                        {
                            // ui_ModulationSyncSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_modulation_sync_switch = obj;
                            lv_obj_set_pos(obj, 393, -71);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ModulationParam1Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_param1_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param1");
                        }
                        {
                            // ui_ModulationTSDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_modulation_ts_dropdown = obj;
                            lv_obj_set_pos(obj, 106, 76);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "1/32\n1/32 D\n1/32 T\n1/16\n1/16 D\n1/16 T\n1/8\n1/8 D\n1/8 T\n1/4\n1/4 D\n1/4 T\n1/2\n1/2 D\n1/2 T\n1/1\n1/1 D\n1/1 T");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj3 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ModulationParam1Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_modulation_param1_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_value(obj, 5, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ModulationParam1Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_param1_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ModulationParam2Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_param2_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param2");
                        }
                        {
                            // ui_ModulationParam2Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_modulation_param2_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 0, 1000);
                            lv_slider_set_value(obj, 5, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ModulationParam2Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_param2_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ModulationParam3Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_param3_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param3");
                        }
                        {
                            // ui_ModulationParam3Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_modulation_param3_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ModulationParam3Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_param3_value = obj;
                            lv_obj_set_pos(obj, 417, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ModulationParam4Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_param4_label = obj;
                            lv_obj_set_pos(obj, 4, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param3");
                        }
                        {
                            // ui_ModulationParam4Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_modulation_param4_slider = obj;
                            lv_obj_set_pos(obj, 113, 80);
                            lv_obj_set_size(obj, 290, 15);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ModulationParam4Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_param4_value = obj;
                            lv_obj_set_pos(obj, 417, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_DelayTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Dly");
                    objects.ui_delay_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_DelayEnableLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_delay_enable_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_DelayEnableSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_delay_enable_switch = obj;
                            lv_obj_set_pos(obj, 106, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_DelayPostLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_delay_post_label = obj;
                            lv_obj_set_pos(obj, 309, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Post");
                        }
                        {
                            // ui_DelayPostSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_delay_post_switch = obj;
                            lv_obj_set_pos(obj, 393, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_DelayModeLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_delay_mode_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Model");
                        }
                        {
                            // ui_DelayModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_delay_model_dropdown = obj;
                            lv_obj_set_pos(obj, 106, 37);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "Digital\nTape");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj4 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_DelaySyncLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_delay_sync_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Sync");
                        }
                        {
                            // ui_DelaySyncSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_delay_sync_switch = obj;
                            lv_obj_set_pos(obj, 106, -34);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_DelayPingPongLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_delay_ping_pong_label = obj;
                            lv_obj_set_pos(obj, 309, -71);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "PPng");
                        }
                        {
                            // ui_DelayPingPongSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_delay_ping_pong_switch = obj;
                            lv_obj_set_pos(obj, 393, -71);
                            lv_obj_set_size(obj, 60, 26);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_DelayTSLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_delay_ts_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Time");
                        }
                        {
                            // ui_DelayTSSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_delay_ts_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 0, 1000);
                            lv_slider_set_value(obj, 5, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_DelayTSValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_delay_ts_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_DelayTSDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_delay_ts_dropdown = obj;
                            lv_obj_set_pos(obj, 106, 114);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "1/32\n1/32 D\n1/32 T\n1/16\n1/16 D\n1/16 T\n1/8\n1/8 D\n1/8 T\n1/4\n1/4 D\n1/4 T\n1/2\n1/2 D\n1/2 T\n1/1\n1/1 D\n1/1 T");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj5 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_DelayFeedbackLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_delay_feedback_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Feedback");
                        }
                        {
                            // ui_DelayFeedbackSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_delay_feedback_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_DelayFeedbackValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_delay_feedback_value = obj;
                            lv_obj_set_pos(obj, 417, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_DelayMixLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_delay_mix_label = obj;
                            lv_obj_set_pos(obj, 4, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Mix");
                        }
                        {
                            // ui_DelayMixSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_delay_mix_slider = obj;
                            lv_obj_set_pos(obj, 113, 80);
                            lv_obj_set_size(obj, 290, 15);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_DelayMixValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_delay_mix_value = obj;
                            lv_obj_set_pos(obj, 417, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_ReverbTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Rev");
                    objects.ui_reverb_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_ReverbEnableLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_reverb_enable_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_ReverbEnableSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_reverb_enable_switch = obj;
                            lv_obj_set_pos(obj, 106, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ReverbPostLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_reverb_post_label = obj;
                            lv_obj_set_pos(obj, 309, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Post");
                        }
                        {
                            // ui_ReverbPostSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_reverb_post_switch = obj;
                            lv_obj_set_pos(obj, 393, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ReverbModeLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_reverb_mode_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Model");
                        }
                        {
                            // ui_ReverbModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_reverb_model_dropdown = obj;
                            lv_obj_set_pos(obj, 106, 37);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "Spring 1\nSpring 2\nSpring 3\nSpring 4\nRoom\nPlate");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj6 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ReverbMixLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_reverb_mix_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Mix");
                        }
                        {
                            // ui_ReverbMixSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_reverb_mix_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 0, 10);
                            lv_slider_set_value(obj, 5, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ReverbMixValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_reverb_mix_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ReverbTimeLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_reverb_time_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Time");
                        }
                        {
                            // ui_ReverbTimeSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_reverb_time_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 0, 10);
                            lv_slider_set_value(obj, 5, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ReverbTimeValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_reverb_time_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ReverbPredelayLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_reverb_predelay_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Pre-Delay");
                        }
                        {
                            // ui_ReverbPredelaySlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_reverb_predelay_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 0, 500);
                            lv_slider_set_value(obj, 5, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ReverbPredelayValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_reverb_predelay_value = obj;
                            lv_obj_set_pos(obj, 417, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ReverbColorLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_reverb_color_label = obj;
                            lv_obj_set_pos(obj, 4, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Color");
                        }
                        {
                            // ui_ReverbColorSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_reverb_color_slider = obj;
                            lv_obj_set_pos(obj, 113, 80);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -10, 10);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ReverbColorValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_reverb_color_value = obj;
                            lv_obj_set_pos(obj, 417, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_GlobalTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Glob");
                    objects.ui_global_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_CabBypassLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_cab_bypass_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Cab Byp");
                        }
                        {
                            // ui_CabBypassSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_cab_bypass_switch = obj;
                            lv_obj_set_pos(obj, 106, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_TempoSourcetLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_tempo_sourcet_label = obj;
                            lv_obj_set_pos(obj, 309, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Tempo");
                        }
                        {
                            // ui_TempoSourceSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_tempo_source_switch = obj;
                            lv_obj_set_pos(obj, 393, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff674d35), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_BPMLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_bpm_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "BPM");
                        }
                        {
                            // ui_BPMSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_bpm_slider = obj;
                            lv_obj_set_pos(obj, 113, -72);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 40, 240);
                            lv_slider_set_value(obj, 60, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_BPMValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_bpm_value = obj;
                            lv_obj_set_pos(obj, 417, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_InputTrimLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_input_trim_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Input Trim");
                        }
                        {
                            // ui_InputTrimSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_input_trim_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -15, 15);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_InputTrimValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_input_trim_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_TuningReferenceLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_tuning_reference_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Tuning Ref");
                        }
                        {
                            // ui_TuningReferenceSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_tuning_reference_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, 415, 465);
                            lv_slider_set_value(obj, 440, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_TuningReferenceValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_tuning_reference_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_VolumeLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_volume_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Volume");
                        }
                        {
                            // ui_VolumeSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_volume_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -40, 3);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff424242), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffb9230), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_VolumeValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_volume_value = obj;
                            lv_obj_set_pos(obj, 417, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
            }
        }
        {
            // ui_Closeimage
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.ui_closeimage = obj;
            lv_obj_set_pos(obj, 202, 266);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_tick);
            lv_img_set_zoom(obj, 120);
            lv_obj_add_event_cb(obj, action_close_settings_page, LV_EVENT_PRESSED, (void *)0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
    }
    
    tick_screen_settings();
}

void tick_screen_settings() {
}

void create_screen_val_settings() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.val_settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ui_ValSettingsTabView
            lv_obj_t *obj = lv_tabview_create(parent_obj, LV_DIR_TOP, 52);
            objects.ui_val_settings_tab_view = obj;
            lv_obj_set_pos(obj, 5, 5);
            lv_obj_set_size(obj, 480, 320);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_color(obj, lv_color_hex(0xff3b3c40), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_tabview_get_tab_btns(parent_obj);
                    objects.obj7 = obj;
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2a2a2a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_ITEMS | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_ITEMS | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_LEFT, LV_PART_ITEMS | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff96969e), LV_PART_ITEMS | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_ITEMS | LV_STATE_CHECKED);
                }
                {
                    // ui_ValNRTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "NR");
                    objects.ui_val_nr_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffafafa), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_ValNRBlockLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_nr_block_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_ValNRBlockSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_val_nr_block_switch = obj;
                            lv_obj_set_pos(obj, 113, -109);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ValNRModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_val_nr_model_dropdown = obj;
                            lv_obj_set_pos(obj, 298, 1);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "Noise Gate");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED | LV_STATE_PRESSED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj8 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ValNRParam0Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_nr_param0_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param0");
                        }
                        {
                            // ui_ValNRParam0Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_nr_param0_slider = obj;
                            lv_obj_set_pos(obj, 113, -72);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValNRParam0Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_nr_param0_value = obj;
                            lv_obj_set_pos(obj, 417, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValNRParam1Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_nr_param1_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param1");
                        }
                        {
                            // ui_ValNRParam1Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_nr_param1_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValNRParam1Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_nr_param1_value = obj;
                            lv_obj_set_pos(obj, 417, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValNRParam2Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_nr_param2_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param2");
                        }
                        {
                            // ui_ValNRParam2Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_nr_param2_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValNRParam2Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_nr_param2_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValNRParam3Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_nr_param3_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param3");
                        }
                        {
                            // ui_ValNRParam3Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_nr_param3_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValNRParam3Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_nr_param3_value = obj;
                            lv_obj_set_pos(obj, 417, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValNRParam4Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_nr_param4_label = obj;
                            lv_obj_set_pos(obj, 4, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param4");
                        }
                        {
                            // ui_ValNRParam4Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_nr_param4_slider = obj;
                            lv_obj_set_pos(obj, 113, 80);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValNRParam4Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_nr_param4_value = obj;
                            lv_obj_set_pos(obj, 417, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_ValPreTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Pre");
                    objects.ui_val_pre_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_ValNPreBlockLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_npre_block_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_ValPreBlockSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_val_pre_block_switch = obj;
                            lv_obj_set_pos(obj, 113, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ValPreModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_val_pre_model_dropdown = obj;
                            lv_obj_set_pos(obj, 298, 1);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "Comp\nComp4\nBoost\nMicro Boost\nB-Boost\nToucher\nCrier\nOcta\nPitch\nDetune");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED | LV_STATE_PRESSED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj9 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ValPreParam0Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_pre_param0_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param0");
                        }
                        {
                            // ui_ValPreParam0Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_pre_param0_slider = obj;
                            lv_obj_set_pos(obj, 113, -72);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValPreParam0Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_pre_param0_value = obj;
                            lv_obj_set_pos(obj, 417, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValPreParam1Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_pre_param1_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param1");
                        }
                        {
                            // ui_ValPreParam1Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_pre_param1_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValPreParam1Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_pre_param1_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValPreParam2Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_pre_param2_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param2");
                        }
                        {
                            // ui_ValPreParam2Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_pre_param2_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValPreParam2Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_pre_param2_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValPreParam3Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_pre_param3_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param3");
                        }
                        {
                            // ui_ValPreParam3Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_pre_param3_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValPreParam3Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_pre_param3_value = obj;
                            lv_obj_set_pos(obj, 417, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValPreParam4Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_pre_param4_label = obj;
                            lv_obj_set_pos(obj, 4, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param4");
                        }
                        {
                            // ui_ValPreParam4Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_pre_param4_slider = obj;
                            lv_obj_set_pos(obj, 113, 80);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValPreParam4Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_pre_param4_value = obj;
                            lv_obj_set_pos(obj, 417, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_ValDstTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Dst");
                    objects.ui_val_dst_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_ValDstBlockLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dst_block_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_ValDstBlockSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_val_dst_block_switch = obj;
                            lv_obj_set_pos(obj, 113, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ValDstModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_val_dst_model_dropdown = obj;
                            lv_obj_set_pos(obj, 298, 1);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "Green OD\nYellow OD\nSuper OD\nSM Dist\nPlustortion\nLa Charger\nDarktale\nSora Fuzz\nRed Haze\nBass OD");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED | LV_STATE_PRESSED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj10 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ValDstParam0Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dst_param0_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param0");
                        }
                        {
                            // ui_ValDstParam0Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_dst_param0_slider = obj;
                            lv_obj_set_pos(obj, 113, -72);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValDstParam0Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dst_param0_value = obj;
                            lv_obj_set_pos(obj, 417, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValDstParam1Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dst_param1_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param1");
                        }
                        {
                            // ui_ValDstParam1Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_dst_param1_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValDstParam1Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dst_param1_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValDstParam2Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dst_param2_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param2");
                        }
                        {
                            // ui_ValDstParam2Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_dst_param2_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValDstParam2Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dst_param2_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValDstParam3Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dst_param3_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param3");
                        }
                        {
                            // ui_ValDstParam3Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_dst_param3_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValDstParam3Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dst_param3_value = obj;
                            lv_obj_set_pos(obj, 417, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValDstParam4Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dst_param4_label = obj;
                            lv_obj_set_pos(obj, 4, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param4");
                        }
                        {
                            // ui_ValDstParam4Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_dst_param4_slider = obj;
                            lv_obj_set_pos(obj, 113, 80);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValDstParam4Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dst_param4_value = obj;
                            lv_obj_set_pos(obj, 417, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_ValAmpTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Amp");
                    objects.ui_val_amp_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_ValAmpBlockLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_amp_block_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_ValAmpBlockSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_val_amp_block_switch = obj;
                            lv_obj_set_pos(obj, 113, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ValAmpModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_val_amp_model_dropdown = obj;
                            lv_obj_set_pos(obj, 298, 1);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "Tweedy\nBellman 59N\nDark Twin\nFoxy 30N\nJ-120 CL\nMatch CL\nL-Star CL\nUK 45\nUK 50JP\nUK 800\nBellman 59B\nFoxy 30TB\nSup Dual OD\nSolo100 OD\nZ38 OD\nBad KT OD\nJuiceW R100\nDizz VH\nDizz VH+\nEagle 120\nEV51\nSolo100 LD\nMess Dual V\nMess Dual M\nPower LD\nFlagman+\nBog Red V\nClassic Bass\nFoxy Bass\nMess Bass\nAC Pre 1\nAC Pre 2\n");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED | LV_STATE_PRESSED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj11 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ValAmpParam0Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_amp_param0_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param0");
                        }
                        {
                            // ui_ValAmpParam0Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_amp_param0_slider = obj;
                            lv_obj_set_pos(obj, 113, -72);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValAmpParam0Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_amp_param0_value = obj;
                            lv_obj_set_pos(obj, 417, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValAmpParam1Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_amp_param1_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param1");
                        }
                        {
                            // ui_ValAmpParam1Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_amp_param1_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValAmpParam1Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_amp_param1_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValAmpParam2Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_amp_param2_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param2");
                        }
                        {
                            // ui_ValAmpParam2Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_amp_param2_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValAmpParam2Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_amp_param2_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValAmpParam3Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_amp_param3_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param3");
                        }
                        {
                            // ui_ValAmpParam3Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_amp_param3_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValAmpParam3Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_amp_param3_value = obj;
                            lv_obj_set_pos(obj, 417, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValAmpParam4Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_amp_param4_label = obj;
                            lv_obj_set_pos(obj, 4, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param4");
                        }
                        {
                            // ui_ValAmpParam4Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_amp_param4_slider = obj;
                            lv_obj_set_pos(obj, 113, 80);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValAmpParam4Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_amp_param4_value = obj;
                            lv_obj_set_pos(obj, 417, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_ValCabTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Cab");
                    objects.ui_val_cab_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_ValCabBlockLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_cab_block_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_ValCabBlockSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_val_cab_block_switch = obj;
                            lv_obj_set_pos(obj, 113, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ValCabModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_val_cab_model_dropdown = obj;
                            lv_obj_set_pos(obj, 298, 1);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "Twd CP\nDark VIT\nFoxy 1x12\nL-Star 1x12\nDark CS 2x12\nDark Twin 2x12\nSup Star 2x12\nJ-120 2x12\nFoxy 2x12\nUK Grn 2x12\nUK Grn 4x12\nBog 4x12\nDizz 4x12\nEV 4x12\nSolo 4x12\nMess 4x12\nEagle 4x12\nJuice 4x12\nBellman 2x12\nAmpg 4x10");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED | LV_STATE_PRESSED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj12 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ValCabParam0Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_cab_param0_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param0");
                        }
                        {
                            // ui_ValCabParam0Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_cab_param0_slider = obj;
                            lv_obj_set_pos(obj, 113, -72);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValCabParam0Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_cab_param0_value = obj;
                            lv_obj_set_pos(obj, 417, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValCabParam1Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_cab_param1_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param1");
                        }
                        {
                            // ui_ValCabParam1Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_cab_param1_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValCabParam1Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_cab_param1_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValCabParam2Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_cab_param2_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param2");
                        }
                        {
                            // ui_ValCabParam2Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_cab_param2_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValCabParam2Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_cab_param2_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValCabParam3Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_cab_param3_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param3");
                        }
                        {
                            // ui_ValCabParam3Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_cab_param3_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValCabParam3Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_cab_param3_value = obj;
                            lv_obj_set_pos(obj, 417, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValCabParam4Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_cab_param4_label = obj;
                            lv_obj_set_pos(obj, 4, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param4");
                        }
                        {
                            // ui_ValCabParam4Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_cab_param4_slider = obj;
                            lv_obj_set_pos(obj, 113, 80);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValCabParam4Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_cab_param4_value = obj;
                            lv_obj_set_pos(obj, 417, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_ValEQTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "EQ");
                    objects.ui_val_eq_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_ValEQBlockLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_eq_block_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_ValEQBlockSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_val_eq_block_switch = obj;
                            lv_obj_set_pos(obj, 113, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ValEQModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_val_eq_model_dropdown = obj;
                            lv_obj_set_pos(obj, 298, 1);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "Guitar 1\nGuitar 2\nBass 1\nBass 2\nMess");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED | LV_STATE_PRESSED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj13 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ValEQParam0Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_eq_param0_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param0");
                        }
                        {
                            // ui_ValEQParam0Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_eq_param0_slider = obj;
                            lv_obj_set_pos(obj, 113, -72);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValEQParam0Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_eq_param0_value = obj;
                            lv_obj_set_pos(obj, 417, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValEQParam1Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_eq_param1_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param1");
                        }
                        {
                            // ui_ValEQParam1Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_eq_param1_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValEQParam1Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_eq_param1_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValEQParam2Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_eq_param2_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param2");
                        }
                        {
                            // ui_ValEQParam2Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_eq_param2_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValEQParam2Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_eq_param2_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValEQParam3Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_eq_param3_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param3");
                        }
                        {
                            // ui_ValEQParam3Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_eq_param3_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValEQParam3Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_eq_param3_value = obj;
                            lv_obj_set_pos(obj, 417, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValEQParam4Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_eq_param4_label = obj;
                            lv_obj_set_pos(obj, 4, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param4");
                        }
                        {
                            // ui_ValEQParam4Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_eq_param4_slider = obj;
                            lv_obj_set_pos(obj, 113, 80);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValEQParam4Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_eq_param4_value = obj;
                            lv_obj_set_pos(obj, 417, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_ValModTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Mod");
                    objects.ui_val_mod_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_ValModBlockLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_mod_block_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_ValModBlockSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_val_mod_block_switch = obj;
                            lv_obj_set_pos(obj, 113, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ValModModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_val_mod_model_dropdown = obj;
                            lv_obj_set_pos(obj, 298, 1);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "A-Chorus\nB-Chorus\nJet\nN-Jet\nO-Phase\nM-Vibe\nV-Roto\nVibrato\nO-Trem\nSine Trem\nBias Trem");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED | LV_STATE_PRESSED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj14 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ValModParam0Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_mod_param0_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param0");
                        }
                        {
                            // ui_ValModParam0Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_mod_param0_slider = obj;
                            lv_obj_set_pos(obj, 113, -72);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValModParam0Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_mod_param0_value = obj;
                            lv_obj_set_pos(obj, 417, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValModParam1Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_mod_param1_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param1");
                        }
                        {
                            // ui_ValModParam1Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_mod_param1_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValModParam1Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_mod_param1_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValModParam2Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_mod_param2_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param2");
                        }
                        {
                            // ui_ValModParam2Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_mod_param2_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValModParam2Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_mod_param2_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValModParam3Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_mod_param3_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param3");
                        }
                        {
                            // ui_ValModParam3Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_mod_param3_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValModParam3Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_mod_param3_value = obj;
                            lv_obj_set_pos(obj, 417, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValModParam4Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_mod_param4_label = obj;
                            lv_obj_set_pos(obj, 4, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param4");
                        }
                        {
                            // ui_ValModParam4Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_mod_param4_slider = obj;
                            lv_obj_set_pos(obj, 113, 80);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValModParam4Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_mod_param4_value = obj;
                            lv_obj_set_pos(obj, 417, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_ValDlyTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Dly");
                    objects.ui_val_dly_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_ValDlyBlockLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dly_block_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_ValDlyBlockSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_val_dly_block_switch = obj;
                            lv_obj_set_pos(obj, 113, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ValDlyModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_val_dly_model_dropdown = obj;
                            lv_obj_set_pos(obj, 298, 1);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "Pure\nAnalog\nSlapback\nSweet Echo\nTape\nTube\nRev Echo\nRing Echo\nSweep Echo\nPing Pong");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED | LV_STATE_PRESSED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj15 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ValDlyParam0Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dly_param0_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param0");
                        }
                        {
                            // ui_ValDlyParam0Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_dly_param0_slider = obj;
                            lv_obj_set_pos(obj, 113, -72);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValDlyParam0Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dly_param0_value = obj;
                            lv_obj_set_pos(obj, 417, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValDlyParam1Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dly_param1_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param1");
                        }
                        {
                            // ui_ValDlyParam1Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_dly_param1_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValDlyParam1Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dly_param1_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValDlyParam2Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dly_param2_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param2");
                        }
                        {
                            // ui_ValDlyParam2Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_dly_param2_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValDlyParam2Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dly_param2_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValDlyParam3Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dly_param3_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param3");
                        }
                        {
                            // ui_ValDlyParam3Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_dly_param3_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValDlyParam3Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dly_param3_value = obj;
                            lv_obj_set_pos(obj, 417, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValDlyParam4Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dly_param4_label = obj;
                            lv_obj_set_pos(obj, 4, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param4");
                        }
                        {
                            // ui_ValDlyParam4Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_dly_param4_slider = obj;
                            lv_obj_set_pos(obj, 113, 80);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValDlyParam4Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_dly_param4_value = obj;
                            lv_obj_set_pos(obj, 417, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_ValRvbTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Rvb");
                    objects.ui_val_rvb_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_ValRvbBlockLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_rvb_block_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_ValRvbBlockSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_val_rvb_block_switch = obj;
                            lv_obj_set_pos(obj, 113, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ValRvbModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_val_rvb_model_dropdown = obj;
                            lv_obj_set_pos(obj, 298, 1);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "Air\nRoom\nHall\nChurch\nPlate L\nPlate\nSpring\nN-Star\nDeepsea\nSweet Space");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED | LV_STATE_PRESSED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj16 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ValRvbParam0Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_rvb_param0_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param0");
                        }
                        {
                            // ui_ValRvbParam0Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_rvb_param0_slider = obj;
                            lv_obj_set_pos(obj, 113, -72);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValRvbParam0Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_rvb_param0_value = obj;
                            lv_obj_set_pos(obj, 417, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValRvbParam1Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_rvb_param1_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param1");
                        }
                        {
                            // ui_ValRvbParam1Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_rvb_param1_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValRvbParam1Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_rvb_param1_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValRvbParam2Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_rvb_param2_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param2");
                        }
                        {
                            // ui_ValRvbParam2Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_rvb_param2_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValRvbParam2Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_rvb_param2_value = obj;
                            lv_obj_set_pos(obj, 417, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValRvbParam3Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_rvb_param3_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param3");
                        }
                        {
                            // ui_ValRvbParam3Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_rvb_param3_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValRvbParam3Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_rvb_param3_value = obj;
                            lv_obj_set_pos(obj, 417, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValRvbParam4Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_rvb_param4_label = obj;
                            lv_obj_set_pos(obj, 4, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param4");
                        }
                        {
                            // ui_ValRvbParam4Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_rvb_param4_slider = obj;
                            lv_obj_set_pos(obj, 113, 80);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValRvbParam4Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_rvb_param4_value = obj;
                            lv_obj_set_pos(obj, 417, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_ValNSTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "NS");
                    objects.ui_val_ns_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_ValNSBlockLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_ns_block_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_ValNSBlockSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_val_ns_block_switch = obj;
                            lv_obj_set_pos(obj, 113, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ValNSModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_val_ns_model_dropdown = obj;
                            lv_obj_set_pos(obj, 298, 1);
                            lv_obj_set_size(obj, 165, 38);
                            lv_dropdown_set_options(obj, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n52\n53\n54\n55\n56\n57\n58\n59\n60\n61\n62\n63\n64\n65\n66\n67\n68\n69\n70\n71\n72\n73\n74\n75\n76\n77\n78\n79");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_SELECTED | LV_STATE_CHECKED | LV_STATE_PRESSED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj17 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ValNSParam0Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_ns_param0_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param0");
                        }
                        {
                            // ui_ValNSParam0Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_ns_param0_slider = obj;
                            lv_obj_set_pos(obj, 113, -72);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValNSParam0Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_ns_param0_value = obj;
                            lv_obj_set_pos(obj, 417, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValNSParam1Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_ns_param1_label = obj;
                            lv_obj_set_pos(obj, 4, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param1");
                        }
                        {
                            // ui_ValNSParam1Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_ns_param1_slider = obj;
                            lv_obj_set_pos(obj, 113, -34);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValNSParam1Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_ns_param1_value = obj;
                            lv_obj_set_pos(obj, 417, -34);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValNSParam2Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_ns_param2_label = obj;
                            lv_obj_set_pos(obj, 4, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param2");
                        }
                        {
                            // ui_ValNSParam2Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_ns_param2_slider = obj;
                            lv_obj_set_pos(obj, 113, 4);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValNSParam2Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_ns_param2_value = obj;
                            lv_obj_set_pos(obj, 413, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValNSParam3Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_ns_param3_label = obj;
                            lv_obj_set_pos(obj, 4, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param3");
                        }
                        {
                            // ui_ValNSParam3Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_ns_param3_slider = obj;
                            lv_obj_set_pos(obj, 113, 42);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValNSParam3Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_ns_param3_value = obj;
                            lv_obj_set_pos(obj, 413, 42);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ValNSParam4Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_ns_param4_label = obj;
                            lv_obj_set_pos(obj, 4, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param4");
                        }
                        {
                            // ui_ValNSParam4Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_ns_param4_slider = obj;
                            lv_obj_set_pos(obj, 113, 80);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValNSParam4Value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_ns_param4_value = obj;
                            lv_obj_set_pos(obj, 417, 80);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
                {
                    // ui_ValGlobTab
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Glob");
                    objects.ui_val_glob_tab = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffb9230), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_ValGlobNoCabLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_glob_no_cab_label = obj;
                            lv_obj_set_pos(obj, 4, -110);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "No Cab");
                        }
                        {
                            // ui_ValGlobNoCabSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_val_glob_no_cab_switch = obj;
                            lv_obj_set_pos(obj, 113, -110);
                            lv_obj_set_size(obj, 60, 24);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_CHECKED);
                        }
                        {
                            // ui_ValGlobInputLevelLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_glob_input_level_label = obj;
                            lv_obj_set_pos(obj, 4, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Input Level");
                        }
                        {
                            // ui_ValGlobInputLevelSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_val_glob_input_level_slider = obj;
                            lv_obj_set_pos(obj, 113, -72);
                            lv_obj_set_size(obj, 290, 15);
                            lv_slider_set_range(obj, -100, -20);
                            lv_slider_set_value(obj, -64, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_RELEASED, (void *)0);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffaeb1b4), LV_PART_MAIN | LV_STATE_SCROLLED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3890d5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            // ui_ValGlobInputLevelValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_val_glob_input_level_value = obj;
                            lv_obj_set_pos(obj, 417, -72);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                    }
                }
            }
        }
        {
            // ui_ValCloseimage
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.ui_val_closeimage = obj;
            lv_obj_set_pos(obj, 202, 266);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_tick);
            lv_img_set_zoom(obj, 120);
            lv_obj_add_event_cb(obj, action_close_settings_page, LV_EVENT_PRESSED, (void *)0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
    }
    
    tick_screen_val_settings();
}

void tick_screen_val_settings() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_screen1,
    tick_screen_settings,
    tick_screen_val_settings,
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
    create_screen_settings();
    create_screen_val_settings();
}
