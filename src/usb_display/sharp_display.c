#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

char vcomByte = 0b01000000;

typedef struct {
  spi_inst_t* spi;
  uint8_t sck;
  uint8_t mosi;
  uint8_t cs;
  bool vcom;
} Sharp_Display;

Sharp_Display* NewSharpDisplay(spi_inst_t* spi,
                               uint8_t sck,
                               uint8_t mosi,
                               uint8_t cs,
                               bool vcom
) {
  printf("Allocating Sharp_Display struct...\n");
  Sharp_Display* d = (Sharp_Display*) malloc(sizeof(Sharp_Display));
  if (!d) {
    printf("Failed to allocate Sharp_Display\n");
    return NULL;
  }

  printf("Initialising display struct values...\n");
  d->spi = spi;
  d->sck = sck;
  d->mosi = mosi;
  d->cs = cs;
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

void StartCommand(Sharp_Display* d) {
  uint8_t commandByte = 0b10000000;
  printf("Executing command: %d\n", commandByte);
  //Toggle vcom bit if vcom is true
  if (d->vcom)
    commandByte |= vcomByte;
  d->vcom = !d->vcom;

  //Start command with cs high and sending comandByte
  gpio_put(d->cs, 1);
  spi_write_blocking(d->spi, &commandByte, 1);
}

void EndCommand(Sharp_Display* d) {
  uint8_t endByte =  0b00000000;
  printf("Ending command transfer.");
  //End command with sending endByte cs low
  spi_write_blocking(d->spi, &endByte, 1);
  gpio_put(d->cs, 0);
  sleep_ms(10);
}

void TransmitData(Sharp_Display* d, uint8_t *buffer, size_t length) {
  printf("Transmitting data...");
  spi_write_blocking(d->spi, buffer, length);
}
