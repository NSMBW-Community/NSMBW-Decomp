#pragma once

#include <lib/nw4r/g3d/g3d_obj.hpp>
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

} // namespace g3d
} // namespace nw4r
