#pragma once
#include <types.h>
#include <lib/egg/core/eggFrmHeap.h>
#include <game/framework/f_profile.hpp>
#include <game/framework/f_base_id.hpp>
#include <game/framework/f_helper_unk.hpp>
#include <game/framework/f_manager.hpp>
#include <game/framework/f_list_mg.hpp>

#define ACTOR_PARAM_CONFIG(name, offset, size) static const u16 PARAM_##name = ((offset << 8) | size)
#define ACTOR_PARAM(name) ((mParam >> (PARAM_##name >> 8)) & ((1 << (PARAM_##name & 0xff)) - 1))

/// @brief The base class for all scenes, actors and various other processes.
/// @ingroup framework
class fBase_c {
public:

    /// @brief The possible lifecycle states.
    enum LIFECYCLE_e {
        CREATING, ///< The base's @p create operation has yet to conclude.
        ACTIVE, ///< The base is in the main execution cycle.
        DELETING, ///< The base's @p delete operation is about to run.
    };

    /// @brief The possible group types.
    enum GROUP_TYPE_e {
        OTHER, ///< The base is a @ref dBase_c "generic process".
        SCENE, ///< The base is a @ref dScene_c "scene".
        ACTOR ///< The base is an @ref dBaseActor_c "actor".
    };

    /// @brief The possible operation results.
    enum MAIN_STATE_e {
        CANCELED, ///< The operation was canceled early.
        ERROR, ///< The operation could not be completed.
        SUCCESS, ///< The operation was completed successfully.
        WAITING ///< The operation is waiting for something and cannot be completed yet.
    };

    /// @brief The possible operation step results.
    enum PACK_RESULT_e {
        NOT_READY, ///< The step could not completed at this time.
        SUCCEEDED, ///< The step was completed successfully.
        FAILED, ///< The step could not be completed.
    };

    /// @brief Controls if the @p execute and @p draw operations should be skipped.
    enum PROC_DISABLE_e {
        ROOT_DISABLE_EXECUTE = BIT_FLAG(0), ///< Execution is disabled, and this is a root base.
        DISABLE_EXECUTE      = BIT_FLAG(1), ///< Execution is disabled.
        ROOT_DISABLE_DRAW    = BIT_FLAG(2), ///< Drawing is disabled, and this is a root base.
        DISABLE_DRAW         = BIT_FLAG(3) ///< Drawing is disabled.
    };

    /// @brief The base's unique identifier.
    /// @details This value is incremented for every created base. Should it reach @ref fBaseID_e::BASE_ID_MAX,
    /// the game will intentionally stall.
    fBaseID_e mUniqueID;
    u32 mParam; ///< A bitfield that configures the base's behaviour. Its usage varies from profile to profile.
    ProfileName mProfName; ///< The base's profile name.

protected:
    u8 mLifecycleState; ///< The base's lifecycle state. Value is a ::LIFECYCLE_e.

    /// @brief If deletion of the base was requested, but the @p delete operation has not been
    /// scheduled yet.
    bool mDeleteRequested;

    /// @brief If the @p create operation was completed, but scheduling the @p execute and @p draw
    /// operations isn't possible at this time.
    /// @details If true, scheduling will be deferred to the next @p connect operation.
    bool mDeferExecute;

    /// @brief If the @p create operation has not been completed, and rescheduling it isn't possible at
    /// this time.
    /// @details If true, rescheduling will be deferred to the next @p connect operation.
    bool mDeferRetryCreate;

    u8 mGroupType; ///< The base's group type. Value is a ::GROUP_TYPE_e.
    u8 mProcControl; ///< The operations to be skipped. Value is a ::PROC_DISABLE_e.

    /// @brief Checks if a flag is set in ::mProcControl.
    bool isProcControlFlag(u8 flag) const { return (mProcControl & flag) != 0; }
    /// @brief Sets a flag in ::mProcControl.
    void setProcControlFlag(u8 flag) { mProcControl |= flag; }
    /// @brief Clears a flag in ::mProcControl.
    void clearProcControlFlag(u8 flag) { mProcControl &= ~flag; }

    fManager_c mMng; ///< The base's process manager.

    fBaHelper_c *mpUnusedHelper; ///< See [Unused Content](#unused-content). @unused
    fLiMgBa_c mUnusedList; ///< See [Unused Content](#unused-content). @unused

    // [No p because of the string "fBase_c::mHeap"]
    EGG::FrmHeap *mHeap; ///< The base's dedicated heap. @unused

public:
    fBase_c(); ///< Constructs a new base.

    /// @brief @p new operator override for all bases.
    /// @details Bases are allocated in mHeap::g_gameHeaps[0] in a top-down direction, and are
    /// zero-initialized.
    static void *operator new(size_t);
    static void operator delete(void *); ///< @p delete operator override for all bases.

protected:
    /// @brief @p do method for the @p create operation.
    /// @return A ::PACK_RESULT_e value.
    virtual int create();

    /// @brief @p pre method for the @p create operation.
    /// @return A ::PACK_RESULT_e value.
    virtual int preCreate();

    /// @brief @p post method for the @p create operation.
    virtual void postCreate(MAIN_STATE_e state);

    /// @brief @p do method for the @p delete operation.
    /// @details This method was renamed due to conflict with the @p delete C++ keyword.
    /// @return A ::PACK_RESULT_e value.
    virtual int doDelete();

    /// @brief @p pre method for the @p delete operation.
    /// @return A ::PACK_RESULT_e value.
    virtual int preDelete();

    /// @brief @p post method for the @p delete operation.
    virtual void postDelete(MAIN_STATE_e state);

    /// @brief @p do method for the @p execute operation.
    /// @return A ::PACK_RESULT_e value.
    virtual int execute();

    /// @brief @p pre method for the @p execute operation.
    /// @return A ::PACK_RESULT_e value.
    virtual int preExecute();

    /// @brief @p post method for the @p execute operation.
    virtual void postExecute(MAIN_STATE_e state);

    /// @brief @p do method for the @p draw operation.
    /// @return A ::PACK_RESULT_e value.
    virtual int draw();

    /// @brief @p pre method for the @p draw operation.
    /// @return A ::PACK_RESULT_e value.
    virtual int preDraw();

    /// @brief @p post method for the @p draw operation.
    virtual void postDraw(MAIN_STATE_e state);

    /// @brief Informs the base that it's about to be deleted.
    virtual void deleteReady();

    /**
     * @brief Creates a heap of the given size for the base.
     * @unused
     * @details If the requested heap space is not available, the heap is adjusted to allocate all the
     * available memory. If that also fails, the base is deleted.
     * @param size The heap's size, or @p -1 to allocate all available space.
     * @param parentHeap The parent heap, or @p nullptr to use the current heap.
     * @return If the heap creation was successful.
     */
    virtual bool entryFrmHeap(unsigned long size, EGG::Heap *parentHeap);

    /**
     * @brief Creates a heap of the given size for the base.
     * @unused
     * @details Unlike ::entryFrmHeap, the base is immediately deleted if the requested space is not
     * available.
     * @param size The heap's size, or @p -1 to allocate all available space.
     * @param parentHeap The parent heap, or @p nullptr to use the current heap.
     * @return If the heap creation was successful.
     */
    virtual bool entryFrmHeapNonAdjust(unsigned long size, EGG::Heap *parentHeap);
    virtual bool createHeap(); ///< [Does nothing]. @unused

    virtual ~fBase_c(); ///< Destroys the base.

public:
    /// @brief Requests deletion of the base.
    /// @details Calling this function multiple times has no effect.
    void deleteRequest();

    fBase_c *getConnectParent() const; ///< Gets the base's parent.
    fBase_c *getConnectChild() const; ///< Gets the base's first child.
    fBase_c *getConnectBrNext() const; ///< Gets the base's next sibling.

    /// @brief Checks if the base has at least one child in the @ref ::LIFECYCLE_e::CREATING "CREATING" state.
    /// @return If such a child base exists.
    bool checkChildProcessCreateState() const;

private:
    int createPack(); ///< Executes the @p create operation. See ::commonPack.
    int deletePack(); ///< Executes the @p delete operation. See ::commonPack.
    int executePack(); ///< Executes the @p execute operation. See ::commonPack.
    int drawPack(); ///< Executes the @p draw operation. See ::commonPack.

    /**
     * @brief Executes an operation. See [here](#operation-flow) for more details.
     *
     * @param doFunc The operation's @p do method.
     * @param preFunc The operation's @p pre method.
     * @param postFunc The operation's @p post method.
     * @return A ::PACK_RESULT_e value returned from doFunc, or preFunc if doFunc was not executed.
     */
    int commonPack(int (fBase_c::*doFunc)(), int (fBase_c::*preFunc)(), void (fBase_c::*postFunc)(MAIN_STATE_e));

    /**
     * @brief Executes the @p connect operation.
     * @details This operation carries out the following tasks:
     * - Schedule the base (and its children) for deletion if deletion was requested (see ::mDeleteRequested)
     * - Propagate updates to the root base's ::mProcControl field down the tree
     * - Update the base's position in the @p execute and @p draw lists on priority changes
     * - Process any deferred schedule requests (see ::mDeferExecute and ::mDeferRetryCreate)
     * @return The operation always returns ::SUCCEEDED.
     */
    int connectProc();

    /// @brief Kickstarts the base's lifecycle by running the @p create operation.
    void runCreate();

    /// @brief Gets a child of the base in the @ref ::LIFECYCLE_e::CREATING "CREATING" state.
    /// @return The first child satisfying this condition, or @p nullptr if none is found.
    fBase_c *getChildProcessCreateState() const;

public:
    /**
     * @brief Creates a child base under the given parent.
     *
     * @param profName The base's profile name.
     * @param parent The base's parent. Must not be @p nullptr .
     * @param param The base's parameters.
     * @param groupType The base's group type.
     * @return A pointer to the instantiated base, or @p nullptr .
     */
    static fBase_c *createChild(ProfileName profName, fBase_c *parent, unsigned long param, u8 groupType);

    /**
     * @brief Creates a root base.
     *
     * @param profName The base's profile name.
     * @param param The base's parameters.
     * @param groupType The base's group type.
     * @return A pointer to the instantiated base, or @p nullptr .
     */
    static fBase_c *createRoot(ProfileName profName, unsigned long param, u8 groupType);

private:
    /**
     * @brief Sets temporary data to be used for the next base's construction.
     *
     * @param profName The base's profile name.
     * @param connectParent The connect node of the base's parent, or @p nullptr .
     * @param param The base's parameters.
     * @param groupType The base's group type.
     */
    static void setTmpCtData(ProfileName profName, fTrNdBa_c *connectParent, unsigned long param, u8 groupType);

    /**
     * @brief Internal function for base construction.
     *
     * @param profName The base's profile name.
     * @param connectParent The parent base's connect node.
     * @param param The base's parameters.
     * @param groupType The base's group type.
     * @return A pointer to the instantiated base, or @p nullptr .
     */
    static fBase_c *fBase_make(ProfileName profName, fTrNdBa_c *connectParent, unsigned long param, u8 groupType);

protected:
    static int (*sLoadAsyncCallback)(); ///< See [Unused Content](#unused-content). @unused
    static void (*sUnloadCallback)(); ///< See [Unused Content](#unused-content). @unused

private:
    static fBaseID_e m_rootUniqueID; ///< Unique ID counter for base construction. See ::mUniqueID.
    static u32 m_tmpCtParam; ///< Temporary storage for the next constructed base's params. See ::mParam.
    static ProfileName m_tmpCtProfName; ///< Temporary storage for the next constructed base's profile name. See ::mProfName.
    static u8 m_tmpCtGroupType; ///< Temporary storage for the next constructed base's group type. See ::mGroupType.
    static fTrNdBa_c *m_tmpCtConnectParent; ///< Temporary storage for the next constructed base's parent connect node.

    friend class fManager_c;
    friend class fLiNdBa_c;
    friend class fTrMgBa_c;
};
