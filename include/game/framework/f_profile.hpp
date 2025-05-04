#pragma once
#include <types.h>
#include <game/framework/f_profile_name.hpp>
/// @file

/// @addtogroup profile
/// @{

/// @brief Creates a basic profile with the given execute and draw order values.
/// @hideinitializer
#define CUSTOM_BASE_PROFILE(profName, className, executeOrder, drawOrder) void *className##_classInit() { return new className(); } \
    fProfile::fBaseProfile_c g_profile_##profName = { &className##_classInit, executeOrder, drawOrder }

/// @brief Creates an actor profile with the given execute/draw order and actor property values.
/// @hideinitializer
#define CUSTOM_ACTOR_PROFILE(profName, className, executeOrder, drawOrder, properties) void *className##_classInit() { return new className(); } \
    const fProfile::fActorProfile_c g_profile_##profName = { &className##_classInit, executeOrder, drawOrder, properties }

/// @brief Creates a basic profile, using the profile number as the execute and draw order value.
/// @details The execution order is set to the profile number.
/// @hideinitializer
/// @see ACTOR_PROFILE
#define BASE_PROFILE(profName, className) CUSTOM_BASE_PROFILE(profName, className, fProfile::profName, fProfile::DRAW_ORDER::profName);

/// @brief Creates an actor profile, using the profile number as the execute and draw order value.
/// @details The execution order is set to the profile number.
/// @hideinitializer
/// @see BASE_PROFILE
#define ACTOR_PROFILE(profName, className, properties) CUSTOM_ACTOR_PROFILE(profName, className, fProfile::profName, fProfile::DRAW_ORDER::profName, properties);

/// @brief The name of a profile. Value is a fProfile::PROFILE_NAME_e.
typedef u16 ProfileName;

/**
 * @brief Obtains a string representing the profile name.
 * @unused
 * @param profName The profile name to get the name string of.
 * @return The profile name in string form.
 */
char *dProf_getName(ProfileName profName);

/// @}

/// @brief For all profile related structures.
/// @unofficial{The compilation order suggests that this file might have been grouped together with the
/// rest of @ref framework, so a similar naming scheme has been applied here.}
namespace fProfile {

    /// @addtogroup profile
    /// @{

    /// @brief A set of basic information needed to construct a generic base.
    struct fBaseProfile_c {
        void *(*mpClassInit)(); ///< The constructor function.
        u16 mExecuteOrder; ///< The execution priority of the base. Lower values mean higher priority.
        u16 mDrawOrder; ///< The draw priority of the base. Lower values mean higher priority.
    };

    /// @brief A set of basic information needed to construct an actor base.
    struct fActorProfile_c : fBaseProfile_c {

        /// @brief Various actor-related properties.
        /// @details These properties will be copied @ref dBaseActor_c::mActorProperties "into the actor"
        /// when it is constructed; this grants easy access to the properties and allows individual actor
        /// configuration.
        /// @todo Document the bitfield.
        u32 mActorProperties;
    };

    extern const fBaseProfile_c *(*sProfileList)[PROFILE_COUNT]; ///< A list of all profiles.
    /// @}

} // namespace fProfile
