#pragma once

#include <game/bases/d_enemy.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/bases/d_a_obj_center.hpp>
#include <game/bases/d_a_obj_center2.hpp>

class daEnDoor_c : public dEn_c {
public:
    daEnDoor_c() : mIsOpen(false) {}

    /// @unofficial
    enum DoorStatus_e {
        DOOR_STATUS_WAIT, ///< Set when entering the Wait state. Indicates that the door is ready to be opened.
        DOOR_STATUS_OPEN_OUT, ///< Set by the player. Makes the door enter the Open state. Indicates that the door is getting opened for players to enter it.
        DOOR_STATUS_OPEN_IN, ///< Set by the player. Makes the door enter the Open state. Indicates that the door is getting opened for players to exit it.
        DOOR_STATUS_CLOSED ///< Set by the player. Makes the door enter the Close state if it finished its opening animation.
    };

    virtual ~daEnDoor_c() override {}
    virtual int create() override;
    virtual int doDelete() override;
    virtual int execute() override;
    virtual int draw() override;
    virtual bool ActorDrawCullCheck() override { return false; }

    virtual bool isClosed() { return isState(StateID_Wait); } ///< Checks whether the door is closed.
    virtual bool isDummyOpen() { return false; } ///< Returns true if the door must act like Fake Ghost House Doors.
    virtual void createMdl() {} ///< Creates the model and animation for the door.
    virtual void initAnm() {} ///< Initializes the animation for the door.
    virtual void initCcData() {} ///< Initializes collision data for the door.
    virtual void initialize() { mCc.entry(); } ///< Initializes other elements, called by create().
    virtual void waitProc() {} ///< Executed every frame during the Wait state.
    virtual void setOpenAnm() {} ///< Changes animation properties so the door opening animation plays.
    virtual void setOpenSE() {} ///< Plays the door opening SFX.
    virtual void setCloseAnm() {} ///<  Changes animation properties so the door closing animation plays.
    virtual void setCloseSE() {} ///< Plays the door closed SFX.
    virtual void setCloseMoveSE() {} ///< Plays the door closing SFX.
    virtual void setWaitAnm() {} ///<  Changes animation properties so the door stays closed.

    void rotation_move(dActor_c *parent); ///< Moves along the passed parent's Z angle.
    dActor_c *rotation_move(); ///< Moves along the parent's Z angle.
    void setParentInfo(dActor_c *parent); ///< Sets mParentUniqueID and computes mDistanceWithParent.
    dActor_c *setParentInfo(u32 rotationID); ///< Searches for a parent, then sets mParentUniqueID and computes mDistanceWithParent accordingly.
    void allocate(); ///< Wraps createMdl between mAllocator.createFrmHeap and mAllocator.adjustFrmHeap.
    void calcMdl(); ///< Updates mMatrix and mModel.
    bool checkOpenOk(); ///< Checks if the door is opening for players to enter it.

    STATE_FUNC_DECLARE(daEnDoor_c, Search);
    STATE_FUNC_DECLARE(daEnDoor_c, Open);
    STATE_FUNC_DECLARE(daEnDoor_c, Close);
    STATE_FUNC_DECLARE(daEnDoor_c, Wait);
    STATE_FUNC_DECLARE(daEnDoor_c, Dummy);

    static daObjCenter_c *searchParent_centerA(u32 rotationID); ///< Searches for a daObjCenter_c having a specific Rotation ID. @see mRotationID
    static daObjCenter2_c *searchParent_centerB(u32 rotationID); ///< Searches for a daObjCenter2_c having a specific Rotation ID. @see mRotationID
    static dActor_c *searchParent(u32 rotationID); ///< Searches for a daObjCenter_c or daObjCenter2_c having a specific Rotation ID. @see mRotationID
    static void ccCallback(dCc_c *self, dCc_c *other); ///< Collision callback for the door.

	dHeapAllocator_c mAllocator; ///< The allocator.
	nw4r::g3d::ResFile mResFile; ///< The resource file.
	m3d::mdl_c mModel; ///< The door model.
	m3d::anmChr_c mAnim; ///< The door model animation.
    fBaseID_e mParentUniqueID; ///< The unique ID of the parent actor if linked (daObjCenter_c or daObjCenter2_c).
    mVec3_c mDistanceToParent; ///< The distance between the parent controller's position and the door when it spawned.
    DoorStatus_e mDoorStatus; ///< The current status of the door. Used by dAcPy_c.
    BOOL mIsOpen; ///< Whether the door finished opening or not.
    BOOL mMayOpen; ///< Whether the door may open when changing its status or not.
    mAng mOrientation; ///< The orientation of the door.
    u8 mHasParent; ///< Whether the door is controlled by a parent actor.
    u8 mRotationID; ///< The rotation ID that links the door to the corresponding controller. @see daObjCenter_c::mRotationID, daObjCenter2_c::mRotationID

    ACTOR_PARAM_CONFIG(Orientation, 12, 2); ///< @copydoc mOrientation
    ACTOR_PARAM_CONFIG(HasParent, 8, 1); ///< @copydoc mHasParent
    ACTOR_PARAM_CONFIG(RotID, 0, 8); ///< @copydoc mRotationID
};
