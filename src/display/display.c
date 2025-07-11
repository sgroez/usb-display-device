#include "pico/stdlib.h"
#include "sharp_display.h"

char commandByte = 0b10000000;
char vcomByte    = 0b01000000;
char clearByte   = 0b00100000;
char paddingByte = 0b00000000;

int main() {
  stdio_init_all();
  sleep_ms(1000);

  Sharp_Display* display = NewSharpDisplay(spi0, 2, 3, 1, 400, 240, false);

  char buffer[2] = {clearByte, paddingByte};
  ExecuteCommand(display, buffer, 2);

  while(1) {
    sleep_ms(1000);
  }
}
