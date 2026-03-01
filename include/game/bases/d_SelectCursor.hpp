#pragma once
#include <types.h>
#include <game/bases/d_base.hpp>
#include <game/bases/d_lytbase.hpp>
#include <game/bases/d_2d/multi.hpp>

/// @brief Manages the animated L-shaped corner rectangles around selected buttons.
/// @details Owns and updates multiple @ref dSelectCursor_c::Layout_c "cursor layout instances"
/// that can be attached to target @ref nw4r::lyt::Pane "panes".
/// @todo Add the draw order values to the dedicated enum.
/// @ingroup bases
class dSelectCursor_c : public dBase_c {
public:

    /// @brief The possible cursor slots.
    /// @todo Figure out the behaviour differences between slots and give each one a name.
    /// @unofficial
    enum LAYOUT_e {
        UNK_0,
        UNK_1,
        UNK_2,
        UNK_3,
        UNK_4,
        LAYOUT_COUNT
    };

    /// @brief The null panes used in the layout.
    /// @unofficial
    enum N_PANE_e {
        N_cursor_00,
        N_LU_00,
        N_RU_00,
        N_LD_00,
        N_RD_00,
        N_COUNT
    };

    /// @brief The picture panes used in the layout.
    /// @unofficial
    enum P_PANE_e {
        P_cursor_00,
        P_cursor_01,
        P_cursor_02,
        P_cursor_03,
        P_COUNT
    };

    /// @brief The animations used in the layout.
    /// @unofficial
    enum ANIM_e {
        ANIM_CURSOR,
        ANIM_COUNT
    };

    /// @brief A cursor layout instance.
    class Layout_c {
    public:
        LytBase_c mBase; ///< The layout of the instance.
        nw4r::lyt::Pane *mpRootPane; ///< The root pane of the view.
        nw4r::lyt::Picture *mpPicturePanes[P_COUNT]; ///< The picture panes of the view.
        nw4r::lyt::Pane *mpNullPanes[N_COUNT]; ///< The null panes of the view.
        bool mIsActive; ///< Whether the layout is drawn.

        char mPaneName[100]; ///< The name of the parent pane.
        nw4r::lyt::Size mPaneSize; ///< The size of the parent pane.
        nw4r::lyt::Size mPaneOffset; ///< The offset of the parent pane.
        mVec2_c mPaneGlbMtxScale; ///< The scale of the parent pane's global matrix.
        mVec2_c mPaneGlbMtxTrans; ///< The translation of the parent pane's global matrix.

        u32 m_248; ///< @unused
        mVec2_c mRootPaneOffset; ///< Additional offset from the layout root pane. @unused
        float m_254; ///< Only set to 0, never read. @unused

        u8 mPaneBasePosH; ///< The horizontal base position of the parent pane. Value is a nw4r::lyt::HorizontalPosition.
        u8 mPaneBasePosV; ///< The vertical base position of the parent pane. Value is a nw4r::lyt::VerticalPosition.
        int mPaneAlpha; ///< The opacity of the layout.
        bool mDoFade; ///< Whether the layout should be faded out and deactivated.
    };

    dSelectCursor_c(); ///< @copydoc dBase_c::dBase_c
    virtual ~dSelectCursor_c(); ///< @copydoc dBase_c::~dBase_c

    int create();
    int doDelete();
    int execute();
    int draw();

    /// @brief Updates the position of an attached cursor layout.
    /// @param layoutId The cursor slot index.
    void PosSet(int layoutId);

    /// @brief Deactivates and hides the specified cursor layout.
    /// @param layoutId The cursor slot index.
    void Cancel(int layoutId);

    /// @brief Attaches a cursor to the given pane.
    /// @param pane The pane to attach the cursor to.
    /// @param layoutId The cursor slot index.
    /// @param forceTopDrawOrder If @p true , sets this cursor's draw order to a higher priority value.
    /// If @p false , it restores the default draw order for all cursors.
    void SetPane(const nw4r::lyt::Pane *pane, int layoutId, bool forceTopDrawOrder);

    /// @brief Sets the alpha of a cursor to match the given pane.
    /// @param pane The source pane whose alpha value will be copied.
    /// @param layoutId The cursor slot index.
    void SetAlpha(const nw4r::lyt::Pane *pane, int layoutId);

    d2d::ResAccMultLoader_c mResLoader; ///< The resource loader for the layout.
    Layout_c mLayouts[LAYOUT_COUNT]; ///< The cursor layout instances.
    bool mHasLoadedLayout; ///< Whether the layout has been loaded.

    static dSelectCursor_c *m_instance; ///< The static instance of the selection cursor manager.
};
