#pragma once

#include <game/bases/d_wm_actor.hpp>
#include <game/bases/d_wm_sv_mdl.hpp>

/// @ingroup bases
class dWmDemoActor_c : public dWmActor_c {
public:
    dWmDemoActor_c();

    ~dWmDemoActor_c();

    dWmDemoActor_c *FUN_800f6130(dWmDemoActor_c **outActor); ///< @unofficial

    virtual int doDelete();
    virtual int draw();
    virtual int create();
    virtual int GetActorType();

    virtual void setCutEndSpecific(int csId, bool p2); ///< @unofficial
    virtual void setCutEnd();

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
    void FUN_800f6820(const char *arc, const char *path, const char *mdlName);
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
    bool FUN_800f6e70(int *csList, int csCount);
    void clearCutEnd();
    bool checkCutEnd();

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
