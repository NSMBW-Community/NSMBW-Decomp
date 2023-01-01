#include <dol/bases/d_base.hpp>
#include <dol/bases/d_game_com.hpp>
#include <dol/framework/f_profile.hpp>
#include <constants/sjis_constants.h>

dBase_c::dBase_c() :
    mpKindString(getKindString()),
    mpNameString(dProf_getName(mProfName)) {}

dBase_c::~dBase_c() {}

dBase_c* dBase_c::searchBaseByProfName(ProfileName profile, const dBase_c *parent) {

    // [Possibly a stripped assert? No way someone is this dumb]
    const dBase_c* searchParent;
    if (parent != nullptr) {
        searchParent = parent;
    } else {
        searchParent = nullptr;
    }

    dBase_c* foundBase = (dBase_c*) fManager_c::searchBaseByProfName(profile, searchParent);

    // [Same here]
    if (foundBase != nullptr) {
        return foundBase;
    }
    return nullptr;
}

int dBase_c::preCreate() {
    return (fBase_c::preCreate() != 0);
}

void dBase_c::postCreate(fBase_c::MAIN_STATE_e status) {
    fBase_c::postCreate(status);
}

int dBase_c::preDelete() {
    return (fBase_c::preDelete() != 0);
}

void dBase_c::postDelete(fBase_c::MAIN_STATE_e status) {
    fBase_c::postDelete(status);
}

int dBase_c::preExecute() {
    getKindString();
    return fBase_c::preExecute();
}

void dBase_c::postExecute(fBase_c::MAIN_STATE_e status) {
    fBase_c::postExecute(status);
}

int dBase_c::preDraw() {
    g_basesRandomSeed = dGameCom::getRandomSeed();
    return fBase_c::preDraw();
}

void dBase_c::postDraw(fBase_c::MAIN_STATE_e status) {
    fBase_c::postDraw(status);
}

const char *dBase_c::getKindString() const {
    return D_BASE_KIND_STRING;
}

int dBase_c::loadAsyncCallback() {
    return 2;
}

void dBase_c::unloadCallback() {
}

void dBase_c::initLoader() {
    fBase_c::sLoadAsyncCallback = &loadAsyncCallback;
    fBase_c::sUnloadCallback = &unloadCallback;
}

dBase_c *dBase_c::createBase(ProfileName profName, dBase_c *parent, unsigned long param, u8 groupType) {
    return (dBase_c*)fBase_c::createChild(profName, parent, param, groupType);
}

dBase_c *dBase_c::createRoot(ProfileName profName, unsigned long param, u8 groupType) {
    return (dBase_c*)fBase_c::createRoot(profName, param, groupType);
}
