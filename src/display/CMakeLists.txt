cmake_minimum_required(VERSION 3.13)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(display LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(display
  display.c
  sharp_display.c
)

target_include_directories(display PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(display PUBLIC
  pico_stdlib
  hardware_spi
)

pico_enable_stdio_usb(display 1)     # Enable USB serial
pico_enable_stdio_uart(display 0)    # Disable UART stdio (optional)

pico_add_extra_outputs(display)
