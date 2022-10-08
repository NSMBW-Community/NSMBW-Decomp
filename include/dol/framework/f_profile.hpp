#pragma once
#include <types.h>
#include <dol/framework/f_profile_name.hpp>
/// @file

/// @brief The name of a profile. Value is a fProfile::PROFILE_NAME_e.
/// @note The compilation order suggests that this file might have been grouped together
/// with the other f classes, so a similar naming scheme has been applied here.
typedef u16 ProfileName;

/**
 * @brief Gets a string representing the profile name.
 * 
 * @param profName The profile name for which to get the name string of.
 * @return A string containing the profile name.
 */
char *dProf_getName(ProfileName profName);

/// @brief for all profile related structures.
/// @note Unofficial name.
namespace fProfile {

    /// @brief A set of basic information needed to construct a base.
    /// @details A profile consists of a pointer to a constructor function
    /// and a priority value for execution and drawing order.
    struct fBaseProfile_c {
        void *(*mpClassInit)(); ///< The constructor function.
        u16 mExecuteOrder; ///< The execution priority of the base.
        u16 mDrawOrder; ///< The draw priority of the base.
    };
    
    /// @brief A set of basic information needed to construct an actor.
    /// @details In addition to the fields in fBaseProfile_c, it also contains some properties about the actor.
    struct fActorProfile_c : fBaseProfile_c {
        /// @brief Some actor-related properties.
        /// @todo Document the bitfield.
        u32 mActorProperties;
    };

    extern fBaseProfile_c *(*sProfileList)[NUMBER_OF_ACTORS]; ///< A list of all profiles.

} // namespace fProfile
