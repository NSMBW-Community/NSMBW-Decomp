#pragma once

#include <game/bases/d_enemy.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/bases/d_a_obj_center.hpp>
#include <game/bases/d_a_obj_center2.hpp>

class daEnDoor_c : public dEn_c {
public:
    /// @unofficial
    enum DoorStatus_e {
        DOOR_SUBSTATE_WAIT,
        DOOR_SUBSTATE_OPEN_OUT,
        DOOR_SUBSTATE_OPEN_IN,
        DOOR_SUBSTATE_CLOSED
    };

    virtual ~daEnDoor_c() override;
    virtual int create() override;
    virtual int doDelete() override;
    virtual int execute() override;
    virtual int draw() override;
    virtual bool ActorDrawCullCheck() override;
    
    virtual bool isClosed();
    virtual bool isDummyOpen();
    virtual void createMdl();
    virtual void initAnm();
    virtual void initCcData();
    virtual void initialize();
    virtual void waitProc();
    virtual void setOpenAnm();
    virtual void setOpenSE();
    virtual void setCloseAnm();
    virtual void setCloseSE();
    virtual void setCloseMoveSE();
    virtual void setWaitAnm();

    void rotation_move(dActor_c* actor);
    dActor_c* rotation_move();
    void setParentInfo(dActor_c* parent);
    dActor_c* setParentInfo(u32 rotationID);
    void allocate();
    void calcMdl();
    bool checkOpenOk();

    STATE_FUNC_DECLARE(daEnDoor_c, Search);
    STATE_FUNC_DECLARE(daEnDoor_c, Open);
    STATE_FUNC_DECLARE(daEnDoor_c, Close);
    STATE_FUNC_DECLARE(daEnDoor_c, Wait);
    STATE_FUNC_DECLARE(daEnDoor_c, Dummy);

    static daObjCenter_c* searchParent_centerA(u32 rotationID); ///< Searches for a daObjCenter_c having a specific Rotation ID. @see mRotationID
    static daObjCenter2_c* searchParent_centerB(u32 rotationID); ///< Searches for a daObjCenter2_c having a specific Rotation ID. @see mRotationID
    static dActor_c* searchParent(u32 rotationID); ///< Searches for a daObjCenter_c or daObjCenter2_c having a specific Rotation ID. @see mRotationID
    static void ccCallback(dCc_c *self, dCc_c *other); ///< Collision callback for the door.

	dHeapAllocator_c mAllocator; ///< The allocator.
	nw4r::g3d::ResFile mResFile; ///< The resource file.
	m3d::mdl_c mModel; ///< The door model.
	m3d::anmChr_c mAnim; ///< The door model animation.
    fBaseID_e mParentUniqueID; ///< The unique ID of the parent actor if linked (daObjCenter_c or daObjCenter2_c).
    mVec3_c mDistanceWithParent; ///< The distance between the parent's position and the door when it spawned.
    DoorStatus_e mDoorStatus; ///< The current status of the door. Used by dAcPy_c.
    int mIsOpen; ///< Whether the door finished opening or not.
    int mMayOpen; ///< Whether the door may open when changing its status or not.
    mAng mOrientation; ///< Orientation of the door. Determined by mParam.
    u8 mHasParent; ///< Whether the door has a parent actor. Determined by mParam.
    u8 mRotationID; ///< Rotation ID that links the door and its parent. Determined by mParam. @see daObjCenter_c::mRotationID, daObjCenter2_c::mRotationID

    static const float smc_unused; ///< [Unreferenced, probably padding?] @unused @unofficial

    ACTOR_PARAM_CONFIG(Orientation, 12, 2); ///< @copydoc mOrientation
    ACTOR_PARAM_CONFIG(HasParent, 8, 1); ///< @copydoc mHasParent
    ACTOR_PARAM_CONFIG(RotID, 0, 8); ///< @copydoc mRotationID
};
