cmake_minimum_required(VERSION 3.16)

# This allows building development and production configs side by side,
# without having them influence each other.
set(SDKCONFIG "${CMAKE_BINARY_DIR}/sdkconfig")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(TonexController)

if(CONFIG_TONEX_CONTROLLER_DISPLAY_FULL_UI)
    # Flash the custom skins partition
    set(partition skins)
    idf_build_get_property(project_dir PROJECT_DIR)

    if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43B)
        set(image_file ${project_dir}/skin_images/16bit/skins.bin)
    endif()

    if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43DEVONLY)
        set(image_file ${project_dir}/skin_images/16bit/skins_minimal.bin)
    endif()

    if(CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_35B OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_JC3248W535 OR CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_PIRATE_MIDI_POLAR_MAX_V2)
        set(image_file ${project_dir}/skin_images/16bitswapped/skins.bin)
    endif()

    partition_table_get_partition_info(offset "--partition-name ${partition}" "offset")
    esptool_py_flash_target_image(flash "${partition}" "${offset}" "${image_file}")
endif()