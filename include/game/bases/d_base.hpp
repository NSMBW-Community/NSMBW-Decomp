#pragma once
#include <game/framework/f_base.hpp>
#include <game/cLib/c_owner_set.hpp>

/**
 * @brief The minimum required implementation for a base.
 * @ingroup bases
 * @details
 * ## Overview
 * From a technical standpoint, dBase_c does not add any particularly relevant components, which is useful
 * for bases that don't need any additional functionality. A few overloaded functions from the fManager_c
 * search API (::searchBaseByProfName) and the fBase_c base creation API (::createBase, ::createRoot) are
 * provided for convenience.
 *
 * ## Unused Content
 * ### Base Linking
 * The @ref cOwnerSetMg_c "inherited owner set" can be used by bases to link themselves to a specific base
 * (called the owner base) anywhere in the @ref fManager_c::m_connectManage "connect tree", avoiding the
 * parent/child relationship requirement. The owner base can then iterate through the set and interact with
 * the linked bases. When the owner base is deleted, all the linked bases are unlinked automatically.
 *
 * Several bases use this method to link themselves to dBgGm_c, which however does not use the set at all,
 * leaving the feature unused.
 *
 * ### Debug Strings
 * Bases store a pointer to a @ref mpKindString "kind string" (which roughly describes the
 * @ref fBase_c::GROUP_TYPE_e "group type") and to a @ref mpNameString "profile name string". The former
 * can be obtained by calling ::getKindString, while the latter can be obtained by calling ::dProf_getName
 * with the profile name.
 *
 * The ::preExecute function gets the kind string every frame and discards the result immediately.
 *
 * ### Random Seed
 * The global variable ::g_basesRandomSeed stores the current random seed. Every base updates it every
 * frame when its ::preDraw method is called, but no other code accesses it.
 *
 * ### Callbacks
 * The ::initLoader function sets two unused callbacks related to the scrapped relocatable profile system.
 * See fBase_c for more details.
 */
class dBase_c : public fBase_c, public cOwnerSetMg_c {
public:
    /// @copydoc fBase_c::fBase_c
    dBase_c();

    virtual int preCreate();
    virtual void postCreate(fBase_c::MAIN_STATE_e status);

    virtual int preDelete();
    virtual void postDelete(fBase_c::MAIN_STATE_e status);

    virtual int preExecute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);

    virtual int preDraw();
    virtual void postDraw(fBase_c::MAIN_STATE_e status);

    /// @copydoc fBase_c::~fBase_c
    virtual ~dBase_c();

    /// @brief Gets the base's kind string.
    virtual const char *getKindString() const;

    /// @copydoc fManager_c::searchBaseByProfName
    static dBase_c *searchBaseByProfName(ProfileName profile, const dBase_c* parent);

    /// @unused Sets the callbacks for the scrapped relocatable profile system.
    static void initLoader();

    /// @copydoc fBase_c::createChild
    static dBase_c *createBase(ProfileName profName, dBase_c *parent, unsigned long param, u8 groupType);

    /// @copydoc fBase_c::createRoot
    static dBase_c *createRoot(ProfileName profName, unsigned long param, u8 groupType);

protected:
    const char* mpKindString; ///< @unused The base's kind string.
    const char* mpNameString; ///< @unused The base's profile name string.

private:
    /// @unused Module loading callback for the scrapped relocatable profile system.
    /// @unofficial
    /// @return Always returns @ref fBase_c::PACK_RESULT_e::FAILED "FAILED".
    static int loadAsyncCallback();

    /// @unused Module unloading callback for the scrapped relocatable profile system.
    /// @unofficial
    static void unloadCallback();
};
