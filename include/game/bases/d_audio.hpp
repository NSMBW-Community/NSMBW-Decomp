#pragma once
#include <game/framework/f_profile.hpp>
#include <game/mLib/m_vec.hpp>
#include <nw4r/snd.h>
#include <nw4r/math.h>

/// @file

/// @ingroup bases

class NMSndObjectBase : nw4r::snd::SoundActor {
    u32 mNumHandles;
    u32 mRemotePlayer;
    void * mSnd2DCalc; // TODO: Snd2DCalc *
    u32 mObjType;
};

template<int T>
class NMSndObject : NMSndObjectBase {
public:
    virtual void startSound(unsigned long, const nw4r::math::VEC2 &, unsigned long);
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
    mVec2_c cvtSndObjctPos(const mVec3_c &);

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
