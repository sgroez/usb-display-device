# DRM structure and functionality research documentation
## Setup
1. Base struct of drm driver (drm_driver)
- specifies driver information such as name, desc, major, minor and patch level version number
- lists driver features supported and used in driver such as DRIVER_MODESET, DRIVER_ATOMIC or DRIVER_GEM
- maps function callbacks for drm core to call like open, unload or release(deprecated)

2. Allocate managed memory for device (devm_drm_dev_alloc())
- drm_device represents an instance of a drm driver
- allocation usually takes place in driver probe function
- initializes core drm fields like (dev, driver, kref, locks, lists, etc.) ?(chatgpt)
- links private data to drm->dev_private ?(chatgpt)
- ensures automatic cleanup (no need to manually call drm_dev_put())
- does not register device to user space

3. Initialize drm mode configurations (drmm_mode_config_init())
- initializes mode configuration structure to support CRTCs, planes, encoders and connectors ?(chatgpt)

4. Allocate managed memory for user space data (drmm_kzalloc())
- allocates memory to store user space data
- ensures automatic cleanup

5. Setup further modules

6. Store drm device in driver device data
- store drm device in usb device for example using (usb_set_intfdata)

7. Reset drm mode configuration (drm_mode_config_reset())

8. Register device to user space (drm_dev_register())
- makes device visible to user space

9. (optional) setup legacy framebuffer support (drm\_fbdev\_{...}\_setup())
- creates legacy framebuffer device

## Remove
1. Retrieve drm device from driver
- from usb interface for example

2. Unregister drm device (drm_dev_unregister())

3. Shutdown atomic state (drm_atomic_helper_shutdown())

## Power management
- Suspend (driver_pm_suspend())
- Resume (driver_pm_resume())
- Shutdown (driver_shutdown())
