#pragma once
#include <lib/egg/heap/heap.hpp>
#include <lib/egg/color_fader.hpp>
#include <types.h>

namespace dReset {

    /// @brief System reset management class.
    class Manage_c {
    public:

        /// @brief The game's possible running states.
        enum Mode_e {
            NORMAL, ///< The game is running regularly.
            SOFT_RESET, ///< The game is being soft reset (from the Home Menu).
            HBM_WAIT, ///< The home menu is open.
            DISK_WAIT, ///< The game cannot read the DVD and cannot proceed.
            FATAL_ERROR, ///< The game cannot read the NAND and cannot proceed.
            SAFETY_WAIT, ///< The game is about to execute the hard reset procedure indicated in ::execMode.
            NO_CHANGE ///< The game's running state hasn't changed [value used in ::modeInit].
        };

        /// @brief The possible hard reset procedures.
        enum Exec_e {
            RESTART_GAME, ///< Reboots the game's DOL. [Unused].
            REBOOT_SYSTEM, ///< Reboots the entire console. [Unused].
            RETURN_WII_MENU, ///< Returns to the Wii Menu.
            RETURN_DATA_MANAGER, ///< Returns to the Wii Menu's Data Manager.
            POWER_OFF, ///< Turns off the console.
            NONE ///< Does not execute any reset procedure.
        };

        EGG::Heap *heap;
        EGG::ColorFader *colorFader;

        int modeProc; ///< The game's current running state. See ::Mode_e.

        /// @brief The running state the game's about to switch to. See ::Mode_e.
        /// @details The state does not change if the state is set to Mode_e::NO_CHANGE.
        int modeInit;

        int prevMode;
        int execMode; ///< The hard reset procedure to be executed. See ::Exec_e.

        bool hbmReset;
        bool hbmReturnMenu;

        bool resetCallbackCalled;
        bool powerCallbackCalled;
        bool interiorReturnDataManager;

        u32 hbmResetTypeMaybe;
        u32 hbmPowerOffTypeMaybe;
        u32 softResetStateMaybe;
        bool softResetPermitted;
        bool _2D;

        bool softResetRequested;
        bool fadeOutRequested;
        bool _30;

        void *audioResetFunc;
        void *audioExitFunc;

        bool bootCompleted;
        bool executingFade;
        bool _3E;

        static dReset::Manage_c* GetInstance();
    };

} // namespace dReset
