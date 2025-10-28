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

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[9];


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/