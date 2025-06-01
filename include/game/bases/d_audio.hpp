#pragma once
#include <game/framework/f_profile.hpp>
#include <lib/nw4r/math/vec.hpp>
#include <game/mLib/m_vec.hpp>
#include <lib/nw4r/snd/snd_actor.hpp>
#include <game/snd/snd_audio_mgr.hpp>

/// @file

/// @ingroup bases

template <int T>
class NMSndObjectBase : public nw4r::snd::SoundActor {
public:
    enum OBJ_TYPE {
        OBJ_TYPE_0 = 0
    };

    NMSndObjectBase(OBJ_TYPE, nw4r::snd::SoundArchivePlayer &);
    virtual ~NMSndObjectBase();

    nw4r::snd::SoundArchivePlayer &mArcPlayer;
    u8 mPad[0x50];
    u32 mTotalCount;
    u32 m_58;
    OBJ_TYPE mObjType;
};

template<int T>
class NMSndObject : public NMSndObjectBase<T> {
    class SoundHandlePrm : public nw4r::snd::SoundHandle {};
public:
    NMSndObject<T>() :
        NMSndObjectBase<T>(NMSndObjectBase<T>::OBJ_TYPE_0, SndAudioMgr::sInstance->mArcPlayer),
        m_64(1.0f), m_68(0), m_6c(1.0f), m_70(1.0f)
    {
        SetPlayableSoundCount(0, T);
        mTotalCount = 2 + T;
        m_58 = 1;
    }

    virtual void startSound(unsigned long, const nw4r::math::VEC2 &, unsigned long);

    float m_64;
    u32 m_68;
    float m_6c;
    float m_70;
    SoundHandlePrm mParams[2 + T];
};

class SndObjctPly : public NMSndObject<4> {
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

    int getRemotePlayer(int);
    nw4r::math::VEC2 cvtSndObjctPos(const mVec3_c &);

    class SndObjctPly_c : SndObjctPly {
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
