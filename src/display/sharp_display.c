#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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

Sharp_Display* NewSharpDisplay(spi_inst_t* spi,
                               uint8_t sck,
                               uint8_t mosi,
                               uint8_t cs,
                               uint16_t width,
                               uint16_t height,
                               bool vcom
) {
  printf("Allocating Sharp_Display struct...\n");
  Sharp_Display* d = (Sharp_Display*) malloc(sizeof(Sharp_Display));
  if (!d) {
    printf("Failed to allocate Sharp_Display\n");
    return NULL;
  }

  size_t buffer_size = (width / 8) * height;
  printf("Allocating framebuffer of size %zu bytes...\n", buffer_size);
  d->buffer = (uint8_t*) calloc(buffer_size, sizeof(uint8_t));  // zero initialized
  if (!d->buffer) {
    printf("Failed to allocate framebuffer\n");
    free(d);
    return NULL;
  }

  printf("Initialising display struct values...\n");
  d->spi = spi;
  d->sck = sck;
  d->mosi = mosi;
  d->cs = cs;
  d->width = width;
  d->height = height;
  d->vcom = vcom;

  printf("Setting up SPI...\n");
  spi_init(d->spi, 2000000);  // 2 MHz
  spi_set_format( d->spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
  gpio_set_function(d->mosi, GPIO_FUNC_SPI);  // TX
  gpio_set_function(d->sck, GPIO_FUNC_SPI);  // SCK
  gpio_init(d->cs);
  gpio_set_dir(d->cs, GPIO_OUT);
  gpio_put(d->cs, 0);
  sleep_ms(10);

  return d;
}

void ExecuteCommand(Sharp_Display* d, uint8_t *buffer, size_t length) {
  printf("Executing command: %d\n", buffer[0]);
  gpio_put(d->cs, 1);
  spi_write_blocking(d->spi, buffer, length);
  gpio_put(d->cs, 0);
  sleep_ms(10);
}
