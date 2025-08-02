#include <game/bases/d_SmallScore.hpp>
#include <constants/message_list.h>
#include <game/bases/d_info.hpp>
#include <game/bases/d_game_com.hpp>
#include <lib/nw4r/ut/Color.h>
#include <lib/nw4r/lyt/lyt_material.hpp>
#include <game/mLib/m_video.hpp>
#include <game/mLib/m_vec.hpp>
#include <game/bases/d_bg_parameter.hpp>
#include <lib/egg/math/eggMath.h>

dSmallScore_c *dSmallScore_c::m_instance = nullptr;

dSmallScore_c::dSmallScore_c() : mPos(0.0f, 0.0f), mScale(1.0f, 1.0f), mPosDelta(0.0f, 0.0f), mInitialized(false), mIsGoalScore(false) {
    mPlayerType = 0;
}

dSmallScore_c::~dSmallScore_c() {
    dSmallScore_c::m_instance = nullptr;
}

bool dSmallScore_c::createLayout(d2d::ResAccMultLoader_c *res) {
    static const char *T_PANE_NAME_TBL[] = {
        "T_100_00", "T_1000_00", "T_red2_00", "T_1UP_00", "T_coin_x_00",
        "T_coinPoint_00",
    };

    static const char *N_PANE_NAME_TBL[] = {
        "N_coin_00"
    };

    if (mInitialized) {
        return true;
    }

    mLayout.mpResAccessor = res;
    mLayout.build("pointGet_02.brlyt", nullptr);
    mpRootPane = mLayout.getRootPane();

    mLayout.TPaneRegister(T_PANE_NAME_TBL, &T_100_00, ARRAY_SIZE(T_PANE_NAME_TBL));
    T_coin_x_00->setMessage(dMessage_c::getMesRes(), BMG_CATEGORY_SMALL_SCORE, MSG_LOWERCASE_X, 0);

    mLayout.NPaneRegister(N_PANE_NAME_TBL, &N_coin_00, ARRAY_SIZE(N_PANE_NAME_TBL));

    T_100_00->setVisible(false);
    T_1000_00->setVisible(false);
    T_red2_00->setVisible(false);
    T_1UP_00->setVisible(false);
    T_coin_x_00->setVisible(false);
    T_coinPoint_00->setVisible(false);

    N_coin_00->setVisible(false);
    mpRootPane->setVisible(false);

    mLayout.mDrawOrder = 7;
    mState = dSmallScore_c::STATE_NONE;
    mInitialized = true;
    mEnableColorChange = false;
    mEnableBigSmallAnim = false;
    mHasBlueColor = false;

    return true;
}

void dSmallScore_c::execute() {

    static const ProcFunc Proc_tbl[] = {
        &dSmallScore_c::MakeStart,
        &dSmallScore_c::UpMove,
        &dSmallScore_c::DispWait,
        &dSmallScore_c::GoalScoreDisp,
    };

    if (mState == dSmallScore_c::STATE_NONE) {
        return;
    }

    (this->*Proc_tbl[mState])();
    PositionSet();
    mLayout.calc();
}

void dSmallScore_c::draw() {
    if (mState == dSmallScore_c::STATE_NONE) {
        return;
    }

    if (mState <= dSmallScore_c::STATE_MAKE_START) {
        return;
    }

    mLayout.entry();
}

void dSmallScore_c::doDelete() {
    mLayout.doDelete();
}

void dSmallScore_c::setPlayer1upColor(int player_id) {
    dGameCom::Player1upColor(T_1UP_00, player_id);
}

void dSmallScore_c::setPlayer1000Color(int player_id) {
    static const nw4r::ut::Color UP_COLOR_DATA_TBL[] = {
        nw4r::ut::Color(255, 120, 0, 255), // #FF7800
        nw4r::ut::Color(50, 250, 50, 255), // #32FA32
        nw4r::ut::Color(0, 185, 220, 255), // #00B9DC
        nw4r::ut::Color(255, 255, 0, 255), // #FFFF00
        nw4r::ut::Color(255, 255, 255, 255), // #FFFFFF
    };

    static const nw4r::ut::Color DOWN_COLOR_DATA_TBL[] = {
        nw4r::ut::Color(255, 200, 40, 255), // #FFC828
        nw4r::ut::Color(255, 255, 0, 255), // #FFFF00
        nw4r::ut::Color(210, 255, 250, 255), // #D2FFFA
        nw4r::ut::Color(255, 255, 180, 255), // #FFFFB4
        nw4r::ut::Color(255, 255, 255, 255), // #FFFFFF
    };

    T_1000_00->SetVtxColor(0, UP_COLOR_DATA_TBL[player_id]);
    T_1000_00->SetVtxColor(2, DOWN_COLOR_DATA_TBL[player_id]);
}

void dSmallScore_c::setPlayer100Color(int playerType) {
    static const nw4r::ut::Color COLOR_DATA_TBL[] = {
        nw4r::ut::Color(255, 150, 85, 255), // #FF9655
        nw4r::ut::Color(70, 250, 70, 255), // #46FA46
        nw4r::ut::Color(70, 200, 230, 255), // #46C8E6
        nw4r::ut::Color(250, 255, 80, 255), // #FAFF50
        nw4r::ut::Color(250, 255, 255, 255), // #FFFFFF
    };

    nw4r::lyt::Material *mat = T_100_00->GetMaterial();
    mat->setTev(1, nw4r::lyt::GXColorS10(COLOR_DATA_TBL[playerType]));
}

void dSmallScore_c::chgColor() {
    if (!mEnableColorChange) {
        return;
    }
    if (++mChgColorCounter < 10) {
        return;
    }
    mChgColorCounter = 0;

    int type = mPlayerColor;

    while (true) {
        if (++type >= 4) {
            type = 0;
            break;
        }

        if (dInfo_c::m_instance->mCharIDs[type] == 3) {
            break;
        }
    }

    setPlayer1upColor(type);
    mPlayerColor = type;
}

void dSmallScore_c::setNormalOrBlueColor() {
    static const nw4r::ut::Color UP_COLOR_DATA_TBL[] = {
        nw4r::ut::Color(255, 10, 10, 255), // #FF0A0A
        nw4r::ut::Color(60, 120, 255, 255), // #3C78FF
    };
    static const nw4r::ut::Color DOWN_COLOR_DATA_TBL[] = {
        nw4r::ut::Color(255, 150, 150, 255), // #FF9696
        nw4r::ut::Color(200, 240, 255, 255), // #C8F0FF
    };

    int colorIdx = 0;
    if (mHasBlueColor) {
        mHasBlueColor = false;
        colorIdx = 1;
    }

    T_red2_00->SetVtxColor(0, UP_COLOR_DATA_TBL[colorIdx]);
    T_red2_00->SetVtxColor(2, DOWN_COLOR_DATA_TBL[colorIdx]);
}

void dSmallScore_c::ScissorMaskSet() {
    d2d::ClipSettings clip;

    if (dGameCom::GetAspectRatio() == 0) {
        clip.mPos.y = (mVideo::m_video->mRenderModeObj.mEFBHeight - mClipScale.y) * 0.5f;
        clip.mSize = mClipScale;
        clip.mEnabled = 1;
    }

    mLayout.mClipSettings = clip;
}

void dSmallScore_c::BigSmallAnime() {
    mVec2_c sum = mScale;
    mVec2_c delta(0.08f, 0.08f);
    sum += mAnimScale;

    getTextBox(mCurTextbox)->setScale(sum);
    if (++mAnimCounter >= 10) {
        mAnimCounter = 0;

        if (mAnimIsShrinking) {
            mAnimIsShrinking = false;
        } else {
            mAnimIsShrinking = true;
        }
    }

    if (mAnimIsShrinking) {
        mAnimScale -= delta;
    } else {
        mAnimScale += delta;
    }
}

void dSmallScore_c::MakeStart() {
    static const u32 SUB_ID_TBL[] = {
        MSG_100, MSG_200, MSG_400, MSG_800, MSG_1000, MSG_2000, MSG_4000,
        MSG_8000, MSG_ONE, MSG_TWO, MSG_THREE, MSG_FOUR, MSG_FIVE, MSG_SIX,
        MSG_SEVEN, MSG_EIGHT, MSG_1UP, MSG_2UP, MSG_3UP, MSG_4UP, MSG_1UP,
        MSG_COIN_2, MSG_COIN_3, MSG_COIN_5, MSG_COIN_10, MSG_COIN_15,
        MSG_COIN_20
    };

    int temp = mPopupType;
    mPlayerColor = mPlayerType;
    if (temp >= 21) {
        temp = 5;

        T_coin_x_00->setVisible(true);
        T_coinPoint_00->setVisible(true);
        N_coin_00->setVisible(true);
    } else {
        if (temp <= 3) {
            setPlayer100Color(mPlayerType);
            temp = 0;
        } else if (temp <= 7) {
            setPlayer1000Color(mPlayerType);
            temp = 1;
        } else if (temp <= 15) {
            setNormalOrBlueColor();
            temp = 2;
        } else {
            setPlayer1upColor(mPlayerType);
            temp = 3;
        }

        getTextBox(temp)->setVisible(true);
    }

    MsgRes_c *bmg = dMessage_c::getMesRes();

    getTextBox(temp)->setMessage(bmg, BMG_CATEGORY_SMALL_SCORE, SUB_ID_TBL[mPopupType], 0);
    getTextBox(temp)->setScale(mScale);
    mCurTextbox = temp;
    mMaxHeight = dBgParameter_c::ms_Instance_p->mPos.y - 20.0f;
    mpRootPane->setVisible(true);

    if (mIsGoalScore) {
        mState = dSmallScore_c::STATE_GOAL_DISP;
    } else {
        mPosDelta.y = 1.0f;
        mPosDeceleration.y = 0.025f;
        mDispWaitTime = 0;
        mDispWaitCounter = 0;
        mAnimCounter = 0;
        mAnimIsShrinking = false;
        mAnimScale.x = 0.0f;
        mAnimScale.y = 0.0f;
        mState = dSmallScore_c::STATE_UP_MOVE;
    }

    if (mPopupType == 20) {
        mEnableColorChange = true;
    } else {
        mEnableColorChange = false;
    }

    mChgColorCounter = 0;
    ScissorMaskSet();
}

void dSmallScore_c::UpMove() {
    if (mEnableBigSmallAnim) {
        BigSmallAnime();
        mDispWaitTime = 120;
    }

    chgColor();

    if (mPosDelta.y == EGG::Math<float>::zero()) {
        mState = dSmallScore_c::STATE_DISP_WAIT;
        if (mPopupType >= 21) {
            mDispWaitTime = 30;
        }
    }
}

void dSmallScore_c::DispWait() {
    if (mEnableBigSmallAnim) {
        BigSmallAnime();
    }

    chgColor();

    mDispWaitCounter++;
    if (mPlayerType == 4) {
        if (mDispWaitCounter < 60) {
            return;
        }
    } else if (mDispWaitCounter < mDispWaitTime) {
        return;
    }

    mDispWaitCounter = 0;
    mpRootPane->setVisible(false);
    T_100_00->setVisible(false);
    T_1000_00->setVisible(false);
    T_red2_00->setVisible(false);
    T_1UP_00->setVisible(false);
    T_coin_x_00->setVisible(false);
    T_coinPoint_00->setVisible(false);
    N_coin_00->setVisible(false);
    mEnableBigSmallAnim = false;
    getTextBox(mCurTextbox)->setScale(mScale);
    mState = dSmallScore_c::STATE_NONE;
}

void dSmallScore_c::GoalScoreDisp() {}

void dSmallScore_c::PositionSet() {
    if (!(mpRootPane->mFlags & 1)) {
        return;
    }

    if (mPosDelta.y <= 0.0f) {
        mPosDelta.y = 0.0f;
    } else {
        mPosOffset.y += mPosDelta.y;
        mPosDelta.y -= mPosDeceleration.y;
    }

    mVec3_c globalPos;
    globalPos.x = mPos.x;
    globalPos.y = mPos.y + mPosOffset.y;

    if (globalPos.y >= mMaxHeight) {
        globalPos.y = mMaxHeight;
    }

    dGameCom::getGlbPosToLyt(globalPos);
    mVec2_c pos;
    pos.x = globalPos.x;
    pos.y = globalPos.y;

    mpRootPane->mPos = mVec3_c(pos, 0.0f);
}

void dSmallScore_c::CreateSmallScore(const mVec3_c &pos, int popupType, int playerType) {
    mpRootPane->setVisible(false);

    if ((dInfo_c::mGameFlag & 0x40) && (popupType <= 7))
        return;

    mPopupType = popupType;
    mPlayerType = playerType;
    mPos.x = pos.x;
    mPos.y = pos.y;
    mPosDelta.x = 0.0f;
    mPosDelta.y = 0.0f;
    mPosOffset.x = 0.0f;
    mPosOffset.y = 0.0f;
    mIsGoalScore = false;
    mState = dSmallScore_c::STATE_MAKE_START;
}

void dSmallScore_c::PosSet(const mVec3_c &pos) {
    mPos.x = pos.x;
    mPos.y = pos.y;
}
