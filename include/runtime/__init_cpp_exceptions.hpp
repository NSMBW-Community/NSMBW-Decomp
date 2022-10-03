#pragma once
#include <types.h>
/// @file

extern "C" {

/// @brief C++ exception table info.
typedef struct __eti_init_info {
    void *eti_start; ///< Address of start of exception data for this code segment.
    void *eti_end; ///< Address of end of exception data for this code segment.
    void *code_start; ///< Address of first function of segment.
    ulong code_size; ///< Address of the end of the segment's last function.
} __eti_init_info;

} // extern "C"
