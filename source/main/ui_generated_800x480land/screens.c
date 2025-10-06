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
    lv_obj_set_size(obj, 800, 480);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ui_BottomPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.ui_bottom_panel = obj;
            lv_obj_set_pos(obj, 0, 409);
            lv_obj_set_size(obj, 800, 71);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2a2a2a), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ui_SettingsImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_settings_image = obj;
                    lv_obj_set_pos(obj, 618, -11);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_settings);
                    lv_obj_add_event_cb(obj, action_show_settings_page, LV_EVENT_PRESSED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                }
                {
                    // ui_IconReverb
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_icon_reverb = obj;
                    lv_obj_set_pos(obj, 549, -9);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_effect_icon_reverb_off);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                }
                {
                    // ui_IconDelay
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_icon_delay = obj;
                    lv_obj_set_pos(obj, 479, -9);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_effect_icon_delay_off);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                }
                {
                    // ui_IconMod
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_icon_mod = obj;
                    lv_obj_set_pos(obj, 416, -9);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_effect_icon_mod_off);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                }
                {
                    // ui_IconComp
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_icon_comp = obj;
                    lv_obj_set_pos(obj, 356, -9);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_effect_icon_comp_off);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                }
                {
                    // ui_IconCab
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_icon_cab = obj;
                    lv_obj_set_pos(obj, 291, -9);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_effect_icon_cab_off);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                }
                {
                    // ui_IconAmp
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_icon_amp = obj;
                    lv_obj_set_pos(obj, 223, -9);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_effect_icon_amp_off);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                }
                {
                    // ui_IconGate
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_icon_gate = obj;
                    lv_obj_set_pos(obj, 156, -9);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_effect_icon_gate_off);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                }
                {
                    // ui_IconEQ
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_icon_eq = obj;
                    lv_obj_set_pos(obj, 89, -9);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_effect_icon_eq);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_SHORT_CLICKED, (void *)0);
                    lv_obj_add_event_cb(obj, action_effect_icon_clicked, LV_EVENT_LONG_PRESSED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                }
                {
                    // ui_NextContainer
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.ui_next_container = obj;
                    lv_obj_set_pos(obj, 703, -16);
                    lv_obj_set_size(obj, 70, 60);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_add_event_cb(obj, action_next_clicked, LV_EVENT_PRESSED, (void *)0);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_NextButton
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_next_button = obj;
                            lv_obj_set_pos(obj, 8, 7);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_next);
                            lv_obj_add_event_cb(obj, action_next_clicked, LV_EVENT_PRESSED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                    }
                }
                {
                    // ui_PreviousContainer
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.ui_previous_container = obj;
                    lv_obj_set_pos(obj, -16, -16);
                    lv_obj_set_size(obj, 70, 60);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_add_event_cb(obj, action_previous_clicked, LV_EVENT_PRESSED, (void *)0);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ui_PreviousButton
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.ui_previous_button = obj;
                            lv_obj_set_pos(obj, 8, 7);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_previous);
                            lv_obj_add_event_cb(obj, action_previous_clicked, LV_EVENT_PRESSED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                    }
                }
            }
        }
        {
            // ui_TopPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.ui_top_panel = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 800, 51);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2a2a2a), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ui_USBStatusFail
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_usb_status_fail = obj;
                    lv_obj_set_pos(obj, 709, -21);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_usb_fail);
                }
                {
                    // ui_ProjectHeadingLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ui_project_heading_label = obj;
                    lv_obj_set_pos(obj, 110, -15);
                    lv_obj_set_size(obj, 508, 38);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "TONEX Controller");
                }
                {
                    // ui_WiFiStatusDisconn
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_wi_fi_status_disconn = obj;
                    lv_obj_set_pos(obj, 625, -19);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_wifi_disconn);
                }
                {
                    // ui_BTStatusDisconn
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_bt_status_disconn = obj;
                    lv_obj_set_pos(obj, 680, -19);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_bt_disconn);
                }
                {
                    // ui_IKLogo
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_ik_logo = obj;
                    lv_obj_set_pos(obj, -22, -17);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_smythbuilt);
                }
                {
                    // ui_WiFiStatusConn
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_wi_fi_status_conn = obj;
                    lv_obj_set_pos(obj, 625, -19);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_wifi_conn);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                }
                {
                    // ui_BTStatusConn
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_bt_status_conn = obj;
                    lv_obj_set_pos(obj, 680, -19);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_bt_conn);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                }
                {
                    // ui_USBStatusOK
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_usb_status_ok = obj;
                    lv_obj_set_pos(obj, 709, -21);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_usb_ok);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                }
            }
        }
        {
            // ui_Skins
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.ui_skins = obj;
            lv_obj_set_pos(obj, 156, 146);
            lv_obj_set_size(obj, 460, 143);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ui_SkinImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.ui_skin_image = obj;
                    lv_obj_set_pos(obj, 32, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_skin_jcm);
                }
            }
        }
        {
            // ui_PresetHeadingLabel
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.ui_preset_heading_label = obj;
            lv_obj_set_pos(obj, 6, 68);
            lv_obj_set_size(obj, 789, 60);
            lv_obj_add_event_cb(obj, action_enable_skin_edit, LV_EVENT_LONG_PRESSED, (void *)0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_SCROLLED);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffd1a60c), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Preset Name");
        }
        {
            // ui_PresetDetailsTextArea
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.ui_preset_details_text_area = obj;
            lv_obj_set_pos(obj, 14, 300);
            lv_obj_set_size(obj, 773, 99);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_text(obj, "Preset Description...");
            lv_textarea_set_one_line(obj, false);
            lv_textarea_set_password_mode(obj, false);
            lv_obj_add_event_cb(obj, action_preset_description_pressed, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff563f2a), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // ui_BankTitleLabel
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.ui_bank_title_label = obj;
            lv_obj_set_pos(obj, 6, 164);
            lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffd1a60c), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Bank");
        }
        {
            // ui_BankValueLabel
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.ui_bank_value_label = obj;
            lv_obj_set_pos(obj, 0, 202);
            lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "1");
        }
        {
            // ui_BPMTitleLabel
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.ui_bpm_title_label = obj;
            lv_obj_set_pos(obj, 585, 164);
            lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffd1a60c), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "BPM");
        }
        {
            // ui_BPMValueLabel
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.ui_bpm_value_label = obj;
            lv_obj_set_pos(obj, 587, 202);
            lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "0");
        }
        {
            // ui_BPMIndicator
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.ui_bpm_indicator = obj;
            lv_obj_set_pos(obj, 667, 244);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffd1a60c), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xffd1b44d), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // ui_LeftArrow
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.ui_left_arrow = obj;
            lv_obj_set_pos(obj, 88, 202);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_arrow_left);
            lv_obj_add_event_cb(obj, action_amp_skin_previous, LV_EVENT_PRESSED, (void *)0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN|LV_OBJ_FLAG_CLICKABLE);
        }
        {
            // ui_RightArrow
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.ui_right_arrow = obj;
            lv_obj_set_pos(obj, 640, 194);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_arrow_right);
            lv_obj_add_event_cb(obj, action_amp_skin_next, LV_EVENT_PRESSED, (void *)0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN|LV_OBJ_FLAG_CLICKABLE);
        }
        {
            // ui_EntryKeyboard
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.ui_entry_keyboard = obj;
            lv_obj_set_pos(obj, 0, 3);
            lv_obj_set_size(obj, 800, 293);
            lv_obj_add_event_cb(obj, action_keyboard_ok, LV_EVENT_READY, (void *)0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // ui_OKTick
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.ui_ok_tick = obj;
            lv_obj_set_pos(obj, 343, 405);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_tick);
            lv_obj_add_event_cb(obj, action_save_skin_edit, LV_EVENT_PRESSED, (void *)0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN|LV_OBJ_FLAG_CLICKABLE);
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
    lv_obj_set_size(obj, 800, 480);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ui_SettingsTabView
            lv_obj_t *obj = lv_tabview_create(parent_obj, LV_DIR_TOP, 60);
            objects.ui_settings_tab_view = obj;
            lv_obj_set_pos(obj, 5, 5);
            lv_obj_set_size(obj, 790, 428);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_pos(obj, 13, -148);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_NoiseGateSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_noise_gate_switch = obj;
                            lv_obj_set_pos(obj, 168, -148);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 591, -148);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Post");
                        }
                        {
                            // ui_NoiseGatePostSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_noise_gate_post_switch = obj;
                            lv_obj_set_pos(obj, 665, -148);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 13, -92);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Threshold");
                        }
                        {
                            // ui_NoiseGateThresholdSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_noise_gate_threshold_slider = obj;
                            lv_obj_set_pos(obj, 168, -92);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, -92);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_NoiseGateReleaseLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_noise_gate_release_label = obj;
                            lv_obj_set_pos(obj, 13, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Release");
                        }
                        {
                            // ui_NoiseGateReleaseSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_noise_gate_release_slider = obj;
                            lv_obj_set_pos(obj, 168, -36);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_NoiseGateDepthLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_noise_gate_depth_label = obj;
                            lv_obj_set_pos(obj, 13, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Depth");
                        }
                        {
                            // ui_NoiseGateDepthSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_noise_gate_depth_slider = obj;
                            lv_obj_set_pos(obj, 168, 21);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_pos(obj, 13, -148);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_CompressorEnableSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_compressor_enable_switch = obj;
                            lv_obj_set_pos(obj, 168, -148);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 591, -148);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Post");
                        }
                        {
                            // ui_CompressorPostSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_compressor_post_switch = obj;
                            lv_obj_set_pos(obj, 665, -148);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 13, -92);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Threshold");
                        }
                        {
                            // ui_CompressorThresholdSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_compressor_threshold_slider = obj;
                            lv_obj_set_pos(obj, 168, -92);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, -92);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_CompressorAttackLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_compressor_attack_label = obj;
                            lv_obj_set_pos(obj, 13, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Attack");
                        }
                        {
                            // ui_CompressorAttackSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_compressor_attack_slider = obj;
                            lv_obj_set_pos(obj, 168, -36);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_CompressorGainLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_compressor_gain_label = obj;
                            lv_obj_set_pos(obj, 13, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Gain");
                        }
                        {
                            // ui_CompressorGainSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_compressor_gain_slider = obj;
                            lv_obj_set_pos(obj, 168, 21);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_pos(obj, 13, -148);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_AmpEnableSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_amp_enable_switch = obj;
                            lv_obj_set_pos(obj, 168, -148);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 13, -92);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Cab");
                        }
                        {
                            // ui_CabinetModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_cabinet_model_dropdown = obj;
                            lv_obj_set_pos(obj, 168, 69);
                            lv_obj_set_size(obj, 202, LV_SIZE_CONTENT);
                            lv_dropdown_set_options(obj, "Tone Model\nVIR\nDisabled");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj1 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_AmplifierGainLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_amplifier_gain_label = obj;
                            lv_obj_set_pos(obj, 13, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Gain");
                        }
                        {
                            // ui_AmplifierGainSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_amplifier_gain_slider = obj;
                            lv_obj_set_pos(obj, 168, -36);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_AmplifierVolumeLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_amplifier_volume_label = obj;
                            lv_obj_set_pos(obj, 13, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Volume");
                        }
                        {
                            // ui_AmplifierVolumeSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_amplifier_volume_slider = obj;
                            lv_obj_set_pos(obj, 168, 21);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_AmplifierPresenseLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_amplifier_presense_label = obj;
                            lv_obj_set_pos(obj, 13, 77);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Presence");
                        }
                        {
                            // ui_AmplifierPresenseSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_amplifier_presense_slider = obj;
                            lv_obj_set_pos(obj, 168, 77);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 77);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_AmplifierDepthLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_amplifier_depth_label = obj;
                            lv_obj_set_pos(obj, 13, 133);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Depth");
                        }
                        {
                            // ui_AmplifierDepthSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_amplifier_depth_slider = obj;
                            lv_obj_set_pos(obj, 168, 133);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 133);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_pos(obj, 13, -148);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Post");
                        }
                        {
                            // ui_EQPostSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_eq_post_switch = obj;
                            lv_obj_set_pos(obj, 168, -148);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 13, -92);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Bass");
                        }
                        {
                            // ui_EQBassSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_eq_bass_slider = obj;
                            lv_obj_set_pos(obj, 168, -92);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, -92);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_EQMidLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_eq_mid_label = obj;
                            lv_obj_set_pos(obj, 13, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Mid");
                        }
                        {
                            // ui_EQMidSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_eq_mid_slider = obj;
                            lv_obj_set_pos(obj, 168, -36);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_EQMidQLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_eq_mid_qlabel = obj;
                            lv_obj_set_pos(obj, 13, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "MidQ");
                        }
                        {
                            // ui_EQMidQSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_eq_mid_qslider = obj;
                            lv_obj_set_pos(obj, 168, 21);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_EQTrebleLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_eq_treble_label = obj;
                            lv_obj_set_pos(obj, 13, 77);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Treble");
                        }
                        {
                            // ui_EQTrebleSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_eq_treble_slider = obj;
                            lv_obj_set_pos(obj, 168, 77);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 76);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_pos(obj, 13, -148);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_ModulationEnableSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_modulation_enable_switch = obj;
                            lv_obj_set_pos(obj, 168, -148);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 591, -148);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Post");
                        }
                        {
                            // ui_ModulationPostSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_modulation_post_switch = obj;
                            lv_obj_set_pos(obj, 665, -148);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 13, -92);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Model");
                        }
                        {
                            // ui_ModulationModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_modulation_model_dropdown = obj;
                            lv_obj_set_pos(obj, 168, 69);
                            lv_obj_set_size(obj, 202, LV_SIZE_CONTENT);
                            lv_dropdown_set_options(obj, "Chorus\nTremolo\nPhaser\nFlanger\nRotary");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj2 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ModulationSyncLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_sync_label = obj;
                            lv_obj_set_pos(obj, 591, -92);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Sync");
                        }
                        {
                            // ui_ModulationSyncSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_modulation_sync_switch = obj;
                            lv_obj_set_pos(obj, 665, -92);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 13, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param1");
                        }
                        {
                            // ui_ModulationTSDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_modulation_ts_dropdown = obj;
                            lv_obj_set_pos(obj, 168, 124);
                            lv_obj_set_size(obj, 202, LV_SIZE_CONTENT);
                            lv_dropdown_set_options(obj, "1/32\n1/32 D\n1/32 T\n1/16\n1/16 D\n1/16 T\n1/8\n1/8 D\n1/8 T\n1/4\n1/4 D\n1/4 T\n1/2\n1/2 D\n1/2 T\n1/1\n1/1 D\n1/1 T");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj3 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ModulationParam1Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_modulation_param1_slider = obj;
                            lv_obj_set_pos(obj, 168, -36);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ModulationParam2Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_param2_label = obj;
                            lv_obj_set_pos(obj, 13, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param2");
                        }
                        {
                            // ui_ModulationParam2Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_modulation_param2_slider = obj;
                            lv_obj_set_pos(obj, 168, 21);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ModulationParam3Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_param3_label = obj;
                            lv_obj_set_pos(obj, 13, 77);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param3");
                        }
                        {
                            // ui_ModulationParam3Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_modulation_param3_slider = obj;
                            lv_obj_set_pos(obj, 168, 77);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 76);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ModulationParam4Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_modulation_param4_label = obj;
                            lv_obj_set_pos(obj, 13, 133);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Param3");
                        }
                        {
                            // ui_ModulationParam4Slider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_modulation_param4_slider = obj;
                            lv_obj_set_pos(obj, 168, 133);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 133);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_pos(obj, 13, -148);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_DelayEnableSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_delay_enable_switch = obj;
                            lv_obj_set_pos(obj, 168, -148);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 591, -148);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Post");
                        }
                        {
                            // ui_DelayPostSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_delay_post_switch = obj;
                            lv_obj_set_pos(obj, 665, -148);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 13, -92);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Model");
                        }
                        {
                            // ui_DelayModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_delay_model_dropdown = obj;
                            lv_obj_set_pos(obj, 168, 69);
                            lv_obj_set_size(obj, 202, LV_SIZE_CONTENT);
                            lv_dropdown_set_options(obj, "Digital\nTape");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj4 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_DelaySyncLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_delay_sync_label = obj;
                            lv_obj_set_pos(obj, 13, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Sync");
                        }
                        {
                            // ui_DelaySyncSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_delay_sync_switch = obj;
                            lv_obj_set_pos(obj, 168, -36);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 591, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "PPng");
                        }
                        {
                            // ui_DelayPingPongSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_delay_ping_pong_switch = obj;
                            lv_obj_set_pos(obj, 665, -36);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 13, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Time");
                        }
                        {
                            // ui_DelayTSSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_delay_ts_slider = obj;
                            lv_obj_set_pos(obj, 168, 21);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_DelayTSDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_delay_ts_dropdown = obj;
                            lv_obj_set_pos(obj, 168, 180);
                            lv_obj_set_size(obj, 202, LV_SIZE_CONTENT);
                            lv_dropdown_set_options(obj, "1/32\n1/32 D\n1/32 T\n1/16\n1/16 D\n1/16 T\n1/8\n1/8 D\n1/8 T\n1/4\n1/4 D\n1/4 T\n1/2\n1/2 D\n1/2 T\n1/1\n1/1 D\n1/1 T");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj5 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_DelayFeedbackLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_delay_feedback_label = obj;
                            lv_obj_set_pos(obj, 13, 77);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Feedback");
                        }
                        {
                            // ui_DelayFeedbackSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_delay_feedback_slider = obj;
                            lv_obj_set_pos(obj, 168, 77);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 77);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_DelayMixLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_delay_mix_label = obj;
                            lv_obj_set_pos(obj, 13, 133);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Mix");
                        }
                        {
                            // ui_DelayMixSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_delay_mix_slider = obj;
                            lv_obj_set_pos(obj, 168, 133);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 133);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_pos(obj, 13, -148);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Enable");
                        }
                        {
                            // ui_ReverbEnableSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_reverb_enable_switch = obj;
                            lv_obj_set_pos(obj, 168, -148);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 591, -148);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Post");
                        }
                        {
                            // ui_ReverbPostSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_reverb_post_switch = obj;
                            lv_obj_set_pos(obj, 665, -148);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 13, -92);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Model");
                        }
                        {
                            // ui_ReverbModelDropdown
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.ui_reverb_model_dropdown = obj;
                            lv_obj_set_pos(obj, 168, 69);
                            lv_obj_set_size(obj, 202, LV_SIZE_CONTENT);
                            lv_dropdown_set_options(obj, "Spring 1\nSpring 2\nSpring 3\nSpring 4\nRoom\nPlate");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_add_event_cb(obj, action_parameter_changed, LV_EVENT_VALUE_CHANGED, (void *)0);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_dropdown_get_list(parent_obj);
                                    objects.obj6 = obj;
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff513d2b), LV_PART_SELECTED | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_SELECTED | LV_STATE_CHECKED);
                                }
                            }
                        }
                        {
                            // ui_ReverbMixLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_reverb_mix_label = obj;
                            lv_obj_set_pos(obj, 13, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Mix");
                        }
                        {
                            // ui_ReverbMixSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_reverb_mix_slider = obj;
                            lv_obj_set_pos(obj, 168, -36);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ReverbTimeLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_reverb_time_label = obj;
                            lv_obj_set_pos(obj, 13, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Time");
                        }
                        {
                            // ui_ReverbTimeSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_reverb_time_slider = obj;
                            lv_obj_set_pos(obj, 168, 21);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ReverbPredelayLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_reverb_predelay_label = obj;
                            lv_obj_set_pos(obj, 13, 77);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Pre-Delay");
                        }
                        {
                            // ui_ReverbPredelaySlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_reverb_predelay_slider = obj;
                            lv_obj_set_pos(obj, 168, 77);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 77);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_ReverbColorLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_reverb_color_label = obj;
                            lv_obj_set_pos(obj, 13, 133);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Color");
                        }
                        {
                            // ui_ReverbColorSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_reverb_color_slider = obj;
                            lv_obj_set_pos(obj, 168, 133);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 133);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_pos(obj, 13, -148);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Cab Bypass");
                        }
                        {
                            // ui_CabBypassSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_cab_bypass_switch = obj;
                            lv_obj_set_pos(obj, 168, -148);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 574, -148);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Tempo");
                        }
                        {
                            // ui_TempoSourceSwitch
                            lv_obj_t *obj = lv_switch_create(parent_obj);
                            objects.ui_tempo_source_switch = obj;
                            lv_obj_set_pos(obj, 665, -148);
                            lv_obj_set_size(obj, 80, 35);
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
                            lv_obj_set_pos(obj, 13, -92);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "BPM");
                        }
                        {
                            // ui_BPMSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_bpm_slider = obj;
                            lv_obj_set_pos(obj, 168, -92);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, -92);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_InputTrimLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_input_trim_label = obj;
                            lv_obj_set_pos(obj, 13, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Input Trim");
                        }
                        {
                            // ui_InputTrimSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_input_trim_slider = obj;
                            lv_obj_set_pos(obj, 168, -36);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, -36);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_TuningReferenceLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_tuning_reference_label = obj;
                            lv_obj_set_pos(obj, 13, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Tuning Ref");
                        }
                        {
                            // ui_TuningReferenceSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_tuning_reference_slider = obj;
                            lv_obj_set_pos(obj, 168, 21);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 21);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // ui_VolumeLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ui_volume_label = obj;
                            lv_obj_set_pos(obj, 13, 77);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Volume");
                        }
                        {
                            // ui_VolumeSlider
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.ui_volume_slider = obj;
                            lv_obj_set_pos(obj, 168, 77);
                            lv_obj_set_size(obj, 500, 20);
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
                            lv_obj_set_pos(obj, 683, 76);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_set_pos(obj, 354, 410);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_tick);
            lv_obj_add_event_cb(obj, action_close_settings_page, LV_EVENT_PRESSED, (void *)0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
    }
    
    tick_screen_settings();
}

void tick_screen_settings() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_screen1,
    tick_screen_settings,
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
}
