#pragma once
#include <types.h>
#include <game/bases/d_base.hpp>
#include <game/bases/d_lytbase.hpp>
#include <game/bases/d_2d/multi.hpp>

class dSelectCursor_c : public dBase_c {
public:
    class Layout_c {
        public:
        LytBase_c mBase;
        nw4r::lyt::Pane * mpRootPane;
        nw4r::lyt::Picture * mPPanes[4];
        nw4r::lyt::Pane * mNPanes[5];
        bool mIsActive;
        char mPaneName[NW4R_LYT_RES_NAME_LEN + 1];
        u8 mPad[86];
        nw4r::lyt::Size mPaneSize;
        nw4r::lyt::Size mPaneOffset; ///< Always (0, 0)
        mVec2_c mPaneGlbMtxScale;
        mVec2_c mPaneGlbMtxTrans;
        u8 mPad2[4];
        mVec2_c mRootPaneOffset; ///< Always (0, 0)
        float m_254; ///< Only set to 0, never read
        u8 mPaneBasePosH;
        u8 mPaneBasePosV;
        int mPaneAlpha;
        bool mDoFade; ///< Always false
    };

    d2d::ResAccMultLoader_c mResLoader;
    Layout_c mLayouts[5];
    bool mIsLoaded;

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

    static dSelectCursor_c * m_instance;
};
