#pragma once
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *mpArchiveStartAddr;
    void *mpFstStart;
    void *mpFileStart;
    u32 mEntryNum;
    char *mpFstStringStart;
    u32 mFstLength;
    u32 mCurrDir;
} ARCHandle;

typedef struct {
    ARCHandle *mpHandle;
    int mEntryNum;
    int mIsDirectory;
    const char *mpName;
} ARCDirEntry;

#ifdef __cplusplus
}
#endif
