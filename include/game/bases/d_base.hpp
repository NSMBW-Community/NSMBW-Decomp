#pragma once
#include <game/framework/f_base.hpp>
#include <game/cLib/c_owner_set.hpp>

/// @brief The minimum required implementation for a base.
/// @ingroup bases
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

    /// @brief Sets the callbacks for the scrapped relocatable profile system.
    /// @unused
    static void initLoader();

    /// @copydoc fBase_c::createChild
    static dBase_c *createBase(ProfileName profName, dBase_c *parent, unsigned long param, u8 groupType);

    /// @copydoc fBase_c::createRoot
    static dBase_c *createRoot(ProfileName profName, unsigned long param, u8 groupType);

protected:
    const char* mpKindString; ///< @brief The base's kind string. @unused
    const char* mpNameString; ///< @brief The base's profile name string. @unused

private:
    /// @brief Module loading callback for the scrapped relocatable profile system.
    /// @unofficial
    /// @unused
    /// @return Always returns @ref fBase_c::PACK_RESULT_e::FAILED "FAILED".
    static int loadAsyncCallback();

    /// @brief Module unloading callback for the scrapped relocatable profile system.
    /// @unofficial
    /// @unused
    static void unloadCallback();
};
