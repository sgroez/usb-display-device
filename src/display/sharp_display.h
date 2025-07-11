#ifndef SHARP_DISPLAY_H
#define SHARP_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>       // for size_t
#include "pico/stdlib.h"
#include "hardware/spi.h"

typedef struct {
    spi_inst_t* spi;
    uint8_t sck;
    uint8_t mosi;
    uint8_t cs;
    uint16_t width;
    uint16_t height;
    uint8_t* buffer;
    bool vcom;
} Sharp_Display;

/**
 * @brief Allocate and initialize a new Sharp_Display instance.
 * 
 * @param spi Pointer to spi instance (e.g., spi0, spi1)
 * @param sck GPIO pin for SCK
 * @param mosi GPIO pin for MOSI
 * @param cs GPIO pin for Chip Select
 * @param width Display width in pixels
 * @param height Display height in pixels
 * @param vcom Initial vcom state
 * @return Sharp_Display* Pointer to allocated Sharp_Display struct, or NULL on failure
 */
Sharp_Display* NewSharpDisplay(spi_inst_t* spi,
                              uint8_t sck,
                              uint8_t mosi,
                              uint8_t cs,
                              uint16_t width,
                              uint16_t height,
                              bool vcom);

/**
 * @brief Send a command buffer to the display via SPI.
 * 
 * @param d Pointer to Sharp_Display instance
 * @param buffer Pointer to command data buffer
 * @param length Length of the buffer in bytes
 */
void ExecuteCommand(Sharp_Display* d, uint8_t *buffer, size_t length);

#endif // SHARP_DISPLAY_H

