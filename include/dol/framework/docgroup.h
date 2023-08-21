#pragma once
// [This header file defines documentation groups for better categorization]

/**
 * @defgroup framework Framework
 * @ingroup game
 * @brief Manages processes across the game, forming the core of the game engine.
 * @details
 * ## Introduction
 * In New Super Mario Bros. Wii, almost all game logic exists within individual processes. Each process
 * represents a distinct unit of game behavior and encapsulates specific functionalities.
 *
 * Nintendo refers to these processes as bases. Each type of base is defined by a profile, which
 * determines its behaviour (is it a Goomba, or Mario, or the HUD, or the abstract concept of a world
 * map?) and establishes its priority relative to other base types. More details about profiles can be
 * found @ref profile "here".
 *
 * Processes follow a hierarchical tree structure:
 * - Each process can have multiple siblings and children; this allows creating complex and intricate
 * relationships between bases. An example is the Hammer Bro: all the hammers it throws are siblings of
 * each other, and the Hammer Bro is their parent.
 * - The root process for all bases is the current scene; this allows the game to clean up after itself by
 * deleting all processes when switching between scenes. Further information on this topic can be found
 * @ref dScene_c "here".
 *
 * ## Base Implementation
 * All bases inherit from the fBase_c class, which defines the core elements of a process to provide
 * common behaviour across all bases. For more detailed information, please refer to the fBase_c
 * documentation.
 *
 * The lifecycle of a base consists of multiple operations, whose behaviour can be overridden at any
 * point. Each operation has an @ref fManager_c "associated linked list", containing all bases for which
 * said operation is scheduled for the current frame.
 *
 * fBase_c defines five core operations:
 * - @p create runs immediately after construction (generally only once), and can be used to set up the
 * base or load resources for it.
 * - @p execute serves as the base's own main loop, running every frame.
 * - @p draw offers an alternative main loop specifically for rendering code. It also runs every frame.
 * - @p delete runs immediately before destruction (generally only once), and can be used to deallocate
 * resources or remove links to other bases.
 * - @p connect is an internal operation for process management. Bases should not override it.
 *
 * The fManager_c class is responsible for managing the execution cycle of each base. It also offers
 * various utilities for searching for bases meeting specific criteria.
 *
 * @{
 *
 * @defgroup profile fProfile
 * @brief A profile is a basic set of information needed to construct a base.
 * @details
 *
 * There exist two kinds of profiles:
 * - @ref fProfile::fBaseProfile_c "fBaseProfile_c": A basic profile that includes a constructor function
 * and a value for execution/drawing priority. All @ref fBase_c::GROUP_TYPE_e "non-actor bases" utilize this
 * profile type.
 * - @ref fProfile::fActorProfile_c "fActorProfile_c": A specialized profile dedicated to
 * @ref fBase_c::GROUP_TYPE_e::ACTOR "actor bases", featuring additional properties.
 *
 * The game maintains a @ref fProfile::sProfileList "list" of all profiles for base construction purposes.
 *
 * ## Profile Names
 * Each profile has an @ref fProfile::PROFILE_NAME_e "associated name", which can be used to determine a
 * base's profile and also acts as an index into the profile list. A list of all profile names in string
 * form is also available, although unused.
 *
 * A @ref ProfileName "typedef" for profile names is provided to enhance code readability.
 *
 * ## Creating Profiles
 * Use the following macros to create a profile:
 * - Use ::BASE_PROFILE (or ::ACTOR_PROFILE) to use the priority values given by @ref ::PROFILE_NAME_e (execute order)
 * and @ref ::DRAW_ORDER::DRAW_ORDER_e "DRAW_ORDER_e" (draw order).
 * - Use ::CUSTOM_BASE_PROFILE (or ::CUSTOM_ACTOR_PROFILE) if custom priority values are required.
 *
 * Addition of the created profile and its name to the respective lists requires manual intervention.
 *
 * ## Unused Content
 * A profile name in string format can be obtained by calling ::dProf_getName. This has no practical
 * use (and is most likely a debug leftover), but it has greatly helped the research for official game
 * entity names.
 *
 * @todo Add a link to the profile name list when it gets decompiled.
 * @}
 */
