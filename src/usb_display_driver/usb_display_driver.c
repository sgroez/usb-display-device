// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/platform_device.h>
#include <drm/drm_drv.h>
#include <drm/drm_simple_kms_helper.h>
#include <drm/drm_gem_shmem_helper.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_modeset_helper_vtables.h>

#define DRIVER_NAME "simplekms"
#define DRIVER_DESC "Minimal DRM/KMS driver"
#define DRIVER_DATE "20250702"
#define DRIVER_MAJOR 1
#define DRIVER_MINOR 0

static const uint32_t formats[] = { DRM_FORMAT_XRGB8888 };
static struct drm_simple_display_pipe pipe;

static const struct drm_simple_display_pipe_funcs pipe_funcs = {
    .enable = NULL,   // plug in hardware mode-on
    .disable = NULL,  // plug in hardware mode-off
    .check = NULL,    // check if config is valid
    .update = NULL,   // deliver buffer to scanout
};

static int simplekms_probe(struct platform_device *pdev)
{
    struct drm_device *drm;
    int ret;

    drm = drm_dev_alloc(&simplekms_driver, &pdev->dev);
    if (IS_ERR(drm)) return PTR_ERR(drm);

    drm->fops = &drm_simple_kms_fops;
    drm->mode_config.funcs = &drm_simple_kms_mode_config_funcs;
    drm->mode_config.helper_private = &drm_simple_kms_mode_config_helpers;
    drm_mode_config_init(drm);

    ret = drm_simple_display_pipe_init(drm, &pipe, &pipe_funcs,
                                       formats, ARRAY_SIZE(formats),
                                       NULL);
    if (ret) goto err;

    ret = drm_gem_shmem_helper_init(drm);
    if (ret) goto err;

    ret = drm_dev_register(drm, 0);
    if (ret) {
        drm_dev_put(drm);
        return ret;
    }

    platform_set_drvdata(pdev, drm);
    return 0;

err:
    drm_dev_put(drm);
    return ret;
}

static int simplekms_remove(struct platform_device *pdev)
{
    struct drm_device *drm = platform_get_drvdata(pdev);
    drm_dev_unregister(drm);
    drm_dev_put(drm);
    return 0;
}

static const struct drm_driver simplekms_driver = {
    .driver_features = DRIVER_MODESET,
    .fops = &drm_simple_kms_fops,
    .gem_free_object = drm_gem_shmem_free_object,
    .name = DRIVER_NAME,
    .desc = DRIVER_DESC,
    .date = DRIVER_DATE,
    .major = DRIVER_MAJOR,
    .minor = DRIVER_MINOR,
};

static struct platform_driver simplekms_platform = {
    .probe = simplekms_probe,
    .remove = simplekms_remove,
    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
    },
};

module_platform_driver(simplekms_platform);
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Minimal DRM/KMS Driver");
MODULE_LICENSE("GPL");

