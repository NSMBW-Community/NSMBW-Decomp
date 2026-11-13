#pragma once

#include <game/bases/d_enemy.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/bases/d_a_obj_center.hpp>
#include <game/bases/d_a_obj_center2.hpp>

/// @brief The base class for door actors.
/// @statetable
/// @paramtable
/// @ingroup bases
class daEnDoor_c : public dEn_c {
public:
    daEnDoor_c() : mIsOpen(false) {}
    
    /// @brief The possible door states.
    /// @unofficial
    enum DoorStatus_e {
        DOOR_STATUS_WAIT, ///< The door is ready to be opened.
        DOOR_STATUS_OPEN_OUT, ///< The door must open for players to enter it.
        DOOR_STATUS_OPEN_IN, ///< The door must open for players to exit it.
        DOOR_STATUS_CLOSED ///< The door must close.
    };

    virtual ~daEnDoor_c() override {}
    virtual int create() override;
    virtual int doDelete() override;
    virtual int execute() override;
    virtual int draw() override;
    virtual bool ActorDrawCullCheck() override { return false; }

    virtual bool isClosed() { return isState(StateID_Wait); } ///< Checks whether the door is closed.
    virtual bool isDummyOpen() { return false; } ///< Returns whether the door must act like fake Ghost House Doors.
    virtual void createMdl() {} ///< Creates the model and animation for the door.
    virtual void initAnm() {} ///< Initializes the animation for the door.
    virtual void initCcData() {} ///< Initializes collision data for the door.
    virtual void initialize() { mCc.entry(); } ///< Initializes other elements, called by create().
    virtual void waitProc() {} ///< Executed every frame during the Wait state.
    virtual void setOpenAnm() {} ///< Sets up the door open animation.
    virtual void setOpenSE() {} ///< Plays the door opening SFX.
    virtual void setCloseAnm() {} ///< Sets up the door close animation.
    virtual void setCloseSE() {} ///< Plays the door closed SFX.
    virtual void setCloseMoveSE() {} ///< Plays the door closing SFX.
    virtual void setWaitAnm() {} ///< Stops all animations for the door.

    /// @brief Moves the door along its parent's Z angle.
    /// @return The parent actor, or @p nullptr if the door is not parent controlled.
    dActor_c *rotation_move();
    void rotation_move(dActor_c *parent); ///< Moves the door along the given parent's Z angle.

    /// @brief Links the door to its parent.
    /// @return The parent actor, or @p nullptr if the door is not parent controlled.
    dActor_c *setParentInfo(u32 rotationID);
    void setParentInfo(dActor_c *parent); ///< Links the door to the given parent.

    void allocate(); ///< Wraps createMdl to allocate memory for the door's assets.
    void calcMdl(); ///< Updates mMatrix and mModel.
    bool checkOpenOk(); ///< Checks if the door is opening for players to enter it.

    STATE_FUNC_DECLARE(daEnDoor_c, Search); ///< Searching for the parent controller actor.
    STATE_FUNC_DECLARE(daEnDoor_c, Open); ///< Playing the open animation.
    STATE_FUNC_DECLARE(daEnDoor_c, Close); ///< Playing the close animation.
    STATE_FUNC_DECLARE(daEnDoor_c, Wait); ///< Waiting for players to interact with the door.
    STATE_FUNC_DECLARE(daEnDoor_c, Dummy); ///< Acting as a fake Ghost House door.

    static daObjCenter_c *searchParent_centerA(u32 rotationID); ///< Searches for a daObjCenter_c with the given Rotation ID.
    static daObjCenter2_c *searchParent_centerB(u32 rotationID); ///< Searches for a daObjCenter2_c with the given Rotation ID.
    static dActor_c *searchParent(u32 rotationID); ///< Searches for a daObjCenter_c or daObjCenter2_c with the given Rotation ID.
    static void ccCallback(dCc_c *self, dCc_c *other); ///< Collision callback for the door.

    dHeapAllocator_c mAllocator; ///< The allocator.
    nw4r::g3d::ResFile mResFile; ///< The resource file.
    m3d::mdl_c mModel; ///< The door model.
    m3d::anmChr_c mAnim; ///< The door model animation.
    fBaseID_e mParentUniqueID; ///< The unique ID of the parent actor if linked (daObjCenter_c or daObjCenter2_c).
    mVec3_c mDistanceToParent; ///< The distance between the parent controller's position and the door when it spawned.
    DoorStatus_e mDoorStatus; ///< The current status of the door.
    BOOL mIsOpen; ///< Whether the door finished opening or not.
    BOOL mMayOpen; ///< Whether the door may open.
    mAng mOrientation; ///< The orientation of the door.
    u8 mHasParent; ///< Whether the door is controlled by a parent actor.
    u8 mRotationID; ///< The rotation ID that links the door to the corresponding controller. @see daObjCenter_c::mRotationID, daObjCenter2_c::mRotationID

    ACTOR_PARAM_CONFIG(Orientation, 12, 2); ///< @copydoc mOrientation
    ACTOR_PARAM_CONFIG(HasParent, 8, 1); ///< @copydoc mHasParent
    ACTOR_PARAM_CONFIG(RotationID, 0, 8); ///< @copydoc mRotationID
};
