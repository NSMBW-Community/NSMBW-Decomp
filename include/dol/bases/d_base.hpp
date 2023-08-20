#pragma once
#include <dol/framework/f_base.hpp>
#include <dol/cLib/c_owner_set.hpp>
/// @file

/// @brief An extension of fBase_c with base kind and name strings.
/// @details All bases should inherit from this class, not from fBase_c.
/// @ingroup bases
class dBase_c : public fBase_c, public cOwnerSetMg_c {
public:

    dBase_c();

    virtual int preCreate(); ///< See fBase_c::preCreate.
    virtual void postCreate(fBase_c::MAIN_STATE_e status); ///< See fBase_c::postCreate.

    virtual int preDelete(); ///< See fBase_c::preDelete.
    virtual void postDelete(fBase_c::MAIN_STATE_e status); ///< See fBase_c::postDelete.

    /// @brief Code to be executed before ::execute.
    /// @details [The function obtains the kind string for no reason, before passing execution
    /// to the base class].
    virtual int preExecute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status); ///< See fBase_c::postExecute.

    /// @brief Code to be executed before ::draw.
    /// @details [The function calculates ::g_basesRandomSeed for no reason before passing
    /// execution to the base class].
    virtual int preDraw();
    virtual void postDraw(fBase_c::MAIN_STATE_e status); ///< See fBase_c::postDraw.

    virtual ~dBase_c();

    /// @brief Gets a string describing the kind of this base.
    virtual const char *getKindString() const;

    /// @copydoc fManager_c::searchBaseByProfName
    static dBase_c *searchBaseByProfName(ProfileName profile, const dBase_c* parent);

    static void initLoader(); ///< [Unknown purpose. Callbacks do nothing and don't seem to ever be called].

    /// @brief See fBase_c::createChild.
    static dBase_c *createBase(ProfileName profName, dBase_c *parent, unsigned long param, u8 groupType);
    /// @brief See fBase_c::createRoot.
    static dBase_c *createRoot(ProfileName profName, unsigned long param, u8 groupType);

private:
    const char* mpKindString; ///< @unused A string describing the kind of this base.
    const char* mpNameString; ///< @unused A string representing the name of the profile this base belongs to.

    static int loadAsyncCallback(); ///< @unused [Does nothing]. @unofficial
    static void unloadCallback(); ///< @unused [Does nothing]. @unofficial
};
