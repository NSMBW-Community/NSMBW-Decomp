#include <game/bases/d_SmallScore.hpp>
#include <constants/message_list.h>
#include <game/bases/d_info.hpp>
#include <game/bases/d_game_com.hpp>
#include <lib/nw4r/ut/color.hpp>
#include <lib/nw4r/lyt/lyt_material.hpp>
#include <game/mLib/m_video.hpp>
#include <game/mLib/m_vec.hpp>
#include <game/bases/d_bg_parameter.hpp>
#include <lib/egg/math.hpp>

dSmallScore_c::dSmallScore_c() : mPos(0.0f, 0.0f), mScale(1.0f, 1.0f), _1a4(0.0f), _1a8(0.0f), mInitialized(false), _21c(0) {
    _208 = 0;
}

dSmallScore_c::~dSmallScore_c() {
    dSmallScore_c::m_instance = nullptr;
}

bool dSmallScore_c::createLayout(d2d::ResAccMultLoader_c * res) {
    static const char *T_PANE_NAME_TBL[] = {
        "T_100_00", "T_1000_00", "T_red2_00", "T_1UP_00", "T_coin_x_00",
        "T_coinPoint_00",
    };

    static const char *N_PANE_NAME_TBL[] = {
        "N_coin_00"
    };

    if (mInitialized) return true;

    mLayout.mpResAccessor = res;
    mLayout.build("pointGet_02.brlyt", nullptr);
    mpRootPane = mLayout.getRootPane();

    mLayout.TPaneRegister(T_PANE_NAME_TBL, &T_100_00, ARRAY_SIZE(T_PANE_NAME_TBL));
    T_coin_x_00->setMessage(dMessage_c::getMesRes(), BMG_CATEGORY_SMALL_SCORE, MSG_COINS, 0);

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
    _1f8 = 4;
    mInitialized = true;
    _21e = 0;
    _21f = 0;
    _221 = 0;

    return true;
}

void dSmallScore_c::execute() {

    static void (dSmallScore_c::*Proc_tbl[])() = {
        &dSmallScore_c::MakeStart,
        &dSmallScore_c::UpMove,
        &dSmallScore_c::DispWait,
        &dSmallScore_c::GoalScoreDisp,
    };

    if (_1f8 == 4) return;

    (this->*Proc_tbl[_1f8])();
    PositionSet();
    mLayout.calc();
}

void dSmallScore_c::draw() {
    if (_1f8 == 4) return;
    if (_1f8 <= 0) return;

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
        nw4r::ut::Color(255, 120, 0, 255),
        nw4r::ut::Color(50, 250, 50, 255),
        nw4r::ut::Color(0, 185, 220, 255),
        nw4r::ut::Color(255, 255, 0, 255),
        nw4r::ut::Color(255, 255, 255, 255),
    };

    static const nw4r::ut::Color DOWN_COLOR_DATA_TBL[] = {
        nw4r::ut::Color(255, 200, 40, 255),
        nw4r::ut::Color(255, 255, 0, 255),
        nw4r::ut::Color(210, 255, 250, 255),
        nw4r::ut::Color(255, 255, 180, 255),
        nw4r::ut::Color(255, 255, 255, 255),
    };

    T_1000_00->SetVtxColor(0, UP_COLOR_DATA_TBL[player_id]);
    T_1000_00->SetVtxColor(2, DOWN_COLOR_DATA_TBL[player_id]);
}

void dSmallScore_c::setPlayer100Color(int player_id) {
    static const nw4r::ut::Color COLOR_DATA_TBL[] = {
        nw4r::ut::Color(255, 150, 85, 255),
        nw4r::ut::Color(70, 250, 70, 255),
        nw4r::ut::Color(70, 200, 230, 255),
        nw4r::ut::Color(250, 255, 80, 255),
        nw4r::ut::Color(250, 255, 255, 255),
    };

    nw4r::lyt::Material* mat = T_100_00->GetMaterial();
    mat->setTev(1, nw4r::lyt::GXColorS10(COLOR_DATA_TBL[player_id]));
}

void dSmallScore_c::chgColor() {
    if (_21e == 0) return;
    if (++_20c < 10) return;
    _20c = 0;

    int i = _210;

    do {
        if (++i >= 4) {
            i = 0;
            break;
        }

        if (dInfo_c::m_instance->mCharIDs[i] == 3)
            break;
    } while (true);

    setPlayer1upColor(i);
    _210 = i;
}

void dSmallScore_c::setNormalOrBlueColor() {
    static const nw4r::ut::Color A[] = {
        nw4r::ut::Color(255, 10, 10, 255),
        nw4r::ut::Color(60, 120, 255, 255),
    };
    static const nw4r::ut::Color B[] = {
        nw4r::ut::Color(255, 150, 150, 255),
        nw4r::ut::Color(200, 240, 255, 255),
    };

    int v0 = 0;
    if (_221 != 0) {
        _221 = 0;
        v0 = 1;
    }

    T_red2_00->SetVtxColor(0, A[v0]);
    T_red2_00->SetVtxColor(2, B[v0]);
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
    mVec2_c delta(0.008f, 0.008f);
    sum += mAnimScale;

    getTextBox(_218)->setScale(sum);
    if (++mAnimCounter >= 10) {
        mAnimCounter = 0;

        if (mAnimIsShrinking)
            mAnimIsShrinking = false;
        else
            mAnimIsShrinking = true;
    }

    if (mAnimIsShrinking) {
        mAnimScale -= delta;
    } else {
        mAnimScale += delta;
    }
}

void dSmallScore_c::MakeStart() {
    static unsigned int SUB_ID_TBL[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 16,
        8, 17, 18, 19, 8, 51, 52, 53, 54, 55, 56, 0
    };

    int v0 = _1fc;
    _210 = _208;
    if (v0 >= 21) {
        v0 = 5;

        T_coin_x_00->setVisible(true);
        T_coinPoint_00->setVisible(true);
        N_coin_00->setVisible(true);
    } else {
        if (v0 <= 3) {
            setPlayer100Color(_208);
            v0 = 0;
        } else if (v0 <= 7) {
            setPlayer1000Color(_208);
            v0 = 1;
        } else if (v0 <= 15) {
            setNormalOrBlueColor();
            v0 = 2;
        } else {
            setPlayer1upColor(_208);
            v0 = 3;
        }

        (&T_100_00)[v0]->setVisible(true);
    }

    MsgRes_c* bmg = dMessage_c::getMesRes();

    (&T_100_00)[v0]->setMessage(bmg, BMG_CATEGORY_SMALL_SCORE, SUB_ID_TBL[_1fc], 0);
    (&T_100_00)[v0]->mScale = mScale;
    _218 = v0;
    _1f4 = dBgParameter_c::ms_Instance_p->mPos.y - 20.0f;
    mpRootPane->setVisible(true);

    if (_21c != 0) {
        _1f8 = 3;
    } else {
        _1a8 = 1.0f;
        _1b0 = 0.025f;
        _204 = 0;
        _200 = 0;
        mAnimCounter = 0;
        mAnimIsShrinking = false;
        mAnimScale.x = 0.0f;
        mAnimScale.y = 0.0f;
        _1f8 = 1;
    }

    if (_1fc == 20) {
        _21e = 1;
    } else {
        _21e = 0;
    }

    _20c = 0;
    ScissorMaskSet();
}

void dSmallScore_c::UpMove() {
    if (_21f != 0) {
        BigSmallAnime();
        _204 = 120;
    }

    chgColor();

    if (_1a8 == EGG::Math<float>::zero()) {
        _1f8 = 2;
        if (_1fc >= 21)
            _204 = 30;
    }
}

void dSmallScore_c::DispWait() {
    if (_21f != 0)
        BigSmallAnime();

    chgColor();

    _200++;
    if (_208 == 4) {
        if (_200 < 60) return;
    } else if (_200 < _204)
        return;

    _200 = 0;
    mpRootPane->setVisible(false);
    T_100_00->setVisible(false);
    T_1000_00->setVisible(false);
    T_red2_00->setVisible(false);
    T_1UP_00->setVisible(false);
    T_coin_x_00->setVisible(false);
    T_coinPoint_00->setVisible(false);
    N_coin_00->setVisible(false);
    _21f = 0;
    getTextBox(_218)->setScale(mScale);
    _1f8 = 4;
}

void dSmallScore_c::GoalScoreDisp() {}

void dSmallScore_c::PositionSet() {
    if (! (mpRootPane->mFlags & 1)) return;

    if (_1a8 <= 0.0f) {
        _1a8 = 0.0f;
    } else {
        _1b8 += _1a8;
        _1a8 -= _1b0;
    }

    mVec3_c v;
    v.x = mPos.x;
    v.y = mPos.y + _1b8;

    if (v.y >= _1f4)
        v.y = _1f4;

    dGameCom::getGlbPosToLyt(v);

    mVec2_c a;
    a.x = v.x;
    a.y = v.y;
    mpRootPane->mPos = mVec3_c(a.x, a.y, 0.0f);
}

void dSmallScore_c::CreateSmallScore(const mVec3_c &v, int a, int b) {
    mpRootPane->setVisible(false);

    if ((dInfo_c::mGameFlag & 0x40) && (a <= 7))
        return;

    _1fc = a;
    _208 = b;
    mPos.x = v.x;
    mPos.y = v.y;
    _1a4 = 0.0f;
    _1a8 = 0.0f;
    _1b4 = 0.0f;
    _1b8 = 0.0f;
    _21c = 0;
    _1f8 = 0;
}

void dSmallScore_c::PosSet(const mVec3_c &v) {
    mPos.x = v.x;
    mPos.y = v.y;
}
