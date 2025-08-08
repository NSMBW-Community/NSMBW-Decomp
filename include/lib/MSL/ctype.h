#ifndef MSL_CTYPE_H
#define MSL_CTYPE_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct __CMap {
    char UNK_0x0[0x10];
    const u8* to_lower_table; // at 0x10
    const u8* to_upper_table; // at 0x14
} __CMap;

typedef struct __Locale {
    char UNK_0x0[0x38];
    struct __CMap* cmap; // at 0x38
    char UNK_0x3C[0x44 - 0x3C];
} __Locale;

extern __Locale _current_locale;

inline int tolower(int x) {
    return (x < 0 || x >= 256)
               ? x
               : (int)(&_current_locale)->cmap->to_lower_table[x];
}

inline int toupper(int x) {
    return (x < 0 || x >= 256)
               ? x
               : (int)(&_current_locale)->cmap->to_upper_table[x];
}

#ifdef __cplusplus
}
#endif
#endif
