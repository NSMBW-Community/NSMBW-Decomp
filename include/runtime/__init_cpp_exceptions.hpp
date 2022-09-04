#pragma once
#include <types.h>

extern "C" {

typedef struct __eti_init_info {
    void *eti_start;
    void *eti_end;
    void *code_start;
    ulong code_size;
} __eti_init_info;

} // extern "C"
