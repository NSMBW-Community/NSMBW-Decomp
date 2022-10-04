#pragma once
#include <types.h>
/// @file

extern "C" {

/// @brief C++ exception table info.
/// @details [Since the game doesn't support exceptions, this table is unused].
typedef struct __eti_init_info {
    void *eti_start; ///< The address where the exception data for the code segment begins.
    void *eti_end; ///< The address where the exception data for the code segment ends.
    void *code_start; ///< The address where the segment's first function begins.
    ulong code_size; ///< The size of the code segment.
} __eti_init_info;

} // extern "C"
