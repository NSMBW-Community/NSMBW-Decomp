#pragma once
#include <types.h>
#include <lib/egg/core/eggHeap.h>
#include <lib/egg/core/eggArchive.h>

class mDvd_command_c {
public:
    virtual ~mDvd_command_c();
    virtual void execute();
    virtual void init();

    void destroy();

    int mReadLength;
    bool mDone;
};

class mDvd_mountMemArchive_c : public mDvd_command_c {
public:
    void *getArcBinary() const;

    static mDvd_mountMemArchive_c *create(const char *name, u8 allocDirection, EGG::Heap *heap);

    u8 mAllocDirection;
    u8 mCompressionType;
    int mEntryNum;
    EGG::Archive *mpArchive;
    EGG::Heap *mpHeap;
    unsigned long mArchiveSize;
};
