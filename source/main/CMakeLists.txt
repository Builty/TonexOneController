
idf_component_register(SRCS "midi_control.c" "control.c" "footswitches.c" "CH422G.c" "display.c" "main.c" "tonex_params.c" "SX1509.c"
                            "usb_comms.c" "usb_tonex_one.c" "CH422G.c" "midi_serial.c" "wifi_config.c" "leds.c" "midi_helper.c" "LP5562.c"
                            EMBED_TXTFILES index.html 
                            INCLUDE_DIRS "." "./")
                                                       
if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43B OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43DEVONLY)
    target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated")

    target_sources(${COMPONENT_LIB} PRIVATE "ui_generated/ui.c" "ui_generated/ui_helpers.c"
                            "ui_generated/images/ui_img_smythbuilt_png.c" "ui_generated/screens/ui_Screen1.c" "ui_generated/screens/ui_Settings.c" "ui_generated/components/ui_comp_hook.c"
			                "ui_generated/images/ui_img_usb_fail_png.c" "ui_generated/images/ui_img_next_png.c" "ui_generated/images/ui_img_previous_png.c" "ui_generated/images/ui_temporary_image.c"
                            "ui_generated/images/ui_img_usb_ok_png.c" "ui_generated/images/ui_img_next_down_png.c" "ui_generated/images/ui_img_previous_down_png.c" "ui_generated/images/ui_img_settings_png.c"
                            "ui_generated/images/ui_img_wifi_conn_png.c" "ui_generated/images/ui_img_wifi_disconn_png.c"
                            "ui_generated/images/ui_img_skin_2001rb_png.c" "ui_generated/images/ui_img_skin_5150_png.c" 
                           "ui_generated/images/ui_img_effect_icon_amp_off_png.c" "ui_generated/images/ui_img_effect_icon_amp_on_png.c" 
                           "ui_generated/images/ui_img_effect_icon_cab_off_png.c" "ui_generated/images/ui_img_effect_icon_cab_on_png.c" 
                           "ui_generated/images/ui_img_effect_icon_comp_off_png.c" "ui_generated/images/ui_img_effect_icon_comp_on_png.c" 
                           "ui_generated/images/ui_img_effect_icon_delay_off_png.c" "ui_generated/images/ui_img_effect_icon_delay_on_png.c" 
                           "ui_generated/images/ui_img_effect_icon_gate_off_png.c" "ui_generated/images/ui_img_effect_icon_gate_on_png.c" 
                           "ui_generated/images/ui_img_effect_icon_mod_off_png.c" "ui_generated/images/ui_img_effect_icon_mod_on_png.c" 
                           "ui_generated/images/ui_img_effect_icon_mod_on_chorus_png.c" "ui_generated/images/ui_img_effect_icon_mod_on_flanger_png.c" 
                           "ui_generated/images/ui_img_effect_icon_mod_on_phaser_png.c" "ui_generated/images/ui_img_effect_icon_mod_on_rotary_png.c" 
                           "ui_generated/images/ui_img_effect_icon_mod_on_tremolo_png.c" 
                           "ui_generated/images/ui_img_effect_icon_reverb_off_png.c" "ui_generated/images/ui_img_effect_icon_reverb_on_png.c" 
                            "ui_generated/images/ui_img_skin_b18n_png.c" "ui_generated/images/ui_img_skin_bluesdeluxe_png.c"
                            "ui_generated/images/ui_img_skin_deville_png.c" "ui_generated/images/ui_img_skin_dualrectifier_png.c"
                            "ui_generated/images/ui_img_skin_goldfinger_png.c" "ui_generated/images/ui_img_skin_invader_png.c" "ui_generated/images/ui_img_skin_jazzchorus_png.c"
                            "ui_generated/images/ui_img_skin_jcm800_png.c" "ui_generated/images/ui_img_skin_or50_png.c" "ui_generated/images/ui_img_skin_powerball_png.c"
                            "ui_generated/images/ui_img_skin_princeton_png.c" "ui_generated/images/ui_img_skin_twinreverb_png.c"
                            "ui_generated/images/ui_img_skin_svtcl_png.c" "ui_generated/images/ui_img_skin_invader_png.c"
                            "ui_generated/images/ui_img_skin_maverick_png.c" "ui_generated/images/ui_img_skin_mk3_png.c"
                            "ui_generated/images/ui_img_skin_superbass_png.c" 
                            "ui_generated/images/ui_img_skin_dumble_png.c" "ui_generated/images/ui_img_skin_jetcity_png.c"
                            "ui_generated/images/ui_img_skin_ac30_png.c" "ui_generated/images/ui_img_skin_evh5150_png.c"
                            "ui_generated/images/ui_img_skin_2020_png.c"
                            "ui_generated/images/ui_img_skin_pinktaco_png.c" "ui_generated/images/ui_img_skin_supro50_png.c"
                            "ui_generated/images/ui_img_skin_diezel_png.c"
                            "ui_generated/images/ui_img_arrow_left_png.c" "ui_generated/images/ui_img_arrow_right_png.c" "ui_generated/images/ui_img_tick_png.c"
                            "ui_generated/images/ui_img_bt_conn_png.c" "ui_generated/images/ui_img_bt_disconn_png.c"
                            "ui_generated/images/ui_img_pskin_arion_png.c" "ui_generated/images/ui_img_pskin_bigmuff_png.c" "ui_generated/images/ui_img_pskin_darkglass_png.c"
                            "ui_generated/images/ui_img_pskin_dod_png.c" "ui_generated/images/ui_img_pskin_ehx_png.c" "ui_generated/images/ui_img_pskin_fender_png.c"
                            "ui_generated/images/ui_img_pskin_fulltone_png.c" "ui_generated/images/ui_img_pskin_fzs_png.c" "ui_generated/images/ui_img_pskin_jhs_png.c"
                            "ui_generated/images/ui_img_pskin_klon_png.c" "ui_generated/images/ui_img_pskin_landgraf_png.c" "ui_generated/images/ui_img_pskin_mxr_png.c"
                            "ui_generated/images/ui_img_pskin_mxr2_png.c" "ui_generated/images/ui_img_pskin_od1_png.c" "ui_generated/images/ui_img_pskin_plimsoul_png.c"
                            "ui_generated/images/ui_img_pskin_rogermayer_png.c" "ui_generated/images/ui_img_pskin_seymour_png.c" "ui_generated/images/ui_img_pskin_strymon_png.c"
                            "ui_generated/images/ui_img_pskin_trex_png.c" "ui_generated/images/ui_img_pskin_tubescreamer_png.c" "ui_generated/images/ui_img_pskin_wampler_png.c"
                            "ui_generated/images/ui_img_pskin_zvex_png.c")                                               
endif()

if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169 OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169TOUCH)
   target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_169")

   target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_169/ui.c" "ui_generated_169/ui_helpers.c"
                            "ui_generated_169/images/ui_img_smythbuilt_png.c" "ui_generated_169/screens/ui_Screen1.c" "ui_generated_169/components/ui_comp_hook.c" "ui_generated_169/images/ui_img_usb_fail_png.c"
                            "ui_generated_169/images/ui_img_usb_ok_png.c"
                            "ui_generated_169/images/ui_img_wifi_conn_png.c" "ui_generated_169/images/ui_img_wifi_disconn_png.c"
                            "ui_generated_169/images/ui_img_bt_conn_png.c" "ui_generated_169/images/ui_img_bt_disconn_png.c")
endif()

if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_M5ATOMS3R)
   target_include_directories(${COMPONENT_LIB} PRIVATE include "ui_generated_085")

   target_sources(${COMPONENT_LIB} PRIVATE "ui_generated_085/ui.c" "ui_generated_085/ui_helpers.c"
                            "ui_generated_085/images/ui_img_smythbuilt_png.c" "ui_generated_085/screens/ui_Screen1.c" "ui_generated_085/components/ui_comp_hook.c" "ui_generated_085/images/ui_img_usb_fail_png.c"
                            "ui_generated_085/images/ui_img_usb_ok_png.c"
                            "ui_generated_085/images/ui_img_wifi_conn_png.c" "ui_generated_085/images/ui_img_wifi_disconn_png.c"
                            "ui_generated_085/images/ui_img_bt_conn_png.c" "ui_generated_085/images/ui_img_bt_disconn_png.c")
endif()

