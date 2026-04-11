#pragma once

#include <game/bases/d_base.hpp>
#include <game/bases/d_lytbase.hpp>

class dWiiStrapScreen_c : public dBase_c {
public:
    /// @brief The animation names used in the layout.
    /// @unofficial
    enum ANIM_NAME_e {
        roop,
        ANIM_NAME_COUNT
    };

    /// @brief The animations used for the layout.
    /// @unofficial
    enum ANIM_e {
        ANIM_STRAP,
        ANIM_COUNT
    };

    dWiiStrapScreen_c();
    virtual ~dWiiStrapScreen_c();

    bool createLayout();

    virtual int create();
    virtual int preExecute();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

    LytBase_c mLayout;
    bool mHasLoadedLayout;
    bool mVisible;
};
