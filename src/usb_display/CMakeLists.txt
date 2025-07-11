cmake_minimum_required(VERSION 3.13)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(usb LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(usb_display
  usb_display.c
  sharp_display.c
  usb_descriptors.c
)

target_include_directories(usb_display PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(usb_display PUBLIC
  pico_stdlib
  tinyusb_device
  tinyusb_board
  hardware_spi
)

pico_enable_stdio_usb(usb_display 1)
pico_enable_stdio_uart(usb_display 0)

set(FAMILY rp2040)
set(BOARD pico_sdk)
set(TINYUSB_FAMILY_PROJECT_NAME_PREFIX "tinyusb_dev_")
# Provide an LWIP path for net_lwip_webserver in case the module does not exist in tinyusb
set(TINYUSB_LWIP_PATH ${PICO_LWIP_PATH})
# Some examples use this, and we need to set this here due to a bug in the TinyUSB CMake config
set(TOP ${PICO_TINYUSB_PATH})

pico_add_extra_outputs(usb_display)
