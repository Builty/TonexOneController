#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_next_clicked(lv_event_t * e);
extern void action_previous_clicked(lv_event_t * e);
extern void action_show_settings_tab(lv_event_t * e);
extern void action_effect_toggle(lv_event_t * e);
extern void action_amp_skin_next(lv_event_t * e);
extern void action_amp_skin_previous(lv_event_t * e);
extern void action_parameter_changed(lv_event_t * e);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/