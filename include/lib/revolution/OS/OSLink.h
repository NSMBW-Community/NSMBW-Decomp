#ifndef RVL_SDK_OS_LINK_H
#define RVL_SDK_OS_LINK_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif


#define OS_MODULE_VERSION 3

typedef u32 OSModuleID;

struct OSModuleInfo;

typedef struct OSModuleQueue {
    OSModuleInfo* head;
    OSModuleInfo* tail;
} OSModuleQueue;

typedef struct OSModuleLink {
    OSModuleInfo* next;
    OSModuleInfo* prev;
} OSModuleLink;

typedef struct OSSectionInfo {
    u32 offset;
    u32 size;
} OSSectionInfo;

typedef struct OSModuleInfo {
    OSModuleID id;          // unique identifier for the module
    OSModuleLink link;      // doubly linked list of modules
    u32 numSections;        // # of sections
    u32 sectionInfoOffset;  // offset to section info table
    u32 nameOffset;         // offset to module name
    u32 nameSize;           // size of module name
    u32 version;            // version number
} OSModuleInfo;

typedef struct OSModuleHeader {
    OSModuleInfo info;

    // OS_MODULE_VERSION == 1
    u32 bssSize;  // total size of bss sections in bytes
    u32 relOffset;
    u32 impOffset;
    u32 impSize;           // size in bytes
    u8 prologSection;      // section # for prolog function
    u8 epilogSection;      // section # for epilog function
    u8 unresolvedSection;  // section # for unresolved function
    u8 bssSection;         // section # for bss section (set at run-time)
    u32 prolog;            // prolog function offset
    u32 epilog;            // epilog function offset
    u32 unresolved;        // unresolved function offset

    // OS_MODULE_VERSION == 2
#if (2 <= OS_MODULE_VERSION)
    u32 align;     // module alignment constraint
    u32 bssAlign;  // bss alignment constraint
#endif

    // OS_MODULE_VERSION == 3
#if (3 <= OS_MODULE_VERSION)
    u32 fixSize;
#endif
} OSModuleHeader;

BOOL OSLink(OSModuleInfo* newModule, void* bss);
BOOL OSLinkFixed(OSModuleInfo* newModule, void* bss);
BOOL OSUnlink(OSModuleInfo* module);
void OSSetStringTable(void* string_table);
void __OSModuleInit(void);

#ifdef __cplusplus
}
#endif
#endif
