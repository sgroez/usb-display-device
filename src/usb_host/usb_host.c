#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define VENDOR_ID  0xCafe   // Replace with your device's VID
#define PRODUCT_ID 0x4010   // Replace with your device's PID
#define INTERFACE  0        // Usually 0 if only one interface

#define EP_OUT 0x01  // Endpoint 1 OUT
#define EP_IN  0x81  // Endpoint 1 IN (0x80 | 0x01)

#define WIDTH 400
#define HEIGHT 240
#define FRAMEBUFFER_SIZE (WIDTH * HEIGHT / 8)
#define PACKET_SIZE 64

uint8_t framebuffer[FRAMEBUFFER_SIZE];

// Generate a white square (100x100) in center of screen
void generate_framebuffer() {
    memset(framebuffer, 0x00, FRAMEBUFFER_SIZE);  // clear to black

    int square_x_start = (WIDTH - 100) / 2;
    int square_x_end = square_x_start + 100;
    int square_y_start = (HEIGHT - 100) / 2;
    int square_y_end = square_y_start + 100;

    int bytes_per_row = WIDTH / 8;

    for (int y = square_y_start; y < square_y_end; y++) {
        for (int x = square_x_start; x < square_x_end; x++) {
            int byte_index = y * bytes_per_row + (x / 8);
            int bit_index = 7 - (x % 8);
            framebuffer[byte_index] |= (1 << bit_index);
        }
    }
}

int main(void)
{
    libusb_context *ctx = NULL;
    libusb_device_handle *dev_handle = NULL;
    int transferred;
    int res;

    generate_framebuffer();

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

    // Send framebuffer in 64-byte chunks
    for (int i = 0; i < FRAMEBUFFER_SIZE; i += PACKET_SIZE) {
        int chunk_size = (i + PACKET_SIZE <= FRAMEBUFFER_SIZE) ? PACKET_SIZE : (FRAMEBUFFER_SIZE - i);
        res = libusb_bulk_transfer(dev_handle, EP_OUT, framebuffer + i, chunk_size, &transferred, 1000);
        if (res != 0) {
            fprintf(stderr, "Error sending packet at offset %d: %s\n", i, libusb_error_name(res));
            break;
        }
    }

    // Cleanup
    libusb_release_interface(dev_handle, INTERFACE);
    libusb_close(dev_handle);
    libusb_exit(ctx);
    return 0;
}

