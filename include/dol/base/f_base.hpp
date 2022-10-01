#pragma once
#include <types.h>
#include <lib/egg/heap/heap.hpp>
#include <dol/profile/profile.hpp>
#include <dol/base/f_base_id.hpp>
#include <dol/base/f_helper_unk.hpp>
#include <dol/base/f_manager.hpp>
#include <dol/base/f_list_mg.hpp>

class fBase_c {
public:
    fBaseID_e mUniqueID;
    u32 mParam;
    Profile mProfName;

    u8 mLifecycleState; // 0: object has not yet been added to the fManager_c lists
                        // 1: object has been added to the fManager_c lists
                        // 2: object has been removed from the fManager_c lists
    bool mWantsDelete;
    bool mIsNotDeferred;
    bool mIsDeferred;
    u8 mGroupType; // 1: scene, 2: actor, 3: other
    u8 mProcessFlags; // &1 = connect, &2 = create, &4 = execute, &8 = delete, &16 = draw

    fManager_c mMng;

    fBaHelper_c *mpUnusedHelper;
    fLiMgBa_c mUnusedList;
    EGG::Heap *mpHeap;

    enum MAIN_STATE_e {
        UNSUCCESSFUL,
        ERROR,
        SUCCESS,
        WAITING
    };

public:
    fBase_c();
    
    static void *operator new(unsigned long);
    static void operator delete(void *);

    virtual int create();
    virtual bool preCreate();
    virtual void postCreate(MAIN_STATE_e state);

    void createPack();

    virtual int doDelete();
    virtual bool preDelete();
    virtual void postDelete(MAIN_STATE_e state);
    
    void deletePack();

    virtual int execute();
    virtual bool preExecute();
    virtual void postExecute(MAIN_STATE_e state);
    
    void executePack();

    virtual int draw();
    virtual bool preDraw();
    virtual void postDraw(MAIN_STATE_e state);
    
    void drawPack();

    virtual void deleteReady();

    void connectProc();
    void deleteRequest();
    
    fBase_c *getConnectParent();
    fBase_c *getConnectChild();
    fBase_c *getConnectBrNext();

    virtual bool entryFrmHeap(u32 size, void *parentHeap);
    virtual bool entryFrmHeapNonAdjust(u32 size, void *parentHeap);
    virtual void createHeap();
    
    virtual ~fBase_c();

    void runCreate();

    static void setTmpCtData(Profile prof, fTrNdBa_c *connectParent, u32 param, u8 groupType);
    static fBase_c *fBase_make(Profile prof, fTrNdBa_c *connectParent, u32 param, u8 groupType);
    static fBase_c *createChild(Profile prof, fBase_c *parent, u32 param, u8 groupType);
    static fBase_c *createRoot(Profile prof, u32 param, u8 groupType);

private:
    // Unofficial name
    void commonPack(bool (fBase_c::*preFunc)(), int (fBase_c::*doFunc)(), bool (fBase_c::*postFunc)(MAIN_STATE_e));

    static fBaseID_e m_rootUniqueID;
    static u32 m_tmpCtParam;
    static Profile m_tmpCtProfName;
    static u8 m_tmpCtGroupType;
    static fTrNdBa_c *m_tmpCtConnectParent;
};