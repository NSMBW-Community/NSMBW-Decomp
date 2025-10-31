#include <game/bases/d_SelectCursor.hpp>

ACTOR_PROFILE(SELECT_CURSOR, dSelectCursor_c, 0);

dSelectCursor_c *dSelectCursor_c::m_instance = nullptr;

dSelectCursor_c::dSelectCursor_c() {
    m_instance = this;
    m_d38 = 0;
}

dSelectCursor_c::Layout_c::Layout_c() : mPaneSize(0.0f, 0.0f), m_230(0.0f), m_234(0.0f) { }

dSelectCursor_c::~dSelectCursor_c() {
    dSelectCursor_c::m_instance = nullptr;
}

int dSelectCursor_c::create() {

    static const char *AnmNameTbl[] = {
        "select_cursor_04_loopCursor.brlan"
    };

    static const char *GROUP_NAME_DT[] = {
        "A00_cursor"
    };

    static const int ANIME_INDEX_TBL[ARRAY_SIZE(GROUP_NAME_DT)] = { 0 };

    static const char *PPANE_TABLE[] = {
        "P_cursor_00",
        "P_cursor_01",
        "P_cursor_02",
        "P_cursor_03"
    };

    static const char *NPANE_TABLE[] = {
        "N_cursor_00",
        "N_LU_00",
        "N_RU_00",
        "N_LD_00",
        "N_RD_00"
    };


    if (m_d38) {
        return SUCCEEDED;
    }

    if (! mResLoader.request("Layout/select_cursor/select_cursor.arc")) {
        return NOT_READY;
    }

    for (int i = 0; i < (int) ARRAY_SIZE(mLayouts); i++) {
        mLayouts[i].mBase.mpResAccessor = &mResLoader;
    }

    for (int i = 0; i < (int) ARRAY_SIZE(mLayouts); i++) {
        mLayouts[i].mBase.build("select_cursor_04.brlyt", nullptr);
        mLayouts[i].mBase.AnimeResRegister(AnmNameTbl, ARRAY_SIZE(AnmNameTbl));
        mLayouts[i].mBase.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, ARRAY_SIZE(GROUP_NAME_DT));
        mLayouts[i].mpRootPane = mLayouts[i].mBase.getRootPane();
        mLayouts[i].mBase.PPaneRegister(PPANE_TABLE, mLayouts[i].mPPanes, ARRAY_SIZE(PPANE_TABLE));
        mLayouts[i].mBase.NPaneRegister(NPANE_TABLE, mLayouts[i].mNPanes, ARRAY_SIZE(NPANE_TABLE));
        mLayouts[i].mpRootPane->SetVisible(false);

        Cancel(i);

        if (i == 4) {
            mLayouts[i].mBase.mDrawOrder = 0xE;
        } else {
            mLayouts[i].mBase.mDrawOrder = 0x93;
        }

        mLayouts[i].mBase.LoopAnimeStartSetup(0);
    }

    m_d38 = 1;

    return SUCCEEDED;
}


int dSelectCursor_c::execute() {
    for (int i = 0; i < (int) ARRAY_SIZE(mLayouts); i++) {
        if (mLayouts[i].mIsActive) {
            PosSet(i);
            mLayouts[i].mBase.AnimePlay();
            mLayouts[i].mBase.calc();
        }
    }

    return SUCCEEDED;
}

int dSelectCursor_c::draw() {
    for (int i = 0; i < (int) ARRAY_SIZE(mLayouts); i++) {
        if (mLayouts[i].mIsActive) {
            mLayouts[i].mBase.entry();
        }
    }

    return SUCCEEDED;
}

int dSelectCursor_c::doDelete() {
    if (! mResLoader.remove()) {
        return NOT_READY;
    }

    for (int i = 0; i < (int) ARRAY_SIZE(mLayouts); i++) {
        if (! mLayouts[i].mBase.doDelete()) {
            return NOT_READY;
        }
    }

    return SUCCEEDED;
}

void dSelectCursor_c::PosSet(int param_2) {
    mLayouts[param_2].mpRootPane->SetVisible(true);

    mVec2_c pos;

    float b, c, d, e;

    d = mLayouts[param_2].mPaneGlbMtxScale.x;
    e = mLayouts[param_2].mPaneGlbMtxScale.y;

    b = mLayouts[param_2].mPaneSize.x / 2.0f;
    c = mLayouts[param_2].mPaneSize.y / 2.0f;

    pos.x = mLayouts[param_2].mPaneGlbMtxTrans.x + mLayouts[param_2].m_24c;

    if (mLayouts[param_2].mPaneBasePosH == 0) {
        pos.x += b * d;
    } else if (mLayouts[param_2].mPaneBasePosH == 2) {
        pos.x -= b * d;
    }

    pos.y = mLayouts[param_2].mPaneGlbMtxTrans.y + mLayouts[param_2].m_250;
    if (mLayouts[param_2].mPaneBasePosV == 0) {
        pos.y -= c * e;
    } else if (mLayouts[param_2].mPaneBasePosV == 2) {
        pos.y += c * e;
    }

    mLayouts[param_2].mpRootPane->SetTranslate(mVec3_c(pos, 0.0f));

    float m_230 = mLayouts[param_2].m_230;
    float m_234 = mLayouts[param_2].m_234;
    for (int i = 1; i < ARRAY_SIZE(mLayouts); i++) {
        mVec3_c pos2;
        if ((i == 1) || (i == 3)) {
            pos2.x = (b * d);
            pos2.x = -pos2.x;
            pos2.x -= m_230;
        } else {
            pos2.x = (b * d);
            pos2.x += m_230;
        }

        if ((i == 1) || (i == 2)) {
            pos2.y = (c * e);
            pos2.y += m_234;
        } else {
            pos2.y = (c * e);
            pos2.y = -pos2.y;
            pos2.y -= m_234;
        }

        pos2.z = 0.0f;
        mLayouts[param_2].mNPanes[i]->SetTranslate(pos2);
    }

    if (mLayouts[param_2].m_260 != 0) {
        mLayouts[param_2].m_25c -= 26;
        mLayouts[param_2].mNPanes[0]->SetAlpha(mLayouts[param_2].m_25c);

        if (mLayouts[param_2].m_25c < 0) {
            mLayouts[param_2].m_25c = 0;
            mLayouts[param_2].m_260 = 0;
            Cancel(param_2);
        }
    }
}

void dSelectCursor_c::Cancel(int param_2) {
    if (mLayouts[param_2].mIsActive) {
        mLayouts[param_2].mIsActive = false;
        strcpy(mLayouts[param_2].mPaneName, "");
        mLayouts[param_2].mpRootPane->SetVisible(false);
    }
}

void dSelectCursor_c::SetPane(const nw4r::lyt::Pane *param_2, int param_3, bool param_4) {
    mLayouts[param_3].mIsActive = true;

    strcpy(mLayouts[param_3].mPaneName, param_2->GetName());

    mLayouts[param_3].mPaneSize.x = param_2->GetSize().width;
    mLayouts[param_3].mPaneSize.y = param_2->GetSize().height;

    nw4r::math::MTX34 mtx = param_2->GetGlobalMtx();

    mLayouts[param_3].mPaneGlbMtxTrans.x = mtx._03;
    mLayouts[param_3].mPaneGlbMtxTrans.y = mtx._13;
    mLayouts[param_3].mPaneGlbMtxScale.x = mtx._00;
    mLayouts[param_3].mPaneGlbMtxScale.y = mtx._11;

    mLayouts[param_3].mPaneBasePosH = param_2->GetBasePositionH();
    mLayouts[param_3].mPaneBasePosV = param_2->GetBasePositionV();

    mLayouts[param_3].m_230 = 0.0;
    mLayouts[param_3].m_234 = 0.0;
    mLayouts[param_3].m_24c = 0.0;
    mLayouts[param_3].m_250 = 0.0;
    mLayouts[param_3].m_254 = 0.0;
    mLayouts[param_3].m_25c = 0xff;
    mLayouts[param_3].m_260 = 0;

    if (param_4) {
        mLayouts[param_3].mBase.mDrawOrder = 152;
    } else {
        for (int i = 0; i < ARRAY_SIZE(mLayouts); i++) {
            if (i == 4) {
                mLayouts[i].mBase.mDrawOrder = 14;
            } else {
                mLayouts[i].mBase.mDrawOrder = 147;
            }
        }
    }
}

void dSelectCursor_c::SetAlpha(const nw4r::lyt::Pane *param_2, int param_3) {
    u8 alpha = param_2->GetGlbAlpha();
    Layout_c & layout = mLayouts[param_3];

    layout.mPPanes[0]->SetAlpha(alpha);
    layout.mPPanes[1]->SetAlpha(alpha);
    layout.mPPanes[2]->SetAlpha(alpha);
    layout.mPPanes[3]->SetAlpha(alpha);
}
