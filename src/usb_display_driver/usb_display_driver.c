// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/platform_device.h>
#include <drm/drm_drv.h>
#include <drm/drm_device.h>
#include <drm/drm_file.h>
#include <drm/drm_ioctl.h>

#define DRIVER_NAME     "my_drm"
#define DRIVER_DESC     "Minimal DRM driver"
#define DRIVER_DATE     "20250805"
#define DRIVER_MAJOR    1
#define DRIVER_MINOR    0

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

static int my_platform_probe(struct platform_device *pdev)
{
    struct drm_device *drm;
    int ret;

    drm = drm_dev_alloc(&my_drm_driver, &pdev->dev);
    if (IS_ERR(drm))
        return PTR_ERR(drm);

    platform_set_drvdata(pdev, drm);

    ret = drm_dev_register(drm, 0);
    if (ret) {
        drm_dev_put(drm);
        return ret;
    }

    pr_info("my_drm: platform device probed\n");
    return 0;
}

static void my_platform_remove(struct platform_device *pdev)
{
    struct drm_device *drm = platform_get_drvdata(pdev);

    drm_dev_unregister(drm);
    drm_dev_put(drm);

    pr_info("my_drm: platform device removed\n");
}

static struct platform_driver my_platform_driver = {
    .probe  = my_platform_probe,
    .remove = my_platform_remove,
    .driver = {
        .name = DRIVER_NAME,
    },
};

static struct platform_device *my_platform_device;

static int __init my_drm_init(void)
{
    int ret;

    my_platform_device = platform_device_register_simple(DRIVER_NAME, -1, NULL, 0);
    if (IS_ERR(my_platform_device))
        return PTR_ERR(my_platform_device);

    ret = platform_driver_register(&my_platform_driver);
    if (ret) {
        platform_device_unregister(my_platform_device);
        return ret;
    }

    pr_info("my_drm: module loaded\n");
    return 0;
}

static void __exit my_drm_exit(void)
{
    platform_driver_unregister(&my_platform_driver);
    platform_device_unregister(my_platform_device);
    pr_info("my_drm: module unloaded\n");
}

module_init(my_drm_init);
module_exit(my_drm_exit);

MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Minimal DRM Driver Skeleton");
MODULE_LICENSE("GPL");

