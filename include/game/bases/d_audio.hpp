#pragma once
#include <game/framework/f_profile.hpp>
#include <lib/nw4r/math/vec.hpp>
#include <game/mLib/m_vec.hpp>

/// @file

/// @ingroup bases

template<int T>
class NMSndObject { // : NMSndObjectBase
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
    nw4r::math::VEC2 cvtSndObjctPos(const mVec3_c &);

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
    class ComboSELookup {
    public:
        ComboSELookup(u32 v0, u32 v1, u32 v2, u32 v3, u32 v4, u32 v5, u32 v6, u32 v7, u32 v8) {
            lookup[0] = v0;
            lookup[1] = v1;
            lookup[2] = v2;
            lookup[3] = v3;
            lookup[4] = v4;
            lookup[5] = v5;
            lookup[6] = v6;
            lookup[7] = v7;
            lookup[8] = v8;
        }

        template<class T>
        void playObjSound(T *obj, int i, const mVec3_c &pos, int playerNo) const {
            obj->startSound(lookup[i], dAudio::cvtSndObjctPos(pos), playerNo);
        }

        void playEmySound(int i, const mVec3_c &pos, int playerNo) const {
            playObjSound(dAudio::g_pSndObjEmy, i, pos, playerNo);
        }

        void playMapSound(int i, const mVec3_c &pos, int playerNo) const {
            playObjSound(dAudio::g_pSndObjMap, i, pos, playerNo);
        }

        u32 size() const { return 9; }

        u32 lookup[9];
    };
};
