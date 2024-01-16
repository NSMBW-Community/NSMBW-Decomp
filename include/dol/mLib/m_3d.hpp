#pragma once

#include <dol/mLib/m_allocator.hpp>
#include <lib/egg/gx/eggLightManager.hpp>
#include <lib/egg/gx/eggFogManager.hpp>
#include <lib/nw4r/g3d/anm_obj.hpp>
#include <lib/nw4r/g3d/anm_obj_chr.hpp>
#include <lib/nw4r/g3d/scn_leaf.hpp>
#include <lib/nw4r/g3d/scn_mdl_simple.hpp>
#include <lib/nw4r/g3d/scn_proc.hpp>
#include <lib/nw4r/g3d/scn_root.hpp>
#include <lib/nw4r/g3d/res_anm.hpp>
#include <lib/nw4r/g3d/world.hpp>
#include <lib/rvl/gx/GX.h>

#include <_dummy_classes.hpp>

/// @ingroup mlib
namespace m3d {
    class internal {
    public:
        static mAllocator_c *l_allocator_p;
        static nw4r::g3d::ScnRoot *l_scnRoot_p;
        static unsigned int l_numLightMgr;
        static EGG::LightManager **l_lightMgr_pp;
        static unsigned int l_numFogMgr;
        static EGG::FogManager **l_fogMgr_pp;
    };

    class calcRatio_c {
    public:
        calcRatio_c();
        virtual ~calcRatio_c() {} // change to {} later

        bool isEnd() const;
        void calc();
        void set(float);
        void remove();
        void reset();
        void offUpdate();
    
        float f1, f2, f3, mScaleSel, mScaleAnm, mSlerpParam;
        bool b1, b2;
    
        float getScaleSel() { return mScaleSel; }
        float getScaleAnm() { return mScaleAnm; }
        float getSlerpParam() { return mSlerpParam; }
    };

    class capture_c {
    public:
        virtual ~capture_c();
    
        bool create(EGG::Heap *, u16, u16, GXTexFmt, GXTexFilter, bool);
        void remove();
        void capture(u16, u16, bool);
        void captureEx(u16, u16, GXTexFmt, GXTexFmt, GXTexFilter, bool, u16, u16, bool);

        GXTexObj texObj;
        void *texBuf;
        int bufferSize;
        u16 mTexWidth;
        u16 mTexHeight;
        GXTexFmt texFmt;
        GXTexFilter texFilter;
        bool texHalfSize;
    };
    
    class scnLeaf_c {
    public:
        scnLeaf_c();
        virtual ~scnLeaf_c();

        virtual int getType() const = 0;
        virtual void remove();
        virtual void entry();

        void setOption(unsigned long sel, unsigned long opt);
        bool getOption(unsigned long sel, unsigned long *opt);
        void setScale(float x, float y, float z);
        void setScale(const nw4r::math::VEC3 &scale);
        void setLocalMtx(const nw4r::math::MTX34 *mtx);
        void getLocalMtx(nw4r::math::MTX34 *mtx) const;
        void getViewMtx(nw4r::math::MTX34 *mtx) const;
        void calc(bool);
        void calcVtx(bool);
        void setPriorityDraw(int prioOpa, int prioXlu);

    //protected:
        nw4r::g3d::ScnLeaf *mpScn;
    };

    class banm_c {
    public:
        banm_c() : mpObj(nullptr), mpHeap(nullptr) {}
        virtual ~banm_c();
        virtual int getType() const = 0;
        virtual void remove();
        virtual void play();
    
        bool createAllocator(mAllocator_c *, unsigned long *);
        bool IsBound() const;
        float getFrame() const;
        void setFrameOnly(float frame);
        float getRate() const;
        void setRate(float rate);

        nw4r::g3d::AnmObj *getObj() { return mpObj; }
    
    //protected:
        nw4r::g3d::AnmObj *mpObj;
        EGG::FrmHeap *mpHeap;
        mAllocator_c mAllocator;
    };

    class bmdl_c : public scnLeaf_c {
    public:
        bmdl_c() : mpAnm(nullptr) {}
        virtual ~bmdl_c();
    
        virtual int getType() const;
        virtual void remove();
        virtual void setAnm(m3d::banm_c &);
        virtual void play();
    
        bool getNodeWorldMtx(unsigned long, nw4r::math::MTX34 *) const;
        bool getNodeWorldMtxMultVecZero(unsigned long, nw4r::math::VEC3 &) const;
    
        nw4r::g3d::ResMdl getResMdl() const;
        nw4r::g3d::ResMat *getResMat(unsigned long) const;
        void removeAnm(nw4r::g3d::ScnMdlSimple::AnmObjType);
    
        void setTevColor(unsigned long, _GXTevRegID, _GXColor, bool);
        void setTevColorAll(_GXTevRegID, _GXColor, bool);
        void setTevKColor(unsigned long, _GXTevKColorID, _GXColor, bool);
        void setTevKColorAll(_GXTevKColorID, _GXColor, bool);
    
    //protected:
        banm_c *mpAnm;
    };

    void proc_c_drawProc(nw4r::g3d::ScnProc *, bool);

    class proc_c : public scnLeaf_c {
    public:
        virtual void drawOpa();
        virtual void drawXlu();
    
        bool create(mAllocator_c *, unsigned long *);
    };

    enum playMode_e {
        DEFAULT = 0,
        REVERSE = 1,
        THREE = 3,
        FOUR = 4
    };
    
    class fanm_c : public banm_c {
    public:
        fanm_c();
    
        virtual ~fanm_c();
        virtual void play();
    
        void set(float, m3d::playMode_e, float, float);
        void setFrame(float);
        bool isStop() const;
        bool checkFrame(float) const;
    
        float mF1;
        float mF2;
        float mF3;
        u8 mPlayMode;
    };

    class anmChr_c : public fanm_c {
    public:
        
        bool create(nw4r::g3d::ResMdl, nw4r::g3d::ResAnmChr, mAllocator_c *, unsigned long *);
        void setAnm(m3d::bmdl_c &, nw4r::g3d::ResAnmChr, m3d::playMode_e);
        void setAnmAfter(m3d::bmdl_c &, nw4r::g3d::ResAnmChr, m3d::playMode_e);
        void setFrmCtrlDefault(nw4r::g3d::ResAnmChr &, m3d::playMode_e);

    };

    class anmChrBlend_c : public fanm_c {
    public:
        
        bool create(nw4r::g3d::ResMdl, int, mAllocator_c *, unsigned long *);
        void attach(int, nw4r::g3d::AnmObjChrRes *, float);
        void attach(int, m3d::anmChr_c *, float);
        void detach(int);
    };

    class anmVis_c : public fanm_c {
    public:
        
        bool create(nw4r::g3d::ResMdl, nw4r::g3d::ResAnmVis, mAllocator_c *, unsigned long *);
        void setAnm(m3d::bmdl_c &, nw4r::g3d::ResAnmVis, m3d::playMode_e);
        void setFrmCtrlDefault(nw4r::g3d::ResAnmVis &, m3d::playMode_e);
    };

    class anmMatClr_c : public banm_c {
    public:
        virtual ~anmMatClr_c();
        virtual void remove();
        virtual void play();
        virtual int getType() const;
    
        static unsigned long heapCost( nw4r::g3d::ResMdl, nw4r::g3d::ResAnmClr, long, bool );
        bool create( nw4r::g3d::ResMdl, nw4r::g3d::ResAnmClr, mAllocator_c*, unsigned long*, long );
        void setAnm( m3d::bmdl_c&, nw4r::g3d::ResAnmClr, long, m3d::playMode_e );
        void releaseAnm( long );
        void play( long );
        float getFrame( long ) const;
        void setFrame( float, long );
        float getRate( long ) const;
        void setRate( float, long );
        bool isStop( long ) const;
        bool checkFrame( float, long ) const;
        void setPlayMode( m3d::playMode_e, long );
        float getFrameMax( long ) const;
        float getFrameStart( long ) const;
    
        class child_c : public fanm_c {
        public:
            virtual int getType( void ) const;// { return 2; };
            virtual ~child_c();// {}
    
            static unsigned long heapCost( nw4r::g3d::ResMdl, nw4r::g3d::ResAnmClr, bool );
            bool create( nw4r::g3d::ResMdl, nw4r::g3d::ResAnmClr, mAllocator_c*, unsigned long* );
            void setAnm( m3d::bmdl_c&, nw4r::g3d::ResAnmClr, m3d::playMode_e );
            void releaseAnm( void );
            void setFrmCtrlDefault( nw4r::g3d::ResAnmClr&, m3d::playMode_e );
        };
        
        child_c *children;
    };
    
    class anmTexPat_c : public banm_c {
    public:
        virtual ~anmTexPat_c();
        virtual void remove();
        virtual void play();
        virtual int getType() const;
    
        static unsigned long heapCost( nw4r::g3d::ResMdl, nw4r::g3d::ResAnmTexPat, long, bool );
        bool create( nw4r::g3d::ResMdl, nw4r::g3d::ResAnmTexPat, mAllocator_c*, unsigned long*, long );
        void setAnm( m3d::bmdl_c&, nw4r::g3d::ResAnmTexPat, long, m3d::playMode_e );
        void releaseAnm( long );
        void play( long );
        float getFrame( long ) const;
        void setFrame( float, long );
        float getRate( long ) const;
        void setRate( float, long );
        bool isStop( long ) const;
        bool checkFrame( float, long ) const;
        void setPlayMode( m3d::playMode_e, long );
        float getFrameMax( long ) const;
        float getFrameStart( long ) const;
    
        class child_c : public fanm_c {
        public:
            virtual int getType( void ) const;// { return 3; };
            virtual ~child_c();// {}
    
            static unsigned long heapCost( nw4r::g3d::ResMdl, nw4r::g3d::ResAnmTexPat, bool );
            bool create( nw4r::g3d::ResMdl, nw4r::g3d::ResAnmTexPat, mAllocator_c*, unsigned long* );
            void setAnm( m3d::bmdl_c&, nw4r::g3d::ResAnmTexPat, m3d::playMode_e );
            void releaseAnm( void );
            void setFrmCtrlDefault( nw4r::g3d::ResAnmTexPat&, m3d::playMode_e );
        };
        
        child_c *children;
    };
    
    class anmTexSrt_c : public banm_c {
    public:
        virtual ~anmTexSrt_c();
        virtual void remove();
        virtual void play();
        virtual int getType() const;
    
        static unsigned long heapCost( nw4r::g3d::ResMdl, nw4r::g3d::ResAnmTexSrt, long, bool );
        bool create( nw4r::g3d::ResMdl, nw4r::g3d::ResAnmTexSrt, mAllocator_c*, unsigned long*, long );
        void setAnm( m3d::bmdl_c&, nw4r::g3d::ResAnmTexSrt, long, m3d::playMode_e );
        void releaseAnm( long );
        void play( long );
        float getFrame( long ) const;
        void setFrame( float, long );
        float getRate( long ) const;
        void setRate( float, long );
        bool isStop( long ) const;
        bool checkFrame( float, long ) const;
        void setPlayMode( m3d::playMode_e, long );
        float getFrameMax( long ) const;
        void setFrameStart( long, float ) const;
    
        class child_c : public fanm_c {
        public:
            virtual int getType( void ) const;// { return 4; };
            virtual ~child_c();// {}
    
            static unsigned long heapCost( nw4r::g3d::ResMdl, nw4r::g3d::ResAnmTexSrt, bool );
            bool create( nw4r::g3d::ResMdl, nw4r::g3d::ResAnmTexSrt, mAllocator_c*, unsigned long* );
            void setAnm( m3d::bmdl_c&, nw4r::g3d::ResAnmTexSrt, m3d::playMode_e );
            void releaseAnm( void );
            void setFrmCtrlDefault( nw4r::g3d::ResAnmTexSrt&, m3d::playMode_e );
        };
        
        child_c *children;
    };
    
    class smdl_c : public bmdl_c {
    public:
        smdl_c();
        virtual ~smdl_c();
    
        bool create(nw4r::g3d::ResMdl mdl, mAllocator_c *allocator, unsigned long, int, unsigned long *);
    };
    
    class mdl_c : public smdl_c {
    
        class callback_c {
        public:
            virtual ~callback_c();
            virtual void timingA(unsigned long, nw4r::g3d::ChrAnmResult *, nw4r::g3d::ResMdl );
            virtual void timingB(unsigned long, nw4r::g3d::WorldMtxManip *, nw4r::g3d::ResMdl );
            virtual void timingC(nw4r::math::MTX34 *, nw4r::g3d::ResMdl);
        };
    
        class mdlCallback_c {
        public:
            mdlCallback_c();
            virtual ~mdlCallback_c();
    
            virtual void ExecCallbackA(nw4r::g3d::ChrAnmResult *, nw4r::g3d::ResMdl, nw4r::g3d::FuncObjCalcWorld *);
            virtual void ExecCallbackB(nw4r::g3d::WorldMtxManip *, nw4r::g3d::ResMdl, nw4r::g3d::FuncObjCalcWorld *);
            virtual void ExecCallbackC(nw4r::math::MTX34 *, nw4r::g3d::ResMdl, nw4r::g3d::FuncObjCalcWorld *);
    
    
            bool create(nw4r::g3d::ResMdl, mAllocator_c *, unsigned long *);
            void remove();
            void setBlendFrame(float);
            void calcBlend();
    
            calcRatio_c mCalcRatio;
            int countObjs;
            nw4r::g3d::ChrAnmResult *objs;
            callback_c *mpCallback;
            mAllocator_c *mpAllocator;
        };
        
    public:
        mdl_c();
        virtual ~mdl_c();
    
        virtual void remove();
    
        bool create(nw4r::g3d::ResMdl, mAllocator_c *, unsigned long, int, unsigned long *);
        void setCallback(callback_c *);
    
        void setAnm(m3d::banm_c &);
        void setAnm(m3d::banm_c &, float);
    
        void play();
    
    private:
        mdlCallback_c mCallback;
    };

    void initHio(const char *);
    bool create(EGG::Heap*, unsigned long, unsigned long, unsigned long, unsigned long);
    bool create(EGG::Heap*, GXPixelFmt, GXColor, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);
    bool createLightMgr(EGG::Heap*, unsigned short, unsigned short, unsigned char, bool, int);
    void removeLightMgr(int);
    bool createFogMgr(EGG::Heap*, int, int);
    void removeFogMgr(int);
    nw4r::g3d::ScnRoot *getScnRoot();
    nw4r::g3d::Camera getCamera(int);
    nw4r::g3d::Camera getCurrentCamera();
    int getCurrentCameraID();
    void setCurrentCamera(int);
    nw4r::g3d::LightSetting *getLightSettingP();
    EGG::LightManager *getLightMgr(int);
    EGG::FogManager *getFogMgr(int);
    void drawDone(int);
    void drawLightMapTexture(int);
    bool replaceLightMapTexture(const char *, m3d::bmdl_c &, int);
    void calcWorld(int);
    void calcMaterial();
    void calcView(int);
    void drawOpa();
    void drawXlu();
    void pushBack(nw4r::g3d::ScnObj *);
    void clear();
    void reset();
    int getNodeID(nw4r::g3d::ResMdl, const char *);
    void resetMaterial(); ///< Turns off all indirect texture processing.
}
