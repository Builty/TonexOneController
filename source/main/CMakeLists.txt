
idf_component_register(SRCS "midi_control.c" "control.c" "footswitches.c" "CH422G.c" "display.c" "main.c" "tonex_params.c" "SX1509.c"
                            "usb_comms.c" "usb_tonex_one.c" "usb_tonex_common.c" "usb_tonex.c" "CH422G.c" "midi_serial.c" "wifi_config.c" "leds.c" "midi_helper.c" "LP5562.c"
                            "platform_devkitc.c" "platform_lgtdisps3.c" "platform_m5atoms3r.c" "platform_ws19t.c" "platform_ws43.c" "platform_wszero.c" "platform_ws169.c" 
                            "platform_ws35b.c" "platform_jc3248w.c"
                            INCLUDE_DIRS "." "./"
                            EMBED_TXTFILES index.html)
                                                       
if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43B OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43DEVONLY)
    target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_800x480land")

    target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_800x480land/ui.c" "ui_generated_800x480land/styles.c"
                           "ui_generated_800x480land/screens.c" "ui_generated_800x480land/images.c"
                           "ui_generated_800x480land/ui_image_smythbuilt.c"
			                  "ui_generated_800x480land/ui_image_usb_fail.c" "ui_generated_800x480land/ui_image_next.c" "ui_generated_800x480land/ui_image_previous.c" 
                           "ui_generated_800x480land/ui_image_usb_ok.c" "ui_generated_800x480land/ui_image_next_down.c" "ui_generated_800x480land/ui_image_previous_down.c" "ui_generated_800x480land/ui_image_settings.c"
                           "ui_generated_800x480land/ui_image_wifi_conn.c" "ui_generated_800x480land/ui_image_wifi_disconn.c"
                           "ui_generated_800x480land/ui_image_effect_icon_amp_off.c" "ui_generated_800x480land/ui_image_effect_icon_amp_on.c" 
                           "ui_generated_800x480land/ui_image_effect_icon_cab_off.c" "ui_generated_800x480land/ui_image_effect_icon_cab_on.c" 
                           "ui_generated_800x480land/ui_image_effect_icon_comp_off.c" "ui_generated_800x480land/ui_image_effect_icon_comp_on.c" 
                           "ui_generated_800x480land/ui_image_effect_icon_delay_off.c" "ui_generated_800x480land/ui_image_effect_icon_delay_on.c" 
                           "ui_generated_800x480land/ui_image_effect_icon_gate_off.c" "ui_generated_800x480land/ui_image_effect_icon_gate_on.c" 
                           "ui_generated_800x480land/ui_image_effect_icon_mod_off.c" "ui_generated_800x480land/ui_image_effect_icon_mod_on.c" 
                           "ui_generated_800x480land/ui_image_effect_icon_mod_on_chorus.c" "ui_generated_800x480land/ui_image_effect_icon_mod_on_flanger.c" 
                           "ui_generated_800x480land/ui_image_effect_icon_mod_on_phaser.c" "ui_generated_800x480land/ui_image_effect_icon_mod_on_rotary.c" 
                           "ui_generated_800x480land/ui_image_effect_icon_mod_on_tremolo.c" 
                           "ui_generated_800x480land/ui_image_effect_icon_reverb_off.c" "ui_generated_800x480land/ui_image_effect_icon_reverb_on.c" 
                           "ui_generated_800x480land/ui_image_effect_icon_reverb_on_r.c" "ui_generated_800x480land/ui_image_effect_icon_reverb_on_s1.c" 
                           "ui_generated_800x480land/ui_image_effect_icon_reverb_on_s2.c" "ui_generated_800x480land/ui_image_effect_icon_reverb_on_s3.c" 
                           "ui_generated_800x480land/ui_image_effect_icon_reverb_on_s4.c" "ui_generated_800x480land/ui_image_effect_icon_reverb_on_p.c" 
                           "ui_generated_800x480land/ui_image_effect_icon_delay_on_d.c" "ui_generated_800x480land/ui_image_effect_icon_delay_on_t.c" 
                           "ui_generated_800x480land/ui_image_effect_icon_eq.c"
                           "ui_generated_800x480land/ui_image_arrow_left.c" "ui_generated_800x480land/ui_image_arrow_right.c" "ui_generated_800x480land/ui_image_tick.c"
                           "ui_generated_800x480land/ui_image_bt_conn.c" "ui_generated_800x480land/ui_image_bt_disconn.c"
                           "ui_generated_800x480land/ui_image_skin_jcm.c")
endif()

if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_35B OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_JC3248W535)
    target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_480x320land")

    target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_480x320land/ui.c" "ui_generated_480x320land/styles.c"
                           "ui_generated_480x320land/screens.c" "ui_generated_480x320land/images.c"
                           "ui_generated_480x320land/ui_image_smythbuilt.c" 
			                  "ui_generated_480x320land/ui_image_usb_fail.c" "ui_generated_480x320land/ui_image_next.c" "ui_generated_480x320land/ui_image_previous.c" 
                           "ui_generated_480x320land/ui_image_usb_ok.c" "ui_generated_480x320land/ui_image_next_down.c" "ui_generated_480x320land/ui_image_previous_down.c" "ui_generated_480x320land/ui_image_settings.c"
                           "ui_generated_480x320land/ui_image_wifi_conn.c" "ui_generated_480x320land/ui_image_wifi_disconn.c"
                           "ui_generated_480x320land/ui_image_effect_icon_amp_off.c" "ui_generated_480x320land/ui_image_effect_icon_amp_on.c" 
                           "ui_generated_480x320land/ui_image_effect_icon_cab_off.c" "ui_generated_480x320land/ui_image_effect_icon_cab_on.c" 
                           "ui_generated_480x320land/ui_image_effect_icon_comp_off.c" "ui_generated_480x320land/ui_image_effect_icon_comp_on.c" 
                           "ui_generated_480x320land/ui_image_effect_icon_delay_off.c" 
                           "ui_generated_480x320land/ui_image_effect_icon_gate_off.c" "ui_generated_480x320land/ui_image_effect_icon_gate_on.c" 
                           "ui_generated_480x320land/ui_image_effect_icon_mod_off.c" 
                           "ui_generated_480x320land/ui_image_effect_icon_mod_on_chorus.c" "ui_generated_480x320land/ui_image_effect_icon_mod_on_flanger.c" 
                           "ui_generated_480x320land/ui_image_effect_icon_mod_on_phaser.c" "ui_generated_480x320land/ui_image_effect_icon_mod_on_rotary.c" 
                           "ui_generated_480x320land/ui_image_effect_icon_mod_on_tremolo.c" 
                           "ui_generated_480x320land/ui_image_effect_icon_reverb_off.c" 
                           "ui_generated_480x320land/ui_image_effect_icon_reverb_on_r.c" "ui_generated_480x320land/ui_image_effect_icon_reverb_on_s1.c" 
                           "ui_generated_480x320land/ui_image_effect_icon_reverb_on_s2.c" "ui_generated_480x320land/ui_image_effect_icon_reverb_on_s3.c" 
                           "ui_generated_480x320land/ui_image_effect_icon_reverb_on_s4.c" "ui_generated_480x320land/ui_image_effect_icon_reverb_on_p.c" 
                           "ui_generated_480x320land/ui_image_effect_icon_delay_on_d.c" "ui_generated_480x320land/ui_image_effect_icon_delay_on_t.c" 
                           "ui_generated_480x320land/ui_image_effect_icon_eq.c"                           
                           "ui_generated_480x320land/ui_image_arrow_left.c" "ui_generated_480x320land/ui_image_arrow_right.c" "ui_generated_480x320land/ui_image_tick.c"
                           "ui_generated_480x320land/ui_image_bt_conn.c" "ui_generated_480x320land/ui_image_bt_disconn.c"
                           "ui_generated_480x320land/ui_image_skin_jcm.c")
endif()

if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169 OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169TOUCH)
   if(CONFIG_TONEX_CONTROLLER_WAVESHARE_169_LANDSCAPE)
      target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_169_land")

      target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_169_land/ui.c" "ui_generated_169_land/ui_helpers.c"
                               "ui_generated_169_land/images/ui_image_smythbuilt_png.c" "ui_generated_169_land/screens/ui_Screen1.c" "ui_generated_169_land/components/ui_comp_hook.c" "ui_generated_169_land/images/ui_image_usb_fail_png.c"
                               "ui_generated_169_land/images/ui_image_usb_ok_png.c"
                               "ui_generated_169_land/images/ui_image_wifi_conn_png.c" "ui_generated_169_land/images/ui_image_wifi_disconn_png.c"
                               "ui_generated_169_land/images/ui_image_bt_conn_png.c" "ui_generated_169_land/images/ui_image_bt_disconn_png.c")

   else()
      target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_169")

      target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_169/ui.c" "ui_generated_169/ui_helpers.c"
                               "ui_generated_169/images/ui_image_smythbuilt_png.c" "ui_generated_169/screens/ui_Screen1.c" "ui_generated_169/components/ui_comp_hook.c" "ui_generated_169/images/ui_image_usb_fail_png.c"
                               "ui_generated_169/images/ui_image_usb_ok_png.c"
                               "ui_generated_169/images/ui_image_wifi_conn_png.c" "ui_generated_169/images/ui_image_wifi_disconn_png.c"
                               "ui_generated_169/images/ui_image_bt_conn_png.c" "ui_generated_169/images/ui_image_bt_disconn_png.c")
   endif()
endif()

if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_LILYGO_TDISPLAY_S3 OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_19TOUCH)
   target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_19")

      target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_19/ui.c" "ui_generated_19/ui_helpers.c"
                               "ui_generated_19/images/ui_image_smythbuilt_png.c" "ui_generated_19/screens/ui_Screen1.c" "ui_generated_19/components/ui_comp_hook.c" "ui_generated_19/images/ui_image_usb_fail_png.c"
                               "ui_generated_19/images/ui_image_usb_ok_png.c"
                               "ui_generated_19/images/ui_image_wifi_conn_png.c" "ui_generated_19/images/ui_image_wifi_disconn_png.c"
                               "ui_generated_19/images/ui_image_bt_conn_png.c" "ui_generated_19/images/ui_image_bt_disconn_png.c")
endif()

target_compile_options(${COMPONENT_LIB} PRIVATE -Werror)
