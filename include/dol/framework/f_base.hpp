#pragma once
#include <types.h>
#include <lib/egg/heap/eggFrmHeap.hpp>
#include <dol/framework/f_profile.hpp>
#include <dol/framework/f_base_id.hpp>
#include <dol/framework/f_helper_unk.hpp>
#include <dol/framework/f_manager.hpp>
#include <dol/framework/f_list_mg.hpp>

/**
 * @brief The base class for all scenes, actors and various other processes.
 * @ingroup framework dol
 * @details
 * ## Overview
 * fBase_c defines the core elements of a process, to provide common behaviour across all bases.
 *
 * The most significant components are:
 * - ::mUniqueID, a unique identifier for every created base.
 * - ::mParam, a 32-bit value used to configure the base. For profiles representing Reggie sprites, this
 * field is equivalent to nybbles 5-12 of the Reggie sprite data.
 * - ::mProfName, an identifier for the base's profile.
 * - ::mGroupType, which specifies the base's group type (see
 * [Inheritance and Group Types](#inheritance-and-group-types) for more details).
 * - The other fields are related to the base's lifecycle or are entirely unused.
 *
 * ## Creating Bases
 * A base can be created by calling the ::createRoot or the ::createChild functions. Derivative classes
 * offer their own implementations of these functions with additional parameters available; their use
 * is recommended.
 *
 * Overloaded @ref ::operator new "new" and @ref ::operator delete "delete" operators are provided
 * for convenience.
 *
 * ## Deleting Bases
 * Bases can be deleted by calling ::deleteRequest on them. A base will be informed of its impending
 * deletion when its ::deleteReady method is called. Please note that deleting a base will delete all
 * its children as well.
 *
 * ## Iterating Bases
 * A small iteration API is provided:
 * - Use ::getConnectParent to get a base's parent.
 * - Use ::getConnectChild to get a base's first child.
 * - Use ::getConnectBrNext to get a base's next sibling.
 *
 * For conducting global base searches, please refer to fManager_c.
 *
 * ## Inheritance and Group Types
 * The ::mGroupType field offers basic type information about a base:
 * - Bases with group type ::OTHER are generic processes, and inherit from dBase_c.
 * - Bases with group type ::SCENE are scene profiles, and inherit from dScene_c.
 * - Bases with group type ::ACTOR are game entities, and inherit from dBaseActor_c.
 *
 * No bases inherit fBase_c directly.
 *
 * ## Base Lifecycle
 * The lifecycle of a base consists of multiple operations, whose behaviour can be overridden at any
 * point. Each operation has an @ref fManager_c "associated linked list", containing all bases for which
 * said operation is scheduled for the current frame. fBase_c manages operation scheduling internally,
 * therefore developer interaction is not required.
 * 
 * ### Operation Flow
 * Every operation is composed by three steps: @p pre , @p do and @p post (each with their own methods).
 * While the @p do method is generally reserved for profile-specific behaviour, the @p pre and @p post
 * methods are commonly used to supply shared behaviour in base classes.
 *
 * The @p pre and @p do steps return a ::PACK_RESULT_e value, which is converted to a ::MAIN_STATE_e value
 * to determine the result of the operation. Said value is then passed to the @p post method, which acts
 * accordingly.
 * @p pre step result | @p do step result | @p post argument
 * :-----------------: | :----------------: | :--------------:
 * ::NOT_READY | <i>N/A (not run)</i> | ::CANCELED
 * ::SUCCEEDED | ::NOT_READY | ::WAITING
 * ::SUCCEEDED | ::SUCCEEDED | ::SUCCESS
 * ::SUCCEEDED | ::FAILED | ::ERROR
 * Operations are carried out by the ::commonPack function.
 *
 * ### Core Operations
 * fBase_c defines five core operations:
 * - @p create runs immediately after construction, and can be used to set up the base or load resources
 * for it. The operation result leads to three possible outcomes:
 *  - If the operation result is ::SUCCESS, the base enters the main execution cycle.
 *  - If the operation result is ::CANCELED or ::WAITING, the operation is repeated the next frame.
 *  - If the operation result is ::ERROR, the base is deleted.
 *
 * - @p execute serves as the base's own main loop, running every frame. This operation can be skipped by
 * enabling the relevant ::PROC_DISABLE_e flag.
 * - @p draw offers an alternative main loop specifically for rendering code. It also runs every frame,
 * and can be skipped by enabling the relevant ::PROC_DISABLE_e flag.
 * - @p delete runs immediately before destruction, and can be used to deallocate resources or remove
 * links to other bases. This operation will not proceed until all the base's children are deleted.
 * - @p connect is an internal operation for process management that runs along the previous four;
 * bases should not override it. See ::connectProc for more information on the tasks carried out in this
 * operation.
 * 
 * @image html dol/framework/fBase_lifecycle.svg The lifecycle of a base.
 * 
 * ## Unused Content
 * - ::sLoadAsyncCallback and ::sUnloadCallback are presumably related to the scrapped relocatable
 * profile system (more details here). These callbacks are set to empty placeholder functions
 * by dBase_c::initLoader. Judging by their names, they were supposed to be called after a profile module
 * would have been loaded/unloaded.
 * - Each base supports having its own @ref ::mpHeap "heap". The heap name, translated from Japanese,
 * is <i>Heap that each process can have individually (fBase_c::mHeap)</i>. Two working methods for creating
 * this heap are still in the game (::entryFrmHeap, ::entryFrmHeapNonAdjust), but are unused. This per-base
 * allocation method was most likely discontinued in favour of mAllocator_c and its derivatives.
 * - Two additional unused list-like structures are present in the class: ::mpUnusedHelper and
 * ::mUnusedList. Since the symbols for the related functions have not yet been cracked, it's difficult
 * to tell what their purpose might have been.
 * 
 * @todo Link to unused relocation system article when it gets written.
 */
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
        ROOT_DISABLE_EXECUTE = 1, ///< Execution is disabled, and this is a root base.
        DISABLE_EXECUTE = 2, ///< Execution is disabled.
        ROOT_DISABLE_DRAW = 4, ///< Drawing is disabled, and this is a root base.
        DISABLE_DRAW = 8 ///< Drawing is disabled.
    };

    /// @brief The base's unique identifier.
    /// @details This value is incremented for every created base. Should it max out, the game will
    /// intentionally stall.
    fBaseID_e mUniqueID;
    u32 mParam; ///< A bitfield that configures the base's behaviour. [Represents nybbles 5 to 12 of Reggie's spritedata].
    ProfileName mProfName; ///< The base's profile name.

protected:
    u8 mLifecycleState; ///< The base's lifecycle state. Value is a ::LIFECYCLE_e.

    /// @brief If deletion of the base was requested, but the corresponding operation has not been
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

    fBaHelper_c *mpUnusedHelper; ///< @unused See [Unused Content](#unused-content).
    fLiMgBa_c mUnusedList; ///< @unused See [Unused Content](#unused-content).

    EGG::FrmHeap *mpHeap; ///< @unused The base's dedicated heap.

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
     * @unused Creates a heap of the given size for the base.
     * @details If the requested heap space is not available, the heap is adjusted to allocate all the
     * available memory. If that also fails, the base is deleted.
     * @param size The heap's size, or @p -1 to allocate all available space.
     * @param parentHeap The parent heap, or @p nullptr to use the current heap.
     * @return If the heap creation was successful.
     */
    virtual bool entryFrmHeap(unsigned long size, EGG::Heap *parentHeap);

    /**
     * @unused Creates a heap of the given size for the base.
     * @details Unlike ::entryFrmHeap, the base is immediately deleted if the requested space is not
     * available.
     * @param size The heap's size, or @p -1 to allocate all available space.
     * @param parentHeap The parent heap, or @p nullptr to use the current heap.
     * @return If the heap creation was successful.
     */
    virtual bool entryFrmHeapNonAdjust(unsigned long size, EGG::Heap *parentHeap);
    virtual bool createHeap(); ///< @unused [Does nothing].

    virtual ~fBase_c(); ///< Destroys the base.

public:
    /// @brief Requests deletion of the base.
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
     * - Propagate updates to ::mProcControl on the root base.
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
    static int (*sLoadAsyncCallback)(); ///< @unused See [Unused Content](#unused-content).
    static void (*sUnloadCallback)(); ///< @unused See [Unused Content](#unused-content).

private:
    static fBaseID_e m_rootUniqueID; ///< Unique ID counter for base construction. See ::mUniqueID.
    static u32 m_tmpCtParam; ///< Temporary storage for the next constructed base's params. See ::mParam.
    static ProfileName m_tmpCtProfName; ///< Temporary storage for the next constructed base's profile name. See ::mProfName.
    static u8 m_tmpCtGroupType; ///< Temporary storage for the next constructed base's group type. See ::mGroupType.
    static fTrNdBa_c *m_tmpCtConnectParent; ///< Temporary storage for the next constructed base's parent connect node.

    /// @cond
    friend class fManager_c;
    friend class fLiNdBa_c;
    friend class fTrMgBa_c;
    /// @endcond
};
