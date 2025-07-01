#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#define CFG_TUSB_MCU OPT_MCU_RP2040
#define CFG_TUSB_OS  OPT_OS_NONE

#define CFG_TUSB_RHPORT0_MODE OPT_MODE_DEVICE

// Enable CDC (USB serial) class
#define CFG_TUD_CDC    1
#define CFG_TUD_HID    0
#define CFG_TUD_MSC    0
#define CFG_TUD_MIDI   0
#define CFG_TUD_VENDOR 0

#endif

