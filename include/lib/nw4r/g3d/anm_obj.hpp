#pragma once

#include <lib/nw4r/g3d/g3d_obj.hpp>
#include <lib/nw4r/g3d/res_anm.hpp>
#include <lib/nw4r/g3d/res_mdl.hpp>

namespace nw4r {
namespace g3d {

class ClrAnmResult;

class AnmObj : public G3dObj {
public:
    virtual ~AnmObj();
    virtual void G3dProc(unsigned long, unsigned long, void*);
    virtual void SetFrame(float);
    virtual float GetFrame() const;
    virtual void UpdateFrame();
    virtual void SetUpdateRate(float);
    virtual float GetUpdateRate() const;
    virtual bool Bind(nw4r::g3d::ResMdl);
    virtual void Release();
    virtual int GetResult(nw4r::g3d::ClrAnmResult *, unsigned long);
    virtual void Attach(int, nw4r::g3d::AnmObj *);
    virtual void Detach(int);

    enum AnmFlag {
        IS_BOUND = 4
    };

    bool TestAnmFlag(AnmFlag) const;

    AnmObj *getNode() const { return node; }

    AnmObj *node;
};

class AnmObjVisRes : public AnmObj {
public:    
    static AnmObjVisRes *Construct(MEMAllocator*, unsigned long*, nw4r::g3d::ResAnmVis, nw4r::g3d::ResMdl);

    NW4R_G3D_TYPE_OBJ_DECL(AnmObjVisRes);
};

class AnmObjMatClrRes : public AnmObj {
public:
    static AnmObjMatClrRes *Construct(MEMAllocator*, unsigned long*, nw4r::g3d::ResAnmClr, nw4r::g3d::ResMdl, bool);

    NW4R_G3D_TYPE_OBJ_DECL(AnmObjMatClrRes);
};

class AnmObjTexPatRes : public AnmObj {
public:
    static AnmObjTexPatRes *Construct(MEMAllocator*, unsigned long*, nw4r::g3d::ResAnmTexPat, nw4r::g3d::ResMdl, bool);

    NW4R_G3D_TYPE_OBJ_DECL(AnmObjTexPatRes);
};

class AnmObjTexSrtRes : public AnmObj {
public:
    static AnmObjTexSrtRes *Construct(MEMAllocator*, unsigned long*, nw4r::g3d::ResAnmTexSrt, nw4r::g3d::ResMdl, bool);

    NW4R_G3D_TYPE_OBJ_DECL(AnmObjTexSrtRes);
};

class AnmObjTexSrtOverride : public AnmObj {
public:
    static AnmObjTexSrtOverride *Construct(MEMAllocator*, unsigned long*, nw4r::g3d::ResMdl, int);

    int getCount() const { return count; }

    NW4R_G3D_TYPE_OBJ_DECL(AnmObjTexSrtOverride);

    char pad[0x10];
    int count;
    AnmObj **arr;
};

class AnmObjTexPatOverride : public AnmObj {
public:
    static AnmObjTexPatOverride *Construct(MEMAllocator*, unsigned long*, nw4r::g3d::ResMdl, int);

    int getCount() const { return count; }

    NW4R_G3D_TYPE_OBJ_DECL(AnmObjTexPatOverride);

    char pad[0x10];
    int count;
    AnmObj **arr;
};

class AnmObjMatClrOverride : public AnmObj {
public:
    static AnmObjMatClrOverride *Construct(MEMAllocator*, unsigned long*, nw4r::g3d::ResMdl, int);

    int getCount() const { return count; }

    NW4R_G3D_TYPE_OBJ_DECL(AnmObjMatClrOverride);

    char pad[0x10];
    int count;
    AnmObj **arr;
};

} // namespace g3d
} // namespace nw4r
