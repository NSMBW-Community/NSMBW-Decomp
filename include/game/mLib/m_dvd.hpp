#pragma once
#include <types.h>
#include <lib/egg/core/eggHeap.h>
#include <lib/egg/core/eggArchive.h>

/// @brief Low-level DVD/archive loading layer, wrapped by @ref dDvd for game use.
/// @ingroup mlib
namespace mDvd {
void create(long, EGG::Heap *, EGG::Heap *, EGG::Heap *); ///< Initializes the DVD subsystem.
EGG::Heap *getArchiveHeap(); ///< Returns the default heap used for archive loads.
void setAutoStreamDecomp(bool); ///< Enables or disables automatic stream decompression.
} // namespace mDvd

/// [TODO: unidentified function, no known symbol name yet]
extern "C" void fn_8016B1E0(); ///< @unofficial

class mDvd_command_c {
public:
    virtual ~mDvd_command_c();
    virtual void execute();
    virtual void init();

    void destroy();
    void waitDone() const;

    bool isDone() const { return mDone; }

    int mReadLength;
    bool mDone;
};

class mDvd_callback_c : public mDvd_command_c {
public:
    static mDvd_callback_c *create(void *(*callback)(void *), void *param);
    static mDvd_callback_c *createOrDie(void *(*callback)(void *), void *param);
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

class mDvd_toMainRam_c : public mDvd_command_c {
public:
    static mDvd_toMainRam_c *create(const char *path, u8 allocDirection, EGG::Heap *heap);

    u8 mPad[0x4];
    void *mpData;
    u32 mSize; ///< @unofficial
};
