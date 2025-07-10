#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SGROEZ");
MODULE_DESCRIPTION("USB Echo driver");

#define USB_VENDOR_ID  0xcafe
#define USB_PRODUCT_ID 0x4010

#define EP_OUT     0x01
#define EP_IN      0x81
#define PACKET_SIZE 64
#define TIMEOUT_MS 1000

//Device table to match device with driver
static struct usb_device_id dev_table[] = {
  {USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID)},
  {}
};

//Exports the device table so the system can auto match when device is plugged in
MODULE_DEVICE_TABLE(usb, dev_table);

static int dev_probe(struct usb_interface *interface, const struct usb_device_id *id);
static void dev_disconnect(struct usb_interface *interface);

//Base usb struct
static struct usb_driver usb_echo_driver = {
  .name = "usb_echo_driver",
  .id_table = dev_table,
  .probe = dev_probe,
  .disconnect = dev_disconnect,
};

static int dev_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    struct usb_device *udev = interface_to_usbdev(interface);
    int retval;
    int actual_len;

    uint8_t *out_buf = kzalloc(PACKET_SIZE, GFP_KERNEL);
    uint8_t *in_buf  = kzalloc(PACKET_SIZE, GFP_KERNEL);

    if (!out_buf || !in_buf) {
        dev_err(&interface->dev, "Failed to allocate buffers\n");
        retval = -ENOMEM;
        goto free_bufs;
    }

    dev_info(&interface->dev, "USB device plugged in\n");

    // Fill some test data
    out_buf[0] = 0xAB;  // dummy test byte

    // Send bulk OUT transfer
    retval = usb_bulk_msg(udev,
                          usb_sndbulkpipe(udev, EP_OUT),
                          out_buf,
                          PACKET_SIZE,
                          &actual_len,
                          TIMEOUT_MS);
    if (retval) {
        dev_err(&interface->dev, "Bulk OUT failed: %d\n", retval);
        goto free_bufs;
    }
    dev_info(&interface->dev, "Sent %d bytes to device\n", actual_len);

    // Receive bulk IN transfer
    retval = usb_bulk_msg(udev,
                          usb_rcvbulkpipe(udev, EP_IN),
                          in_buf,
                          PACKET_SIZE,
                          &actual_len,
                          TIMEOUT_MS);
    if (retval) {
        dev_err(&interface->dev, "Bulk IN failed: %d\n", retval);
        goto free_bufs;
    }

    dev_info(&interface->dev, "Received %d bytes from device\n", actual_len);
    for (int i = 0; i < actual_len; i++) {
        dev_info(&interface->dev, "Byte %d: 0x%02X\n", i, in_buf[i]);
    }

free_bufs:
    kfree(out_buf);
    kfree(in_buf);
    return 0;
}

static void dev_disconnect(struct usb_interface *interface) {
  dev_info(&interface->dev, "USB device disconnected\n");
}

static int __init usb_display_driver_init(void) {
  return usb_register(&usb_display_driver);
}

static void __exit usb_display_driver_exit(void) {
  usb_deregister(&usb_display_driver);
}

module_init(usb_display_driver_init);
module_exit(usb_display_driver_exit);

