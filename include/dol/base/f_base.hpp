#pragma once
#include <types.h>
#include <lib/egg/heap/frm_heap.hpp>
#include <dol/base/f_profile.hpp>
#include <dol/base/f_base_id.hpp>
#include <dol/base/f_helper_unk.hpp>
#include <dol/base/f_manager.hpp>
#include <dol/base/f_list_mg.hpp>

/**
 * @brief The base class for all scenes, actors, etc.
 * A class that inherits from fBase_c is called a "base".
 */
class fBase_c {
public:
    fBaseID_e mUniqueID; ///< A unique identifier for every base.
    u32 mParam; ///< Parameters that can be set to configure the behaviour. [These are the sprite data fields in Reggie]
    ProfileName mProfName; ///< The name of the profile.

    /// The possible states in the lifecycle.
    enum LIFECYCLE_e {
        WAITING_FOR_CREATE,
        ACTIVE,
        TO_BE_DELETED
    };
    u8 mLifecycleState; ///< Current lifecycle state. Value is a ::LIFECYCLE_e.
    
    bool mDeleteRequested; ///< If the base is to be deleted.
    bool delayManageAdd; ///< If the adding of the base should be delayed until the next ::connectProc call.
    bool mRetryCreate; ///< If the next ::connectProc call should add the base to fManager_c::m_createManage.

    /// The possible group types.
    enum GROUP_TYPE_e {
        SCENE = 1,
        ACTOR = 2,
        OTHER = 3
    };
    u8 mGroupType; ///< Current lifecycle state. Value is a ::GROUP_TYPE_e.
    u8 mProcessFlags; ///< Which process is to be executed. Value is a bitfield induced by fManager_c::PROC_FLAGS.

    fManager_c mMng; ///< This base's manager.

    fBaHelper_c *mpUnusedHelper;
    fLiMgBa_c mUnusedList;
    EGG::FrmHeap *mpHeap; ///< This object's own FrmHeap. [Seems unused]

    /// The states commonPack can be in after running the @e pre and @e do methods.
    enum MAIN_STATE_e {
        UNSUCCESSFUL, //< The @e pre method failed.
        ERROR, //< The @e post method failed.
        SUCCESS, //< The @e post method succeeded.
        WAITING //< [TODO: what does this mean?]
    };

public:
    fBase_c();
    
    static void *operator new(size_t);
    static void operator delete(void *);
    
    int createPack(); //< @see ::commonPack()
    int deletePack(); //< @see ::commonPack()
    int executePack(); //< @see ::commonPack()
    int drawPack(); //< @see ::commonPack()

    /// Deal with the connect tree and object lifecycle.
    int connectProc();

    /// Request deletion of the base.
    void deleteRequest();
    
    fBase_c *getConnectParent();
    fBase_c *getConnectChild();
    fBase_c *getConnectBrNext();

    /// Attempt to finalize creation of the base.
    void runCreate();
    
    // [Unofficial name]
    /**
     * @brief Get a child base that is waiting to be added to fManager_c::m_createManage.
     * 
     * @return A child base satisfying this condition, or else nullptr.
     */
    fBase_c *getNonReadyChild() const;

    // [Unofficial name]
    /**
     * @brief Check if the base has a child base which is waiting to be added to fManager_c::m_createManage.
     * 
     * @return If such a child base exists.
     */
    bool hasNonReadyChild() const;

    /// Check if a flag is set in ::mProcessFlags.
    bool isProcessFlag(u8 flag) const { return (mProcessFlags & flag) != 0; }

    /**
     * @brief Set data for the base which is constructed next.
     * 
     * @param profName The profile name.
     * @param connectParent The connect node of the parent base.
     * @param param The parameter for the base.
     * @param groupType The group type of the base.
     */
    static void setTmpCtData(ProfileName profName, fTrNdBa_c *connectParent, unsigned long param, u8 groupType);
    /**
     * @brief Instantiate a base under a parent base.
     * 
     * @param profName The profile name.
     * @param parent The parent base.
     * @param param The parameter for the base.
     * @param groupType The group type of the base.
     * @return A pointer to the instantiated base.
     */
    static fBase_c *createChild(ProfileName profName, fBase_c *parent, unsigned long param, u8 groupType);
    /**
     * @brief Instantiate a base as a root node in the connect tree.
     * 
     * @param profName The profile name.
     * @param param The parameter for the base.
     * @param groupType The group type of the base.
     * @return A pointer to the instantiated base.
     */
    static fBase_c *createRoot(ProfileName profName, unsigned long param, u8 groupType);

    static void (*sLoadAsyncCallback)(); ///< [Unused.]
    static void (*sUnloadCallback)(); ///< [Unused.]

private:
    /**
     * @brief The common code for the create/delete/execute/draw pack tasks.
     * 
     * @param doFunc The main function for this task.
     * @param preFunc The function that is called before the main function.
     * @param postFunc The function that is called after the main function.
     * @return The return value of doFunc, or of preFunc, if doFunc was not executed.
     */
    int commonPack(int (fBase_c::*doFunc)(), int (fBase_c::*preFunc)(), void (fBase_c::*postFunc)(MAIN_STATE_e));

    virtual int create();
    virtual int preCreate();
    virtual void postCreate(MAIN_STATE_e state);

    virtual int doDelete();
    virtual int preDelete();
    virtual void postDelete(MAIN_STATE_e state);

    virtual int execute();
    virtual int preExecute();
    virtual void postExecute(MAIN_STATE_e state);

    virtual int draw();
    virtual int preDraw();
    virtual void postDraw(MAIN_STATE_e state);

    /// Inform the base that it will be deleted.
    virtual void deleteReady();

    // [TODO: document the exact purpose these functions serve]
    virtual bool entryFrmHeap(unsigned long size, EGG::Heap *parentHeap);
    virtual bool entryFrmHeapNonAdjust(unsigned long size, EGG::Heap *parentHeap);
    virtual bool createHeap();

    virtual ~fBase_c();

    /**
     * @brief Instantiate a base.
     * 
     * @param profName The profile name.
     * @param connectParent The connect node of the parent base.
     * @param param The parameter for the base.
     * @param groupType The group type of the base.
     * @return A pointer to the instantiated base.
     */
    static fBase_c *fBase_make(ProfileName profName, fTrNdBa_c *connectParent, unsigned long param, u8 groupType);

    static fBaseID_e m_rootUniqueID; ///< The ID which the base that is created next will be given. \see ::mUniqueID
    static u32 m_tmpCtParam; ///< The parameter which the base that is created next will be given.
    static ProfileName m_tmpCtProfName; ///< The profile name which the base that is created next will be given.
    static u8 m_tmpCtGroupType; ///< The group type which the base that is created next will be given.
    static fTrNdBa_c *m_tmpCtConnectParent; ///< The parent which the base that is created next will be a child of.
};
