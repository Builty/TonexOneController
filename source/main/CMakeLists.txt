idf_component_register(SRCS "platform_pirate_pro_169.c" "platform_pirate_max35.c" "valeton_params.c" "midi_control.c" "control.c" "footswitches.c" "CH422G.c" "display.c" "main.c" "tonex_params.c" "SX1509.c" "display_tonex.c" "display_valeton.c"
                            "usb_comms.c" "usb_tonex_one.c" "usb_tonex_common.c" "usb_tonex.c" "usb_valeton_gp5.c" "CH422G.c" "midi_serial.c" "wifi_config.c" "leds.c" "midi_helper.c" "midi_helper_tonex.c" "midi_helper_valeton.c" "LP5562.c"
                            "platform_devkitc.c" "platform_lgtdisps3.c" "platform_m5atoms3r.c" "platform_ws19t.c" "platform_ws43.c" "platform_wszero.c" "platform_ws169.c" 
                            "platform_ws35b.c" "platform_jc3248w.c"
                            INCLUDE_DIRS "." "./" 
                            EMBED_FILES
                                "web/index.html" "web/amp_disabled.png" "web/amp_off.png" "web/amp_on.png" "web/cab_disabled.png" "web/cab_off.png" "web/cab_on.png"
                                "web/dly_off.png" "web/dly_on.png" "web/dst_off.png" "web/dst_on.png" "web/effect_icon_amp_off.png" "web/effect_icon_amp_on.png" 
                                "web/effect_icon_cab_off.png" "web/effect_icon_cab_on.png" "web/effect_icon_comp_off.png" "web/effect_icon_comp_on.png"
                                "web/effect_icon_delay_off.png" "web/effect_icon_delay_on_d.png" "web/effect_icon_delay_on_t.png" "web/effect_icon_eq.png"
                                "web/effect_icon_gate_on.png" "web/effect_icon_gate_off.png" "web/effect_icon_mod_off.png" "web/effect_icon_mod_on_chorus.png"
                                "web/effect_icon_mod_on_flanger.png" "web/effect_icon_mod_on_phaser.png" "web/effect_icon_mod_on_rotary.png" "web/effect_icon_mod_on_tremolo.png"
                                "web/effect_icon_reverb_off.png" "web/effect_icon_reverb_on.png" "web/effect_icon_reverb_on_p.png" "web/effect_icon_reverb_on_r.png"
                                "web/effect_icon_reverb_on_s1.png" "web/effect_icon_reverb_on_s2.png" "web/effect_icon_reverb_on_s3.png" "web/effect_icon_reverb_on_s4.png"
                                "web/eq_off.png" "web/eq_on.png" "web/mod_off.png" "web/mod_on.png" "web/nr_off.png" "web/nr_on.png" "web/pre_off.png" "web/pre_on.png"
                                "web/rvb_off.png" "web/rvb_on.png" "web/tc_off.png" "web/tc_on.png"
                                "web/favicon.ico" "web/bootstrap.js" "web/jquery.js" "web/index.css" "web/bootstrap.css" "web/script.js"
)                           
                             
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
                           "ui_generated_800x480land/ui_image_eq_off.c" "ui_generated_800x480land/ui_image_eq_on.c"
                           "ui_generated_800x480land/ui_image_mod_off.c" "ui_generated_800x480land/ui_image_mod_on.c"
                           "ui_generated_800x480land/ui_image_pre_off.c" "ui_generated_800x480land/ui_image_pre_on.c"
                           "ui_generated_800x480land/ui_image_nr_off.c" "ui_generated_800x480land/ui_image_nr_on.c"
                           "ui_generated_800x480land/ui_image_rvb_off.c" "ui_generated_800x480land/ui_image_rvb_on.c"
                           "ui_generated_800x480land/ui_image_tc_off.c" "ui_generated_800x480land/ui_image_tc_on.c"
                           "ui_generated_800x480land/ui_image_amp_off.c" "ui_generated_800x480land/ui_image_amp_on.c" "ui_generated_800x480land/ui_image_amp_disabled.c"
                           "ui_generated_800x480land/ui_image_dly_off.c" "ui_generated_800x480land/ui_image_dly_on.c"
                           "ui_generated_800x480land/ui_image_dst_off.c" "ui_generated_800x480land/ui_image_dst_on.c"
                           "ui_generated_800x480land/ui_image_cab_off.c" "ui_generated_800x480land/ui_image_cab_on.c" "ui_generated_800x480land/ui_image_cab_disabled.c"
                           "ui_generated_800x480land/ui_image_skin_jcm.c")
endif()

if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_35B OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_JC3248W535 OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_PIRATE_MIDI_POLAR_MAX_V2)
    target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_480x320land")

    target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_480x320land/ui.c" "ui_generated_480x320land/styles.c"
                           "ui_generated_480x320land/screens.c" "ui_generated_480x320land/images.c"
                           "ui_generated_480x320land/ui_image_smythbuilt.c" 
			               "ui_generated_480x320land/ui_image_usb_fail.c" "ui_generated_480x320land/ui_image_next.c" "ui_generated_480x320land/ui_image_previous.c" 
                           "ui_generated_480x320land/ui_image_usb_ok.c" "ui_generated_480x320land/ui_image_next_down.c" "ui_generated_480x320land/ui_image_previous_down.c" "ui_generated_480x320land/ui_image_settings.c"
                           "ui_generated_480x320land/ui_image_wifi_conn.c" "ui_generated_480x320land/ui_image_wifi_disconn.c"
                           "ui_generated_480x320land/ui_image_effect_icon_amp_off.c" "ui_generated_480x320land/ui_image_effect_icon_amp_on.c"  "ui_generated_48x320land/ui_image_amp_disabled.c"
                           "ui_generated_480x320land/ui_image_effect_icon_cab_off.c" "ui_generated_480x320land/ui_image_effect_icon_cab_on.c"  "ui_generated_48x320land/ui_image_cab_disabled.c"
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
                           "ui_generated_480x320land/ui_image_eq_off.c" "ui_generated_480x320land/ui_image_eq_on.c"
                           "ui_generated_480x320land/ui_image_mod_off.c" "ui_generated_480x320land/ui_image_mod_on.c"
                           "ui_generated_480x320land/ui_image_pre_off.c" "ui_generated_480x320land/ui_image_pre_on.c"
                           "ui_generated_480x320land/ui_image_nr_off.c" "ui_generated_480x320land/ui_image_nr_on.c"
                           "ui_generated_480x320land/ui_image_rvb_off.c" "ui_generated_480x320land/ui_image_rvb_on.c"
                           "ui_generated_480x320land/ui_image_tc_off.c" "ui_generated_480x320land/ui_image_tc_on.c"
                           "ui_generated_480x320land/ui_image_amp_off.c" "ui_generated_480x320land/ui_image_amp_on.c"
                           "ui_generated_480x320land/ui_image_dly_off.c" "ui_generated_480x320land/ui_image_dly_on.c"
                           "ui_generated_480x320land/ui_image_dst_off.c" "ui_generated_480x320land/ui_image_dst_on.c"
                           "ui_generated_480x320land/ui_image_cab_off.c" "ui_generated_480x320land/ui_image_cab_on.c"
                           "ui_generated_480x320land/ui_image_skin_jcm.c")
endif()

if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169 OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169TOUCH OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_PIRATE_MIDI_POLAR_PRO)
   if(CONFIG_TONEX_CONTROLLER_WAVESHARE_169_LANDSCAPE)
      target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_280x240land")

      target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_280x240land/ui.c" "ui_generated_280x240land/styles.c"
                               "ui_generated_280x240land/screens.c" "ui_generated_240x280port/images.c"
                               "ui_generated_280x240land/ui_image_smythbuilt.c" "ui_generated_280x240land/ui_image_usb_fail.c"
                               "ui_generated_280x240land/ui_image_usb_ok.c"
                               "ui_generated_280x240land/ui_image_wifi_conn.c" "ui_generated_280x240land/ui_image_wifi_disconn.c"
                               "ui_generated_280x240land/ui_image_bt_conn.c" "ui_generated_280x240land/ui_image_bt_disconn.c")

   else()
      target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_240x280port")

      target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_240x280port/ui.c" "ui_generated_240x280port/styles.c"
                               "ui_generated_240x280port/screens.c" "ui_generated_240x280port/images.c"
                               "ui_generated_240x280port/ui_image_smythbuilt.c" "ui_generated_240x280port/ui_image_usb_fail.c"
                               "ui_generated_240x280port/ui_image_usb_ok.c"
                               "ui_generated_240x280port/ui_image_wifi_conn.c" "ui_generated_240x280port/ui_image_wifi_disconn.c"
                               "ui_generated_240x280port/ui_image_bt_conn.c" "ui_generated_240x280port/ui_image_bt_disconn.c")
   endif()
endif()

if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_LILYGO_TDISPLAY_S3 OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_19TOUCH)
   target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_320x170land")

      target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_320x170land/ui.c" "ui_generated_320x170land/styles.c"
                               "ui_generated_320x170land/screens.c" "ui_generated_320x170land/images.c"
                               "ui_generated_320x170land/ui_image_smythbuilt.c" "ui_generated_320x170land/ui_image_usb_fail.c"
                               "ui_generated_320x170land/ui_image_usb_ok.c"
                               "ui_generated_320x170land/ui_image_wifi_conn.c" "ui_generated_320x170land/ui_image_wifi_disconn.c"
                               "ui_generated_320x170land/ui_image_bt_conn.c" "ui_generated_320x170land/ui_image_bt_disconn.c")
endif()

if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_M5ATOMS3R)
   target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_128x128")

   target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_128x128/ui.c" "ui_generated_128x128/styles.c"
                                "ui_generated_128x128/screens.c" "ui_generated_128x128/images.c"
                                "ui_generated_128x128/ui_image_smythbuilt.c" "ui_generated_128x128/ui_image_usb_fail.c"
                                "ui_generated_128x128/ui_image_usb_ok.c"
                                "ui_generated_128x128/ui_image_wifi_conn.c" "ui_generated_128x128/ui_image_wifi_disconn.c"
                                "ui_generated_128x128/ui_image_bt_conn.c" "ui_generated_128x128/ui_image_bt_disconn.c")
endif()

target_compile_options(${COMPONENT_LIB} PRIVATE -Werror)
