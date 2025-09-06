cmake_minimum_required(VERSION 3.13)

# initialize the SDK directory
include (/code/pico/pico-sdk/pico_sdk_init.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

add_executable(usbled)

target_sources(usbled PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
        )

# Make sure TinyUSB can find tusb_config.h
target_include_directories(usbled PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

# Add pico_stdlib library with aggregates commonly used features
#target_link_libraries(usbled pico_stdlib)
target_link_libraries(usbled PUBLIC pico_stdlib tinyusb_device tinyusb_board)
#target_link_libraries(dev_hid_composite PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board)



# create map/pin/uf2 file in addition to ELF.
pico_add_extra_outputs(usbled)
