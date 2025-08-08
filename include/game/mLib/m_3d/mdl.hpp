#pragma once
#include <game/mLib/m_3d/calc_ratio.hpp>
#include <game/mLib/m_3d/smdl.hpp>
#include <nw4r/g3d.h>

namespace m3d {
    class mdl_c : public smdl_c {

        class callback_c {
        public:
            virtual ~callback_c();
            virtual void timingA(ulong nodeId, nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResMdl resMdl);
            virtual void timingB(ulong nodeId, nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl);
            virtual void timingC(nw4r::math::MTX34 *mtx, nw4r::g3d::ResMdl resMdl);
        };

        class mdlCallback_c {
        public:
            mdlCallback_c();
            virtual ~mdlCallback_c();

            virtual void ExecCallbackA(nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResMdl resMdl, nw4r::g3d::FuncObjCalcWorld *cw);
            virtual void ExecCallbackB(nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl, nw4r::g3d::FuncObjCalcWorld *cw);
            virtual void ExecCallbackC(nw4r::math::MTX34 *mtx, nw4r::g3d::ResMdl resMdl, nw4r::g3d::FuncObjCalcWorld *cw);

            bool create(nw4r::g3d::ResMdl resMdl, mAllocator_c *allocator, size_t *pSize);
            void remove();
            void setBlendFrame(float blendFrame);
            void calcBlend();

            calcRatio_c mCalcRatio;
            int mNodeCount;
            nw4r::g3d::ChrAnmResult *mpNodeResults;
            callback_c *mpCallback;
            mAllocator_c *mpAllocator;
        };

    public:
        mdl_c();
        virtual ~mdl_c();

        virtual void remove();

        bool create(nw4r::g3d::ResMdl resMdl, mAllocator_c *allocator, ulong bufferOption, int viewCount, size_t *pSize);
        void setCallback(callback_c *callback);

        void setAnm(m3d::banm_c &anm);
        void setAnm(m3d::banm_c &anm, float blendFrame);

        void play();

    private:
        mdlCallback_c mCallback;
    };
}
