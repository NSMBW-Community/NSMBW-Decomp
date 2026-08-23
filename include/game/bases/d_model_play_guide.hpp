#pragma once
#include <game/bases/d_base.hpp>
#include <game/bases/d_lytbase.hpp>

/**
 * @brief The screen overlay for Hint Movies.
 * @ingroup bases
 * @details This is the border that appears around the screen during Hint Movies. It's
 * spawned automatically by the stage scene (dScStage_c) when the game is in the Hint
 * Movie mode.
 */
class dModelPlayGuide_c : public dBase_c {
public:
    dModelPlayGuide_c(); ///< @copydoc fBase_c::fBase_c
    virtual ~dModelPlayGuide_c(); ///< @copydoc fBase_c::~fBase_c
    virtual int create();
    virtual int doDelete();
    virtual int preExecute();
    virtual int execute();
    virtual int draw();

    /// @brief Loads the resources and creates the layout for the base.
    /// @return If the creation was successful.
    bool createLayout();

    LytBase_c mLayout; ///< The layout of the overlay.
    nw4r::lyt::Pane *mpRootPane; ///< The root pane of the layout.
    bool mIsCreated; ///< If the layout has been successfully created.
};
