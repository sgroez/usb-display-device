#include "tusb.h"

// USB Device Descriptor
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00, // Each interface specifies its own class
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = 0xCafe,  // Your custom VID
    .idProduct          = 0x4001,  // Your custom PID
    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 1
};

// Configuration Descriptor with 1 Vendor Interface
#define EPNUM_OUT   0x01
#define EPNUM_IN    0x81

uint8_t const desc_configuration[] = {
    // Config descriptor
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUD_CONFIG_DESC_LEN + TUD_VENDOR_DESC_LEN, 0x00, 100),

    // Vendor Interface Descriptor
    TUD_VENDOR_DESCRIPTOR(0, 0, EPNUM_OUT, 64, EPNUM_IN, 64)
};

uint8_t const* tud_descriptor_device_cb(void) {
    return (uint8_t const*)&desc_device;
}

uint8_t const* tud_descriptor_configuration_cb(uint8_t index) {
    (void) index;
    return desc_configuration;
}

const char* string_desc_arr[] = {
    "https://github.com/sgroez",
    "usb-display-device",
    "1",
};

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    static uint16_t desc[32];
    uint8_t chr_count;

    if ( index == 0 ) {
        desc[1] = 0x0409; // English
        chr_count = 1;
    } else {
        const char* str = string_desc_arr[index];
        chr_count = strlen(str);
        desc[1] = 0;
        for (uint8_t i = 0; i < chr_count; ++i) {
            desc[1 + i] = str[i];
        }
    }

    desc[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);
    return desc;
}

