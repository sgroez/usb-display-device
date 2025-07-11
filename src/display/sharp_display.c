#include "pico/stdlib.h"
#include "hardware/spi.h"

typedef struct {
  spi_inst_t* spi;
  uint8_t sck;
  uint8_t mosi;
  uint8_t cs;
  uint8_t width;
  uint8_t height;
  uint8_t* buffer;
  bool vcom;
} Sharp_Display;

void Setup(Sharp_Display* d) {
  spi_init(spi0, 2'000'000);  // 2 MHz
  spi_set_format( spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
  gpio_set_function(d->mosi, GPIO_FUNC_SPI);  // TX
  gpio_set_function(d->sck, GPIO_FUNC_SPI);  // SCK

  //setup chip select pin
  gpio_init(d->cs);
  gpio_set_dir(d->cs, GPIO_OUT);
  gpio_put(d->cs, 0);
  sleep_ms(10);
}

void ClearScreen(Sharp_Display* d)
{
    if (!d || d->spi || !d->buffer) return;

    size_t buffer_size = (d->width / 8) * d->height;

    // Set all pixels to white (1s)
    memset(d->buffer, 0xFF, buffer_size);

    gpio_put(d->cs, 1);

    uint8_t buf[2];
    if (d->vcom)
    {
        buf[0] = 0b01100000;
        d->vcom = 0;
    }
    else
    {
        buf[0] = 0b00100000;
        d->vcom = 1;
    }
    buf[1] = 0x00;

    spi_write_blocking(d->spi, buf, 2);

    gpio_put(d->cs, 0);
}
