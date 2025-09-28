#ifndef EEZ_LVGL_UI_GUI_H
#define EEZ_LVGL_UI_GUI_H

#include <lvgl.h>

#if !defined(EEZ_FOR_LVGL)
#warning "EEZ_FOR_LVGL is not enabled"
#define EEZ_FOR_LVGL
#endif


#if defined(EEZ_FOR_LVGL)
#include <eez/flow/lvgl_api.h>
#endif

#if !defined(EEZ_FOR_LVGL)
#include "screens.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern const uint8_t assets[12940];

void ui_init();
void ui_tick();

#if !defined(EEZ_FOR_LVGL)
void loadScreen(enum ScreensEnum screenId);
#endif

#ifdef __cplusplus
}
#endif

#endif // EEZ_LVGL_UI_GUI_H