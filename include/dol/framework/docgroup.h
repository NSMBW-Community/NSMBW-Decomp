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
 *
 * @}
 */
