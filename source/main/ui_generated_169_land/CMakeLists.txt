SET(SOURCES screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_usb_ok_png.c
    images/ui_img_usb_fail_png.c
    images/ui_img_smythbuilt_png.c
    images/ui_img_bt_disconn_png.c
    images/ui_img_bt_conn_png.c
    images/ui_img_wifi_disconn_png.c
    images/ui_img_wifi_conn_png.c)

add_library(ui ${SOURCES})
