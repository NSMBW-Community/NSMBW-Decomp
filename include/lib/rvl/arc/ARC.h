#pragma once
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *mArchiveStartAddr;
    void *mFstStart;
    void *mFileStart;
    u32 mEntryNum;
    char *mFstStringStart;
    u32 mFstLength;
    u32 mCurrDir;
} ARCHandle;

#ifdef __cplusplus
}
#endif
