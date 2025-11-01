#include <game/bases/d_SelectCursor.hpp>

ACTOR_PROFILE(SELECT_CURSOR, dSelectCursor_c, 0);

dSelectCursor_c *dSelectCursor_c::m_instance = nullptr;

dSelectCursor_c::dSelectCursor_c() {
    m_instance = this;
    mIsLoaded = false;
}

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


    if (mIsLoaded) {
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
            mLayouts[i].mBase.mDrawOrder = 14;
        } else {
            mLayouts[i].mBase.mDrawOrder = 147;
        }

        mLayouts[i].mBase.LoopAnimeStartSetup(0);
    }

    mIsLoaded = true;

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

void dSelectCursor_c::PosSet(int layoutId) {
    mLayouts[layoutId].mpRootPane->SetVisible(true);

    mVec2_c pos;
    float paneMidX, paneMidY, paneScaleX, paneScaleY;

    paneScaleX = mLayouts[layoutId].mPaneGlbMtxScale.x;
    paneScaleY = mLayouts[layoutId].mPaneGlbMtxScale.y;

    paneMidX = mLayouts[layoutId].mPaneSize.width / 2.0f;
    paneMidY = mLayouts[layoutId].mPaneSize.height / 2.0f;

    pos.x = mLayouts[layoutId].mPaneGlbMtxTrans.x + mLayouts[layoutId].mRootPaneOffset.x;

    if (mLayouts[layoutId].mPaneBasePosH == 0) {
        pos.x += paneMidX * paneScaleX;
    } else if (mLayouts[layoutId].mPaneBasePosH == 2) {
        pos.x -= paneMidX * paneScaleX;
    }

    pos.y = mLayouts[layoutId].mPaneGlbMtxTrans.y + mLayouts[layoutId].mRootPaneOffset.y;
    if (mLayouts[layoutId].mPaneBasePosV == 0) {
        pos.y -= paneMidY * paneScaleY;
    } else if (mLayouts[layoutId].mPaneBasePosV == 2) {
        pos.y += paneMidY * paneScaleY;
    }

    mLayouts[layoutId].mpRootPane->SetTranslate(mVec3_c(pos, 0.0f));

    float hOffset = mLayouts[layoutId].mPaneOffset.width;
    float vOffset = mLayouts[layoutId].mPaneOffset.height;
    for (int i = 1; i < ARRAY_SIZE(mLayouts); i++) {
        mVec3_c pos2;
        if ((i == 1) || (i == 3)) {
            pos2.x = paneMidX * paneScaleX;
            pos2.x = -pos2.x;
            pos2.x -= hOffset;
        } else {
            pos2.x = paneMidX * paneScaleX;
            pos2.x += hOffset;
        }

        if ((i == 1) || (i == 2)) {
            pos2.y = paneMidY * paneScaleY;
            pos2.y += vOffset;
        } else {
            pos2.y = paneMidY * paneScaleY;
            pos2.y = -pos2.y;
            pos2.y -= vOffset;
        }

        pos2.z = 0.0f;
        mLayouts[layoutId].mNPanes[i]->SetTranslate(pos2);
    }

    if (mLayouts[layoutId].mDoFade) {
        mLayouts[layoutId].mPaneAlpha -= (255 / 10) + 1;
        mLayouts[layoutId].mNPanes[0]->SetAlpha(mLayouts[layoutId].mPaneAlpha);

        if (mLayouts[layoutId].mPaneAlpha < 0) {
            mLayouts[layoutId].mPaneAlpha = 0;
            mLayouts[layoutId].mDoFade = false;
            Cancel(layoutId);
        }
    }
}

void dSelectCursor_c::Cancel(int layoutId) {
    if (mLayouts[layoutId].mIsActive) {
        mLayouts[layoutId].mIsActive = false;
        strcpy(mLayouts[layoutId].mPaneName, "");
        mLayouts[layoutId].mpRootPane->SetVisible(false);
    }
}

void dSelectCursor_c::SetPane(const nw4r::lyt::Pane *pane, int layoutId, bool dontSetAllDrawOrder) {
    mLayouts[layoutId].mIsActive = true;

    strcpy(mLayouts[layoutId].mPaneName, pane->GetName());

    mLayouts[layoutId].mPaneSize.width = pane->GetSize().width;
    mLayouts[layoutId].mPaneSize.height = pane->GetSize().height;

    nw4r::math::MTX34 mtx = pane->GetGlobalMtx();

    mLayouts[layoutId].mPaneGlbMtxTrans.x = mtx._03;
    mLayouts[layoutId].mPaneGlbMtxTrans.y = mtx._13;
    mLayouts[layoutId].mPaneGlbMtxScale.x = mtx._00;
    mLayouts[layoutId].mPaneGlbMtxScale.y = mtx._11;

    mLayouts[layoutId].mPaneBasePosH = pane->GetBasePositionH();
    mLayouts[layoutId].mPaneBasePosV = pane->GetBasePositionV();

    mLayouts[layoutId].mPaneOffset.width = 0.0f;
    mLayouts[layoutId].mPaneOffset.height = 0.0f;
    mLayouts[layoutId].mRootPaneOffset.x = 0.0f;
    mLayouts[layoutId].mRootPaneOffset.y = 0.0f;
    mLayouts[layoutId].m_254 = 0.0f;
    mLayouts[layoutId].mPaneAlpha = 0xff;
    mLayouts[layoutId].mDoFade = false;

    if (dontSetAllDrawOrder) {
        mLayouts[layoutId].mBase.mDrawOrder = 152;
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

void dSelectCursor_c::SetAlpha(const nw4r::lyt::Pane *pane, int layoutId) {
    u8 alpha = pane->GetGlbAlpha();
    Layout_c & layout = mLayouts[layoutId];

    for (int i = 0; i < ARRAY_SIZE(layout.mPPanes); i++) {
        layout.mPPanes[i]->SetAlpha(alpha);
    }
}
