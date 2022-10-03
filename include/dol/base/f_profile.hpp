#pragma once

#include <types.h>
#include <dol/base/f_profile_name.hpp>

/*  [The compilation order suggests that this file might have been grouped together
 *  with the other f classes, so a similar naming scheme has been applied here.]
 */

/// @brief The name of a profile. Value is a fProfile::PROFILE_NAME_e.
typedef u16 ProfileName;

// [Unofficial name]
/// @brief for all profile related structures.
namespace fProfile {
    /**
     * @brief A set of information about a base.
     * 
     * A profile consists of a pointer to a constructor function
     * and a priority for execution and drawing order.
     */
    struct fBaseProfile_c {
        void *(*mpClassInit)(); ///< The constructor function.
        u16 mExecuteOrder; ///< The execution priority of the actor.
        u16 mDrawOrder; ///< The draw priority of the actor.
    };
    
    /**
     * @brief A set of information about an actor.
     * 
     * In addition to the fields in fBaseProfile_c, it also contains some properties about the actor.
     */
    struct fActorProfile_c : fBaseProfile_c {
        // [TODO: document the bitfield]
        u32 mActorProperties; ///< Some actor-related properties.
    };
        
    extern fBaseProfile_c *(*sProfileList)[NUMBER_OF_ACTORS]; ///< A list of all profiles.

}
