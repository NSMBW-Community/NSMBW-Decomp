#pragma once
#include <types.h>
#include <game/bases/d_base.hpp>
#include <game/bases/d_lytbase.hpp>
#include <game/bases/d_2d/multi.hpp>

/// @brief The world map HUD.
class dSelectCursor_c : public dBase_c {

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

    /// @brief The animations used for the layout.
    /// @unofficial
    enum ANIM_e {
        ANIM_CURSOR,
        ANIM_COUNT
    };

public:
    class Layout_c {
        public:
        LytBase_c mBase;
        nw4r::lyt::Pane *mpRootPane; ///< The root pane of the view.
        nw4r::lyt::Picture *mpPicturePanes[P_COUNT]; ///< The picture panes of the view.
        nw4r::lyt::Pane *mpNullPanes[N_COUNT]; ///< The null panes of the view.
        bool mIsActive; ///< Whether the layout is drawn.
        char mPaneName[100]; ///< The name of the parent pane.
        nw4r::lyt::Size mPaneSize; ///< The size of the parent pane.
        nw4r::lyt::Size mPaneOffset; ///< Always (0, 0).
        mVec2_c mPaneGlbMtxScale; ///< The scale of the parent pane's global matrix.
        mVec2_c mPaneGlbMtxTrans; ///< The transform of the parent pane's global matrix.
        u8 mPad[4];
        mVec2_c mRootPaneOffset; ///< Always (0, 0).
        float m_254; ///< Only set to 0, never read.
        u8 mPaneBasePosH;
        u8 mPaneBasePosV;
        int mPaneAlpha; ///< The opacity of the N_cursor_00 pane.
        bool mDoFade; ///< Always false. If set, it fades out the opacity of the N_cursor_00 pane then deactivates.
    };

    dSelectCursor_c();
    virtual ~dSelectCursor_c();

    int create();
    int doDelete();
    int execute();
    int draw();

    void PosSet(int layoutId);
    void Cancel(int layoutId);
    void SetPane(const nw4r::lyt::Pane *pane, int layoutId, bool dontSetAllDrawOrder);
    void SetAlpha(const nw4r::lyt::Pane *pane, int layoutId);

    d2d::ResAccMultLoader_c mResLoader;
    Layout_c mLayouts[5];
    bool mIsLoaded;

    static dSelectCursor_c *m_instance;
};
