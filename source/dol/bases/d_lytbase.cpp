#include <game/bases/d_lytbase.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_lyttextBox.hpp>
#include <game/mLib/m_video.hpp>

TagProcessor_c LytBase_c::s_TagPrc;

LytBase_c::LytBase_c() {
    mAnimCount = 0;
    mGroupCount = 0;
}

LytBase_c::~LytBase_c() {}

bool LytBase_c::build(const char *name, d2d::ResAccMult_c *resAcc) {
    bool res = d2d::Multi_c::build(name, resAcc);
    if (res) {
        allocStringBuffer(getRootPane());
        mLayout.SetTagProcessor(&s_TagPrc);
    }
    return res;
}

LytTextBox_c *LytBase_c::findTextBox(const char *name) {
    return (LytTextBox_c *) d2d::Multi_c::findTextBoxByName(name);
}

void LytBase_c::allocStringBuffer(nw4r::lyt::Pane *pane) {
    nw4r::lyt::TextBox *box = nw4r::ut::DynamicCast<nw4r::lyt::TextBox *>(pane);
    if (box != nullptr) {
        const wchar_t *buf = box->GetStringBuffer();
        box->AllocStringBuffer(0x1ff);
        if (buf != nullptr) {
            box->SetString(buf, 0);
        }
    }
    for (
        nw4r::lyt::PaneList::Iterator it = pane->GetChildList().GetBeginIter();
        it != pane->GetChildList().GetEndIter();
        ++it
    ) {
        allocStringBuffer(&*it);
    }
}

bool LytBase_c::ReadResourceEx(const char *name, int i, bool isLocalized) {
    char resourcePath[100];
    if (isLocalized) {
        char nonLocalizedPath[100] = "Layout/";
        strncat(nonLocalizedPath, name, sizeof(nonLocalizedPath) - 1);
        dGameCom::AreaLanguageFolder(nonLocalizedPath, resourcePath);
    } else {
        memset(resourcePath, 0, sizeof(resourcePath));
        strncat(resourcePath, "Layout/", sizeof(resourcePath) - 1);
        strncat(resourcePath, name, sizeof(resourcePath) - 1);
    }
    if (!mResAccessorLoader.requestEx(resourcePath, i)) {
        return false;
    }
    mpResAccessor = &mResAccessorLoader;
    return true;
}

bool LytBase_c::ReadResource(const char *name, bool isLocalized) {
    return ReadResourceEx(name, 0, isLocalized);
}

bool LytBase_c::ReadResource2(const char *name, int i) {
    char resourcePath[100];
    memset(resourcePath, 0, sizeof(resourcePath));
    strncat(resourcePath, "EU/", sizeof(resourcePath) - 1);
    strncat(resourcePath, "Layout/", sizeof(resourcePath) - 1);
    strncat(resourcePath, name, sizeof(resourcePath) - 1);
    if (!mResAccessorLoader.requestEx(resourcePath, 0)) {
        return false;
    }
    mpResAccessor = &mResAccessorLoader;
    return true;
}

bool LytBase_c::ReadResource3(const char *name, int i) {
    char resourcePath[100];
    memset(resourcePath, 0, sizeof(resourcePath));
    strncat(resourcePath, "EU/NedEU/Layout/", sizeof(resourcePath) - 1);
    strncat(resourcePath, name, sizeof(resourcePath) - 1);
    if (!mResAccessorLoader.requestEx(resourcePath, i)) {
        return false;
    }
    mpResAccessor = &mResAccessorLoader;
    return true;
}

void LytBase_c::NPaneRegister(const char **paneNames, nw4r::lyt::Pane **panes, int count) {
    for (int i = 0; i < count; i++) {
        panes[i] = findPaneByName(paneNames[i]);
    }
}

void LytBase_c::WPaneRegister(const char **windowPaneNames, nw4r::lyt::Window **panes, int count) {
    for (int i = 0; i < count; i++) {
        panes[i] = findWindowByName(windowPaneNames[i]);
    }
}

void LytBase_c::PPaneRegister(const char **picPaneNames, nw4r::lyt::Picture **panes, int count) {
    for (int i = 0; i < count; i++) {
        panes[i] = findPictureByName(picPaneNames[i]);
    }
}

void LytBase_c::TPaneRegister(const char **textboxNames, LytTextBox_c **panes, int count) {
    for (int i = 0; i < count; i++) {
        panes[i] = findTextBox(textboxNames[i]);
    }
}

void LytBase_c::TPaneNameRegister(const char **textboxNames, const int *messageIDs, int messageGroup, int count) {
    MsgRes_c *bmg = dMessage_c::getMesRes();
    for (int i = 0; i < count; i++) {
        LytTextBox_c *box = findTextBox(textboxNames[i]);
        box->setMessage(bmg, messageGroup, messageIDs[i], 0);
    }
}

void LytBase_c::AnimeResRegister(const char **animeNames, int count) {
    d2d::ResAccMult_c *resAcc = mpResAccessor;
    mpAnimRes = new m2d::AnmResV2_c[count];
    mAnimCount = count;
    for (int i = 0; i < count; i++) {
        mpAnimRes[i].create(animeNames[i], resAcc, &mLayout, true);
    }
}

void LytBase_c::GroupRegister(const char **groupNames, const int *animeIdxs, int count) {
    mpAnimGroup = new m2d::AnmGroup_c[count];
    mpEnabledAnims = new bool[count];
    mGroupCount = count;
    for (int i = 0; i < count; i++) {
        mpAnimGroup[i].create(&mpAnimRes[animeIdxs[i]], groupNames[i]);
    }
}

void LytBase_c::AnimeStartBaseSetup(int animeIdx) {
    mpAnimGroup[animeIdx].setAnmEnable(true);
    m2d::AnmGroup_c &animeGroup = mpAnimGroup[animeIdx];
    animeGroup.mpFrameCtrl->setFrame(0.0f);
    animeGroup.updateFrame();
    mpEnabledAnims[animeIdx] = true;
    mLastStartedAnimNum = animeIdx;
}

void LytBase_c::AnimeStartSetup(int animeIdx, bool b) {
    AnimeStartBaseSetup(animeIdx);
    if (b) {
        mpAnimGroup[animeIdx].setEnd();
    }
    mpAnimGroup[animeIdx].mpFrameCtrl->mFlags = 1;
}

void LytBase_c::LoopAnimeStartSetup(int animeIdx) {
    AnimeStartBaseSetup(animeIdx);
    mpAnimGroup[animeIdx].mpFrameCtrl->mFlags = 0;
}

void LytBase_c::ReverseAnimeStartSetup(int animeIdx, bool b) {
    AnimeStartBaseSetup(animeIdx);
    if (b) {
        mpAnimGroup[animeIdx].mpFrameCtrl->mFlags = 1;
        mpAnimGroup[animeIdx].setEnd();
    } else {
        mpAnimGroup[animeIdx].setStart();
    }
    mpAnimGroup[animeIdx].mpFrameCtrl->mFlags = 3;
}

void LytBase_c::AnimeEndSetup(int animeIdx) {
    if (mpEnabledAnims[animeIdx]) {
        mpAnimGroup[animeIdx].setAnmEnable(false);
        mpEnabledAnims[animeIdx] = false;
    }
}

void LytBase_c::AllAnimeEndSetup() {
    for (int i = 0; i < mGroupCount; i++) {
        AnimeEndSetup(i);
    }
    mLastStartedAnimNum = mGroupCount;
}

void LytBase_c::AnimePlay() {
    for (int i = 0; i < mGroupCount; i++) {
        if (mpEnabledAnims[i]) {
            if (mpAnimGroup[i].mpFrameCtrl->isStop()) {
                AnimeEndSetup(i);
            } else {
                mpAnimGroup[i].play();
            }
        }
    }
}

bool LytBase_c::isAnime(int animeIdx) {
    if (animeIdx < 0) {
        return mpEnabledAnims[mLastStartedAnimNum];
    }
    return mpEnabledAnims[animeIdx];
}

bool LytBase_c::isAllAnime() {
    for (int i = 0; i < mGroupCount; i++) {
        if (mpEnabledAnims[i]) {
            return true;
        }
    }
    return false;
}

void LytBase_c::FUN_800c9770(const nw4r::lyt::Pane *pane, d2d::ClipSettings &clipData) {
    nw4r::ut::Rect view = mDrawInfo.GetViewRect();
    mVec2_c paneScale;
    mVec2_c pos;
    mVec2_c clipSize;
    mVec2_c scale = mDrawInfo.GetLocationAdjustScale();

    float actualScaleY = scale.y;
    if (actualScaleY <= 1e-6f) {
        actualScaleY = 1e-6f;
    }
    float height = view.GetHeight() / actualScaleY;

    float actualScaleX = scale.x;
    if (actualScaleX <= 1e-6f) {
        actualScaleX = 1e-6f;
    }
    float width = view.GetWidth() / actualScaleX;

    if (width < 0.0f) {
        width *= -1.0f;
    }
    if (height < 0.0f) {
        height *= -1.0f;
    }

    nw4r::math::MTX34 mtx = pane->GetGlobalMtx();

    paneScale.x = pane->GetSize().width;
    paneScale.y = pane->GetSize().height;

    float tmpx = mtx._03 / actualScaleX;
    float tmpy = mtx._13 * -1.0f;

    float scaledW = mVideo::m_video->mRenderModeObj.fbWidth / width;
    float scaledH = mVideo::m_video->mRenderModeObj.efbHeight / height;

    float scX = paneScale.x * scaledW;
    float scY = paneScale.y * scaledH;

    scX = mtx._11 * scX;
    scY = mtx._11 * scY;

    tmpx *= scaledW;
    tmpy *= scaledH;

    u32 x, y, w, h;
    GXGetScissor(&x, &y, &w, &h);
    pos.x = x + w * 0.5f + (tmpx - scX * 0.5f);
    pos.y = y + h * 0.5f + (tmpy - scY * 0.5f);
    clipSize.x = scX + 0.5f;
    clipSize.y = scY + 0.5f;
    clipData.setSize(clipSize);
    clipData.setPos(pos);
    clipData.mEnabled = true;
}

bool LytBase_c::doDelete() {
    if (mAnimCount != 0) {
        for (int i = 0; i < mAnimCount; i++) {
            if (!mpAnimRes[i].remove()) {
                return false;
            }
        }
        delete[] mpAnimRes;
        mAnimCount = 0;
    }
    if (mGroupCount != 0) {
        delete[] mpAnimGroup;
        delete[] mpEnabledAnims;
        mGroupCount = 0;
    }
    if (!mResAccessorLoader.remove()) {
        return false;
    }
    return true;
}
