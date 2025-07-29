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

    virtual void setCutEndSpecific(int csId, bool p2); ///< @unofficial
    virtual bool checkCutEnd() { return mIsCutEnd; }
    virtual void setCutEnd() { mIsCutEnd = true; }
    virtual void clearCutEnd() { mIsCutEnd = false; }

    bool isStaff();

    void InitJumpParam(const mVec3_c &v1, const mVec3_c &v2, int p3, float p4, float p5);
    void __initJumpParam1(const mVec3_c &v1, const mVec3_c &v2, int p3, float p4, float p5);
    void __initJumpParam2(const mVec3_c &v1, const mVec3_c &v2, int p3, float p4, float p5);

    void initJumpBase(mVec3_c v, int i, float f);
    bool procJumpBase();
    void setDirection(const mVec3_c &v);
    void rotDirectionY(short s, bool b);
    void rotDirectionX(short s, bool b);
    bool checkArriveTargetXYZ(const mVec3_c &v1, const mVec3_c &v2);
    bool checkArriveTargetXZ(const mVec3_c &v1, const mVec3_c &v2);
    void CreateShadowModel(const char *arc, const char *path, const char *mdlName, bool b);
    void CalcShadow(float yOffs, float scaleX, float scaleY, float scaleZ);
    void CalcShadow(float yOffs, float scale);
    void DrawShadow(bool b);
    void _initDemoJumpBaseCore(const mVec3_c &, int, int, float, float, float, const short &);
    void _initDemoJumpBase(const mVec3_c &, int, int, float, float, float, const mVec3_c &);
    bool _procDemoJumpBase();
    float GetBgPosY(const mVec3_c &, const mVec3_c &, int);
    void CsSPosSimple(int i, float f);
    void clearSpeedAll();
    void adjustHeightBase(const mVec3_c &v1, const mVec3_c &v2, int i);
    bool isCutscenePlaying(int *csList, int csCount); ///< @unofficial

private:
    bool m_00;
    bool mIsCutEnd;
    mHeapAllocator_c mHeapAllocator;
    m3d::smdl_c mModel;
    dWmSVMdl_c *mSvMdl;
    mVec3_c m_30;
    float m_3c;
    float m_40;
    float m_44;
    int m_48;
};
