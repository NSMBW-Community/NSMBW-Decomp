#pragma once

#include <game/bases/d_base.hpp>
#include <game/bases/d_lytbase.hpp>

/// @brief Displays the Wii Strap safety warning screen.
/// @details Loops between the two images that show the safety warning and how to put on the strap.
/// @note The button press to advance from this screen is handled by dScBoot_c.
/// @ingroup bases
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

    virtual int create();
    virtual int preExecute();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

    bool createLayout();

    LytBase_c mLayout; ///< The layout for the screen.

    bool mHasLoadedLayout; ///< Whether the layout has been loaded.
    bool mVisible; ///< Whether the screen is currently visible.
};
