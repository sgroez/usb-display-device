#ifndef SHARP_DISPLAY_H
#define SHARP_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// Forward declaration of Sharp_Display struct
typedef struct {
    spi_inst_t* spi;
    uint8_t sck;
    uint8_t mosi;
    uint8_t cs;
    bool vcom;
} Sharp_Display;

// Create a new Sharp_Display instance and initialize SPI and GPIO pins
Sharp_Display* NewSharpDisplay(spi_inst_t* spi,
                              uint8_t sck,
                              uint8_t mosi,
                              uint8_t cs,
                              bool vcom);

// Begin a command transfer
void StartCommand(Sharp_Display* d);

// End the current command transfer
void EndCommand(Sharp_Display* d);

// Transmit data bytes to the display
void TransmitData(Sharp_Display* d, uint8_t *buffer, size_t length);

#endif // SHARP_DISPLAY_H

