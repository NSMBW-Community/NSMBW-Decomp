#include "game/bases/d_game_com.hpp"
#include "game/bases/d_lyttextBox.hpp"
#include <game/bases/d_lytbase.hpp>

TagProcessor_c LytBase_c::s_TagPrc;

LytBase_c::LytBase_c() {
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

void LytBase_c::WPaneRegister(const char **paneNames, nw4r::lyt::Window **panes, int count) {
    for (int i = 0; i < count; i++) {
        panes[i] = findWindowByName(paneNames[i]);
    }
}

void LytBase_c::PPaneRegister(const char **paneNames, nw4r::lyt::Picture **panes, int count) {
    for (int i = 0; i < count; i++) {
        panes[i] = findPictureByName(paneNames[i]);
    }
}

void LytBase_c::TPaneRegister(const char **paneNames, LytTextBox_c **panes, int count) {
    for (int i = 0; i < count; i++) {
        panes[i] = findTextBox(paneNames[i]);
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
