# 1. IDF version >= 6.0 does not have usb component: usb from IDF component manager will be used
# 2. For linux target, we can't use IDF component manager to get usb component, we need to add it 'the old way'
#    with EXTRA_COMPONENT_DIRS because mocking of managed components is not supported yet.
#    This is acceptable workaround for testing.
set(requires "")
if((${IDF_VERSION_MAJOR} LESS 6) OR ("${IDF_TARGET}" STREQUAL "linux"))
    list(APPEND requires usb)
endif()

idf_component_register(SRCS
                        "midi_host.c"                
                        "midi_host_descriptor_parsing.c"  
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS "private_include" "include/esp_private"
                       REQUIRES "${requires}"
                       )
