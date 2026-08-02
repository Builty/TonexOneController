idf_component_register(SRCS
                        "midi_host.c"                
                        "midi_host_descriptor_parsing.c"  
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS "private_include" "include/esp_private"
                       REQUIRES usb
                       )
