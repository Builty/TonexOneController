#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *screen1;
    lv_obj_t *ui_touch_gesture_container;
    lv_obj_t *ui_top_panel;
    lv_obj_t *ui_usb_status_fail;
    lv_obj_t *ui_wi_fi_status_disconn;
    lv_obj_t *ui_bt_status_disconn;
    lv_obj_t *ui_ik_logo;
    lv_obj_t *ui_wi_fi_status_conn;
    lv_obj_t *ui_bt_status_conn;
    lv_obj_t *ui_usb_status_ok;
    lv_obj_t *ui_cstatus;
    lv_obj_t *ui_mstatus;
    lv_obj_t *ui_dstatus;
    lv_obj_t *ui_rstatus;
    lv_obj_t *ui_preset_heading_label;
    lv_obj_t *ui_bpm_label;
    lv_obj_t *ui_bpm;
    lv_obj_t *ui_bpm_indicator;
    lv_obj_t *bottom_container;
    lv_obj_t *ui_preset_heading_label2;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_SCREEN1 = 1,
};

void create_screen_screen1();
void tick_screen_screen1();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/