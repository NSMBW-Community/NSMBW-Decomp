#pragma once

#include <game/framework/f_profile.hpp>
#include <game/cLib/c_dylink.hpp>
#include <lib/egg/core/eggExpHeap.h>

/// @brief Pretty much the same as DynamicModuleControl.
class dDynamicModuleControl : public DynamicModuleControl {
public:
    dDynamicModuleControl(const char *name, EGG::ExpHeap *heap);
    virtual ~dDynamicModuleControl();

    virtual bool do_link() override;
};

namespace DynamicModuleCallback {
    void *InitCallback(void *self);
}

/// @brief An entry in the @ref dDyl::pDMC "dynamic module name table".
struct sDynNameTableEntry {
    ProfileName mProf; ///< The profile name of this entry.
    const char *mModuleName; ///< The name of the module to load for the profile.
};

namespace dDyl {
    bool Init(int profileCount, const sDynNameTableEntry *pNameTable, int nNameTable, EGG::Heap *heap);
    void InitAsync();
    bool InitAsyncIsDone();
    int LinkASync(ProfileName);
    bool Unlink(ProfileName);

    /// @brief This table keeps track of which profile is associated with which module.
    extern DynamicModuleControlBase **pDMC;
    extern int nDMC; ///< @brief The number of entries in #pDMC.
    extern EGG::FrmHeap *cCc_frmHeap;
    extern BOOL Initialized;
    extern mDvd_callback_c *DVD;
};
