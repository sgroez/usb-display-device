# Analysis of linux kernel tiny drm sharp memory display driver
DEFINE_DRM_GEM_DMA_FOPS is a macro that generates file ops for dma driver
drm_driver struct configures features, fops and more driver information
spi_driver struct configures probe and remove function


## Probe
1. configures spi
2. sets up coherent dma mask
3. allocates managed memory for sharp memory device struct
4. configures drm managed mode configuration
5. sets mode config functions
6. allocates managed memory for framebuffer
7. initializes mutex for synchronized sharp memory device access
8. sets up vcom toggle method (software, external or pwm)
9. sets mode config display dimensions
10. sets up **pipe**
11. sets up plane
12. resets mode config
13. registers drm device
14. sets up drm client ??? (looks like framebuffer fallback support?, not documented in docs.kernel.org)

## Remove
1. unplugs drm device
2. shuts down using atomic helper
3. stops vcom toggling

## Pipe init
1. initializes universal plane
2. initializes crtc with plane
3. initializes encoder
4. initializes connector
