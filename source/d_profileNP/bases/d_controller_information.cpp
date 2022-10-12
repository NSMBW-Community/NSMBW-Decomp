#include <d_profileNP/bases/d_controller_information.hpp>
#include <dol/framework/f_profile.hpp>
#include <dol/framework/f_profile_name.hpp>
#include <dol/snd/snd_audio_mgr.hpp>

DEFAULT_BASE_PROFILE(CONTROLLER_INFORMATION, dControllerInformation_c);

dControllerInformation_c::dControllerInformation_c() {
    mIsCreated = false;
}

dControllerInformation_c::~dControllerInformation_c() {}

int dControllerInformation_c::create() {
    if (mIsCreated) {
        return 1;
    }
    if (!createLayout()) {
        return 0;
    }
    mLayout.ReverseAnimeStartSetup(0, false);
    mLayout.AnimePlay();
    mLayout.calc();
    mIsCreated = true;
    mVisible = false;
    mState = IDLE;
    return 1;
}

bool dControllerInformation_c::createLayout() {
    static const char *animeNames[] = {
        "ControllerInformation_07_in2btn.brlan",
        "ControllerInformation_07_loop2btn.brlan",
        "ControllerInformation_07_hit2btn.brlan"
    };
    
    static const int groupIdxs[] = {0, 1, 2};
    static const char *groupNames[] = {
        "A00_2btn_00",
        "A00_2btn_00",
        "A00_2btn_00"
    };
    
    static const int textBoxVals[] = {8, 8};
    static const char *textBoxNames[] = {
        "T_contInfo_00",
        "T_contInfo_01"
    };
    
    bool res = mLayout.ReadResource("ControllerInformation/ControllerInformation.arc", false);
    
    if (!res) {
        return false;
    }
    mLayout.build("ControllerInformation_07.brlyt", nullptr);
    mLayout.AnimeResRegister(animeNames, 3);
    mLayout.GroupRegister(groupNames, groupIdxs, 3);
    mLayout.TPaneNameRegister(textBoxNames, textBoxVals, 104, 2);
    return true;
}

int dControllerInformation_c::execute() {
    if (mIsCreated && mVisible) {
        switch (mState) {
            case SHOW_IN:
                mLayout.AnimeStartSetup(0, false); // in2btn
                mState = SHOW_LOOP;
                break;
            case SHOW_LOOP:
                if (!mLayout.isAnime(0)) { // in2btn
                    mLayout.LoopAnimeStartSetup(1); // loop2btn
                    mState = WAITING_FOR_END;
                }
                break;
            case END:
                SndAudioMgr::sInstance->startSystemSe(0x85, 1);
                mLayout.AnimeEndSetup(0); // in2btn
                mLayout.AnimeEndSetup(1); // loop2btn
                mLayout.AnimeStartSetup(2, false); // hit2btn
                mState = IDLE;
                break;
        }
        mLayout.AnimePlay();
        mLayout.calc();
    }
    return 1;
}

int dControllerInformation_c::draw() {
    if (mIsCreated && mVisible) {
        mLayout.entry();
    }
    return 1;
}

int dControllerInformation_c::doDelete() {
    return mLayout.doDelete();
}