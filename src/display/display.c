#include <string.h>
#include "pico/stdlib.h"
#include "sharp_display.h"

char clearByte   = 0b00100000;
char paddingByte = 0b00000000;
char commandByte = 0b10000000;

size_t BuildTestImageFrame(uint8_t *buffer, uint8_t commandByte) {
    size_t buf_i = 0;

    // Command byte (no VCOM toggle included here)
    buffer[buf_i++] = commandByte;

    // For each line
    for (uint8_t line = 1; line <= 240; line++) {
        buffer[buf_i++] = line;                  // line address (1-based)
        memset(&buffer[buf_i], 0x00, 50); // all white pixels (0 = white)
        buf_i += 50;
        buffer[buf_i++] = 0x00;                  // line trailer
    }

    buffer[buf_i++] = 0x00; // end of transmission trailer

    return buf_i; // total length of buffer
}

int main() {
  stdio_init_all();
  sleep_ms(1000);

  Sharp_Display* display = NewSharpDisplay(spi0, 2, 3, 1, false);

  //clear screen
  char buffer[2] = {clearByte, paddingByte};
  ExecuteCommand(display, buffer, 2);

  //draw a simple test image
  int8_t framebuffer[1 + 240 * (1 + 50 + 1) + 1]; // size for full frame
  size_t length = BuildTestImageFrame(framebuffer, commandByte);
  ExecuteCommand(display, framebuffer, length);

  while(1) {
    sleep_ms(1000);
  }
}
