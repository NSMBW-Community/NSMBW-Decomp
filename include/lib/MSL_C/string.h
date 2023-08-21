#pragma once
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @addtogroup stdlib
/// @{

void *memset(void *, u32, u32);
void memcpy(void *dst, const void* src, int size);

void snprintf(char *, int, const char *, ...);
void strncpy(char *dst, const char* src, int size);
int strlen(const char *str);
int strcmp(const char *str1, const char *str2);
char *strrchr(const char *, char);

/// @}

#ifdef __cplusplus
} // extern "C"
#endif
