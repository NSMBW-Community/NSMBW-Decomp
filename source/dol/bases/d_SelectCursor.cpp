#include <game/bases/d_SelectCursor.hpp>

ACTOR_PROFILE(SELECT_CURSOR, dSelectCursor_c, 0);

dSelectCursor_c *dSelectCursor_c::m_instance = nullptr;

dSelectCursor_c::dSelectCursor_c() {
    m_instance = this;
    mHasLoadedLayout = false;
}

dSelectCursor_c::~dSelectCursor_c() {
    dSelectCursor_c::m_instance = nullptr;
}

int dSelectCursor_c::create() {
    static const char *AnmNameTbl[ANIM_COUNT] = {
        "select_cursor_04_loopCursor.brlan"
    };

    static const char *GROUP_NAME_DT[ANIM_COUNT] = {
        "A00_cursor"
    };

    static const int ANIME_INDEX_TBL[ANIM_COUNT] = {
        ANIM_CURSOR
    };

    static const char *PPANE_TABLE[P_COUNT] = {
        "P_cursor_00",
        "P_cursor_01",
        "P_cursor_02",
        "P_cursor_03"
    };

    static const char *NPANE_TABLE[N_COUNT] = {
        "N_cursor_00",
        "N_LU_00",
        "N_RU_00",
        "N_LD_00",
        "N_RD_00"
    };


    if (mHasLoadedLayout) {
        return SUCCEEDED;
    }

    if (!mResLoader.request("Layout/select_cursor/select_cursor.arc")) {
        return NOT_READY;
    }

    for (int i = 0; i < LAYOUT_COUNT; i++) {
        mLayouts[i].mBase.mpResAccessor = &mResLoader;
    }

    for (int i = 0; i < LAYOUT_COUNT; i++) {
        mLayouts[i].mBase.build("select_cursor_04.brlyt", nullptr);
        mLayouts[i].mBase.AnimeResRegister(AnmNameTbl, ANIM_COUNT);
        mLayouts[i].mBase.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, ANIM_COUNT);
        mLayouts[i].mpRootPane = mLayouts[i].mBase.getRootPane();
        mLayouts[i].mBase.PPaneRegister(PPANE_TABLE, mLayouts[i].mpPicturePanes, P_COUNT);
        mLayouts[i].mBase.NPaneRegister(NPANE_TABLE, mLayouts[i].mpNullPanes, N_COUNT);
        mLayouts[i].mpRootPane->SetVisible(false);
        Cancel(i);

        if (i == UNK_4) {
            mLayouts[i].mBase.mDrawOrder = 14;
        } else {
            mLayouts[i].mBase.mDrawOrder = 147;
        }

        mLayouts[i].mBase.LoopAnimeStartSetup(ANIM_CURSOR);
    }

    mHasLoadedLayout = true;
    return SUCCEEDED;
}


int dSelectCursor_c::execute() {
    for (int i = 0; i < LAYOUT_COUNT; i++) {
        if (mLayouts[i].mIsActive) {
            PosSet(i);
            mLayouts[i].mBase.AnimePlay();
            mLayouts[i].mBase.calc();
        }
    }

    return SUCCEEDED;
}

int dSelectCursor_c::draw() {
    for (int i = 0; i < LAYOUT_COUNT; i++) {
        if (mLayouts[i].mIsActive) {
            mLayouts[i].mBase.entry();
        }
    }

    return SUCCEEDED;
}

int dSelectCursor_c::doDelete() {
    if (!mResLoader.remove()) {
        return NOT_READY;
    }

    for (int i = 0; i < LAYOUT_COUNT; i++) {
        if (!mLayouts[i].mBase.doDelete()) {
            return NOT_READY;
        }
    }

    return SUCCEEDED;
}

void dSelectCursor_c::PosSet(int layoutId) {
    mLayouts[layoutId].mpRootPane->SetVisible(true);

    mVec2_c panePos;
    float paneHalfWidth, paneHalfHeight, paneScaleX, paneScaleY;

    paneScaleX = mLayouts[layoutId].mPaneGlbMtxScale.x;
    paneScaleY = mLayouts[layoutId].mPaneGlbMtxScale.y;

    paneHalfWidth = mLayouts[layoutId].mPaneSize.width / 2.0f;
    paneHalfHeight = mLayouts[layoutId].mPaneSize.height / 2.0f;

    panePos.x = mLayouts[layoutId].mPaneGlbMtxTrans.x + mLayouts[layoutId].mRootPaneOffset.x;
    if (mLayouts[layoutId].mPaneBasePosH == nw4r::lyt::HORIZONTALPOSITION_LEFT) {
        panePos.x += paneHalfWidth * paneScaleX;
    } else if (mLayouts[layoutId].mPaneBasePosH == nw4r::lyt::HORIZONTALPOSITION_RIGHT) {
        panePos.x -= paneHalfWidth * paneScaleX;
    }

    panePos.y = mLayouts[layoutId].mPaneGlbMtxTrans.y + mLayouts[layoutId].mRootPaneOffset.y;
    if (mLayouts[layoutId].mPaneBasePosV == nw4r::lyt::VERTICALPOSITION_TOP) {
        panePos.y -= paneHalfHeight * paneScaleY;
    } else if (mLayouts[layoutId].mPaneBasePosV == nw4r::lyt::VERTICALPOSITION_BOTTOM) {
        panePos.y += paneHalfHeight * paneScaleY;
    }

    mLayouts[layoutId].mpRootPane->SetTranslate(mVec3_c(panePos, 0.0f));
    float hOffset = mLayouts[layoutId].mPaneOffset.width;
    float vOffset = mLayouts[layoutId].mPaneOffset.height;

    for (int cornerPane = N_LU_00; cornerPane < N_COUNT; cornerPane++) {
        mVec3_c cornerPos;
        if (cornerPane == N_LU_00 || cornerPane == N_LD_00) {
            cornerPos.x = paneHalfWidth * paneScaleX;
            cornerPos.x = -cornerPos.x;
            cornerPos.x -= hOffset;
        } else {
            cornerPos.x = paneHalfWidth * paneScaleX;
            cornerPos.x += hOffset;
        }

        if (cornerPane == N_LU_00 || cornerPane == N_RU_00) {
            cornerPos.y = paneHalfHeight * paneScaleY;
            cornerPos.y += vOffset;
        } else {
            cornerPos.y = paneHalfHeight * paneScaleY;
            cornerPos.y = -cornerPos.y;
            cornerPos.y -= vOffset;
        }

        cornerPos.z = 0.0f;
        mLayouts[layoutId].mpNullPanes[cornerPane]->SetTranslate(cornerPos);
    }

    if (mLayouts[layoutId].mDoFade) {
        mLayouts[layoutId].mPaneAlpha -= (255 / 10) + 1;
        mLayouts[layoutId].mpNullPanes[N_cursor_00]->SetAlpha(mLayouts[layoutId].mPaneAlpha);

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

void dSelectCursor_c::SetPane(const nw4r::lyt::Pane *pane, int layoutId, bool forceTopDrawOrder) {
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

    if (forceTopDrawOrder) {
        mLayouts[layoutId].mBase.mDrawOrder = 152;
    } else {
        for (int i = 0; i < LAYOUT_COUNT; i++) {
            if (i == UNK_4) {
                mLayouts[i].mBase.mDrawOrder = 14;
            } else {
                mLayouts[i].mBase.mDrawOrder = 147;
            }
        }
    }
}

void dSelectCursor_c::SetAlpha(const nw4r::lyt::Pane *pane, int layoutId) {
    u8 alpha = pane->GetGlbAlpha();
    Layout_c &layout = mLayouts[layoutId];

    for (int i = 0; i < P_COUNT; i++) {
        layout.mpPicturePanes[i]->SetAlpha(alpha);
    }
}
