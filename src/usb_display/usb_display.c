#include <stdlib.h>
#include <stdio.h>

#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_descriptors.h"

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "sharp_display.h"

static SharpDisplay* display;

/*--------------MAIN--------------*/
void setup() {
  board_init();

  // init device stack on configured roothub port
  tud_init(BOARD_TUD_RHPORT);

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  stdio_init_all();
  sleep_ms(1000);

  display = NewSharpDisplay(spi0, 2, 3, 1, false);
}

int main(void) {
  setup();

  while (1)
  {
    tud_task(); // tinyusb device task
  }
}

// Called when data is received on vendor OUT endpoint
void tud_vendor_rx_cb(uint8_t itf, uint8_t const* buffer, uint16_t bufsize) {
  ExecuteCommand(display, buffer, bufsize);
}

// Device mount/unmount/suspend/resume callbacks stay unchanged
void tud_mount_cb(void) {}
void tud_umount_cb(void) {}
void tud_suspend_cb(bool remote_wakeup_en) { (void)remote_wakeup_en; }
void tud_resume_cb(void) {}
