#include <game/bases/d_controller_information.hpp>
#include <constants/message_list.h>
#include <constants/sound_list.h>
#include <game/framework/f_profile.hpp>
#include <game/framework/f_profile_name.hpp>
#include <game/snd/snd_audio_mgr.hpp>

BASE_PROFILE(CONTROLLER_INFORMATION, dControllerInformation_c);

dControllerInformation_c::dControllerInformation_c() : mIsCreated(false) {}

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

    static const char *AnmNameTbl[] = {
        "ControllerInformation_07_in2btn.brlan",
        "ControllerInformation_07_loop2btn.brlan",
        "ControllerInformation_07_hit2btn.brlan"
    };

    static const int ANIME_INDEX_TBL[] = {0, 1, 2};
    static const char *GROUP_NAME_DT[] = {
        "A00_2btn_00",
        "A00_2btn_00",
        "A00_2btn_00"
    };

    static const int MESSAGE_DATA_TBL[] = {
        MSG_HOLD_WIIMOTE_SIDEWAYS,
        MSG_HOLD_WIIMOTE_SIDEWAYS,
    };

    static const char *T_PANE_FIXED_NAME_TBL[] = {
        "T_contInfo_00",
        "T_contInfo_01"
    };

    bool res = mLayout.ReadResource("ControllerInformation/ControllerInformation.arc", false);
    if (!res) {
        return false;
    }

    mLayout.build("ControllerInformation_07.brlyt", nullptr);
    mLayout.AnimeResRegister(AnmNameTbl, ARRAY_SIZE(AnmNameTbl));
    mLayout.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, ARRAY_SIZE(GROUP_NAME_DT));
    mLayout.TPaneNameRegister(T_PANE_FIXED_NAME_TBL, MESSAGE_DATA_TBL, BMG_CATEGORY_CONTROLLER_INFORMATION, ARRAY_SIZE(T_PANE_FIXED_NAME_TBL));

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
