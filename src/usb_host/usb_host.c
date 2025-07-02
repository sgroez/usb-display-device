#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define VENDOR_ID  0xCafe   // Replace with your device's VID
#define PRODUCT_ID 0x4010   // Replace with your device's PID
#define INTERFACE  0        // Usually 0 if only one interface

#define EP_OUT 0x01  // Endpoint 1 OUT
#define EP_IN  0x81  // Endpoint 1 IN (0x80 | 0x01)

int main(void)
{
    libusb_context *ctx = NULL;
    libusb_device_handle *dev_handle = NULL;
    int transferred;
    int res;

    uint8_t data_out[] = "Hello TinyUSB!";
    uint8_t data_in[64];

    // Init libusb
    if (libusb_init(&ctx) < 0) {
        fprintf(stderr, "libusb init failed\n");
        return 1;
    }

    // Open device
    dev_handle = libusb_open_device_with_vid_pid(ctx, VENDOR_ID, PRODUCT_ID);
    if (!dev_handle) {
        fprintf(stderr, "Cannot open USB device\n");
        libusb_exit(ctx);
        return 1;
    }

    // Claim the interface
    if (libusb_claim_interface(dev_handle, INTERFACE) < 0) {
        fprintf(stderr, "Cannot claim interface\n");
        libusb_close(dev_handle);
        libusb_exit(ctx);
        return 1;
    }

    // Send data
    res = libusb_bulk_transfer(dev_handle, EP_OUT, data_out, sizeof(data_out), &transferred, 1000);
    if (res == 0) {
        printf("Sent %d bytes: %s\n", transferred, data_out);
    } else {
        fprintf(stderr, "Error in bulk OUT transfer: %s\n", libusb_error_name(res));
    }

    // Receive response
    res = libusb_bulk_transfer(dev_handle, EP_IN, data_in, sizeof(data_in), &transferred, 1000);
    if (res == 0) {
        printf("Received %d bytes: %.*s\n", transferred, transferred, data_in);
    } else {
        fprintf(stderr, "Error in bulk IN transfer: %s\n", libusb_error_name(res));
    }

    // Cleanup
    libusb_release_interface(dev_handle, INTERFACE);
    libusb_close(dev_handle);
    libusb_exit(ctx);
    return 0;
}

