#pragma once
#include <game/framework/f_profile.hpp>
#include <lib/nw4r/math/vec.hpp>
#include <game/mLib/m_vec.hpp>
#include <lib/nw4r/snd/snd_actor.hpp>
#include <game/snd/snd_audio_mgr.hpp>

/// @file

/// @ingroup bases

class Snd2DCalc {
public:
    void fn_8019ee20(float &, nw4r::math::VEC2 &, unsigned long);
};

class NMSndObjectBase : public nw4r::snd::SoundActor {
public:
    enum OBJ_TYPE {
        OBJ_TYPE_0 = 0
    };

    NMSndObjectBase(OBJ_TYPE, nw4r::snd::SoundArchivePlayer &);
    virtual ~NMSndObjectBase();

    nw4r::snd::SoundArchivePlayer &mArcPlayer;
    u8 mPad[0x4c];
    u32 mTotalCount;
    u32 m_58;
    Snd2DCalc *mpSnd2dCalc;
    OBJ_TYPE mObjType;
};

template<int T>
class NMSndObject : public NMSndObjectBase {
    class SoundHandlePrm : public nw4r::snd::SoundHandle {};
public:

    NMSndObject() :
        NMSndObjectBase(NMSndObjectBase::OBJ_TYPE_0, SndAudioMgr::sInstance->mArcPlayer),
        m_64(1.0f), m_68(0), m_6c(1.0f), m_70(0.0f)
    {
        SetPlayableSoundCount(0, T);
        mTotalCount = T + 2;
        m_58 = 1;
        m_ac = T + 1;
        m_b0 = T + 2;
        m_b4 = 0;
    }

    virtual void calc(const nw4r::math::VEC2 &) {
        for (int i = 0; i < T; i++) {
            if (GetPlayingSoundCount(i) > 0) {
                mpSnd2dCalc->fn_8019ee20(m_64, mPos, 0);
                for (int sndIdx = 0; sndIdx < mTotalCount; sndIdx++) {
                    if (mParams[0].m_00 == 0) {
                        continue;
                    }
                    int idx = (mParams[0].m_00 == 0) ? -1 : mParams[0].m_00;
                    u32 flag = SndAudioMgr::sInstance->get3DCtrlFlag(idx);
                    if ((~flag & 1) == 0) {

                    }
                }
                return;
            }
        }
    }

    virtual void startSound(unsigned long, const nw4r::math::VEC2 &, unsigned long);

    float m_64;
    u32 m_68;
    float m_6c;
    float m_70;
    SoundHandlePrm mParams[T + 2];
    nw4r::math::VEC2 mPos;
    u32 m_ac;
    u32 m_b0;
    u8 m_b4;
};

class SndObjctPly : public NMSndObject<4> {
public:
    void calculate(const nw4r::math::VEC2 &pos) {
        NMSndObject<4>::calc(pos);
    }

    void stopPlyJumpSound();
};

class SndObjctCmnEmy : public NMSndObject<4> {
public:
    virtual void startSound(unsigned long, const nw4r::math::VEC2 &, unsigned long);
};

class SndObjctCmnMap : public NMSndObject<4> {
public:
    virtual void startSound(unsigned long, const nw4r::math::VEC2 &, unsigned long);
};

namespace dAudio {
    void requestStartScene(ProfileName sceneProf); ///< Sets up game audio for the given scene.
    void prepareSceneSnd(); ///< @unofficial
    void destroySceneSnd(); ///< @unofficial
    void setNextScene(ProfileName prof, unsigned long bgmIndex);
    void pauseOffGameWithReset(); ///< @unofficial
    bool isBgmAccentSign(u8);

    int getRemotePlayer(int);
    nw4r::math::VEC2 cvtSndObjctPos(const mVec3_c &);

    class SndObjctPlyBase_c : public SndObjctPly {
    };

    class SndObjctPly_c : public SndObjctPlyBase_c {
    };

    class SndObjctCmnEmy_c : SndObjctCmnEmy {
    public:
        void startSound(unsigned long soundID, const nw4r::math::VEC2 &pos, int remPlayer) {
            SndObjctCmnEmy::startSound(soundID, pos, remPlayer);
        }
        void startSound(unsigned long soundID, const mVec3_c &pos, int remPlayer) {
            SndObjctCmnEmy::startSound(soundID, dAudio::cvtSndObjctPos(pos), remPlayer);
        }
    };

    class SndObjctCmnMap_c : SndObjctCmnMap {
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
        void playObjSound(T *obj, const mVec3_c &pos, int playerNo) const {
            obj->startSound(id, dAudio::cvtSndObjctPos(pos), playerNo);
        }

        void playEmySound(const mVec3_c &pos, int playerNo) const {
            playObjSound(dAudio::g_pSndObjEmy, pos, playerNo);
        }

        void playMapSound(const mVec3_c &pos, int playerNo) const {
            playObjSound(dAudio::g_pSndObjMap, pos, playerNo);
        }

    private:
        u32 id;
    };
};
