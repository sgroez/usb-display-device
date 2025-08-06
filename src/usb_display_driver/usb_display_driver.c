#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <drm/drm_drv.h>
#include <drm/drm_device.h>
#include <drm/drm_file.h>
#include <drm/drm_ioctl.h>

MODULE_AUTHOR("SGROEZ");
MODULE_DESCRIPTION("USB display driver");
MODULE_LICENSE("GPL");

#define DRIVER_NAME "usb_display_driver"
#define DRIVER_DESC "USB display driver"
#define DRIVER_DATE "20250805"
#define DRIVER_MAJOR 1
#define DRIVER_MINOR 0

#define USB_VENDOR_ID  0xcafe
#define USB_PRODUCT_ID 0x4010

#define EP_OUT     0x01
#define EP_IN      0x81
#define PACKET_SIZE 64
#define TIMEOUT_MS 1000

static int my_drm_open(struct drm_device *dev, struct drm_file *file)
{
    pr_info("my_drm: device opened\n");
    return 0;
}

static void my_drm_release(struct drm_device *dev)
{
    pr_info("my_drm: device released\n");
}

static const struct drm_driver my_drm_driver = {
    .driver_features = DRIVER_MODESET,
    .open = my_drm_open,
    .release = my_drm_release,
    .name = DRIVER_NAME,
    .desc = DRIVER_DESC,
    .date = DRIVER_DATE,
    .major = DRIVER_MAJOR,
    .minor = DRIVER_MINOR,
};

//Device table to match device with driver
static struct usb_device_id dev_table[] = {
  {USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID)},
  {}
};

//Exports the device table so the system can auto match when device is plugged in
MODULE_DEVICE_TABLE(usb, dev_table);

static int dev_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    struct drm_device *drm;
    int ret;

    drm = drm_dev_alloc(&my_drm_driver, &interface->dev);
    if (IS_ERR(drm))
        return PTR_ERR(drm);

    usb_set_intfdata(interface, drm);

    ret = drm_dev_register(drm, 0);
    if (ret) {
        drm_dev_put(drm);
        return ret;
    }
    dev_info(&interface->dev, "USB device plugged in\n");
    return 0;
}

static void dev_disconnect(struct usb_interface *interface) {
  struct drm_device *drm = usb_get_intfdata(interface);
  drm_dev_unregister(drm);
  drm_dev_put(drm);
  dev_info(&interface->dev, "USB device disconnected\n");
}

//Base usb struct
static struct usb_driver usb_display_driver = {
  .name = "usb_display_driver",
  .id_table = dev_table,
  .probe = dev_probe,
  .disconnect = dev_disconnect,
};

static int __init usb_display_driver_init(void) {
  return usb_register(&usb_display_driver);
}

static void __exit usb_display_driver_exit(void) {
  usb_deregister(&usb_display_driver);
}

module_init(usb_display_driver_init);
module_exit(usb_display_driver_exit);

