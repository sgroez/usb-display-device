cmake_minimum_required(VERSION 3.13)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(usb_display_device LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

pico_sdk_init()

add_subdirectory(${DISPLAY_DRIVER_PATH}/sharp-mip ${CMAKE_BINARY_DIR}/sharp-mip)

add_executable(usb_display_device
  main.cpp
)

target_sources(sharp_mip_display PRIVATE
  ${DISPLAY_DRIVER_PATH}/display.cpp
)

target_include_directories(usb_display_device PRIVATE
  ${DISPLAY_DRIVER_PATH}/sharp-mip
)

target_link_libraries(usb_display_device
  sharp_mip_display
  pico_stdlib
)

pico_enable_stdio_usb(usb_display_device 1)
pico_enable_stdio_uart(usb_display_device 0)

pico_add_extra_outputs(usb_display_device)
