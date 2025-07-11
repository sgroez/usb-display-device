#include "pico/stdlib.h"
#include "sharp_display.h"

static uint8_t framebuffer[(400 / 8) * 240];

int main() {
  stdio_init_all();
  sleep_ms(1000);

  Sharp_Display display = {
    .spi = spi0,
    .sck = 2,
    .mosi = 3,
    .cs = 1,
    .width = 400,
    .height = 240,
    .buffer = framebuffer,
    .vcom = false
  }

  Setup(&display);
  ClearScreen(&display);

  while(1) {
    sleep_ms(1000);
  }
}
