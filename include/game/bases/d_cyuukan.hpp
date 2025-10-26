#pragma once

#include <game/bases/d_s_stage.hpp>
#include <game/mLib/m_vec.hpp>
#include <constants/game_constants.h>

class dCyuukan_c {
public:
    dCyuukan_c();
    virtual ~dCyuukan_c() {}
    void clear();
    void courseIN();
    bool checkEntry();
    void setCyuukanData(int, u8, s8, ulong);
    bool isCyuukanStart(int, u8, u8);
    s8 getPlrNo(int) const;

    void setPos(nw4r::math::VEC3 pos) { mPlayerSetPos = pos; }

    int mActiveCheckpoint;
    mVec3_c mPlayerSetPos;
    u32 mAmbushType;
    u8 mWorldNo;
    u8 mCourseNo;
    u8 mFileNo;
    u8 mNextGoto;
    bool mIsKinopioInChuukan;
    sCollectionCoin mCoinCollection[STAR_COIN_COUNT];
    PLAYER_TYPE_e mHitPlayer[CHECKPOINT_COUNT];
};
