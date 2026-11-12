#pragma once

#include <game/bases/d_actor.hpp>

/// @brief A controller for swaying objects around a center point.
/// @statetable
/// @paramtable
/// @ingroup bases
class daObjCenter_c : public dActor_c {
public:
    daObjCenter_c() {} ///< @copydoc dActorState_c::dActorState_c
    ~daObjCenter_c() {} ///< @copydoc dActorState_c::~dActorState_c

    virtual int create() override;
    virtual int execute() override;

    short m_392;
    short m_394;
    short m_396;
    short m_398;
    u8 m_39A;
    u8 m_39B;
    u8 mRotationID; ///< The rotation ID to be set on the affected objects.

    ACTOR_PARAM_CONFIG(RotationID, 0, 8); ///< @copydoc mRotationID
};
