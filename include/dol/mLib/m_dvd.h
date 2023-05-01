#pragma once
#include <types.h>
#include <lib/egg/heap/heap.hpp>
#include <lib/egg/archive.hpp>

class mDvd_command_c {
public:
    virtual ~mDvd_command_c();
    virtual void execute();
    virtual void init();

    void destroy();

    int mUnk;
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
    unsigned long mUnk;
};
