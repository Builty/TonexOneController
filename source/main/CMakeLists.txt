
idf_component_register(SRCS "midi_control.c" "control.c" "footswitches.c" "CH422G.c" "display.c" "main.c" "tonex_params.c" "SX1509.c"
                            "usb_comms.c" "usb_tonex_one.c" "usb_tonex_common.c" "usb_tonex.c" "CH422G.c" "midi_serial.c" "wifi_config.c" "leds.c" "midi_helper.c" "LP5562.c"
                            "platform_devkitc.c" "platform_lgtdisps3.c" "platform_m5atoms3r.c" "platform_ws19t.c" "platform_ws43.c" "platform_wszero.c" "platform_ws169.c" 
                            "platform_ws35b.c" "platform_jc3248w.c"
                            EMBED_TXTFILES index.html 
                            INCLUDE_DIRS "." "./")
                                                       
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
                           "ui_generated_800x480land/ui_image_skin_5150.c" "ui_generated_800x480land/ui_image_skin_ac30.c" "ui_generated_800x480land/ui_image_skin_ampegchrome.c"
                           "ui_generated_800x480land/ui_image_skin_ba500.c" "ui_generated_800x480land/ui_image_skin_diezel.c" "ui_generated_800x480land/ui_image_skin_elegantblue.c" 
                           "ui_generated_800x480land/ui_image_skin_evh.c" "ui_generated_800x480land/ui_image_skin_fenderhotrod.c" "ui_generated_800x480land/ui_image_skin_fendertweedbig.c"
                           "ui_generated_800x480land/ui_image_skin_fendertwin.c" "ui_generated_800x480land/ui_image_skin_friedmann.c" "ui_generated_800x480land/ui_image_skin_jbdumble1.c"
                           "ui_generated_800x480land/ui_image_skin_jcm.c" "ui_generated_800x480land/ui_image_skin_jetcity.c" "ui_generated_800x480land/ui_image_skin_jtm.c"
                           "ui_generated_800x480land/ui_image_skin_mesaboogiedual.c" "ui_generated_800x480land/ui_image_skin_mesamarkv.c" "ui_generated_800x480land/ui_image_skin_mesamarkwood.c"
                           "ui_generated_800x480land/ui_image_skin_modernblackplexi.c" "ui_generated_800x480land/ui_image_skin_modernwhiteplexi.c" "ui_generated_800x480land/ui_image_skin_orangeor120.c"
                           "ui_generated_800x480land/ui_image_skin_rolandjazz.c" "ui_generated_800x480land/ui_image_skin_tonexampblack.c" "ui_generated_800x480land/ui_image_skin_tonexampred.c"
                           "ui_generated_800x480land/ui_image_skin_silverface.c" "ui_generated_800x480land/ui_image_skin_supro.c" "ui_generated_800x480land/ui_image_skin_whitemodern.c"
                           "ui_generated_800x480land/ui_image_skin_woodamp.c"
                            "ui_generated_800x480land/ui_image_arrow_left.c" "ui_generated_800x480land/ui_image_arrow_right.c" "ui_generated_800x480land/ui_image_tick.c"
                            "ui_generated_800x480land/ui_image_bt_conn.c" "ui_generated_800x480land/ui_image_bt_disconn.c"
                           "ui_generated_800x480land/ui_image_pskin_bigmuff.c" "ui_generated_800x480land/ui_image_pskin_bossblack.c" "ui_generated_800x480land/ui_image_pskin_bosssilver.c"
                           "ui_generated_800x480land/ui_image_pskin_bossyellow.c" "ui_generated_800x480land/ui_image_pskin_fuzzred.c" "ui_generated_800x480land/ui_image_pskin_fuzzsilver.c"
                           "ui_generated_800x480land/ui_image_pskin_ibanezblue.c" "ui_generated_800x480land/ui_image_pskin_ibanezdarkblue.c" "ui_generated_800x480land/ui_image_pskin_ibanezgreen.c"
                           "ui_generated_800x480land/ui_image_pskin_ibanezred.c" "ui_generated_800x480land/ui_image_pskin_klongold.c" "ui_generated_800x480land/ui_image_pskin_lifepedal.c"
                           "ui_generated_800x480land/ui_image_pskin_morningglory.c" "ui_generated_800x480land/ui_image_pskin_mxrdoubleblack.c" "ui_generated_800x480land/ui_image_pskin_mxrdoublered.c"
                           "ui_generated_800x480land/ui_image_pskin_mxrsingleblack.c" "ui_generated_800x480land/ui_image_pskin_mxrsinglegold.c" "ui_generated_800x480land/ui_image_pskin_mxrsinglegreen.c"
                           "ui_generated_800x480land/ui_image_pskin_mxrsingleorange.c" "ui_generated_800x480land/ui_image_pskin_mxrsinglewhite.c" "ui_generated_800x480land/ui_image_pskin_mxrsingleyellow.c"
                           "ui_generated_800x480land/ui_image_pskin_ratyellow.c")
endif()

if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_35B OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_JC3248W535)
    target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_480x320land")

    target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_480x320land/ui.c" "ui_generated_480x320land/ui_helpers.c"
                            "ui_generated_480x320land/images/ui_img_smythbuilt_png.c" "ui_generated_480x320land/screens/ui_Screen1.c" "ui_generated_480x320land/screens/ui_Settings.c" "ui_generated_480x320land/components/ui_comp_hook.c"
			                "ui_generated_480x320land/images/ui_img_usb_fail_png.c" "ui_generated_480x320land/images/ui_img_next_png.c" "ui_generated_480x320land/images/ui_img_previous_png.c" 
                            "ui_generated_480x320land/images/ui_img_usb_ok_png.c" "ui_generated_480x320land/images/ui_img_next_down_png.c" "ui_generated_480x320land/images/ui_img_previous_down_png.c" "ui_generated_480x320land/images/ui_img_settings_png.c"
                            "ui_generated_480x320land/images/ui_img_wifi_conn_png.c" "ui_generated_480x320land/images/ui_img_wifi_disconn_png.c"
                           "ui_generated_480x320land/images/ui_img_effect_icon_amp_off_png.c" "ui_generated_480x320land/images/ui_img_effect_icon_amp_on_png.c" 
                           "ui_generated_480x320land/images/ui_img_effect_icon_cab_off_png.c" "ui_generated_480x320land/images/ui_img_effect_icon_cab_on_png.c" 
                           "ui_generated_480x320land/images/ui_img_effect_icon_comp_off_png.c" "ui_generated_480x320land/images/ui_img_effect_icon_comp_on_png.c" 
                           "ui_generated_480x320land/images/ui_img_effect_icon_delay_off_png.c" 
                           "ui_generated_480x320land/images/ui_img_effect_icon_gate_off_png.c" "ui_generated_480x320land/images/ui_img_effect_icon_gate_on_png.c" 
                           "ui_generated_480x320land/images/ui_img_effect_icon_mod_off_png.c" 
                           "ui_generated_480x320land/images/ui_img_effect_icon_mod_on_chorus_png.c" "ui_generated_480x320land/images/ui_img_effect_icon_mod_on_flanger_png.c" 
                           "ui_generated_480x320land/images/ui_img_effect_icon_mod_on_phaser_png.c" "ui_generated_480x320land/images/ui_img_effect_icon_mod_on_rotary_png.c" 
                           "ui_generated_480x320land/images/ui_img_effect_icon_mod_on_tremolo_png.c" 
                           "ui_generated_480x320land/images/ui_img_effect_icon_reverb_off_png.c" 
                           "ui_generated_480x320land/images/ui_img_effect_icon_reverb_on_r_png.c" "ui_generated_480x320land/images/ui_img_effect_icon_reverb_on_s1_png.c" 
                           "ui_generated_480x320land/images/ui_img_effect_icon_reverb_on_s2_png.c" "ui_generated_480x320land/images/ui_img_effect_icon_reverb_on_s3_png.c" 
                           "ui_generated_480x320land/images/ui_img_effect_icon_reverb_on_s4_png.c" "ui_generated_480x320land/images/ui_img_effect_icon_reverb_on_p_png.c" 
                           "ui_generated_480x320land/images/ui_img_effect_icon_delay_on_d_png.c" "ui_generated_480x320land/images/ui_img_effect_icon_delay_on_t_png.c" 
                           "ui_generated_480x320land/images/ui_img_effect_icon_eq_png.c"
                           "ui_generated_480x320land/images/ui_img_skin_5150_png.c" "ui_generated_480x320land/images/ui_img_skin_ac30_png.c" "ui_generated_480x320land/images/ui_img_skin_ampegchrome_png.c"
                           "ui_generated_480x320land/images/ui_img_skin_ba500_png.c" "ui_generated_480x320land/images/ui_img_skin_diezel_png.c" "ui_generated_480x320land/images/ui_img_skin_elegantblue_png.c" 
                           "ui_generated_480x320land/images/ui_img_skin_evh_png.c" "ui_generated_480x320land/images/ui_img_skin_fenderhotrod_png.c" "ui_generated_480x320land/images/ui_img_skin_fendertweedbig_png.c"
                           "ui_generated_480x320land/images/ui_img_skin_fendertwin_png.c" "ui_generated_480x320land/images/ui_img_skin_friedmann_png.c" "ui_generated_480x320land/images/ui_img_skin_jbdumble1_png.c"
                           "ui_generated_480x320land/images/ui_img_skin_jcm_png.c" "ui_generated_480x320land/images/ui_img_skin_jetcity_png.c" "ui_generated_480x320land/images/ui_img_skin_jtm_png.c"
                           "ui_generated_480x320land/images/ui_img_skin_mesaboogiedual_png.c" "ui_generated_480x320land/images/ui_img_skin_mesamarkv_png.c" "ui_generated_480x320land/images/ui_img_skin_mesamarkwood_png.c"
                           "ui_generated_480x320land/images/ui_img_skin_modernblackplexi_png.c" "ui_generated_480x320land/images/ui_img_skin_modernwhiteplexi_png.c" "ui_generated_480x320land/images/ui_img_skin_orangeor120_png.c"
                           "ui_generated_480x320land/images/ui_img_skin_rolandjazz_png.c" "ui_generated_480x320land/images/ui_img_skin_tonexampblack_png.c" "ui_generated_480x320land/images/ui_img_skin_tonexampred_png.c"
                           "ui_generated_480x320land/images/ui_img_skin_silverface_png.c" "ui_generated_480x320land/images/ui_img_skin_supro_png.c" "ui_generated_480x320land/images/ui_img_skin_whitemodern_png.c"
                           "ui_generated_480x320land/images/ui_img_skin_woodamp_png.c"
                            "ui_generated_480x320land/images/ui_img_arrow_left_png.c" "ui_generated_480x320land/images/ui_img_arrow_right_png.c" "ui_generated_480x320land/images/ui_img_tick_png.c"
                            "ui_generated_480x320land/images/ui_img_bt_conn_png.c" "ui_generated_480x320land/images/ui_img_bt_disconn_png.c"
                           "ui_generated_480x320land/images/ui_img_pskin_bigmuff_png.c" "ui_generated_480x320land/images/ui_img_pskin_bossblack_png.c" "ui_generated_480x320land/images/ui_img_pskin_bosssilver_png.c"
                           "ui_generated_480x320land/images/ui_img_pskin_bossyellow_png.c" "ui_generated_480x320land/images/ui_img_pskin_fuzzred_png.c" "ui_generated_480x320land/images/ui_img_pskin_fuzzsilver_png.c"
                           "ui_generated_480x320land/images/ui_img_pskin_ibanezblue_png.c" "ui_generated_480x320land/images/ui_img_pskin_ibanezdarkblue_png.c" "ui_generated_480x320land/images/ui_img_pskin_ibanezgreen_png.c"
                           "ui_generated_480x320land/images/ui_img_pskin_ibanezred_png.c" "ui_generated_480x320land/images/ui_img_pskin_klongold_png.c" "ui_generated_480x320land/images/ui_img_pskin_lifepedal_png.c"
                           "ui_generated_480x320land/images/ui_img_pskin_morningglory_png.c" "ui_generated_480x320land/images/ui_img_pskin_mxrdoubleblack_png.c" "ui_generated_480x320land/images/ui_img_pskin_mxrdoublered_png.c"
                           "ui_generated_480x320land/images/ui_img_pskin_mxrsingleblack_png.c" "ui_generated_480x320land/images/ui_img_pskin_mxrsinglegold_png.c" "ui_generated_480x320land/images/ui_img_pskin_mxrsinglegreen_png.c"
                           "ui_generated_480x320land/images/ui_img_pskin_mxrsingleorange_png.c" "ui_generated_480x320land/images/ui_img_pskin_mxrsinglewhite_png.c" "ui_generated_480x320land/images/ui_img_pskin_mxrsingleyellow_png.c"
                           "ui_generated_480x320land/images/ui_img_pskin_ratyellow_png.c")
endif()

if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169 OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169TOUCH)
   if(CONFIG_TONEX_CONTROLLER_WAVESHARE_169_LANDSCAPE)
      target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_169_land")

      target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_169_land/ui.c" "ui_generated_169_land/ui_helpers.c"
                               "ui_generated_169_land/images/ui_img_smythbuilt_png.c" "ui_generated_169_land/screens/ui_Screen1.c" "ui_generated_169_land/components/ui_comp_hook.c" "ui_generated_169_land/images/ui_img_usb_fail_png.c"
                               "ui_generated_169_land/images/ui_img_usb_ok_png.c"
                               "ui_generated_169_land/images/ui_img_wifi_conn_png.c" "ui_generated_169_land/images/ui_img_wifi_disconn_png.c"
                               "ui_generated_169_land/images/ui_img_bt_conn_png.c" "ui_generated_169_land/images/ui_img_bt_disconn_png.c")

   else()
      target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_169")

      target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_169/ui.c" "ui_generated_169/ui_helpers.c"
                               "ui_generated_169/images/ui_img_smythbuilt_png.c" "ui_generated_169/screens/ui_Screen1.c" "ui_generated_169/components/ui_comp_hook.c" "ui_generated_169/images/ui_img_usb_fail_png.c"
                               "ui_generated_169/images/ui_img_usb_ok_png.c"
                               "ui_generated_169/images/ui_img_wifi_conn_png.c" "ui_generated_169/images/ui_img_wifi_disconn_png.c"
                               "ui_generated_169/images/ui_img_bt_conn_png.c" "ui_generated_169/images/ui_img_bt_disconn_png.c")
   endif()
endif()

if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_LILYGO_TDISPLAY_S3 OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_19TOUCH)
   target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_19")

      target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_19/ui.c" "ui_generated_19/ui_helpers.c"
                               "ui_generated_19/images/ui_img_smythbuilt_png.c" "ui_generated_19/screens/ui_Screen1.c" "ui_generated_19/components/ui_comp_hook.c" "ui_generated_19/images/ui_img_usb_fail_png.c"
                               "ui_generated_19/images/ui_img_usb_ok_png.c"
                               "ui_generated_19/images/ui_img_wifi_conn_png.c" "ui_generated_19/images/ui_img_wifi_disconn_png.c"
                               "ui_generated_19/images/ui_img_bt_conn_png.c" "ui_generated_19/images/ui_img_bt_disconn_png.c")
endif()

if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_M5ATOMS3R)
   target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_085")

   target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_085/ui.c" "ui_generated_085/ui_helpers.c"
                            "ui_generated_085/images/ui_img_smythbuilt_png.c" "ui_generated_085/screens/ui_Screen1.c" "ui_generated_085/components/ui_comp_hook.c" "ui_generated_085/images/ui_img_usb_fail_png.c"
                            "ui_generated_085/images/ui_img_usb_ok_png.c"
                            "ui_generated_085/images/ui_img_wifi_conn_png.c" "ui_generated_085/images/ui_img_wifi_disconn_png.c"
                            "ui_generated_085/images/ui_img_bt_conn_png.c" "ui_generated_085/images/ui_img_bt_disconn_png.c")
endif()

target_compile_options(${COMPONENT_LIB} PRIVATE -Werror)
