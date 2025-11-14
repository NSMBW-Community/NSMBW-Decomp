#pragma once
#include <game/framework/f_profile.hpp>
#include <game/snd/snd_audio_mgr.hpp>
#include <game/snd/snd_scene_manager.hpp>
#include <game/mLib/m_vec.hpp>
#include <nw4r/snd.h>
#include <nw4r/math.h>

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
    virtual u8 vf1C(ulong, int);

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
        m_ac = T + 1;
        m_b0 = T + 2;
        m_b4 = 0;
    }

    void processParams() {
        for (int idx = 0; idx < mTotalCount; idx++) {
            if (!mParams[idx].IsAttachedSound()) {
                continue;
            }
            u32 flag = SndAudioMgr::sInstance->get3DCtrlFlag(mParams[idx].GetId());
            if (~flag & 1) {
                mParams[idx].SetVolume(m_64, 0);
            } else if (
                SndSceneMgr::sInstance->m_14 == 3 ||
                (SndSceneMgr::sInstance->m_14 == 2 && SndSceneMgr::sInstance->m_10 == 3)
            ) {
                mParams[idx].SetVolume(m_64, 0);
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
                mpSnd2dCalc->fn_8019ee20(m_64, mPos, 0);
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

    float m_64;
    int m_68;
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

    virtual SoundHandlePrm *startSound(ulong p1, ulong p2);
    virtual SoundHandlePrm *holdSound(ulong p1, ulong p2);
    virtual SoundHandlePrm *startSound(ulong p1, short p2, ulong p3);
    virtual SoundHandlePrm *holdSound(ulong p1, short p2, ulong p3);
    virtual SoundHandlePrm *startSound(ulong p1, const nw4r::math::VEC2 &p2, ulong p3);
    virtual SoundHandlePrm *holdSound(ulong p1, const nw4r::math::VEC2 &p2, ulong p3);

    void stopPlyJumpSound();
    void startFootSound(ulong, float, ulong);
    void fn_8019AAB0(ulong, int);
    void fn_8019ABB0(ulong, int);
};

class SndObjctCmnEmy : public NMSndObject<4> {
public:
    virtual SoundHandlePrm *startSound(unsigned long, const nw4r::math::VEC2 &, unsigned long);
};

class SndObjctCmnMap : public NMSndObject<4> {
public:
    virtual SoundHandlePrm *startSound(unsigned long, const nw4r::math::VEC2 &, unsigned long);
};

namespace dAudio {
    void requestStartScene(ProfileName sceneProf); ///< Sets up game audio for the given scene.
    void createSndObjctCmn();
    void deleteSndObjctCmn();
    void setNextScene(ProfileName prof, unsigned long bgmIndex);
    void pauseOffGameWithReset(); ///< @unofficial
    void boot();
    void loadSceneSnd();
    bool isLoadedSceneSnd();
    void FUN_8006a6a0(bool); ///< @unofficial
    bool isBgmAccentSign(u8);

    int getRemotePlayer(int);
    mVec2_c cvtSndObjctPos(const mVec2_c &);
    mVec2_c cvtSndObjctPos(const mVec3_c &);

    class SndObjctPlyBase_c : public SndObjctPly {
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
