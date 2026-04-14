#pragma once

#include <game/mLib/m_dvd.hpp>
#include <lib/egg/core/eggExpHeap.h>
#include <lib/egg/core/eggDvdFile.h>
#include <lib/nw4r/db.h>

/// @brief A debug map file wrapper.
class DbMapFile {
public:
    ~DbMapFile();
    void RegisterOnDvd(const char *, const OSModuleInfo *);
    void Unregister();

    nw4r::db::MapFile *mpMapFile;
    u8 mBuf[0x10];
};

/// @brief A base class that handles one dynamic module.
class DynamicModuleControlBase {
protected:
    u16 mUsageCount; ///< Number of times this modules is being used.
    u16 mLinkCount; ///< Number of times this module is linked.
    DynamicModuleControlBase *mpPrev; ///< The previous module in the linked list of modules.
    DynamicModuleControlBase *mpNext; ///< The next module in the linked list of modules.

public:
    DynamicModuleControlBase();
    virtual ~DynamicModuleControlBase();

    virtual const char *getModuleName() const { return nullptr; }
    virtual size_t getModuleSize() const;
    virtual const char *getModuleTypeString() const { return "Base"; }
    virtual void dump();

    /// @brief Loads the module into memory.
    virtual bool do_load() { return true; }
    virtual bool do_load_async() { return true; }
    virtual bool do_unload() { return true; }

    /// @brief Links the module, making it ready for use.
    virtual bool do_link() { return true; }
    virtual bool do_unlink() { return true; }

    bool link(); ///< Loads and links this module if needed and returns whether the operation was successful.
    bool unlink(); ///< Unlinks and unloads this module if needed and returns whether the operation was successful.
    bool load_async(); ///< Loads this module asynchronously if needed and returns whether the operation was successful.

    static DynamicModuleControlBase *mFirst; ///< The first module in the linked list of modules.
    static DynamicModuleControlBase *mLast; ///< The last module in the linked list of modules.
};

/// @brief Full implementation of a dynamic module handler.
class DynamicModuleControl : public DynamicModuleControlBase {
public:
    /// @unofficial
    enum ModuleType_e {
        MODULE_TYPE_UNKNOWN,
        MODULE_TYPE_MEM,
        MODULE_TYPE_ARAM,
        MODULE_TYPE_DVD
    };

    DynamicModuleControl(const char *name, EGG::ExpHeap *heap);
    virtual ~DynamicModuleControl();

    virtual const char *getModuleName() const override;
    virtual size_t getModuleSize() const override;
    virtual const char *getModuleTypeString() const override;
    virtual void dump() override;
    virtual bool do_load() override;
    virtual bool do_load_async() override;
    virtual bool do_unload() override;
    virtual bool do_link() override;
    virtual bool do_unlink() override;

    /// @brief Checks the status of the heap (allocatable size and total free size) and keeps track of the lowest values.
    void checkHeapStatus();

    static void initialize(EGG::ExpHeap *heap);
    static void *callback(void *self);

    u8 mPad[4];
    OSModuleHeader *mpModule; ///< The data of the module, including the header.
    void *mpBss; ///< The BSS section of the module.
    u32 mpPrologReturn; ///< The return value of the prolog function of the module.
    const char *mModuleName; ///< The name of the module.
    u8 mType; ///< The type of the module. See ModuleType_e.
    u8 mLoadCount; ///< The number of times this module has been loaded.
    u16 mChecksum; ///< The checksum of the module.
    int mModuleBytes; ///< The size of the module in bytes.
    mDvd_callback_c *mpDvdCallback; ///< The DVD command used to load the module asynchronously.
    EGG::ExpHeap *mpHeap; ///< The heap used for module loading and BSS allocation.
    DbMapFile mDebugMapFile; ///< The debug map file for this module.

    static EGG::ExpHeap *sDylinkHeap; ///< The heap used for dynamic module loading and BSS allocation.
    static EGG::DvdFile *sDvdFile; ///< The DVD file used to load modules.
    static size_t sAllocBytes; ///< The total number of bytes allocated for modules.
    static size_t sHeapMinAllocatableSize; ///< The minimum allocatable size of the heap during module loading.
    static size_t sHeapMinTotalFreeSize; ///< The minimum total free size of the heap during module loading.
    static const char *sModulesDir; ///< The directory on the disk where the modules are located.
};
