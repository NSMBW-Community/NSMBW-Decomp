#include <game/bases/d_lytbase.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_lyttextBox.hpp>
#include <game/mLib/m_mtx.hpp>
#include <game/mLib/m_video.hpp>

#define TEXTBOX_STRING_BUFFER_SIZE 0x1FF

TagProcessor_c LytBase_c::s_TagPrc;

LytBase_c::LytBase_c() : mAnimCount(0), mAnimGroupCount(0) {}

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
        box->AllocStringBuffer(TEXTBOX_STRING_BUFFER_SIZE);
        if (buf != nullptr) {
            box->SetString(buf, 0);
        }
    }
    for (
        nw4r::lyt::PaneList::Iterator it = pane->GetChildList().GetBeginIter();
        it != pane->GetChildList().GetEndIter();
        it++
    ) {
        allocStringBuffer(&*it);
    }
}

bool LytBase_c::ReadResourceEx(const char *name, int allocDirection, bool isLocalized) {
    char resourcePath[100];
    if (isLocalized) {
        char nonLocalizedPath[100] = "Layout/";
        strncat(nonLocalizedPath, name, ARRAY_MAX_STRLEN(nonLocalizedPath));
        dGameCom::AreaLanguageFolder(nonLocalizedPath, resourcePath);
    } else {
        memset(resourcePath, 0, ARRAY_SIZE(resourcePath));
        strncat(resourcePath, "Layout/", ARRAY_MAX_STRLEN(resourcePath));
        strncat(resourcePath, name, ARRAY_MAX_STRLEN(resourcePath));
    }
    if (!mResAccessorLoader.requestEx(resourcePath, allocDirection)) {
        return false;
    }

    mpResAccessor = &mResAccessorLoader;
    return true;
}

bool LytBase_c::ReadResource(const char *name, bool isLocalized) {
    return ReadResourceEx(name, 0, isLocalized);
}

bool LytBase_c::ReadResource2(const char *name, int allocDirection) {
    char resourcePath[100];
    memset(resourcePath, 0, ARRAY_SIZE(resourcePath));
    strncat(resourcePath, "EU/", ARRAY_MAX_STRLEN(resourcePath));
    strncat(resourcePath, "Layout/", ARRAY_MAX_STRLEN(resourcePath));
    strncat(resourcePath, name, ARRAY_MAX_STRLEN(resourcePath));
    if (!mResAccessorLoader.requestEx(resourcePath, 0)) {
        return false;
    }

    mpResAccessor = &mResAccessorLoader;
    return true;
}

bool LytBase_c::ReadResource3(const char *name, int i) {
    char resourcePath[100];
    memset(resourcePath, 0, ARRAY_SIZE(resourcePath));
    strncat(resourcePath, "EU/NedEU/Layout/", ARRAY_MAX_STRLEN(resourcePath));
    strncat(resourcePath, name, ARRAY_MAX_STRLEN(resourcePath));
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
    mAnimGroupCount = count;
    for (int i = 0; i < count; i++) {
        mpAnimGroup[i].create(&mpAnimRes[animeIdxs[i]], groupNames[i]);
    }
}

void LytBase_c::AnimeStartBaseSetup(int animeIdx) {
    float dummy_float = 0.5f; // [For .sdata2 ordering]
    mpAnimGroup[animeIdx].setAnmEnable(true);
    m2d::AnmGroup_c &animeGroup = mpAnimGroup[animeIdx];
    animeGroup.mpFrameCtrl->setFrame(0.0f);
    animeGroup.updateFrame();
    mpEnabledAnims[animeIdx] = true;
    mLastStartedAnimNum = animeIdx;
}

void LytBase_c::AnimeStartSetup(int animeIdx, bool startAtEnd) {
    AnimeStartBaseSetup(animeIdx);
    if (startAtEnd) {
        mpAnimGroup[animeIdx].setLast();
    }
    mpAnimGroup[animeIdx].mpFrameCtrl->setFlags(false, false);
}

void LytBase_c::LoopAnimeStartSetup(int animeIdx) {
    AnimeStartBaseSetup(animeIdx);
    mpAnimGroup[animeIdx].mpFrameCtrl->setFlags(true, false);
}

void LytBase_c::ReverseAnimeStartSetup(int animeIdx, bool startAtEnd) {
    AnimeStartBaseSetup(animeIdx);
    if (startAtEnd) {
        mpAnimGroup[animeIdx].mpFrameCtrl->setFlags(false, false);
        mpAnimGroup[animeIdx].setEnd();
    } else {
        mpAnimGroup[animeIdx].setStart();
    }
    mpAnimGroup[animeIdx].mpFrameCtrl->setFlags(false, true);
}

void LytBase_c::AnimeEndSetup(int animeIdx) {
    if (mpEnabledAnims[animeIdx]) {
        mpAnimGroup[animeIdx].setAnmEnable(false);
        mpEnabledAnims[animeIdx] = false;
    }
}

void LytBase_c::AllAnimeEndSetup() {
    for (int i = 0; i < mAnimGroupCount; i++) {
        AnimeEndSetup(i);
    }
    mLastStartedAnimNum = mAnimGroupCount;
}

void LytBase_c::AnimePlay() {
    for (int i = 0; i < mAnimGroupCount; i++) {
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
    for (int i = 0; i < mAnimGroupCount; i++) {
        if (mpEnabledAnims[i]) {
            return true;
        }
    }
    return false;
}

void LytBase_c::SetScissorMask(const nw4r::lyt::Pane *pane, d2d::ScissorMask &scissorMask) {
    nw4r::ut::Rect view = mDrawInfo.GetViewRect();
    mVec2_c paneSize;
    mVec2_c scissorPos;
    mVec2_c scissorSize;
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

    mMtx_c paneMtx = pane->GetGlobalMtx();

    paneSize.x = pane->GetSize().width;
    paneSize.y = pane->GetSize().height;

    float ratioX = mVideo::m_video->getWidth() / width;
    float ratioY = mVideo::m_video->getHeight() / height;

    float trueSizeX = paneMtx.m[1][1] * (paneSize.x * ratioX);
    float trueSizeY = paneMtx.m[1][1] * (paneSize.y * ratioY);

    float translateX = paneMtx.m[0][3] / actualScaleX;
    float translateY = paneMtx.m[1][3] * -1.0f;

    translateX *= ratioX;
    translateY *= ratioY;

    u32 scX, scY, scW, scH;
    GXGetScissor(&scX, &scY, &scW, &scH);
    scissorPos.x = scX + scW * 0.5f + (translateX - trueSizeX * 0.5f);
    scissorPos.y = scY + scH * 0.5f + (translateY - trueSizeY * 0.5f);
    scissorSize.x = trueSizeX + 0.5f;
    scissorSize.y = trueSizeY + 0.5f;
    scissorMask.setSize(scissorSize);
    scissorMask.setPos(scissorPos);
    scissorMask.mEnabled = true;
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

    if (mAnimGroupCount != 0) {
        delete[] mpAnimGroup;
        delete[] mpEnabledAnims;
        mAnimGroupCount = 0;
    }

    if (!mResAccessorLoader.remove()) {
        return false;
    }

    return true;
}
