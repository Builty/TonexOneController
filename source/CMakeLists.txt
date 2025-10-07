cmake_minimum_required(VERSION 3.16)

# This allows building development and production configs side by side,
# without having them influence each other.
set(SDKCONFIG "${CMAKE_BINARY_DIR}/sdkconfig")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(TonexController)

# Flash the custom partition
#CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43DEVONLY)
set(partition skins)
idf_build_get_property(project_dir PROJECT_DIR)
set(image_file ${project_dir}/skin_images/16bit/${partition}.bin)
partition_table_get_partition_info(offset "--partition-name ${partition}" "offset")
esptool_py_flash_target_image(flash "${partition}" "${offset}" "${image_file}")
