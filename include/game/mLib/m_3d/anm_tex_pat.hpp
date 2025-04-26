#pragma once
#include <game/mLib/m_3d/banm.hpp>
#include <game/mLib/m_3d/bmdl.hpp>
#include <game/mLib/m_3d/fanm.hpp>

namespace m3d {
    class anmTexPat_c : public banm_c {
    public:
        virtual ~anmTexPat_c();
        virtual void remove();
        virtual void play();
        virtual banm_c::anmType_e getType( void ) const { return banm_c::TYPE_ANM_TEX_PAT; };

        static size_t heapCost(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexPat anmTexPat, long count, bool calcAligned);
        bool create(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexPat anmTexPat, mAllocator_c *allocator, size_t *objSize, long count);
        void setAnm(m3d::bmdl_c &mdl, nw4r::g3d::ResAnmTexPat anmTexPat, long idx, m3d::playMode_e playMode);
        void releaseAnm(long idx);
        void play(long idx);
        float getFrame(long idx) const;
        void setFrame(float frame, long idx);
        float getRate(long idx) const;
        void setRate(float rate, long idx);
        bool isStop(long idx) const;
        bool checkFrame(float frame, long idx) const;
        void setPlayMode(m3d::playMode_e playMode, long idx);
        float getFrameMax(long idx) const;
        float getFrameStart(long idx) const;

        class child_c : public fanm_c {
        public:
            virtual banm_c::anmType_e getType( void ) const { return banm_c::TYPE_ANM_TEX_PAT; };
            virtual ~child_c() {}

            static size_t heapCost(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexPat anmTexPat, bool calcAligned);
            bool create(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexPat anmTexPat, mAllocator_c *allocator, size_t *objSize);
            void setAnm(m3d::bmdl_c &mdl, nw4r::g3d::ResAnmTexPat anmTexPat, m3d::playMode_e playMode);
            void releaseAnm();
            void setFrmCtrlDefault(nw4r::g3d::ResAnmTexPat &anmTexPat, m3d::playMode_e playMode);
        };

        child_c *children;
    };
}
