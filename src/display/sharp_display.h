#ifndef SHARP_DISPLAY_H
#define SHARP_DISPLAY_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Struct representing a Sharp Memory Display
typedef struct {
    spi_inst_t* spi;     // SPI interface (e.g., spi0 or spi1)
    uint8_t sck;         // GPIO pin for SPI clock
    uint8_t mosi;        // GPIO pin for SPI MOSI
    uint8_t cs;          // Chip Select pin
    uint8_t width;       // Display width in pixels
    uint8_t height;      // Display height in pixels
    uint8_t* buffer;     // Pointer to framebuffer (1-bit per pixel)
    bool vcom;           // VCOM bit toggle state
} Sharp_Display;

// Initializes the display and configures SPI and GPIOs
void Setup(Sharp_Display* d);

// Clears the display buffer and sends clear command to screen
void ClearScreen(Sharp_Display* d);

#endif // SHARP_DISPLAY_H

