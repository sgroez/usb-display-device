cmake_minimum_required(VERSION 3.13)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(usb_display_device)

pico_sdk_init()

add_executable(usb_display_device main.c)
target_link_libraries(usb_display_device pico_stdlib)
pico_add_extra_outputs(usb_display_device)

pico_enable_stdio_usb(usb_display_device 1)
pico_enable_stdio_uart(usb_display_device 0)
