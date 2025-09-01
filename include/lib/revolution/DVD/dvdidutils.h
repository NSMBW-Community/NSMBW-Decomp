#ifndef RVL_SDK_DVD_ID_UTILS_H
#define RVL_SDK_DVD_ID_UTILS_H
#include <revolution/DVD/dvd.h>
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif

BOOL DVDCompareDiskID(const DVDDiskID* id1, const DVDDiskID* id2);

#ifdef __cplusplus
}
#endif
#endif
