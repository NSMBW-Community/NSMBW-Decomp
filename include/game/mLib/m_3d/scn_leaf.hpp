#pragma once
#include <lib/nw4r/g3d/scn_leaf.hpp>
#include <lib/nw4r/math/vec.hpp>
#include <lib/nw4r/math/mtx.hpp>

namespace m3d {
    class scnLeaf_c {
    public:
        /// @unofficial
        enum leafType_e {
            TYPE_BMDL,
            TYPE_UNUSED,
            TYPE_PROC
        };

        scnLeaf_c();
        virtual ~scnLeaf_c();

        virtual leafType_e getType() const = 0;
        virtual void remove();
        virtual void entry();

        void setOption(unsigned long sel, unsigned long opt);
        bool getOption(unsigned long sel, unsigned long *opt) const;
        void setScale(float x, float y, float z);
        void setScale(const nw4r::math::VEC3 &scale);
        void setLocalMtx(const nw4r::math::MTX34 *mtx);
        void getLocalMtx(nw4r::math::MTX34 *mtx) const;
        void getViewMtx(nw4r::math::MTX34 *mtx) const;
        void calc(bool keepEnabledAfter);
        void calcVtx(bool keepEnabledAfter);
        void setPriorityDraw(int prioOpa, int prioXlu);

        nw4r::g3d::ScnObj *getScn() const { return mpScn; }

    protected:
        nw4r::g3d::ScnLeaf *mpScn;
    };
}
