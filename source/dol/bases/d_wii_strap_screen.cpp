#include "game/framework/f_profile_name.hpp"
#include <game/bases/d_wii_strap_screen.hpp>
#include <game/bases/d_base.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_lytbase.hpp>
#include <game/framework/f_base.hpp>
#include <game/framework/f_profile.hpp>
#include <game/mLib/m_fader.hpp>
#include <MSL/string.h>

BASE_PROFILE(WII_STRAP, dWiiStrapScreen_c);

dWiiStrapScreen_c::dWiiStrapScreen_c() {
    mHasLoadedLayout = false;
}
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

const char* lanFra = "wiiStrap_FraEU_00_roop.brlan";
const char* lanGer = "wiiStrap_GerEU_00_roop.brlan";
const char* lanIta = "wiiStrap_ItaEU_00_roop.brlan";
const char* lanSpa = "wiiStrap_SpaEU_00_roop.brlan";
const char* lanNed = "wiiStrap_NedEU_00_roop.brlan";
const char* lanEng = "wiiStrap_EngEU_00_roop.brlan";
const char* a00Strap = "A00_Strap";
int groupRegisterArray;

bool dWiiStrapScreen_c::createLayout() {
    if (mHasLoadedLayout) {
        return true;
    }
    
    if ((u8)(dGameCom::GetLanguageHBM()) == '\x06') {
        if (!mLayout.ReadResource3("WiiStrap/WiiStrap.arc", 2)) {
            return false;
        }
    }
    else {
        if (!mLayout.ReadResourceEx("WiiStrap/WiiStrap.arc", 2, true)) {
            return false;
        }
    }


    char rlyt[112];
    memset(rlyt, 0, 100);
    strncat(rlyt, "wiiStrap_", 99);

    char filename;
    switch ((u8)dGameCom::GetLanguageHBM()) {
        default:
            strncat(&filename,"EngEU_00",99); break;
        case '\x03':
            strncat(&filename,"FraEU_00",99); break;
        case '\x02':
            strncat(&filename,"GerEU_00",99); break;
        case '\x05':
            strncat(&filename,"ItaEU_00",99); break;
        case '\x04':
            strncat(&filename,"SpaEU_00",99); break;
        case '\x06':
            strncat(&filename,"NedEU_00",99); break;
    }
    strncat(&filename, ".brlyt", 99);

    mLayout.build(rlyt, nullptr);
    u8 lang = dGameCom::GetLanguageHBM();
    switch (lang) {
        default:
            mLayout.AnimeResRegister(&lanEng, 1); break;
        case '\x03':
            mLayout.AnimeResRegister(&lanFra, 1); break;
        case '\x02':
            mLayout.AnimeResRegister(&lanGer, 1); break;
        case '\x05':
            mLayout.AnimeResRegister(&lanIta, 1); break;
        case '\x04':
            mLayout.AnimeResRegister(&lanSpa, 1); break;
        case '\x06':
            mLayout.AnimeResRegister(&lanNed, 1); break;
    }
    mLayout.GroupRegister(&a00Strap, &groupRegisterArray, 1);
    mLayout.LoopAnimeStartSetup(0);

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
