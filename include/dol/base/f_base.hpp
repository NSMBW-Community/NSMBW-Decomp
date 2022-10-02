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
 * 
 * A class that inherits from fBase_c is called a "base".
 */
class fBase_c {
public:
    fBaseID_e mUniqueID; ///< A unique identifier for every base.
    u32 mParam; ///< Parameters that can be set to configure the behaviour. [These are the sprite data fields in Reggie]
    ProfileName mProfName; ///< The name of the profile.

    /// @brief The possible states in the lifecycle.
    enum LIFECYCLE_e {
        WAITING_FOR_CREATE,
        ACTIVE,
        TO_BE_DELETED
    };
    u8 mLifecycleState; ///< Current lifecycle state. Value is a ::LIFECYCLE_e.
    
    bool mDeleteRequested; ///< If the base is to be deleted.
    bool delayManageAdd; ///< If the adding of the base should be delayed until the next ::connectProc call.
    bool mRetryCreate; ///< If the next ::connectProc call should add the base to fManager_c::m_createManage.

    /// @brief The possible group types.
    enum GROUP_TYPE_e {
        SCENE = 1,
        ACTOR = 2,
        OTHER = 3
    };
    u8 mGroupType; ///< Current lifecycle state. Value is a ::GROUP_TYPE_e.
    u8 mProcessFlags; ///< Which process is to be executed. Value is a bitfield induced by fManager_c::PROC_FLAGS.

    fManager_c mMng; ///< This base's manager.

    fBaHelper_c *mpUnusedHelper; ///< [Unused.]
    fLiMgBa_c mUnusedList; ///< [Unused.]
    EGG::FrmHeap *mpHeap; ///< This object's own FrmHeap. [Seems unused]

    /// @brief The states commonPack can be in after running the @e pre and @e do methods.
    enum MAIN_STATE_e {
        UNSUCCESSFUL, ///< The @e pre method failed.
        ERROR, ///< The @e do method failed.
        SUCCESS, ///< The @e do method succeeded.
        WAITING // [TODO: document behaviour]
    };

public:
    fBase_c(); ///< Construct a new base.
    
    static void *operator new(size_t);
    static void operator delete(void *);
    
    int createPack(); ///< See ::commonPack.
    int deletePack(); ///< See ::commonPack.
    int executePack(); ///< See ::commonPack.
    int drawPack(); ///< See ::commonPack.

    /// @brief Deal with the connect tree and object lifecycle.
    int connectProc();

    /// @brief Request deletion of the base.
    void deleteRequest();
    
    fBase_c *getConnectParent(); ///< Get the parent base of this base in the connect tree.
    fBase_c *getConnectChild(); ///< Get the first child base of this base in the connect tree.
    fBase_c *getConnectBrNext(); ///< Get the next sibling base of this base in the connect tree.

    /// @brief Attempt to finalize creation of the base.
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

    /// @brief Check if a flag is set in ::mProcessFlags.
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

    virtual int create(); ///< Create the base.
    virtual int preCreate(); ///< To be executed before ::create.
    virtual void postCreate(MAIN_STATE_e state); ///< To be executed after ::create.

    virtual int doDelete(); ///< Delete the base.
    virtual int preDelete(); ///< To be executed before ::doDelete.
    virtual void postDelete(MAIN_STATE_e state); ///< To be executed after ::doDelete.

    virtual int execute(); ///< The main logic of the base.
    virtual int preExecute(); ///< To be executed before ::execute.
    virtual void postExecute(MAIN_STATE_e state); ///< To be executed after ::execute.

    virtual int draw(); ///< Draw the base.
    virtual int preDraw(); ///< To be executed before ::draw.
    virtual void postDraw(MAIN_STATE_e state); ///< To be executed after ::draw.

    /// @brief Inform the base that it will be deleted.
    virtual void deleteReady();

    // [TODO: document the exact purpose these functions serve]
    virtual bool entryFrmHeap(unsigned long size, EGG::Heap *parentHeap); ///< [TODO: document this method]
    virtual bool entryFrmHeapNonAdjust(unsigned long size, EGG::Heap *parentHeap); ///< [TODO: document this method]
    virtual bool createHeap(); ///< [TODO: document this method]

    virtual ~fBase_c(); ///< Destruct the base.

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
