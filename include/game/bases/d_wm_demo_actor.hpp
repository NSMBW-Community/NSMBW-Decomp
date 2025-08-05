#pragma once

#include <game/bases/d_wm_actor.hpp>
#include <game/bases/d_wm_sv_mdl.hpp>

/// @ingroup bases
class dWmDemoActor_c : public dWmActor_c {
public:
    dWmDemoActor_c();

    ~dWmDemoActor_c() {}

    static dBaseActor_c *GetChildDemoActor(dBaseActor_c *prev, dWmDemoActor_c *&next); ///< @unofficial

    virtual int create() { return SUCCEEDED; }
    virtual int draw() { return SUCCEEDED; }
    virtual int doDelete() { return SUCCEEDED; }
    virtual int execute() = 0;
    virtual int GetActorType() { return ACTOR_MAP_DEMO; }

    virtual void setCutEndSpecific(int cutsceneId, bool param2); ///< @unofficial
    virtual bool checkCutEnd() { return mIsCutEnd; }
    virtual void setCutEnd() { mIsCutEnd = true; }
    virtual void clearCutEnd() { mIsCutEnd = false; }

    bool isStaff();

    void InitJumpParam(const mVec3_c &startPos, const mVec3_c &targetPos, int numFrames, float jumpSpeed, float maxYSpeed);
    void __initJumpParam1(const mVec3_c &startPos, const mVec3_c &targetPos, int numFrames, float jumpSpeed, float maxYSpeed);
    void __initJumpParam2(const mVec3_c &startPos, const mVec3_c &targetPos, int numFrames, float jumpSpeed, float maxYSpeed);
    void initJumpBase(mVec3_c pos, int numFrames, float jumpSpeed);
    bool procJumpBase();
    void _initDemoJumpBaseCore(const mVec3_c &pos, int delay, int frames, float jumpSpeed, float startScale, float targetScale, const short &angY);
    void _initDemoJumpBase(const mVec3_c &pos, int delay, int frames, float jumpSpeed, float startScale, float targetScale, const mVec3_c &dir);
    bool _procDemoJumpBase();

    void setDirection(const mVec3_c &dir);
    void rotDirectionY(short angle, bool is3D);
    void rotDirectionX(short angle, bool is3D);
    bool checkArriveTargetXYZ(const mVec3_c &startPos, const mVec3_c &targetPos);
    bool checkArriveTargetXZ(const mVec3_c &startPos, const mVec3_c &targetPos);

    void CreateShadowModel(const char *arc, const char *path, const char *mdlName, bool param4);
    void CalcShadow(float yOffs, float scaleX, float scaleY, float scaleZ);
    void CalcShadow(float yOffs, float scale);
    void DrawShadow(bool param1);

    float GetBgPosY(const mVec3_c &startPos, const mVec3_c &targetPos, int directionType);
    void CsSPosSimple(int directionType, float yTarget);
    void clearSpeedAll();
    void adjustHeightBase(const mVec3_c &startPos, const mVec3_c &targetPos, int directionType);
    bool isCutscenePlaying(const int *csList, int csCount); ///< @unofficial

protected:
    bool m_00;
    bool mIsCutEnd;
    mHeapAllocator_c mHeapAllocator;
    m3d::smdl_c mModel;
    dWmSVMdl_c *mSvMdl;
    mVec3_c mTargetPos;
    float mScaleCurr;
    float mScaleDelta;
    float mScaleTarget;
    int mScaleDelay;
};
