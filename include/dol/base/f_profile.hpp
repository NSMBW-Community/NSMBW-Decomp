#pragma once

#include <types.h>

/*  [The compilation order suggests that this file might have been grouped together
 *  with the other f classes, so a similar naming scheme has been applied here.]
 */

/// The name of a profile. Value is a fProfile::PROFILE_NAME_e.
typedef u16 ProfileName;

// [Unofficial name]
/// Namespace for all profile related structures
namespace fProfile {
    /**
     * @brief A set of information about a base.
     * A profile consists of a pointer to a constructor function
     * and a priority for execution and drawing order.
     */
    struct fBaseProfile_c {
        void *(*mpClassInit)(); ///< The constructor function.
        u16 mExecuteOrder;
        u16 mDrawOrder;
    };
    
    /**
     * @brief A set of information about an actor.
     * In addition to the fields in fBaseProfile_c, it also contains some properties.
     */
    struct fActorProfile_c : fBaseProfile_c {
        // [TODO: document the bitfield]
        u32 mActorProperties;
    };
    
    /// A name of a profile. Can be used to index into the profile list.
    enum PROFILE_NAME_e {
        // [TODO]
    };
        
    extern fBaseProfile_c *(*sProfileList)[750]; ///< A list of all profiles.

}
