#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *screen1;
    lv_obj_t *settings;
    lv_obj_t *settings_image;
    lv_obj_t *icon_reverb;
    lv_obj_t *icon_delay;
    lv_obj_t *icon_mod;
    lv_obj_t *icon_comp;
    lv_obj_t *icon_cab;
    lv_obj_t *icon_amp;
    lv_obj_t *icon_gate;
    lv_obj_t *icon_eq;
    lv_obj_t *next_button;
    lv_obj_t *previous_button;
    lv_obj_t *preset_heading_label;
    lv_obj_t *preset_details_text_area;
    lv_obj_t *entry_keyboard;
    lv_obj_t *ok_tick;
    lv_obj_t *bottom_panel;
    lv_obj_t *next_container;
    lv_obj_t *previous_container;
    lv_obj_t *top_panel;
    lv_obj_t *usb_status_fail;
    lv_obj_t *project_heading_label;
    lv_obj_t *wi_fi_status_disconn;
    lv_obj_t *bt_status_disconn;
    lv_obj_t *ik_logo;
    lv_obj_t *wi_fi_status_conn;
    lv_obj_t *bt_status_conn;
    lv_obj_t *usb_status_ok;
    lv_obj_t *skins;
    lv_obj_t *skin_image;
    lv_obj_t *bank_title_label;
    lv_obj_t *bank_value_label;
    lv_obj_t *bpm_title_label;
    lv_obj_t *bpm_value_label;
    lv_obj_t *bpm_indicator;
    lv_obj_t *left_arrow;
    lv_obj_t *right_arrow;
    lv_obj_t *settings_tab_view;
    lv_obj_t *gate_tab;
    lv_obj_t *compresssor_tab;
    lv_obj_t *eq_tab;
    lv_obj_t *reverb_tab;
    lv_obj_t *modulation_tab;
    lv_obj_t *delay_tab;
    lv_obj_t *amp_tab;
    lv_obj_t *global_tab;
    lv_obj_t *exit_tab;
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