#pragma once
#include <types.h>
/// @file

#ifdef __cplusplus
extern "C" {
#endif

/// @brief The possible memory free modes.
typedef enum {
    MEM_FRMHEAP_FREE_HEAD = BIT_FLAG(0), ///< Deallocates from the bottom of the heap region.
    MEM_FRMHEAP_FREE_TAIL = BIT_FLAG(1), ///< Deallocates from the top of the heap region.
    MEM_FRMHEAP_FREE_ALL = MEM_FRMHEAP_FREE_HEAD | MEM_FRMHEAP_FREE_TAIL, ///< Deallocates all memory blocks from the heap.
} MEMFrmHeapFreeMode;

#ifdef __cplusplus
}
#endif
