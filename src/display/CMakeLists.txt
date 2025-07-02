cmake_minimum_required(VERSION 3.13)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(display LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

pico_sdk_init()

add_subdirectory(${DISPLAY_DRIVER_PATH}/sharp-mip ${CMAKE_BINARY_DIR}/sharp-mip)

add_executable(display
  main.cpp
)

target_sources(sharp_mip_display PRIVATE
  ${DISPLAY_DRIVER_PATH}/display.cpp
)

target_include_directories(display PRIVATE
  ${DISPLAY_DRIVER_PATH}/sharp-mip
)

target_include_directories(display PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)


target_link_libraries(display
  sharp_mip_display
  pico_stdlib
)

pico_enable_stdio_usb(display 1)
pico_enable_stdio_uart(display 0)

pico_add_extra_outputs(display)
