#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *screen1;
    lv_obj_t *settings;
    lv_obj_t *ui_settings_image;
    lv_obj_t *ui_icon_reverb;
    lv_obj_t *ui_icon_delay;
    lv_obj_t *ui_icon_mod;
    lv_obj_t *ui_icon_comp;
    lv_obj_t *ui_icon_cab;
    lv_obj_t *ui_icon_amp;
    lv_obj_t *ui_icon_gate;
    lv_obj_t *ui_icon_eq;
    lv_obj_t *ui_next_button;
    lv_obj_t *ui_previous_button;
    lv_obj_t *ui_preset_heading_label;
    lv_obj_t *ui_preset_details_text_area;
    lv_obj_t *ui_left_arrow;
    lv_obj_t *ui_right_arrow;
    lv_obj_t *ui_entry_keyboard;
    lv_obj_t *ui_ok_tick;
    lv_obj_t *ui_gate_threshold_slider;
    lv_obj_t *ui_closeimage;
    lv_obj_t *ui_bottom_panel;
    lv_obj_t *ui_next_container;
    lv_obj_t *ui_previous_container;
    lv_obj_t *ui_top_panel;
    lv_obj_t *ui_usb_status_fail;
    lv_obj_t *ui_project_heading_label;
    lv_obj_t *ui_wi_fi_status_disconn;
    lv_obj_t *ui_bt_status_disconn;
    lv_obj_t *ui_ik_logo;
    lv_obj_t *ui_wi_fi_status_conn;
    lv_obj_t *ui_bt_status_conn;
    lv_obj_t *ui_usb_status_ok;
    lv_obj_t *ui_skins;
    lv_obj_t *ui_skin_image;
    lv_obj_t *ui_bank_title_label;
    lv_obj_t *ui_bank_value_label;
    lv_obj_t *ui_bpm_title_label;
    lv_obj_t *ui_bpm_value_label;
    lv_obj_t *ui_bpm_indicator;
    lv_obj_t *settings_tab_view;
    lv_obj_t *ui_gate_tab;
    lv_obj_t *ui_gate_row1;
    lv_obj_t *ui_gate_switch1;
    lv_obj_t *ui_gate_switch_enable_label;
    lv_obj_t *ui_gate_switch_enable;
    lv_obj_t *ui_gate_switch2;
    lv_obj_t *ui_gate_switch_post_label;
    lv_obj_t *ui_gate_switch_post;
    lv_obj_t *ui_gate_row2;
    lv_obj_t *ui_gate_threshold_label;
    lv_obj_t *ui_gate_threshold_value;
    lv_obj_t *ui_gate_row3;
    lv_obj_t *ui_gate_release_label;
    lv_obj_t *ui_gate_release_slider;
    lv_obj_t *ui_gate_release_value;
    lv_obj_t *ui_compresssor_tab;
    lv_obj_t *ui_eq_tab;
    lv_obj_t *ui_reverb_tab;
    lv_obj_t *ui_modulation_tab;
    lv_obj_t *ui_delay_tab;
    lv_obj_t *ui_amp_tab;
    lv_obj_t *ui_global_tab;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_SCREEN1 = 1,
    SCREEN_ID_SETTINGS = 2,
};

void create_screen_screen1();
void tick_screen_screen1();

void create_screen_settings();
void tick_screen_settings();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/