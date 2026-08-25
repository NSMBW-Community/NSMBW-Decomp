#pragma once
#include <game/bases/d_base.hpp>
#include <game/bases/d_lytbase.hpp>
#include <game/bases/d_collection_coin_date.hpp>
#include <constants/game_constants.h>

/**
 * @brief The pages for the Star Coins Menu.
 * @ingroup bases
 * @details This is the page that scrolls on the Star Coins Menu, it handles positioning
 * the 12 level entries (dCollectionCoinDate_c) that appear for that world.
 */
class dCollectionCoinBase_c : public dBase_c {
public:

    /// @brief The null panes used in the layout.
    enum N_PANE_e {
        N_coinDateAll_00,
        N_coinBarPos_00,
        N_coinBarPos_01,
        N_coinBarPos_02,
        N_coinBarPos_03,
        N_coinBarPos_04,
        N_coinBarPos_05,
        N_coinBarPos_06,
        N_coinBarPos_07,
        N_coinBarPos_08,
        N_coinBarPos_09,
        N_coinBarPos_10,
        N_coinBarPos_11,
        N_COUNT
    };

    dCollectionCoinBase_c(); ///< @copydoc dBase_c::dBase_c
    virtual ~dCollectionCoinBase_c(); ///< @copydoc dBase_c::~dBase_c

    virtual int create();
    virtual int doDelete();
    virtual int preExecute();
    virtual int execute();
    virtual int draw();

    /// @brief Loads the resources and creates the layout for the base.
    /// @return Whether the creation was successful.
    bool createLayout();

    /// @brief Updates the position of the dates every frame.
    void setDatePos();

    dCollectionCoinDate_c *mpDates[STAR_COIN_MENU_LEVEL_COUNT]; ///< The date bases.

    LytBase_c mLayout; ///< The layout of the page.

    nw4r::lyt::Pane *mpRootPane; ///< The root pane of the layout.
    nw4r::lyt::Pane *mpNullPanes[N_COUNT]; ///< The null panes of the layout.

    bool mIsCreated; ///< Whether the layout has been successfully created.
    bool mIsVisible; ///< Whether the layout is currently visible.
};
