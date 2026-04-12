#include <game/bases/d_WiiStrapScreen.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_lytbase.hpp>
#include <game/mLib/m_fader.hpp>
#include <lib/revolution/SC.h>
#include <MSL/string.h>

BASE_PROFILE(WII_STRAP, dWiiStrapScreen_c);

dWiiStrapScreen_c::dWiiStrapScreen_c() : mHasLoadedLayout(false) {}

dWiiStrapScreen_c::~dWiiStrapScreen_c() {}

int dWiiStrapScreen_c::create() {
    if (mHasLoadedLayout) {
        return SUCCEEDED;
    }
    if (!createLayout()) {
        return NOT_READY;
    }
    mHasLoadedLayout = true;
    mVisible = true;
    return SUCCEEDED;
}

bool dWiiStrapScreen_c::createLayout() {
    static const char *AnmNameTblEng[ANIM_NAME_COUNT] = { "wiiStrap_EngEU_00_roop.brlan" };
    static const char *AnmNameTblFra[ANIM_NAME_COUNT] = { "wiiStrap_FraEU_00_roop.brlan" };
    static const char *AnmNameTblGer[ANIM_NAME_COUNT] = { "wiiStrap_GerEU_00_roop.brlan" };
    static const char *AnmNameTblIta[ANIM_NAME_COUNT] = { "wiiStrap_ItaEU_00_roop.brlan" };
    static const char *AnmNameTblSpa[ANIM_NAME_COUNT] = { "wiiStrap_SpaEU_00_roop.brlan" };
    static const char *AnmNameTblNed[ANIM_NAME_COUNT] = { "wiiStrap_NedEU_00_roop.brlan" };

    static const char *GROUP_NAME_DT[ANIM_COUNT] = { "A00_Strap" };

    static const int ANIME_INDEX_TBL[ANIM_COUNT] = { roop };

    if (mHasLoadedLayout) {
        return true;
    }

    switch (dGameCom::GetLanguageHBM()) {
        case SC_LANG_NL:
            if (!mLayout.ReadResource3("WiiStrap/WiiStrap.arc", 2)) {
                return false;
            }
            break;
        default:
            if (!mLayout.ReadResourceEx("WiiStrap/WiiStrap.arc", 2, true)) {
                return false;
            }
            break;
    }


    char filename[100];
    memset(filename, 0, ARRAY_SIZE(filename));
    strncat(filename, "wiiStrap_", ARRAY_MAX_STRLEN(filename));
    switch (dGameCom::GetLanguageHBM()) {
        default:
            strncat(filename, "EngEU_00", ARRAY_MAX_STRLEN(filename)); break;
        case SC_LANG_FR:
            strncat(filename, "FraEU_00", ARRAY_MAX_STRLEN(filename)); break;
        case SC_LANG_DE:
            strncat(filename, "GerEU_00", ARRAY_MAX_STRLEN(filename)); break;
        case SC_LANG_IT:
            strncat(filename, "ItaEU_00", ARRAY_MAX_STRLEN(filename)); break;
        case SC_LANG_SP:
            strncat(filename, "SpaEU_00", ARRAY_MAX_STRLEN(filename)); break;
        case SC_LANG_NL:
            strncat(filename, "NedEU_00", ARRAY_MAX_STRLEN(filename)); break;
    }
    strncat(filename, ".brlyt", ARRAY_MAX_STRLEN(filename));

    mLayout.build(filename, nullptr);

    switch (dGameCom::GetLanguageHBM()) {
        default:
            mLayout.AnimeResRegister(AnmNameTblEng, ANIM_NAME_COUNT); break;
        case SC_LANG_FR:
            mLayout.AnimeResRegister(AnmNameTblFra, ANIM_NAME_COUNT); break;
        case SC_LANG_DE:
            mLayout.AnimeResRegister(AnmNameTblGer, ANIM_NAME_COUNT); break;
        case SC_LANG_IT:
            mLayout.AnimeResRegister(AnmNameTblIta, ANIM_NAME_COUNT); break;
        case SC_LANG_SP:
            mLayout.AnimeResRegister(AnmNameTblSpa, ANIM_NAME_COUNT); break;
        case SC_LANG_NL:
            mLayout.AnimeResRegister(AnmNameTblNed, ANIM_NAME_COUNT); break;
    }
    mLayout.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, ANIM_COUNT);
    mLayout.LoopAnimeStartSetup(ANIM_STRAP);

    return true;
}

int dWiiStrapScreen_c::preExecute() {
    if (dBase_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }
    return mFader_c::mFader->getStatus() == mFaderBase_c::HIDDEN;
}

int dWiiStrapScreen_c::execute() {
    if (mHasLoadedLayout && mVisible) {
        mLayout.AnimePlay();
        mLayout.calc();
    }
    return SUCCEEDED;
}

int dWiiStrapScreen_c::draw() {
    if (mHasLoadedLayout && mVisible) {
        mLayout.entry();
    }
    return SUCCEEDED;
}

int dWiiStrapScreen_c::doDelete() {
    return mLayout.doDelete();
}
