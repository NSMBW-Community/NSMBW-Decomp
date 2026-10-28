#pragma once
#include <game/framework/f_profile.hpp>
#include <game/snd/snd_2d_calc.hpp>
#include <game/snd/snd_audio_mgr.hpp>
#include <game/snd/snd_scene_manager.hpp>
#include <game/mLib/m_vec.hpp>
#include <lib/egg/core/eggHeap.h>
#include <nw4r/snd.h>
#include <nw4r/math.h>

/// @file

/// @ingroup bases

class NMSndObjectBase : public nw4r::snd::SoundActor {
public:
    enum OBJ_TYPE {
        OBJ_TYPE_0 = 0
    };

    NMSndObjectBase(OBJ_TYPE, nw4r::snd::SoundArchivePlayer &);
    virtual ~NMSndObjectBase();
    virtual u8 getSamePrioSndCount(ulong, int);

    bool sendRemote(nw4r::snd::SoundHandle *p, unsigned long p1, unsigned long p2);

    u32 getTotal() const { return mTotalCount; }

    u32 mTotalCount;
    u32 m_58;
    Snd2DCalc *mpSnd2dCalc;
    OBJ_TYPE mObjType;
};

template<int T>
class NMSndObject : public NMSndObjectBase {
public:
    class SoundHandlePrm : public nw4r::snd::SoundHandle {
    public:
        SoundHandlePrm() : m_04(1.0f) {}

        float m_04;
    };

    NMSndObject() :
        NMSndObjectBase(NMSndObjectBase::OBJ_TYPE_0, SndAudioMgr::sInstance->mArcPlayer),
        m_64(1.0f), m_68(0), m_6c(1.0f), m_70(0.0f)
    {
        SetPlayableSoundCount(0, T);
        mTotalCount = T + 2;
        m_58 = 1;
    }

    void processParams() {
        for (int idx = 0; idx < mTotalCount; idx++) {
            if (!mParams[idx].IsAttachedSound()) {
                continue;
            }
            u32 flag = SndAudioMgr::sInstance->get3DCtrlFlag(mParams[idx].GetId());
            if (~flag & 1) {
                mParams[idx].SetVolume(m_64.m_00, 0);
            } else if (
                SndSceneMgr::sInstance->m_14 == 3 ||
                (SndSceneMgr::sInstance->m_14 == 2 && SndSceneMgr::sInstance->m_10 == 3)
            ) {
                mParams[idx].SetVolume(m_64.m_00, 0);
            }
            if (~flag & 8) {
                nw4r::snd::SoundArchive::SoundInfo info;
                SndAudioMgr::sInstance->mpSndArc->ReadSoundInfo(mParams[idx].GetId(), &info);
                if (m_68 < 0) {
                    mParams[idx].SetPlayerPriority(info.playerPriority + m_68);
                } else {
                    mParams[idx].SetPlayerPriority(info.playerPriority);
                }
            }
            if (~flag & 2) {
                mParams[idx].SetPan(m_70);
            }
        }
    }

    void calc() {
        for (int i = 0; i < T; i++) {
            if (GetPlayingSoundCount(i) > 0) {
                mpSnd2dCalc->calcAmbParam(&m_64, mPos, 0);
                processParams();
                break;
            }
        }
    }

    virtual void calc(const nw4r::math::VEC2 &pos) {
        mPos = pos;
        calc();
    }

    SoundHandlePrm *findHandle(int id) {
        for (int i = 0; i < mTotalCount; i++) {
            if (mParams[i].IsAttachedSound()) {
                if (mParams[i].GetId() == id) {
                    return &mParams[i];
                }
            }
        }
        return nullptr;
    }

    SoundHandlePrm *getFreeHandle() {
        for (int i = 0; i < mTotalCount; i++) {
            if (!mParams[i].IsAttachedSound()) {
                mParams[i].m_04 = 1.0f;
                return &mParams[i];
            }
        }
        return nullptr;
    }

    virtual SoundHandlePrm *startSound(unsigned long p1, unsigned long p2) {
        SoundHandlePrm *p = getFreeHandle();
        if (p != nullptr) {
            detail_StartSound(p, p1, 0);
            if (!p->IsAttachedSound()) {
                return nullptr;
            }
            sendRemote(p, p1, p2);
            return p;
        }
        return nullptr;
    }
    virtual SoundHandlePrm *holdSound(unsigned long p1, unsigned long p2) {
        SoundHandlePrm *p = findHandle(p1);
        if (p == nullptr) {
            p = getFreeHandle();
        }
        if (p != nullptr) {
            detail_HoldSound(p, p1, 0);
            if (!p->IsAttachedSound()) {
                return nullptr;
            }
            sendRemote(p, p1, p2);
            return p;
        }
        return nullptr;
    }

    virtual SoundHandlePrm *prepareSound(unsigned long p1, unsigned long p2) {
        SoundHandlePrm *p = getFreeHandle();
        if (p != nullptr) {
            detail_PrepareSound(p, p1, 0);
            if (!p->IsAttachedSound()) {
                return nullptr;
            }
            sendRemote(p, p1, p2);
            return p;
        }
        return nullptr;
    }

    virtual SoundHandlePrm *startSound(unsigned long p1, short p2, unsigned long p3) {
        SoundHandlePrm *p = getFreeHandle();
        if (p != nullptr) {
            detail_StartSound(p, p1, 0);
            if (!p->IsAttachedSound()) {
                return nullptr;
            }
            sendRemote(p, p1, p3);
            if (SndAudioMgr::sInstance->mpSndArc->GetSoundType(p1) == 1) {
                nw4r::snd::SeqSoundHandle handle(p);
                handle.WriteVariable(0, p2);
            }
            return p;
        }
        return nullptr;
    }
    virtual SoundHandlePrm *holdSound(unsigned long p1, short p2, unsigned long p3) {
        SoundHandlePrm *p = findHandle(p1);
        if (p == nullptr) {
            p = getFreeHandle();
        }
        if (p != nullptr) {
            detail_HoldSound(p, p1, 0);
            if (!p->IsAttachedSound()) {
                return nullptr;
            }
            sendRemote(p, p1, p3);
            if (SndAudioMgr::sInstance->mpSndArc->GetSoundType(p1) == 1) {
                nw4r::snd::SeqSoundHandle handle(p);
                handle.WriteVariable(0, p2);
            }
            return p;
        }
        return nullptr;
    }

    virtual SoundHandlePrm *startSound(unsigned long p1, const nw4r::math::VEC2 &p2, unsigned long p3) {
        SoundHandlePrm *p = getFreeHandle();
        if (p != nullptr) {
            detail_StartSound(p, p1, 0);
            if (!p->IsAttachedSound()) {
                return nullptr;
            }
            SndAudioMgr::sInstance->setSoundPosition(p, p2);
            sendRemote(p, p1, p3);
            return p;
        }
        return nullptr;
    }
    virtual SoundHandlePrm *holdSound(unsigned long p1, const nw4r::math::VEC2 &p2, unsigned long p3) {
        SoundHandlePrm *p = findHandle(p1);
        if (p == nullptr) {
            p = getFreeHandle();
        }
        if (p != nullptr) {
            detail_HoldSound(p, p1, 0);
            if (!p->IsAttachedSound()) {
                return nullptr;
            }
            SndAudioMgr::sInstance->setSoundPosition(p, p2);
            sendRemote(p, p1, p3);
            return p;
        }
        return nullptr;
    }

    Snd2DCalc::Snd2DAmbParam m_64;
    int m_68;
    float m_6c;
    float m_70;
    SoundHandlePrm mParams[T + 2];
    nw4r::math::VEC2 mPos;
};

class SndObjctPly : public NMSndObject<4> {
public:
    /// @brief Player voice sound IDs
    /// @ingroup constants
    enum ESnd_C_Voice {
        VOICE_PLAYER_JOIN,
        VOICE_PJ_JUMP,
        VOICE_PJ_SPIN_JUMP,
        VOICE_PLAYER_DECIDE,
        VOICE_CS_FALL_INTO_MAP,
        VOICE_CS_COURSE_IN,
        VOICE_CS_COURSE_IN_MULTI,
        VOICE_CS_COURSE_IN_HARD,
        VOICE_CS_COURSE_MISS,
        VOICE_CS_JUMP,
        VOICE_CS_DECIDE_JOIN,
        VOICE_CS_ENCOUNT,
        VOICE_CS_SHOVED,
        VOICE_CS_CHASE_SHIP,
        VOICE_CS_NOTICE_JR,
        VOICE_GAME_OVER,
        VOICE_CONTINUE,
        VOICE_CONTINUE_COURSE,
        VOICE_MOTIAGE,
        VOICE_NAGERU,
        VOICE_MOTIAGE_PLAYER,
        VOICE_JUMP_2ND,
        VOICE_JUMP_3RD,
        VOICE_YOSHI_JUMP,
        VOICE_JR_A_BATTLE_APP,
        VOICE_WALL_KICK,
        VOICE_PRPL_JUMP,
        VOICE_PNGN_SLIDE,
        VOICE_ROPE_CATCH,
        VOICE_ROPE_RELEASE,
        VOICE_ROPE_FALL,
        VOICE_GOAL_POLE_CATCH,
        VOICE_CLIFF_DIVE,
        VOICE_CLIFF_UP,
        VOICE_CLIFF_DOWN,
        VOICE_CANNON_SHOT_S,
        VOICE_CANNON_SHOT,
        VOICE_BALLOON_HELP,
        VOICE_BALLOON_HELP_2,
        VOICE_HUKKATSU,
        VOICE_DAMAGE_LAST,
        VOICE_DAMAGE_LAST_2,
        VOICE_TIMEUP,
        VOICE_TIMEUP_MULTI,
        VOICE_SCROLL_OUT,
        VOICE_DAMAGE_FIRE,
        VOICE_DAMAGE_FREEZE,
        VOICE_DAMAGE_POISON,
        VOICE_DAMAGE_ELEC,
        VOICE_DAMAGE_EATEN,
        VOICE_INTO_SANDPILLAR,
        VOICE_QUAKE,
        VOICE_STOMPED,
        VOICE_HIP_ATTACKED,
        VOICE_TYUKAN,
        VOICE_GET_STAR,
        VOICE_ITEM_COMPLETE,
        VOICE_CLEAR_NORMAL,
        VOICE_CLEAR_ANOTHER,
        VOICE_CLEAR_MULTI,
        VOICE_CLEAR_HELPED,
        VOICE_CLEAR_BOSS,
        VOICE_CLEAR_LAST_BOSS,
        VOICE_SAVE_PRINCESS,
        VOICE_CS_COMPLETE_DEMO,
        VOICE_GET_PRIZE,
        VOICE_FLOOR_FALL,
        VOICE_NOTICE,
        VOICE_MISS_PRINCESS,
        VOICE_ACTIVE_TV,
        VOICE_ACTIVE_RC
    };

    void calculate(const nw4r::math::VEC2 &pos) {
        NMSndObject<4>::calc(pos);
    }

    virtual SoundHandlePrm *startSound(ulong p1, ulong p2);
    virtual SoundHandlePrm *holdSound(ulong p1, ulong p2);
    virtual SoundHandlePrm *startSound(ulong p1, short p2, ulong p3);
    virtual SoundHandlePrm *holdSound(ulong p1, short p2, ulong p3);
    virtual SoundHandlePrm *startSound(ulong p1, const nw4r::math::VEC2 &p2, ulong p3);
    virtual SoundHandlePrm *holdSound(ulong p1, const nw4r::math::VEC2 &p2, ulong p3);

    void stopPlyJumpSound();
    void startFootSound(ulong, float, ulong);
    void startVoice(ESnd_C_Voice, ulong);
    void holdVoice(ESnd_C_Voice, ulong);
};

class SndObjctEmy : public NMSndObject<4> {
public:
    virtual SoundHandlePrm *startSound(ulong p1, ulong p2);
    virtual SoundHandlePrm *holdSound(ulong p1, ulong p2);
    virtual SoundHandlePrm *startSound(ulong p1, short p2, ulong p3);
    virtual SoundHandlePrm *holdSound(ulong p1, short p2, ulong p3);
    virtual SoundHandlePrm *startSound(ulong p1, const nw4r::math::VEC2 &p2, ulong p3);
    virtual SoundHandlePrm *holdSound(ulong p1, const nw4r::math::VEC2 &p2, ulong p3);
};

class SndObjctCmnEmy : public NMSndObject<4> {
public:
    virtual SoundHandlePrm *startSound(ulong p1, ulong p2);
    virtual SoundHandlePrm *holdSound(ulong p1, ulong p2);
    virtual SoundHandlePrm *startSound(ulong p1, short p2, ulong p3);
    virtual SoundHandlePrm *holdSound(ulong p1, short p2, ulong p3);
    virtual SoundHandlePrm *startSound(ulong p1, const nw4r::math::VEC2 &p2, ulong p3);
    virtual SoundHandlePrm *holdSound(ulong p1, const nw4r::math::VEC2 &p2, ulong p3);
};

class SndObjctCmnMap : public NMSndObject<4> {
public:
    virtual SoundHandlePrm *startSound(ulong p1, ulong p2);
    virtual SoundHandlePrm *holdSound(ulong p1, ulong p2);
    virtual SoundHandlePrm *startSound(ulong p1, short p2, ulong p3);
    virtual SoundHandlePrm *holdSound(ulong p1, short p2, ulong p3);
    virtual SoundHandlePrm *startSound(ulong p1, const nw4r::math::VEC2 &p2, ulong p3);
    virtual SoundHandlePrm *holdSound(ulong p1, const nw4r::math::VEC2 &p2, ulong p3);
};

namespace dAudio {
    void init(EGG::Heap *heap);
    void execute();
    void requestStartScene(ProfileName sceneProf); ///< Sets up game audio for the given scene.
    void createSndObjctCmn();
    void deleteSndObjctCmn();
    void setNextScene(ProfileName prof, unsigned long bgmIndex);
    void pauseOffGameWithReset();
    void boot();
    void loadSceneSnd();
    bool isLoadedSceneSnd();
    void changeScene(SndSceneMgr::ESnd_Scene scene);
    void setMainBgmNo(int);
    void setBgmMode(int);
    void updateBgmInfo();
    bool isBgmAccentSign(u8);
    void pauseOffMove(int);
    void pauseMove(int);

    int getRemotePlayer(int);
    mVec2_c cvtSndObjctPos(const mVec2_c &);
    mVec2_c cvtSndObjctPos(const mVec3_c &);

    class SndObjctPlyBase_c : public SndObjctPly {
    public:
        SndObjctPlyBase_c() {
            m_00 = 5;
            m_04 = 6;
            m_08 = 0;
        }

        u32 m_00;
        u32 m_04;
        u8 m_08;
    };

    class SndObjctPly_c : public SndObjctPlyBase_c {
    public:
        virtual SoundHandlePrm *startSound(ulong p1, ulong p2) {
            return SndObjctPly::startSound(p1, p2);
        }
        virtual SoundHandlePrm *startSound(ulong p1, short p2, ulong p3) {
            return SndObjctPly::startSound(p1, p2, p3);
        }
        virtual SoundHandlePrm *holdSound(ulong p1, ulong p2) {
            return SndObjctPly::holdSound(p1, p2);
        }
        virtual SoundHandlePrm *holdSound(ulong p1, short p2, ulong p3) {
            return SndObjctPly::holdSound(p1, p2, p3);
        }
    };

    class SndObjctCmnEmy_c : public SndObjctCmnEmy {
    public:
        SoundHandlePrm *startSound(unsigned long soundID, const nw4r::math::VEC2 &pos, int remPlayer) {
            return SndObjctCmnEmy::startSound(soundID, pos, remPlayer);
        }
        SoundHandlePrm *startSound(unsigned long soundID, const mVec3_c &pos, int remPlayer) {
            return SndObjctCmnEmy::startSound(soundID, dAudio::cvtSndObjctPos(pos), remPlayer);
        }
    };

    class SndObjctEmy_c : public SndObjctEmy {
    public:
        virtual SoundHandlePrm *startSound(ulong p1, ulong p2) {
            return SndObjctEmy::startSound(p1, p2);
        }
        virtual SoundHandlePrm *startSound(ulong p1, short p2, ulong p3) {
            return SndObjctEmy::startSound(p1, p2, p3);
        }
        virtual SoundHandlePrm *holdSound(ulong p1, ulong p2) {
            return SndObjctEmy::holdSound(p1, p2);
        }
        virtual SoundHandlePrm *holdSound(ulong p1, short p2, ulong p3) {
            return SndObjctEmy::holdSound(p1, p2, p3);
        }
    };

    class SndObjctCmnMap_c : public SndObjctCmnMap {
    public:
        void startSound(unsigned long soundID, const nw4r::math::VEC2 &pos, int remPlayer) {
            SndObjctCmnMap::startSound(soundID, pos, remPlayer);
        }
        void startSound(unsigned long soundID, const mVec3_c &pos, int remPlayer) {
            SndObjctCmnMap::startSound(soundID, dAudio::cvtSndObjctPos(pos), remPlayer);
        }
    };

    extern SndObjctCmnEmy_c *g_pSndObjEmy;
    extern SndObjctCmnMap_c *g_pSndObjMap;

    /// @unofficial
    class SoundEffectID_t {
    public:
        SoundEffectID_t(u32 soundID) : id(soundID) {}

        operator u32() const {
            return id;
        }

        template<class T>
        void playObjSound(T *obj, const mVec2_c &pos, int playerNo) const {
            obj->startSound(id, dAudio::cvtSndObjctPos(pos), playerNo);
        }

        template<class T>
        void playObjSound(T *obj, const mVec3_c &pos, int playerNo) const {
            obj->startSound(id, dAudio::cvtSndObjctPos(pos), playerNo);
        }

        void playEmySound(const mVec2_c &pos, int playerNo) const {
            playObjSound(dAudio::g_pSndObjEmy, pos, playerNo);
        }

        void playEmySound(const mVec3_c &pos, int playerNo) const {
            playObjSound(dAudio::g_pSndObjEmy, pos, playerNo);
        }

        void playMapSound(const mVec2_c &pos, int playerNo) const {
            playObjSound(dAudio::g_pSndObjMap, pos, playerNo);
        }

        void playMapSound(const mVec3_c &pos, int playerNo) const {
            playObjSound(dAudio::g_pSndObjMap, pos, playerNo);
        }

    private:
        u32 id;
    };
};
