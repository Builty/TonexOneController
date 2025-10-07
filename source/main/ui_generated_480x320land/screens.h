#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *screen1;
    lv_obj_t *settings;
    lv_obj_t *ui_bottom_panel;
    lv_obj_t *ui_settings_image;
    lv_obj_t *ui_icon_reverb;
    lv_obj_t *ui_icon_delay;
    lv_obj_t *ui_icon_mod;
    lv_obj_t *ui_icon_comp;
    lv_obj_t *ui_icon_cab;
    lv_obj_t *ui_icon_amp;
    lv_obj_t *ui_icon_gate;
    lv_obj_t *ui_icon_eq;
    lv_obj_t *ui_next_container;
    lv_obj_t *ui_next_button;
    lv_obj_t *ui_previous_container;
    lv_obj_t *ui_previous_button;
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
    lv_obj_t *ui_preset_heading_label;
    lv_obj_t *ui_preset_details_text_area;
    lv_obj_t *ui_bank_title_label;
    lv_obj_t *ui_bank_value_label;
    lv_obj_t *ui_bpm_title_label;
    lv_obj_t *ui_bpm_value_label;
    lv_obj_t *ui_bpm_indicator;
    lv_obj_t *ui_left_arrow;
    lv_obj_t *ui_right_arrow;
    lv_obj_t *ui_entry_keyboard;
    lv_obj_t *ui_ok_tick;
    lv_obj_t *ui_settings_tab_view;
    lv_obj_t *ui_gate_tab;
    lv_obj_t *ui_noise_gate_enable_label;
    lv_obj_t *ui_noise_gate_switch;
    lv_obj_t *ui_noise_gate_post_label;
    lv_obj_t *ui_noise_gate_post_switch;
    lv_obj_t *ui_noise_gate_threshold_label;
    lv_obj_t *ui_noise_gate_threshold_slider;
    lv_obj_t *ui_noise_gate_threshold_value;
    lv_obj_t *ui_noise_gate_release_label;
    lv_obj_t *ui_noise_gate_release_slider;
    lv_obj_t *ui_noise_gate_release_value;
    lv_obj_t *ui_noise_gate_depth_label;
    lv_obj_t *ui_noise_gate_depth_slider;
    lv_obj_t *ui_noise_gate_depth_value;
    lv_obj_t *ui_compresssor_tab;
    lv_obj_t *ui_compressor_enable_label;
    lv_obj_t *ui_compressor_enable_switch;
    lv_obj_t *ui_compressor_post_label;
    lv_obj_t *ui_compressor_post_switch;
    lv_obj_t *ui_compressor_threshold_label;
    lv_obj_t *ui_compressor_threshold_slider;
    lv_obj_t *ui_compressor_threshold_value;
    lv_obj_t *ui_compressor_attack_label;
    lv_obj_t *ui_compressor_attack_slider;
    lv_obj_t *ui_compressor_attack_value;
    lv_obj_t *ui_compressor_gain_label;
    lv_obj_t *ui_compressor_gain_slider;
    lv_obj_t *ui_compressor_gain_value;
    lv_obj_t *ui_amp_tab;
    lv_obj_t *ui_amp_enable_label;
    lv_obj_t *ui_amp_enable_switch;
    lv_obj_t *ui_amp_cab_label;
    lv_obj_t *ui_cabinet_model_dropdown;
    lv_obj_t *obj0;
    lv_obj_t *ui_amplifier_gain_label;
    lv_obj_t *ui_amplifier_gain_slider;
    lv_obj_t *ui_amplifier_gain_value;
    lv_obj_t *ui_amplifier_volume_label;
    lv_obj_t *ui_amplifier_volume_slider;
    lv_obj_t *ui_amplifier_volume_value;
    lv_obj_t *ui_amplifier_presense_label;
    lv_obj_t *ui_amplifier_presense_slider;
    lv_obj_t *ui_amplifier_presense_value;
    lv_obj_t *ui_amplifier_depth_label;
    lv_obj_t *ui_amplifier_depth_slider;
    lv_obj_t *ui_amplifier_depth_value;
    lv_obj_t *ui_eq_tab;
    lv_obj_t *ui_eq_post_label;
    lv_obj_t *ui_eq_post_switch;
    lv_obj_t *ui_eq_bass_label;
    lv_obj_t *ui_eq_bass_slider;
    lv_obj_t *ui_eq_bass_value;
    lv_obj_t *ui_eq_mid_label;
    lv_obj_t *ui_eq_mid_slider;
    lv_obj_t *ui_eq_mid_value;
    lv_obj_t *ui_eq_mid_qlabel;
    lv_obj_t *ui_eq_mid_qslider;
    lv_obj_t *ui_eq_mid_qvalue;
    lv_obj_t *ui_eq_treble_label;
    lv_obj_t *ui_eq_treble_slider;
    lv_obj_t *ui_eq_treble_value;
    lv_obj_t *ui_modulation_tab;
    lv_obj_t *ui_modulation_enable_label;
    lv_obj_t *ui_modulation_enable_switch;
    lv_obj_t *ui_modulation_post_label;
    lv_obj_t *ui_modulation_post_switch;
    lv_obj_t *ui_modulation_mode_label;
    lv_obj_t *ui_modulation_model_dropdown;
    lv_obj_t *obj1;
    lv_obj_t *ui_modulation_sync_label;
    lv_obj_t *ui_modulation_sync_switch;
    lv_obj_t *ui_modulation_param1_label;
    lv_obj_t *ui_modulation_ts_dropdown;
    lv_obj_t *obj2;
    lv_obj_t *ui_modulation_param1_slider;
    lv_obj_t *ui_modulation_param1_value;
    lv_obj_t *ui_modulation_param2_label;
    lv_obj_t *ui_modulation_param2_slider;
    lv_obj_t *ui_modulation_param2_value;
    lv_obj_t *ui_modulation_param3_label;
    lv_obj_t *ui_modulation_param3_slider;
    lv_obj_t *ui_modulation_param3_value;
    lv_obj_t *ui_modulation_param4_label;
    lv_obj_t *ui_modulation_param4_slider;
    lv_obj_t *ui_modulation_param4_value;
    lv_obj_t *ui_delay_tab;
    lv_obj_t *ui_delay_enable_label;
    lv_obj_t *ui_delay_enable_switch;
    lv_obj_t *ui_delay_post_label;
    lv_obj_t *ui_delay_post_switch;
    lv_obj_t *ui_delay_mode_label;
    lv_obj_t *ui_delay_model_dropdown;
    lv_obj_t *obj3;
    lv_obj_t *ui_delay_sync_label;
    lv_obj_t *ui_delay_sync_switch;
    lv_obj_t *ui_delay_ping_pong_label;
    lv_obj_t *ui_delay_ping_pong_switch;
    lv_obj_t *ui_delay_ts_label;
    lv_obj_t *ui_delay_ts_slider;
    lv_obj_t *ui_delay_ts_value;
    lv_obj_t *ui_delay_ts_dropdown;
    lv_obj_t *obj4;
    lv_obj_t *ui_delay_feedback_label;
    lv_obj_t *ui_delay_feedback_slider;
    lv_obj_t *ui_delay_feedback_value;
    lv_obj_t *ui_delay_mix_label;
    lv_obj_t *ui_delay_mix_slider;
    lv_obj_t *ui_delay_mix_value;
    lv_obj_t *ui_reverb_tab;
    lv_obj_t *ui_reverb_enable_label;
    lv_obj_t *ui_reverb_enable_switch;
    lv_obj_t *ui_reverb_post_label;
    lv_obj_t *ui_reverb_post_switch;
    lv_obj_t *ui_reverb_mode_label;
    lv_obj_t *ui_reverb_model_dropdown;
    lv_obj_t *obj5;
    lv_obj_t *ui_reverb_mix_label;
    lv_obj_t *ui_reverb_mix_slider;
    lv_obj_t *ui_reverb_mix_value;
    lv_obj_t *ui_reverb_time_label;
    lv_obj_t *ui_reverb_time_slider;
    lv_obj_t *ui_reverb_time_value;
    lv_obj_t *ui_reverb_predelay_label;
    lv_obj_t *ui_reverb_predelay_slider;
    lv_obj_t *ui_reverb_predelay_value;
    lv_obj_t *ui_reverb_color_label;
    lv_obj_t *ui_reverb_color_slider;
    lv_obj_t *ui_reverb_color_value;
    lv_obj_t *ui_global_tab;
    lv_obj_t *ui_cab_bypass_label;
    lv_obj_t *ui_cab_bypass_switch;
    lv_obj_t *ui_tempo_sourcet_label;
    lv_obj_t *ui_tempo_source_switch;
    lv_obj_t *ui_bpm_label;
    lv_obj_t *ui_bpm_slider;
    lv_obj_t *ui_bpm_value;
    lv_obj_t *ui_input_trim_label;
    lv_obj_t *ui_input_trim_slider;
    lv_obj_t *ui_input_trim_value;
    lv_obj_t *ui_tuning_reference_label;
    lv_obj_t *ui_tuning_reference_slider;
    lv_obj_t *ui_tuning_reference_value;
    lv_obj_t *ui_volume_label;
    lv_obj_t *ui_volume_slider;
    lv_obj_t *ui_volume_value;
    lv_obj_t *ui_closeimage;
    lv_obj_t *obj6;
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