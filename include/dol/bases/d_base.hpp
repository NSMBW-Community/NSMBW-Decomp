#pragma once
#include <dol/framework/f_base.hpp>
#include <dol/cLib/c_owner_set.hpp>

/// @brief An extension of fBase_c with base kind and name strings.
/// @details All bases should inherit from this class, not from fBase_c.
class dBase_c : public fBase_c, public cOwnerSetMg_c {
    public:
        /// @copydoc fBase_c::fBase_c
        dBase_c();
        
        virtual int preCreate(); ///< @copydoc fBase_c::preCreate
        virtual void postCreate(fBase_c::MAIN_STATE_e status); ///< @copydoc fBase_c::postCreate

        virtual int preDelete(); ///< @copydoc fBase_c::preDelete
        virtual void postDelete(fBase_c::MAIN_STATE_e status); ///< @copydoc fBase_c::postDelete

        virtual int preExecute(); ///< @copydoc fBase_c::preExecute
        virtual void postExecute(fBase_c::MAIN_STATE_e status); ///< @copydoc fBase_c::postExecute

        virtual int preDraw(); ///< @copydoc fBase_c::preDraw
        virtual void postDraw(fBase_c::MAIN_STATE_e status); ///< @copydoc fBase_c::postDraw

        virtual ~dBase_c(); ///< @copydoc fBase_c::~fBase_c
        
        /// @brief Gets a string describing the kind of this base.
        virtual const char *getKindString() const;

        /// @copydoc fManager_c::searchBaseByProfName
        static dBase_c *searchBaseByProfName(ProfileName profile, const dBase_c* parent);

        static void initLoader(); ///< [Unknown purpose. Callbacks do nothing and don't seem to ever be called].

        /// @copydoc fBase_c::createChild
        static dBase_c *createBase(ProfileName profName, dBase_c *parent, unsigned long param, u8 groupType);
        /// @copydoc fBase_c::createRoot
        static dBase_c *createRoot(ProfileName profName, unsigned long param, u8 groupType);

private:
        const char* mpKindString; ///< A string describing the kind of this base. [Never read from.]
        const char* mpNameString; ///< A string representing the name of the profile this base belongs to. [Never read from.]

        static int loadAsyncCallback(); ///< @note Unofficial name.
        static void unloadCallback(); ///< @note Unofficial name.
};

/// @brief Random seed written to after dBase_c::preDraw.
/// @details [Unused. Not part of the dBase_c class as its mangled hash is the same as its demangled hash].
/// @note Unofficial name.
static u32 g_basesRandomSeed;
