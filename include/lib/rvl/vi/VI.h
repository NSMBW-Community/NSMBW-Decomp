#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define VI_VIRTUAL_WIDTH_STD (608.0f)
#define VI_VIRTUAL_WIDTH_WIDE (832.0f)
#define VI_VIRTUAL_HEIGHT (456.0f)

#define VI_VIRTUAL_HALF_WIDTH_STD (VI_VIRTUAL_WIDTH_STD / 2)
#define VI_VIRTUAL_HALF_WIDTH_WIDE (VI_VIRTUAL_WIDTH_WIDE / 2)
#define VI_VIRTUAL_HALF_HEIGHT (VI_VIRTUAL_HEIGHT / 2)

void VIWaitForRetrace();

#ifdef __cplusplus
}
#endif
