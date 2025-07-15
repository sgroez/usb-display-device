#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include "libevdi/evdi_lib.h"  // Make sure to have libevdi headers and link

#define VENDOR_ID  0x1234  // USB device VID
#define PRODUCT_ID 0x5678  // USB device PID
#define INTERFACE_NUM 0
#define ENDPOINT_OUT  0x01

int main() {
    libusb_device_handle *usb_handle = NULL;
    int r;

    // --- Initialize libusb ---
    r = libusb_init(NULL);
    if (r < 0) {
        fprintf(stderr, "libusb init error: %s\n", libusb_error_name(r));
        return 1;
    }

    // Open USB device
    usb_handle = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
    if (!usb_handle) {
        fprintf(stderr, "USB device not found\n");
        libusb_exit(NULL);
        return 1;
    }

    r = libusb_claim_interface(usb_handle, INTERFACE_NUM);
    if (r) {
        fprintf(stderr, "Cannot claim USB interface: %s\n", libusb_error_name(r));
        libusb_close(usb_handle);
        libusb_exit(NULL);
        return 1;
    }

    // --- Initialize evdi ---
    evdi_device_context *evdi_ctx = evdi_open(0);
    if (!evdi_ctx) {
        fprintf(stderr, "Failed to open evdi device\n");
        libusb_release_interface(usb_handle, INTERFACE_NUM);
        libusb_close(usb_handle);
        libusb_exit(NULL);
        return 1;
    }

    evdi_connect(evdi_ctx);
    evdi_enable(evdi_ctx);

    while (1) {
        void *framebuffer;
        int stride;
        int width = evdi_ctx->mode_set.width;
        int height = evdi_ctx->mode_set.height;

        // Grab the latest framebuffer data with a 1000 ms timeout
        r = evdi_grab_pixbuf(evdi_ctx, &framebuffer, &stride, 1000);
        if (r != 0) {
            fprintf(stderr, "Failed to grab evdi framebuffer: %d\n", r);
            continue;
        }

        // Here you have raw ARGB framebuffer data at 'framebuffer'
        // For simplicity, send raw data directly over USB bulk endpoint

        int total_size = stride * height;
        int offset = 0;
        int chunk_size = 4096;
        int actual_len;

        while (offset < total_size) {
            int send_len = (total_size - offset) > chunk_size ? chunk_size : (total_size - offset);
            r = libusb_bulk_transfer(usb_handle, ENDPOINT_OUT,
                                     (unsigned char*)framebuffer + offset,
                                     send_len, &actual_len, 1000);
            if (r != 0 || actual_len != send_len) {
                fprintf(stderr, "USB transfer error: %s\n", libusb_error_name(r));
                break;
            }
            offset += send_len;
        }

        // Optional: add delay or synchronization here
    }

    evdi_disable(evdi_ctx);
    evdi_disconnect(evdi_ctx);
    evdi_close(evdi_ctx);

    libusb_release_interface(usb_handle, INTERFACE_NUM);
    libusb_close(usb_handle);
    libusb_exit(NULL);

    return 0;
}

