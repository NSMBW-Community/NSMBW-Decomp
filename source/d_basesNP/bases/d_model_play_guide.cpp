#include <game/bases/d_model_play_guide.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/framework/f_profile.hpp>
#include <game/framework/f_profile_name.hpp>
#include <game/bases/d_WarningManager.hpp>

BASE_PROFILE(MODEL_PLAY_GUIDE, dModelPlayGuide_c);

dModelPlayGuide_c::dModelPlayGuide_c() : mIsCreated(false) {}

dModelPlayGuide_c::~dModelPlayGuide_c() {}

int dModelPlayGuide_c::create() {
    if (mIsCreated) {
        return SUCCEEDED;
    }

    if (!createLayout()) {
        return NOT_READY;
    }

    mIsCreated = true;
    mLayout.mDrawOrder = 139;
    mpRootPane->SetVisible(true);

    return SUCCEEDED;
}

bool dModelPlayGuide_c::createLayout() {
    bool res = mLayout.ReadResource("modelPlayGuide/modelPlayGuide.arc", false);
    if (!res) {
        return false;
    }

    if (dGameCom::GetAspectRatio() == 1) {
        mLayout.build("modelPlayGuide_02.brlyt", nullptr);
    } else {
        mLayout.build("modelPlayGuide_43.brlyt", nullptr);
    }

    mpRootPane = mLayout.getRootPane();

    return true;
}

int dModelPlayGuide_c::preExecute() {
    if (dBase_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }

    return !dWarningManager_c::isError();
}

int dModelPlayGuide_c::execute() {
    if (mIsCreated) {
        mLayout.calc();
    }

    return SUCCEEDED;
}

int dModelPlayGuide_c::draw() {
    if (mIsCreated) {
        mLayout.entry();
    }

    return SUCCEEDED;
}

int dModelPlayGuide_c::doDelete() {
    return mLayout.doDelete();
}
