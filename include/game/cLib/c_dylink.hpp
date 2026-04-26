#pragma once

#include <game/mLib/m_dvd.hpp>
#include <lib/egg/core/eggExpHeap.h>
#include <lib/egg/core/eggDvdFile.h>
#include <lib/nw4r/db.h>

/// @brief A debug map file wrapper.
/// @details Wraps nw4r::db::MapFile to provide a simpler interface for managing debug map files.
/// @ingroup clib
class DbMapFile {
public:
    /// @brief Creates the map file.
    DbMapFile() : mpMapFile(nullptr) {}

    /// @brief Destroys the map file.
    /// @note The map file is unregistered automatically on destruction.
    ~DbMapFile();

    /// @brief Loads and registers a map file from disc.
    /// @param path The path to the map file on disc.
    /// @param moduleInfo The module information associated with this map file,
    /// or @p nullptr if the map file belongs to a non-relocatable module.
    void RegisterOnDvd(const char *path, const OSModuleInfo *moduleInfo);

    /// @brief Unregisters the map file.
    /// @note Safe to call multiple times; subsequent calls have no effect.
    void Unregister();

    nw4r::db::MapFile *mpMapFile; ///< A pointer to the internal MapFile instance.
    u8 mBuf[sizeof(nw4r::db::MapFile)]; ///< The raw storage for the MapFile instance.
};

/**
 * @brief Base class for managing a relocatable module.
 *
 * The class tracks how many times a module is being used via reference counting.
 * It automatically handles the physical loading/linking when the first user requests it,
 * and unloads it only when the last user is finished. All active modules are tracked in a
 * global doubly-linked list.
 *
 * Derived classes are expected to override the @p do_* methods to implement
 * module-specific behavior.
 * @ingroup clib
 */
class DynamicModuleControlBase {
protected:
    u16 mUsageCount; ///< The number of active users of this module. When this reaches zero, the module may be unloaded.
    u16 mLinkCount; ///< The number of times this module has been successfully linked.
    DynamicModuleControlBase *mpPrev; ///< The previous module in the linked list.
    DynamicModuleControlBase *mpNext; ///< The next module in the linked list.

public:
    DynamicModuleControlBase(); ///< Constructs a new DynamicModuleControlBase and adds it to the linked list.
    virtual ~DynamicModuleControlBase(); ///< Destroys the DynamicModuleControlBase and removes it from the linked list.

    /// @brief Gets the module name.
    /// @return A string identifying the module, or @p nullptr if unnamed.
    virtual const char *getModuleName() const { return nullptr; }

    /// @brief Gets the module size in bytes.
    /// @return The size of the module, or @p 0 if unknown.
    virtual size_t getModuleSize() const;

    /// @brief Gets a human-readable module type string.
    virtual const char *getModuleTypeString() const { return "Base"; }

    /// @brief Outputs debug information about the module.
    /// @decompnote{This function has been stubbed, but was likely used for debugging purposes during development.}
    virtual void dump();

    /// @brief Module-specific implementation for loading the module into memory.
    /// @return Whether the operation was successful.
    virtual bool do_load() { return true; }

    /// @brief Module-specific implementation for loading the module into memory asynchronously.
    /// @return Whether the operation was successfully started.
    virtual bool do_load_async() { return true; }

    /// @brief Module-specific implementation for unloading the module from memory.
    /// @return Whether the operation was successful.
    virtual bool do_unload() { return true; }

    /// @brief Module-specific implementation for linking the module, making it ready for use.
    /// @return Whether the operation was successful.
    virtual bool do_link() { return true; }

    /// @brief Module-specific implementation for unlinking the module.
    /// @return Whether the operation was successful.
    virtual bool do_unlink() { return true; }

    /// @brief Loads and links the module if needed.
    /// @return Whether the operation was successful.
    bool link();

    /// @brief Unlinks and unloads the module if needed.
    /// @decompnote{This function is not present in the final binary and was reconstructed based on 
    /// surrounding logic and expected behavior.}
    /// @return Whether the operation was successful.
    bool unlink();

    /// @brief Loads the module asynchronously if needed.
    /// @return @p true if the load is finished, @p false if it is still in progress.
    bool load_async();

    static DynamicModuleControlBase *mFirst; ///< The first module in the linked list.
    static DynamicModuleControlBase *mLast; ///< The last module in the linked list.
};

/// @brief Full implementation of a dynamic module handler.
/// @ingroup clib
class DynamicModuleControl : public DynamicModuleControlBase {
public:
    /// @brief Identifies where the module data is physically located.
    /// @unofficial
    enum ModuleType_e {
        MODULE_TYPE_UNKNOWN, ///< The module type has not yet been determined.
        MODULE_TYPE_MEM,     ///< The module is loaded from main RAM. @unused
        MODULE_TYPE_ARAM,    ///< The module is loaded from ARAM. @unused
        MODULE_TYPE_DVD      ///< The module is loaded from the disc.
    };

    /// @brief Constructs a new DynamicModuleControl.
    /// @param name The module name.
    /// @param heap The heap to load the module into, or @p nullptr to use @ref sDylinkHeap "the default heap".
    DynamicModuleControl(const char *name, EGG::ExpHeap *heap);
    virtual ~DynamicModuleControl(); ///< Destroys the DynamicModuleControl.

    virtual const char *getModuleName() const override;
    virtual size_t getModuleSize() const override;
    virtual const char *getModuleTypeString() const override;
    virtual void dump() override;
    virtual bool do_load() override;
    virtual bool do_load_async() override;
    virtual bool do_unload() override;
    virtual bool do_link() override;
    virtual bool do_unlink() override;

    /// @brief Updates the @ref sDylinkHeap "module linking heap" statistics.
    /// @details The function tracks the @ref sHeapMinAllocatableSize "minimum allocatable space" and
    /// @ref sHeapMinTotalFreeSize "minimum free space" size during module operations, probably to aid in debugging.
    void checkHeapStatus();

    /// @brief Initializes the global dynamic linking system.
    /// @param heap The heap where all modules will be allocated by default.
    static void initialize(EGG::ExpHeap *heap);

    /// @brief Internal callback used for asynchronous loading operations.
    /// @details The return type is @p void* to satisfy the type required by mDvd_callback_c.
    /// @param self Pointer to the DynamicModuleControl instance.
    /// @return The result of do_load().
    static void *callback(void *self);

    u8 mPad[4];
    OSModuleHeader *mpModule; ///< The data of the module, including the header.
    void *mpBss; ///< The BSS section of the module.
    u32 mPrologReturn; ///< The return value of the module's prolog function.
    const char *mModuleName; ///< The module name.
    u8 mType; ///< The module type. See ModuleType_e.
    u8 mLoadCount; ///< The number of times this module has been loaded.
    u16 mChecksum; ///< The checksum of the module.
    int mModuleSize; ///< The module size, in bytes.
    mDvd_callback_c *mpDvdCallback; ///< The DVD command used to load the module asynchronously.
    EGG::ExpHeap *mpHeap; ///< The heap used for module loading and BSS allocation.
    DbMapFile mDebugMapFile; ///< The debug map file for this module.

    static EGG::ExpHeap *sDylinkHeap; ///< The heap used for dynamic module loading and BSS allocation.
    static EGG::DvdFile *sDvdFile; ///< The DVD file handle used for loading the string table file.
    static size_t sAllocBytes; ///< The total number of bytes allocated across all active modules.
    static size_t sHeapMinAllocatableSize;///< The lowest observed allocatable space in #sDylinkHeap during module loading.
    static size_t sHeapMinTotalFreeSize; ///< The lowest observed free space in #sDylinkHeap during module loading.
    static const char *sModulesDir; ///< The directory on the disk where the modules are located.
};
