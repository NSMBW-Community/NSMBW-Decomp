#pragma once
#include <types.h>
/// @file

#ifdef __cplusplus
extern "C" {
#endif

/// @brief The possible allocation directions.
typedef enum {
    MEM_EXPHEAP_ALLOC_DIR_FRONT, ///< Allocates from the bottom of the free memory region.
    MEM_EXPHEAP_ALLOC_DIR_REAR, ///< Allocates from the top of the free memory region.
} MEMExpHeapAllocDir;

#ifdef __cplusplus
}
#endif
