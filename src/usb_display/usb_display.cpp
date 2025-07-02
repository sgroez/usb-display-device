/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board_api.h"
#include "tusb.h"

#include "usb_descriptors.h"

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "sharp_mip_display.h"

/*-------------CONFIG-------------*/
// SPI pins
#define SPI_SCK_PIN  2U
#define SPI_MOSI_PIN 3U
#define SPI_CS_PIN   1U

//Display dimensions
#define DISPLAY_WIDTH 400U
#define DISPLAY_HEIGHT 240U

//Framebuffer settings
#define FRAMEBUFFER_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT / 8)
#define USB_RX_BUFFER_SIZE 64

/*-------------BUFFER-------------*/
static uint8_t framebuffer[FRAMEBUFFER_SIZE];
static uint32_t framebuffer_index = 0;
static volatile bool frame_ready = false;

/*------------- MAIN -------------*/
int main(void)
{
  stdio_init_all();
  sleep_ms(1000);
  board_init();

  // init device stack on configured roothub port
  tud_init(BOARD_TUD_RHPORT);

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  // Configure SPI
  spi_init(spi0, 2'000'000);  // 2 MHz
  spi_set_format( spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
  gpio_set_function(SPI_MOSI_PIN, GPIO_FUNC_SPI);  // TX
  gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);  // SCK

  gpio_init(SPI_CS_PIN);
  gpio_set_dir(SPI_CS_PIN, GPIO_OUT);
  gpio_put(SPI_CS_PIN, 0);
  sleep_ms(10);

  //Configure display
  SharpMipDisplay* display = new SharpMipDisplay(DISPLAY_WIDTH, DISPLAY_HEIGHT, spi0, SPI_CS_PIN);
  display->ClearScreen();

  while (1)
  {
    tud_task(); // tinyusb device task
    
    if (frame_ready) {
      //process framebuffer
      for (uint16_t y = 0; y < DISPLAY_HEIGHT; ++y) {
        for (uint16_t x = 0; x < DISPLAY_WIDTH; ++x) {
          uint32_t bit_index = y * DISPLAY_WIDTH + x;
          uint32_t byte_index = bit_index / 8;
          uint8_t bit_offset = 7 - (bit_index % 8);  // MSB first

          bool pixel_on = (framebuffer[byte_index] >> bit_offset) & 0x01;

          if (pixel_on) {
            display->SetPixel(x, y);
          } else {
            display->ResetPixel(x, y);
          }
        }
      }
      display->RefreshScreen(0, DISPLAY_HEIGHT);

      //reset framebuffer state
      frame_ready = false;
      framebuffer_index = 0;
    }
  }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Device mount/unmount/suspend/resume callbacks stay unchanged
void tud_mount_cb(void) {}
void tud_umount_cb(void) {}
void tud_suspend_cb(bool remote_wakeup_en) { (void)remote_wakeup_en; }
void tud_resume_cb(void) {}

// Called when data is received on vendor OUT endpoint
void tud_vendor_rx_cb(uint8_t itf, uint8_t const* buffer, uint16_t bufsize)
{
  (void) itf;

  while (tud_vendor_available())
  {
    uint8_t temp[64];
    uint32_t count = tud_vendor_read(temp, sizeof(temp));

    // Store into framebuffer
    if (framebuffer_index + count <= FRAMEBUFFER_SIZE) {
      memcpy(&framebuffer[framebuffer_index], temp, count);
      framebuffer_index += count;

      if (framebuffer_index == FRAMEBUFFER_SIZE) {
        frame_ready = true;
      }
    } else {
      // Overflow detected; reset
      framebuffer_index = 0;
    }
  }
}
