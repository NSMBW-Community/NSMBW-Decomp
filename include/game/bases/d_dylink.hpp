#pragma once

#include <game/framework/f_profile.hpp>
#include <game/cLib/c_dylink.hpp>
#include <lib/egg/core/eggExpHeap.h>

/// @copydoc DynamicModuleControl
/// @ingroup bases
class dDynamicModuleControl : public DynamicModuleControl {
public:
    dDynamicModuleControl(const char *name, EGG::ExpHeap *heap); ///< @copydoc DynamicModuleControl::DynamicModuleControl
    virtual ~dDynamicModuleControl(); ///< @copydoc DynamicModuleControl::~DynamicModuleControl

    virtual bool do_link() override;
};

namespace DynamicModuleCallback {
    /**
    * @brief Second initialization step of the dynamic module system.
    * @details The function performs the following remaining tasks:
    * - Initializes the underlying dynamic module linking system.
    * - Loads the string table file from disc.
    * - Links the @ref s_ProfileDMC "d_profile module".
    * @ingroup bases
    * @note If the core module fails to link, the function enters an infinite loop.
    * @note The return type is @p void* to satisfy the type required by mDvd_callback_c.
    * @param self Unused (heap pointer passed through callback system).
    * @return Always returns @p true .
    */
    void *InitCallback(void *self);
}

/// @brief An entry in the @ref DynamicNameTable "profile to module name table".
/// @ingroup bases
/// @unofficial
struct sDynNameTableEntry {
    ProfileName mProf; ///< The profile name of this entry.
    const char *mModuleName; ///< The name of the module to load for the profile, or @p nullptr if the profile is statically linked.
};

/// @brief Serves as the bridge between the game's profile system and the dynamic module linking system.
/// @ingroup bases
namespace dDyl {

    /**
    * @brief First initialization step of the dynamic module system.
    * @details Creates the necessary data structures and maps the profiles to the
    * corresponding modules using the provided name table.
    * @unofficial
    * @param profileCount The total number of profiles.
    * @param pNameTable The profile to module name table.
    * @param nNameTable The number of entries in @p pNameTable.
    * @param heap The heap to be used for allocations.
    * @return Always returns @p true .
    */
    bool Init(int profileCount, const sDynNameTableEntry *pNameTable, int nNameTable, EGG::Heap *heap);

    /**
    * @brief Initializes the dynamic module system.
    *
    * The initialization occurs in two steps:
    * - #Init, which is run synchronously.
    * - DynamicModuleCallback::InitCallback, which is run asynchronously.
    */
    void InitAsync();

    /// @brief Checks whether initialization has completed.
    /// @return @p true if initialization is complete, else @p false .
    bool InitAsyncIsDone();

    /**
    * @brief Links the module associated with the given profile.
    * @decompnote{The original implementation has been stripped from the final
    * binary. The reconstructed code has been added inside a never-executing block.}
    * @param profile The profile whose module needs to be linked.
    * @return Non-zero on success, else zero.
    */
    int LinkASync(ProfileName profile);

    /**
    * @brief Unlinks the module associated with the given profile.
    * @decompnote{The original implementation has been stripped from the final
    * binary. The reconstructed code has been added inside a never-executing block.}
    * @param profile The profile whose module needs to be unlinked.
    * @return @p true on success, else @p false .
    */
    bool Unlink(ProfileName profile);

    extern DynamicModuleControlBase **pDMC; ///< A table that keeps track of which module is associated with each profile.
    extern int nDMC; ///< The number of entries in #pDMC.

    /// @brief The heap to be used for allocating the @ref DynamicModuleControlBase "module handlers"
    /// and the @ref pDMC "profile to module table".
    extern EGG::FrmHeap *cCc_frmHeap;
    extern BOOL Initialized; ///< Whether the dynamic module system has finished initializing.
    extern mDvd_callback_c *DVD; ///< The asynchronous DVD task used during the dynamic module system initialization.
};
