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
        mVec2_c mPaneSize;
        float m_230 ;
        float m_234 ;
        mVec2_c mPaneGlbMtxScale;
        mVec2_c mPaneGlbMtxTrans;
        u8 mPad2[4];
        float m_24c;
        float m_250;
        float m_254;
        u8 mPaneBasePosH;
        u8 mPaneBasePosV;
        u8 mPad3[2];
        int m_25c;
        u8 m_260;
        u8 mPad4[3];

        Layout_c();
    };

    d2d::ResAccMultLoader_c mResLoader;
    Layout_c mLayouts[5];
    u8 m_d38;
    u8 m_d39;
    u8 m_d3a;
    u8 m_d3b;

    dSelectCursor_c();
    virtual ~dSelectCursor_c();

    int create();
    int doDelete();
    int execute();
    int draw();

    void Cancel(int);
    void PosSet(int);
    void SetPane(const nw4r::lyt::Pane *param_2, int param_3, bool param_4);
    void SetAlpha(const nw4r::lyt::Pane *param_2, int param_3);

    static dSelectCursor_c * m_instance; // 8042a5a8
};
