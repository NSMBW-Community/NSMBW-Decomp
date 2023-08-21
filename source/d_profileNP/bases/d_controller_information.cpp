#include <d_profileNP/bases/d_controller_information.hpp>
#include <constants/message_list.h>
#include <constants/sound_list.h>
#include <dol/framework/f_profile.hpp>
#include <dol/framework/f_profile_name.hpp>
#include <dol/snd/snd_audio_mgr.hpp>

DEFAULT_BASE_PROFILE(CONTROLLER_INFORMATION, dControllerInformation_c);

dControllerInformation_c::dControllerInformation_c() : mIsCreated(false) {
}

dControllerInformation_c::~dControllerInformation_c() {}

int dControllerInformation_c::create() {
    if (mIsCreated) {
        return SUCCEEDED;
    }

    if (!createLayout()) {
        return NOT_READY;
    }

    mLayout.ReverseAnimeStartSetup(IN2BTN, false);
    mLayout.AnimePlay();
    mLayout.calc();
    mIsCreated = true;
    mVisible = false;
    mState = IDLE;

    return SUCCEEDED;
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

    static const int textBoxMsgs[] = {
        MSG_HOLD_WIIMOTE_SIDEWAYS,
        MSG_HOLD_WIIMOTE_SIDEWAYS,
    };

    static const char *textBoxNames[] = {
        "T_contInfo_00",
        "T_contInfo_01"
    };

    bool res = mLayout.ReadResource("ControllerInformation/ControllerInformation.arc", false);
    if (!res) {
        return false;
    }

    mLayout.build("ControllerInformation_07.brlyt", nullptr);
    mLayout.AnimeResRegister(animeNames, ARRAY_SIZE(animeNames));
    mLayout.GroupRegister(groupNames, groupIdxs, ARRAY_SIZE(groupNames));
    mLayout.TPaneNameRegister(textBoxNames, textBoxMsgs, BMG_CATEGORY_CONTROLLER_INFORMATION, ARRAY_SIZE(textBoxNames));

    return true;
}

int dControllerInformation_c::execute() {
    if (mIsCreated && mVisible) {
        switch (mState) {
            case SHOW_IN:
                mLayout.AnimeStartSetup(IN2BTN, false);
                mState = SHOW_LOOP;
                break;

            case SHOW_LOOP:
                if (!mLayout.isAnime(IN2BTN)) {
                    mLayout.LoopAnimeStartSetup(LOOP2BTN);
                    mState = WAITING_FOR_END;
                }
                break;

            case END:
                SndAudioMgr::sInstance->startSystemSe(SE_SYS_BUTTON_SKIP, 1);
                mLayout.AnimeEndSetup(IN2BTN);
                mLayout.AnimeEndSetup(LOOP2BTN);
                mLayout.AnimeStartSetup(HIT2BTN, false);
                mState = IDLE;
                break;
        }
        mLayout.AnimePlay();
        mLayout.calc();
    }
    return SUCCEEDED;
}

int dControllerInformation_c::draw() {
    if (mIsCreated && mVisible) {
        mLayout.entry();
    }
    return SUCCEEDED;
}

int dControllerInformation_c::doDelete() {
    return mLayout.doDelete();
}
