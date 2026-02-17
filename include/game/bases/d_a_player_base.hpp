#pragma once

#include <game/bases/d_actor.hpp>
#include <game/bases/d_a_player_data.hpp>
#include <game/bases/d_a_player_hio.hpp>
#include <game/bases/d_ac_py_key.hpp>
#include <game/bases/d_cc.hpp>
#include <game/bases/d_player_model_manager.hpp>
#include <game/bases/d_quake.hpp>
#include <game/mLib/m_3d.hpp>
#include <game/sLib/s_State.hpp>
#include <game/bases/d_effect.hpp>
#include <game/bases/d_audio.hpp>
#include <constants/game_constants.h>

/**
 * @brief The base class for the player and Yoshi.
 * @statetable
 */
class daPlBase_c : public dActor_c {
public:
    enum DamageType_e {
        DAMAGE_NONE,
        DAMAGE_1, DAMAGE_2, DAMAGE_3, DAMAGE_4,
        DAMAGE_5, DAMAGE_6, DAMAGE_YOGAN, DAMAGE_8,
        DAMAGE_9, DAMAGE_POISON, DAMAGE_B, DAMAGE_C,
        DAMAGE_D, DAMAGE_E, DAMAGE_F, DAMAGE_10,
        DAMAGE_11, DAMAGE_12
    };

    enum DokanDir_e {
        DOKAN_U,
        DOKAN_D,
        DOKAN_L,
        DOKAN_R,
        DOKAN_ROLL
    };

    enum StarSet_e {
        STAR_SET_0,
        STAR_SET_1,
        STAR_SET_2
    };

    /// @brief Blending modes for animations.
    enum AnmBlend_e {
        BLEND_NONE, ///< Do not blend between animations.
        BLEND_DEFAULT ///< Use the default blend duration specified in the animation HIO.
    };

    enum ClearType_e {
        CLEAR_TYPE_GOAL,
        CLEAR_TYPE_BOSS,
        CLEAR_TYPE_FINAL_BOSS
    };

    /// @unofficial
    enum GroundType_e {
        GROUND_TYPE_DEFAULT,
        GROUND_TYPE_SNOW,
        GROUND_TYPE_SAND,
        GROUND_TYPE_ICE,
        GROUND_TYPE_DIRT,
        GROUND_TYPE_WATER,
        GROUND_TYPE_CLOUD,
        GROUND_TYPE_FUNSUI,
        GROUND_TYPE_MANTA,
        GROUND_TYPE_BEACH,
        GROUND_TYPE_CARPET,
        GROUND_TYPE_LEAF,
        GROUND_TYPE_WOOD
    };

    /// @unofficial
    enum SlipSubstate_e {
        SLIP_ACTION_NONE,
        SLIP_ACTION_STOOP,
        SLIP_ACTION_END
    };

    /// @unofficial
    enum HipSubstate_e {
        HIP_ACTION_READY,
        HIP_ACTION_ATTACK_START,
        HIP_ACTION_ATTACK_FALL,
        HIP_ACTION_GROUND,
        HIP_ACTION_STAND_NORMAL,
        HIP_ACTION_STAND_NORMAL_END,
        HIP_ACTION_TO_STOOP
    };

    /// @unofficial
    enum JumpDaiSubstate_e {
        JUMP_DAI_MOVE_DOWN, ///< Moving down on the player or spring.
        JUMP_DAI_HIGH_JUMP, ///< The jump button was pressed to do a higher jump.
    };

    /// @unofficial
    enum FunsuiSubstate_e {
        FUNSUI_ACTION_NONE,
        FUNSUI_ACTION_START
    };

    /// @unofficial
    enum AnimePlaySubstate_e {
        ANIME_PLAY_ACTION_0,
        ANIME_PLAY_ACTION_1,
        ANIME_PLAY_ACTION_2
    };

    /// @unofficial
    enum DemoType_e {
        DEMO_0,
        DEMO_1,
        DEMO_2,
        DEMO_3,
        DEMO_PLAYER,
        DEMO_KINOPIO,
        DEMO_ENDING_DANCE
    };

    /// @unofficial
    enum DemoDokanMode_e {
        DEMO_DOKAN_NONE,
        DEMO_DOKAN_NORMAL,
        DEMO_DOKAN_RAIL,
        DEMO_DOKAN_WATER_TANK
    };

    /// @unofficial
    enum DemoInDokanSubstate_e {
        DEMO_IN_DOKAN_ACTION_0,
        DEMO_IN_DOKAN_ACTION_1,
        DEMO_IN_DOKAN_ACTION_2,
        DEMO_IN_DOKAN_ACTION_3,
        DEMO_IN_DOKAN_ACTION_4
    };

    /// @unofficial
    enum DemoGoalSubstate_e {
        GOAL_DEMO_ACTION_POLE,
        GOAL_DEMO_ACTION_WAIT,
        GOAL_DEMO_ACTION_KIME_POSE,
        GOAL_DEMO_ACTION_RUN
    };

    /// @unofficial
    enum DemoWaitSubstate_e {
        DEMO_WAIT_ACTION_0,
        DEMO_WAIT_ACTION_1
    };

    /// @unofficial
    enum DemoGoalState_Pole_e {
        GOAL_DEMO_POLE_SWING, ///< Swinging around the pole to the other side.
        GOAL_DEMO_WAIT_BELOW_PLAYER, ///< Waiting for the player below to slide far enough down the pole to not be in the way of this player.
        GOAL_DEMO_POLE_SLIDE, ///< Sliding down the pole.
        GOAL_DEMO_POLE_WAIT_JUMP, ///< Waiting at the bottom of the pole to jump off.
        GOAL_DEMO_POLE_JUMP, ///< Jumping off the pole.
        GOAL_DEMO_POLE_LAND, ///< Playing the landing animation after landing.
        GOAL_DEMO_POLE_WAIT_TURN, ///< Waiting #sc_DemoPoleWaitTurn frames to turn toward the screen.
        GOAL_DEMO_POLE_TURN, ///< Turning toward the screen.
        GOAL_DEMO_POLE_WAIT_END ///< Waiting #sc_DemoPoleWaitEnd frames before transitioning to the course clear dance.
    };

    /// @unofficial
    enum KimePoseMode_e {
        KIME_POSE_NONE,
        KIME_POSE_WITH_HAT,
        KIME_POSE_PENGUIN,
        KIME_POSE_NO_HAT,
        KIME_POSE_PROPELLER
    };

    /// @unofficial
    enum ControlDemoState_e {
        CONTROL_DEMO_WAIT,
        CONTROL_DEMO_WALK,
        CONTROL_DEMO_REGULAR_ANIM,
        CONTROL_DEMO_CUTSCENE_ANIM,
        CONTROL_DEMO_4,
        CONTROL_DEMO_KINOPIO_WALK,
        CONTROL_DEMO_KINOPIO_SWIM,
        CONTROL_DEMO_KINOPIO_SINK_SAND,
        CONTROL_DEMO_ENDING_DANCE,
    };

    /// @brief Arguments for transitioning to the @ref StateID_Crouch "crouch" state.
    /// @unofficial
    enum CrouchArg_e {
        CROUCH_ARG_FROM_WALK, ///< Crouching while already on the ground.
        CROUCH_ARG_FROM_OTHER, ///< Crouching after a slide or a ground pound.
        CROUCH_ARG_FROM_SIT_JUMP ///< Landing from a crouch jump.
    };

    /// @brief Arguments for transitioning to the @ref StateID_HipAttack "ground pound" state.
    /// @unofficial
    enum HipAttackArg_e {
        HIP_ATTACK_ARG_PLAYER, ///< A regular player is doing a ground pound.
        HIP_ATTACK_ARG_ITEM_KINOPIO ///< The rescue Toad is doing a ground pound out of the item block. @unused
    };

    /// @brief Arguments for transitioning to the @ref StateID_Swim "swim" state.
    /// @unofficial
    enum SwimArg_e {
        SWIM_ARG_INITIAL, ///< Already in water at the start of the swim action.
        SWIM_ARG_ENTERING, ///< Just entered the water.
        SWIM_ARG_FIREBALL, ///< Player was about to shoot a fireball, shoot it while in water.
        SWIM_ARG_CLIFF_HANG ///< Falling from a cliff into water.
    };

    /// @brief Arguments for transitioning to the @ref StateID_Kani "cliff" state.
    /// @unofficial
    enum KaniArg_e {
        KANI_ARG_WALK, ///< Standing on the cliff and walking.
        KANI_ARG_HANG, ///< Landing high enough on the cliff to stand on it, but hang down from it instead.
        KANI_ARG_JUMP_HANG, ///< Falling onto the cliff, immediately hang from it.
        KANI_ARG_WALK_FORCE, ///< Standing on the cliff, disallow immediately hanging from it by holding down.
        KANI_ARG_HANG_UP_VINE, ///< Climbing onto the cliff from a vine.
        KANI_ARG_HANG_HAND ///< Catching the cliff from below, hang from it.
    };

    /// @brief Arguments for transitioning to the @ref StateID_AnimePlay "animation" state.
    /// @unofficial
    enum AnimePlayArg_e {
        DEMO_ANIME_NORMAL,
        DEMO_ANIME_BOSS_SET_UP,
        DEMO_ANIME_BOSS_GLAD,
        DEMO_ANIME_BOSS_ATTENTION,
        DEMO_ANIME_BOSS_KEY_GET,
        DEMO_ANIME_BOSS_GLAD_2
    };

    /// @unofficial
    enum PowerChangeType_e {
        POWER_CHANGE_0,
        POWER_CHANGE_1,
        POWER_CHANGE_2
    };

    /// @unofficial
    enum SquishState_e {
        SQUISH_OFF,
        SQUISH_INIT,
        SQUISH_SET_REDUCTION,
        SQUISH_ANIMATION
    };

    /// @unofficial
    enum BgPress_e {
        BG_PRESS_FOOT = 9,
        BG_PRESS_HEAD,
        BG_PRESS_R,
        BG_PRESS_L,
        BG_PRESS_COUNT
    };

    /// @unofficial
    enum BgCross1_e {
        BGC_IS_FOOT = BIT_FLAG(0),
        BGC_IS_HEAD = BIT_FLAG(1),
        BGC_IS_WALL = BIT_FLAG(2),
        BGC_WALL_TOUCH_L = BIT_FLAG(3),
        BGC_WALL_TOUCH_R = BIT_FLAG(4),
        BGC_WALL_TOUCH_L_2 = BIT_FLAG(5),
        BGC_WALL_TOUCH_R_2 = BIT_FLAG(6),
        BGC_CARRY_RELATED_L = BIT_FLAG(7),
        BGC_CARRY_RELATED_R = BIT_FLAG(8),
        BGC_OBJBG_CARRY_RELATED_L = BIT_FLAG(9),
        BGC_OBJBG_CARRY_RELATED_R = BIT_FLAG(10),
        BGC_11 = BIT_FLAG(11),
        BGC_12 = BIT_FLAG(12),
        BGC_13 = BIT_FLAG(13),
        BGC_WATER_SHALLOW = BIT_FLAG(14), ///< At least slightly inside of water (hip height or higher).
        BGC_WATER_TOUCH = BIT_FLAG(15), ///< At least touching water.
        BGC_WATER_SUBMERGED = BIT_FLAG(16), ///< Fully submerged in water.
        BGC_17 = BIT_FLAG(17),
        BGC_WATER_BUBBLE = BIT_FLAG(18),
        BGC_SIDE_LIMIT_L = BIT_FLAG(19),
        BGC_SIDE_LIMIT_R = BIT_FLAG(20),
        BGC_ON_SNOW = BIT_FLAG(22),
        BGC_ON_ICE = BIT_FLAG(23),
        BGC_ON_ICE_LOW_SLIP = BIT_FLAG(24),
        BGC_SLOPE_AND_HEAD = BIT_FLAG(25),
        BGC_ON_SAND = BIT_FLAG(26),
        BGC_ON_SINK_SAND = BIT_FLAG(27),
        BGC_IN_SINK_SAND = BIT_FLAG(28),
        BGC_INSIDE_SINK_SAND = BIT_FLAG(29),
        BGC_ON_BELT_L = BIT_FLAG(30),
        BGC_ON_BELT_R = BIT_FLAG(31)
    };

    /// @unofficial
    enum BgCross2_e {
        BGC_SEMISOLID = BIT_FLAG(0),
        BGC_LIFT = BIT_FLAG(1), ///< [Figure out a better name for this].
        BGC_HANG_ROPE = BIT_FLAG(2),
        BGC_AUTOSLIP = BIT_FLAG(3),
        BGC_36 = BIT_FLAG(4),
        BGC_GROUNDED_MOVE_UP = BIT_FLAG(5),
        BGC_37 = BIT_FLAG(6), ///< Cannot wall kick or ground pound while this is set.
        BGC_SLOPE = BIT_FLAG(7),
        BGC_CLIFF = BIT_FLAG(8),
        BGC_41 = BIT_FLAG(9),
        BGC_42 = BIT_FLAG(10),
        BGC_CAN_CLIMB = BIT_FLAG(11),
        BGC_44 = BIT_FLAG(12),
        BGC_45 = BIT_FLAG(13),
        BGC_46 = BIT_FLAG(14),
        BGC_47 = BIT_FLAG(15),
        BGC_48 = BIT_FLAG(16),
        BGC_49 = BIT_FLAG(17),
        BGC_51 = BIT_FLAG(19),
        BGC_52 = BIT_FLAG(20),
        BGC_53 = BIT_FLAG(21),
        BGC_54 = BIT_FLAG(22),
        BGC_55 = BIT_FLAG(23),
        BGC_BLOCK_HIT = BIT_FLAG(24),
        BGC_57 = BIT_FLAG(25),
        BGC_58 = BIT_FLAG(26),
        BGC_LINE_BLOCK_HIT = BIT_FLAG(27),
        BGC_60 = BIT_FLAG(28),
        BGC_61 = BIT_FLAG(29),
        BGC_62 = BIT_FLAG(30),
        BGC_63 = BIT_FLAG(31)
    };

    /// @brief The status IDs to be used with onStatus(), offStatus(), isStatus() and setStatus().
    /// @unofficial
    enum Status_e {
        STATUS_CREATED, ///< The player was created.
        STATUS_CAN_EXECUTE, ///< The player can execute this frame or not.
        STATUS_NO_ANIM, ///< Don't play any animations.
        STATUS_DISABLE_STATE_CHANGE, ///< Disallow state changes.
        STATUS_OUT_OF_PLAY, ///< The player is in a bubble or has died.
        STATUS_ALL_DOWN_FADE, ///< All players have died and the screen is transitioning.
        STATUS_STUNNED, ///< Stunned by electric shock or ice.
        STATUS_07, ///< [Ice related]
        STATUS_QUAKE, ///< The player was stunned by an earthquake.
        STATUS_JUMP = 0x0a, ///< The player is jumping.
        STATUS_CAN_PENGUIN_SLIDE, ///< If the player can start sliding as a penguin.
        STATUS_STAR_JUMP, ///< The player is jumping while in star mode.
        STATUS_KANI_JUMP, ///< The player is doing a crab jump on a cliff.
        STATUS_SINK_SAND_JUMP, ///< The player is jumping while in sinking sand.
        STATUS_SIT_JUMP, ///< The player is doing a sitting jump.
        STATUS_YOSHI_DISMOUNT_JUMP, ///< The player is doing a jump to dismount Yoshi.
        STATUS_CANNON_JUMP, ///< The player is flying out of a pipe cannon.
        STATUS_12, ///< ["Wait jump?"]
        STATUS_WALL_SLIDE, ///< The player is sliding down a wall.
        STATUS_BIG_JUMP, ///< The player is doing a jump on a spring or another player.
        STATUS_SPRING_JUMP, ///< The player is doing a jump on a springboard.
        STATUS_PLAYER_JUMP, ///< The player is doing a jump on another player.
        STATUS_17, ///< [Dokan related]
        STATUS_THROW, ///< The player is throwing something.
        STATUS_KANI_WALK, ///< The player is doing a crab walk on a cliff.
        STATUS_1A,
        STATUS_1B,
        STATUS_HIP_ATTACK_FALL, ///< The player is falling while ground pounding.
        STATUS_HIP_ATTACK_LAND, ///< The player has landed after ground pounding. Only active on one frame.
        STATUS_HIP_ATTACK_STAND_UP, ///< The player is standing up after ground pounding. Only active on one frame.
        STATUS_SPIN_HIP_ATTACK_FALL, ///< The player is falling while doing a down spin.
        STATUS_SPIN_HIP_ATTACK_LANDED,
        STATUS_SPIN_HIP_ATTACK_LANDING,
        STATUS_PRESS_ATTACH, ///< The player is is attached to a enemy while ground pounding or doing a down spin. [Used for the big goombas].
        STATUS_HIP_ATTACK_DAMAGE_PLAYER, ///< The player was ground pounded by another player.
        STATUS_24,
        STATUS_PROPEL = 0x26, ///< The player is flying with the propeller suit.
        STATUS_PROPEL_UP, ///< The player is flying upwards with the propeller suit.
        STATUS_PROPEL_SLOW_FALL = 0x29, ///< The player will fall slowly while spinning down with the propeller suit.
        STATUS_PROPEL_NO_ROLL, ///< Don't rotate the player because of the propeller suit.
        STATUS_SPIN, ///< The player is spinning, either from a spin jump, a propeller spin (upwards or downwards) or screw spinning.
        STATUS_IS_SPIN_HOLD_REQ, ///< If the player spins, stay in place. [Used for the twisting screws].
        STATUS_TWIRL, ///< The player is twirling in midair.
        STATUS_WAS_TWIRL, ///< The player was twirling in midair the previous frame.
        STATUS_30 = 0x30,
        STATUS_31,
        STATUS_32,
        STATUS_VINE, ///< The player is clinging to a vine or a mesh net.
        STATUS_HANG, ///< The player is hanging from a ceiling rope.
        STATUS_POLE, ///< The player is climbing a pole.
        STATUS_36,
        STATUS_KANI_HANG, ///< The player is hanging from a cliff.
        STATUS_KANI_HANG_ANIMATION, ///< The player is animating into the hanging pose on a cliff.
        STATUS_39, ///< [Swim related]
        STATUS_SWIM, ///< The player is swimming.
        STATUS_PENGUIN_SWIM, ///< The player is swimming with the penguin suit.
        STATUS_PENGUIN_SLIDE, ///< The player is sliding with the penguin suit.
        STATUS_PENGUIN_SLIDE_JUMP, ///< The player is doing a penguin slide jump.
        STATUS_INITIAL_SLIDE, ///< The player is in an initial slide action. [Used in 6-6 to slide all the way down automatically].
        STATUS_PENGUIN_RECOIL, ///< The player is bouncing back after hitting an enemy that cannot be killed by a penguin slide.
        STATUS_40, ///< [Water jump?]
        STATUS_SWIM_AGAINST_JET_H, ///< The player is swimming against a horizontal water jet stream.
        STATUS_SWIM_AGAINST_JET_V, ///< The player is swimming against a vertical water jet stream.
        STATUS_43,
        STATUS_45 = 0x45,
        STATUS_46,
        STATUS_47,
        STATUS_48,
        STATUS_49,
        STATUS_4A,
        STATUS_RIDE_YOSHI, ///< The player is riding Yoshi.
        STATUS_JUMP_DAI_COOLDOWN = 0x4d, ///< The player recently failed to perform a big jump because of a ceiling.
        STATUS_4E,
        STATUS_4F,
        STATUS_50,
        STATUS_51,
        STATUS_52,
        STATUS_53,
        STATUS_54,
        STATUS_55,
        STATUS_56,
        STATUS_57,
        STATUS_RIDE_NUT_2,
        STATUS_RIDE_NUT,
        STATUS_5A,
        STATUS_5B,
        STATUS_5C,
        STATUS_5D,
        STATUS_5E,
        STATUS_5F,
        STATUS_60,
        STATUS_61,
        STATUS_62,
        STATUS_63,
        STATUS_64,
        STATUS_GOAL_POLE_TOUCHED, ///< The player has touched the goal pole.
        STATUS_GOAL_POLE_WAIT_BELOW_PLAYER, ///< The player is waiting for the player below to slide down the goal pole.
        STATUS_67,
        STATUS_GOAL_POLE_FINISHED_SLIDE_DOWN, ///< The player has reached the bottom of the goal pole after sliding down.
        STATUS_GOAL_POLE_READY_FOR_JUMP_OFF, ///< The player is ready to jump off the goal pole.
        STATUS_GOAL_POLE_TURN, ///< The player is turning toward the screen after jumping off the goal pole.
        STATUS_6B,
        STATUS_6C,
        STATUS_6D,
        STATUS_6E,
        STATUS_GOAL_POLE_NOT_GOAL_NO_MOVE, ///< The player did not reach the goal pole in time and mustn't move anymore.
        STATUS_70,
        STATUS_71,
        STATUS_72,
        STATUS_73,
        STATUS_74,
        STATUS_ENDING_DANCE_AUTO,
        STATUS_76,
        STATUS_77,
        STATUS_78,
        STATUS_79,
        STATUS_7A,
        STATUS_7D = 0x7d,
        STATUS_7E,
        STATUS_7F,
        STATUS_80,
        STATUS_81,
        STATUS_82,
        STATUS_83,
        STATUS_84,
        STATUS_85,
        STATUS_86,
        STATUS_87,
        STATUS_88,
        STATUS_89,
        STATUS_8A,
        STATUS_QUAKE_BIG, ///< A big quake that stuns the player was triggered.
        STATUS_QUAKE_SMALL, ///< A small quake that makes the player do a hop was triggered.
        STATUS_8D,
        STATUS_8E,
        STATUS_CAN_LAND, ///< The player can land on Yoshi or another player.
        STATUS_91 = 0x91,
        STATUS_92,
        STATUS_93,
        STATUS_94,
        STATUS_95,
        STATUS_96,
        STATUS_97,
        STATUS_98,
        STATUS_99,
        STATUS_9B = 0x9b,
        STATUS_9C,
        STATUS_9D,
        STATUS_9E,
        STATUS_9F,
        STATUS_A0,
        STATUS_A1,
        STATUS_A2,
        STATUS_A3,
        STATUS_A4,
        STATUS_A5, ///< [Jump moving up?]
        STATUS_FIREBALL_PREPARE_SHOOT, ///< The player is about to shoot a fireball.
        STATUS_A7,
        STATUS_A8,
        STATUS_A9,
        STATUS_AA,
        STATUS_AB,
        STATUS_FOLLOW_MAME_KURIBO, ///< Mini Goombas are attached to the player.
        STATUS_AD,
        STATUS_AE,
        STATUS_B3 = 0xb3, /// [Yoshi only?]
        STATUS_ABOUT_TO_BE_DELETED = 0xb5,
        STATUS_B6,
        STATUS_B7,
        STATUS_B8,
        STATUS_B9,
        STATUS_BA,
        STATUS_BB,
        STATUS_INVULNERABLILITY_BLINK, ///< Skip drawing the player this frame to create a blinking effect.
        STATUS_BD,
        STATUS_BE,
        STATUS_BF,
        STATUS_C0,
        STATUS_C1,
        STATUS_C2,
        STATUS_C3,
        STATUS_C4,
        STATUS_C5,
        STATUS_C8 = 0xc8,
        STATUS_C9,
        STATUS_CA
    };

    class jmpInf_c {
    public:
        jmpInf_c(float f, int a, int b) : m_04(f), m_08(a), m_0c(b) {}
        virtual ~jmpInf_c() {};

        float m_04;
        int m_08;
        int m_0c;
    };

    typedef void (daPlBase_c::*ProcFunc)();

    daPlBase_c();
    virtual ~daPlBase_c();

    virtual int create();
    virtual int preExecute();
    virtual int execute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);
    virtual int doDelete();
    virtual int preDraw();
    virtual int draw();

    virtual void executeMain() {}
    virtual void executeLastPlayer() {}
    virtual void executeLastAll() {}
    virtual bool isItemKinopio() { return false; }
    virtual void setPowerup(PLAYER_POWERUP_e, int); ///< @unofficial
    virtual u8 getTallType(s8);
    virtual const sBcPointData *getHeadBgPointData() { return nullptr; };
    virtual const sBcPointData *getWallBgPointData() { return nullptr; };
    virtual const sBcPointData *getFootBgPointData() { return nullptr; };
    virtual float getStandHeadBgPointY() { return 0.0f; }
    virtual void checkBgCrossSub() {}
    virtual void postBgCross();
    virtual float getSandSinkRate() { return 0.0f; }
    virtual void setReductionScale();
    virtual void initStampReduction();
    virtual void calcJumpDaiReductionScale(int, int);
    virtual void setReductionBoyon();
    virtual bool setPressBgDamage(int, int);
    virtual bool setBalloonInDispOut(int) { return false; }
    virtual bool isChange() { return false; }
    virtual void changeNextScene(int);
    virtual bool isEnableDokanInStatus();
    virtual bool setHideNotGoalPlayer();
    virtual int setDemoGoal(mVec3_c &landPos, float goalCastleX, u8 goalType);
    virtual bool setDemoCannonWarp(int, short, short) { return false; }

    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoNone);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoStartWait);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoWait);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanU);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanD);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanR);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanL);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanU);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanD);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanR);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanL);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanRoll);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInWaterTank);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutWaterTank);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoRailDokan);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoDown);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoNextGotoBlock);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoGoal);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoControl);

    virtual void initialDokanUnder();
    virtual void initialDokanUper();
    virtual void initialDokanRight();
    virtual void initialDokanLeft();
    virtual void initialDokanUnderM();
    virtual void initialDokanUperM();
    virtual void initialDokanRightM();
    virtual void initialDokanLeftM();
    virtual void initialDokanDepth();
    virtual void initialDoor();
    virtual void initialFall();
    virtual void initialVine();
    virtual void initialJumpRight();
    virtual void initialJumpLeft();
    virtual void initialHipAttack();
    virtual void initialSlip();
    virtual void initialSwim();
    virtual void initialBlockJump();
    virtual void initialBlockJumpBelow();
    virtual void initialTorideBoss();
    virtual void initialNormal();

    virtual void setCreateAction(int);
    virtual bool setTimeOverDemo() { return false; }
    virtual void setFallDownDemo() {}
    virtual bool setDokanIn(DokanDir_e dir);
    virtual void initDemoOutDokan();
    virtual bool updateDemoKimePose(ClearType_e clearType);
    virtual void initDemoGoalBase();
    virtual void executeDemoGoal_Run();
    virtual void initializeDemoControl() {}

    /// @brief Transition to a new state with the given state ID and argument.
    /// @param stateID The ID of the state to transition to.
    /// @param arg An optional argument to pass to the new state. The type of this argument depends on the state being transitioned to.
    virtual void changeState(const sStateIDIf_c &, void *);

    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, None); ///< Default state, does nothing. Argument: None.
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Walk); ///< Player on the ground. Argument: Blending mode (AnmBlend_e).
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Jump); ///< Jumping. Argument: Jump information (jmpInf_c *).
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, SitJump); ///< Crouch jump. Argument: Should initiate jump (@p bool).
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Fall); ///< Falling. Argument: Should play animation (@p bool).
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Land); ///< Landing after a jump. Argument: None.
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Crouch); ///< Crouching on the ground. Argument: See CrouchArg_e.
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Slip); ///< Sliding down a slope. Argument: None.
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Turn); ///< Turning around after running fast. Argument: None.
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, HipAttack); ///< Ground pounding. Argument: See HipAttackArg_e.
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Swim); ///< Swimming. Argument: See SwimArg_e.
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, JumpDai); ///< Jumping on a spring. Argument: None.
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, PlayerJumpDai); ///< Jumping on a player. Argument: None.
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Funsui); ///< Being blown upwards by a fountain. Argument: None.
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Kani); ///< Moving on a cliff. Argument: See KaniArg_e.
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Cloud); ///< Riding a cloud. Argument: None.
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, AnimePlay); ///< Playing a cutscene animation. Argument: See AnimePlayArg_e.
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, WaitJump); ///< Doing a hop caused by a small quake. Argument: None.

    virtual bool isWaitFrameCountMax() { return false; }
    virtual bool checkWalkNextAction() { return false; }
    virtual void setWaitActionAnm(AnmBlend_e);
    virtual void setWalkActionAnm(AnmBlend_e);
    virtual void walkActionInit_Wait(AnmBlend_e);
    virtual void walkAction_Wait();
    virtual void walkActionInit_Move(AnmBlend_e);
    virtual void walkAction_Move();
    virtual bool checkCrouch();
    virtual bool setCancelCrouch();
    virtual void setSlipAction();
    virtual void slipActionMove(int);
    virtual void setFallAction();
    virtual void setHipAttack_AttackStart();

    virtual void releaseFunsuiAction();

    virtual float getCloudOffsetY();
    virtual bool setRideJrCrown(const dActor_c *) { return false; }
    virtual bool isRideJrCrownOwn(const dActor_c *) { return false; }
    virtual void setRideJrCrownMtx(const mMtx_c *) {}
    virtual void setRideJrCrownAnm(int) {}

    virtual const mVec3_c *getHeadTopPosP() { return nullptr; }
    virtual const float *getGravityData() { return mGravityData; }
    virtual bool isCarry() const { return false; }
    virtual bool isLiftUp() { return false; }

    virtual bool isLiftUpExceptMame() { return false; }
    virtual int isStar() const;
    virtual void setStar(StarSet_e, int);
    virtual void endStar() {}
    virtual void setVirusStar(daPlBase_c *) {}
    virtual void clearStarCount();
    virtual s8 getStarCount() const { return mStarCount; }
    virtual s8 calcStarCount(int);

    virtual bool isNoDamage();
    virtual bool setDamage(dActor_c *, DamageType_e);
    virtual bool setForcedDamage(dActor_c *, DamageType_e);

    /**
     * @brief Starts a jump action with the given parameters.
     * Does not start a jump if the player is in a climbing state.
     * @param jumpSpeed The vertical speed of the jump.
     * @param speedF The @ref mSpeedF "forward speed".
     * @param allowSteer Whether the player can steer in midair.
     * @param keyMode The input settings for the jump. (0: none, 1: force jump pressed, 2: force jump not pressed)
     * @param jumpMode The type of jump to perform. [TODO: document the jump modes]
     */
    virtual bool setJump(float jumpSpeed, float speedF, bool allowSteer, int keyMode, int jumpMode); ///< @unofficial
    /// @brief Starts a jump action unconditionally. See setJump().
    virtual bool _setJump(float jumpSpeed, float speedF, bool allowSteer, int keyMode, int jumpMode); ///< @unofficial
    virtual bool setWaitJump(float jumpSpeed);

    virtual bool setHipAttackOnEnemy(mVec3_c *);

    virtual void clearJumpActionInfo(int) {}
    virtual bool setSwimSpeed(float speedY, float speedF);
    virtual void setLandSE();

    virtual void set1UpKinokoEffect() {}
    virtual void setFlagGetEffect() {}
    virtual void setHipAttackEffect();

    virtual void setZPosition();
    virtual void setZPosition(float);
    virtual void setZPositionDirect(float);
    virtual void offZPosSetNone();

    virtual void startPlayerVoice(int, int); ///< @unofficial
    virtual void holdPlayerVoice(int, int); ///< @unofficial

    virtual void startQuakeShock(dQuake_c::TYPE_SHOCK_e);
    virtual void startPatternRumble(const char *pattern);

    virtual short getMukiAngle(u8 direction);
    virtual int turnAngle();
    virtual void maxFallSpeedSet();

    virtual bool setDamage2(dActor_c *, daPlBase_c::DamageType_e);

    void executeState();
    void changeDemoState(const sStateIDIf_c &, int);

    void onStatus(int id); ///< Enables the status with the given ID. See Status_e.
    void offStatus(int id); ///< Disables the status with the given ID. See Status_e.
    bool isStatus(int id); ///< Checks if the status with the given ID is active. See Status_e.
    /// Enables the status with the given ID and disables all others.
    /// @bug The implemententation of this is broken. Presumably this was written before
    /// the status bits were split up into multiple bytes, because it only clears the bits in the byte
    /// where the selected status bit is and leaves the other status bytes unchanged.
    void setStatus(int id);

    void calcPlayerSpeedXY();
    void posMoveAnglePenguin(mVec3_c, u16);
    void posMoveAnglePlayer(mVec3_c);
    bool setSandMoveSpeed();
    void calcWindSpeed();
    void calcHeadAttentionAngle();

    void calcSpeedOnIceLift();
    void calcAccOnIceLift();
    bool checkStandUpRoofOnLift();
    bool checkStandUpRoof();

    void gravitySet();
    void powerSet();
    void moveSpeedSet();
    void airPowerSet();
    void simpleMoveSpeedSet();
    void normalPowerSet();
    void grandPowerSet(); // [misspelling of "ground"]
    void slipPowerSet(int);
    void icePowerChange(int);
    void fn_8004bf80(SpeedData_t *data); ///< @unofficial
    void getTurnPower(sTurnPowerData &); ///< @unofficial
    PowerChangeType_e getPowerChangeType(bool);
    const float *getSpeedData();

    int addCalcAngleY(short, short);
    short getBesideMukiAngle(u8 direction);
    void turnBesideAngle();
    bool checkTurn();
    void setTurnEnd();

    void setJumpGravity();
    void setButtonJumpGravity();
    void setNormalJumpGravity();
    float setJumpAddSpeedF(float);
    float setAddLiftSpeedF();
    bool setDelayHelpJump();
    bool setCrouchJump();
    bool checkJumpTrigger();
    bool fn_800579c0(int, int); ///< @unofficial

    void setStampReduction();
    void setStampPlayerJump(bool b, float f);
    void calcReductionScale();
    mVec3_c getReductionModelScale();

    bool setJumpDaiRide();
    bool setPlayerJumpDai(daPlBase_c *other);
    void setPlayerJumoDaiPos();

    bool setCloudOn(dActor_c *cloudActor);
    void cancelCloudOn();
    mVec3_c getCloudPos();
    bool updateCloudMove();

    bool setFunsui();
    bool updateFunsuiPos(float, float);
    bool releaseFunsui(float);

    void setRideNat(float);
    void updateRideNat();

    bool isSaka();
    bool isSlipSaka();
    bool checkSakaReverse();
    bool checkSlip();
    bool checkCrouchSlip();
    bool checkSlipEndKey();
    float getSlipMaxSpeedF();
    float getSakaMaxSpeedRatio(u8 direction);
    float getSakaStopAccele(u8 direction);
    float getSakaMoveAccele(u8 direction);
    float getIceSakaSlipOffSpeed();

    void changeActionSlipEnd(AnmBlend_e);
    void setSlipAction_ToStoop();
    void setSlipAction_ToEnd();
    void setSlipActionEnd();
    void setSlipActionViewLimitEnd();

    void setHipAttackDropEffect();
    void setHipBlockBreak();
    void setHipAttack_Ready();
    void setHipAttack_KinopioStart();
    void setHipAttack_AttackFall();
    void setHipAttack_StandNormal();
    void setHipAttack_StandNormalEnd();
    void setHipAttack_ToStoop();
    void HipAction_Ready();
    void HipAction_AttackStart();
    void HipAction_AttackFall();
    void HipAction_Ground();
    void HipAction_StandNormal();
    void HipAction_StandNormalEnd();
    void HipAction_ToStoop();

    void onFollowMameKuribo();
    void clearFollowMameKuribo();
    u32 getFollowMameKuribo();

    bool isDemo();
    bool isControlDemoAll();
    bool isDemoAll();
    bool isDemoType(DemoType_e);
    bool isDemoMode() const;
    void onDemo();
    void offDemo();
    bool executeDemoState();

    bool startControlDemo();
    void setControlDemoDir(u8);
    bool isControlDemoWait();
    void setControlDemoWait();
    bool isControlDemoAnm(int);
    void setControlDemoAnm(int);
    bool isControlDemoWalk();
    void setControlDemoCutscene(AnimePlayArg_e animID); ///< @unofficial
    void setControlDemoKinopioWalk();
    void setControlDemoKinopioSwim();
    void setControlDemoEndingDance();
    void setControlDemoWalk(const float &, const float &);
    void endControlDemo(int);

    void initDemoInDokan();
    void initDemoInDokanUD(u8);
    void initDemoInDokanLR(u8);
    void executeDemoInDokan(u8);
    void endDemoInDokan();
    bool setDemoOutDokanAction(int entranceNextGotoID, DokanDir_e dir);
    void initDemoOutDokanUD(u8);
    void initDemoOutDokanLR(u8);
    void executeDemoOutDokanUD();
    void executeDemoOutDokanLR();
    void endDemoOutDokan();
    float getWaterDokanCenterOffset(float);
    bool demo_dokan_move_x(float, float);
    bool demo_dokan_move_y(float, float);
    void setObjDokanIn(dBg_ctr_c *, mVec3_c &, int);
    void setExitRailDokan();

    void stopGoalOther();
    void playGoalOther();
    void setDemoGoalMode(int, int);
    void setDemoGoal_MultiJump();
    void finalizeDemoGoalBase();
    float getDemoGoalLandPos();
    void initGoalJump(mVec3_c &, float);
    bool calcGoalJump();
    void executeDemoGoal_Pole();
    void executeDemoGoal_Wait();
    void executeDemoGoal_KimePose();
    bool setEnemyStageClearDemo();
    void updateEndingDance();
    void initDemoKimePose();
    void fn_80052ef0(int, int, int); ///< @unofficial

    bool isBossDemoLand();
    void DemoAnmNormal();
    void DemoAnmBossSetUp();
    void DemoAnmBossGlad();
    void DemoAnmBossAttention();
    void DemoAnmBossKeyGet();

    void setRunFootEffect();
    void setVsPlHipAttackEffect();
    void setLandSmokeEffect(int);
    void setLandSmokeEffectLight();
    void setSandEffect();
    bool setSandJumpEffect();
    bool setSandFunsuiLandEffect();
    void setStartJumpEffect(int);
    void setLandJumpEffect(int);
    void setSlipOnWaterEffect(mEf::levelEffect_c *effect);
    void setSlipSmokeEffect();
    void setBrakeSmokeEffect(mVec3_c &offset);
    void setTurnSmokeEffect();
    void fadeOutTurnEffect();

    void startKimePoseVoice(ClearType_e clearType);
    void setSoundPlyMode();
    void setItemCompleteVoice();
    void startFootSoundPlayer(unsigned long);
    void setFootSound();
    void setSlipSE();
    bool suppressSound(int suppressionMode); ///< @unofficial
    void startSound(ulong soundID, bool); ///< @unofficial
    void startSound(ulong soundID, short, bool); ///< @unofficial
    void holdSound(ulong soundID, bool); ///< @unofficial
    void holdSound(ulong soundID, short, bool); ///< @unofficial

    void initCollision(sCcDatNewF *dat1, sCcDatNewF *dat2);
    void entryCollision();
    void releaseCcData();
    void clearCcData();
    void clearCcPlayerRev();
    int getCcLineKind();
    void setCcAtBody(int);
    void setCcAtSlip();
    void setCcAtPenguinSlip();
    void setCcAtHipAttack();
    void setCcAtStar();
    void setCcAtCannon();
    bool isActionRevisionY();
    void setCcPlayerRev(dCc_c *, dCc_c *, float, int);
    bool calcCcPlayerRev(float *);
    bool isEnableStampPlayerJump(dCc_c *, dCc_c *);

    void bgCheck(int);
    void setOldBGCross();
    void checkBgCross();
    bool checkInsideCrossBg(float);
    void clearBgCheckInfo();
    bool isCarryObjBgCarried(u8);
    bool checkBGCrossWall(u8 direction);
    void checkDamageBg();
    bool setBgDamage();
    bool checkSinkSand();
    float getWaterCheckPosY();
    void checkWater();
    bool isHitWallKinopioWalk(int);
    bool checkKinopioWaitBG(int);
    void underOverCheck();
    void checkDispOver();
    bool checkPressBg();
    bool isBgPress(dActor_c *);
    bool isEnablePressUD();
    bool isEnablePressLR();
    void checkDisplayOutDead();
    bool fn_80052500(int, float, int); ///< @unofficial
    void fn_80055d00(); ///< @unofficial
    void fn_80056370(dActor_c *, BgPress_e); ///< @unofficial

    bool isDispOutCheckOn();
    bool calcSideLimitMultL(float);
    bool calcSideLimitMultR(float);
    void checkSideViewLemit();
    bool checkDispSideLemit();
    bool revSideLimitCommon(float);

    void calcTimerProc();
    void changeNormalAction();
    void stopOther();
    void playOther();
    void setStatus87(); ///< @unofficial
    void setStatus5D(float f); ///< @unofficial

    daPlBase_c *getHipAttackDamagePlayer();
    void setHipAttackDamagePlayer(daPlBase_c *player);
    void clearHipAttackDamagePlayer();

    void setNoHitPlayer(const daPlBase_c *, int);
    void updateNoHitPlayer();
    void setNoHitObjBg(dActor_c *, int);
    void calcNoHitObjBgTimer();

    void clearTreadCount();
    s8 calcTreadCount(int);
    void clearComboCount();
    s8 calcComboCount(int);

    dPyMdlBase_c *getModel();
    mVec3_c getAnkleCenterPos();
    bool isMaskDraw();
    bool isMameAction();
    bool isPlayerGameStop();
    bool checkTimeOut();
    bool checkRideActor(daPlBase_c *other);
    bool isRideCheckEnable();

    // [Needed to place getOldStateID in the correct location]
    const sStateIDIf_c &getOldState() {
        return *mStateMgr.getOldStateID();
    }

    bool isState(sStateIDIf_c &id) {
        return mStateMgr.getStateID()->isEqual(id);
    }

    bool isDemoState(sStateIDIf_c &id) {
        return mDemoStateMgr.getStateID()->isEqual(id);
    }

    float calcStarAccel(float f) { return 3.0f * f; }
    void set_m_d80(int i, float f) { m_d80[i] = f; }
    float getHeight() const { return mHeight; }
    float get_1064() const { return m_1064; }
    float get_1068() const { return m_1068; }
    float get_106c() const { return m_106c; }

    u32 isNowBgCross(BgCross1_e m) { return mNowBgCross1 & m; }
    u32 isNowBgCross(BgCross2_e m) { return mNowBgCross2 & m; }
    void onNowBgCross(BgCross1_e m) { mNowBgCross1 |= m; }
    void onNowBgCross(BgCross2_e m) { mNowBgCross2 |= m; }
    void offNowBgCross(BgCross1_e m) { mNowBgCross1 &= ~m; }
    void offNowBgCross(BgCross2_e m) { mNowBgCross2 &= ~m; }
    void clearNowBgCross() { mNowBgCross1 = mNowBgCross2 = 0; }

    u32 isOldBgCross(BgCross1_e m) { return mOldBgCross1 & m; }
    u32 isOldBgCross(BgCross2_e m) { return mOldBgCross2 & m; }
    void onOldBgCross(BgCross1_e m) { mOldBgCross1 |= m; }
    void onOldBgCross(BgCross2_e m) { mOldBgCross2 |= m; }
    void offOldBgCross(BgCross1_e m) { mOldBgCross1 &= ~m; }
    void offOldBgCross(BgCross2_e m) { mOldBgCross2 &= ~m; }
    void clearOldBgCross() { mOldBgCross1 = mOldBgCross2 = 0; }

    bool isOnSinkSand() { return isNowBgCross(BGC_ON_SINK_SAND) | isNowBgCross(BGC_IN_SINK_SAND); }
    bool wasOnSinkSand() { return isOldBgCross(BGC_ON_SINK_SAND) | isOldBgCross(BGC_IN_SINK_SAND); }

    float getDirSpeed() const { return sc_DirSpeed[mDirection]; }

    PLAYER_TYPE_e getPlayerType() const { return mPlayerType; }
    u8 getDirection() const { return mDirection; }
    PLAYER_POWERUP_e getPowerup() const { return mPowerup; }

    void changeState(const sStateIDIf_c &stateID) {
        changeState(stateID, 0);
    }

    template <typename T>
    void changeState(const sStateIDIf_c &stateID, T arg) {
        changeState(stateID, (void *) arg);
    }

    template <typename T>
    T stateChangeArg() const {
        return (T) mStateChangeArg;
    }

    SquishState_e mSquishState; ///< The player's current squish state for being jumped on by another player.
    int mSquishKeyframeIdx; ///< The current target index for the squishing animation keyframes.
    float mSquishScale; ///< The current scale of the player during the squish animation.
    int mSquishNoMoveTimer; ///< Timer for how long to freeze the squished player in position.
    int mSquishCooldownTimer; ///< Cooldown for another player to squish this player.

    const daPlBase_c *mpNoHitPlayer; ///< The player that cannot collide with this player.
    int mNoHitTimer; ///< Timer for how long the no-hit status lasts.

    u32 mBgPressActive;
    u32 mBgPressFlags;
    fBaseID_e mBgPressIDs[13]; ///< Index into this array with BgPress_e.
    float mViewLimitPadding;

    KimePoseMode_e mKimePoseMode;
    s8 mDemoState; /// Value is a ControlDemoState_e.

    int mDokanEnterNextGotoID; ///< The ID of the next-goto of the pipe being entered.
    /// Position of the door or pipe the player is entering.
    /// Also used as the target for the player running towards the castle after touching the goal pole.
    mVec3_c mWarpPos;
    mVec2_c mDokanMoveSpeed; ///< Direction to move the player while entering a rolling hill pipe or a rail pipe.
    short mRollDokanAngle;
    DemoDokanMode_e mDokanMode;
    u8 mDokanDir;
    dBg_ctr_c *mpDokanBgCtr;
    /// Counts up while walking to the left, and allows the
    /// player to enter a pipe when it reaches #sc_DokanEnterThreshold.
    u8 mDokanCounterL;
    /// Counts up while walking to the right, and allows the
    /// player to enter a pipe when it reaches #sc_DokanEnterThreshold.
    u8 mDokanCounterR;
    float mDokanOffsetY;
    float mDokanOffsetX;
    short mRailDokanRailIndex;
    short mRailDokanNextNodeTimer;

    int mGoalDemoIndex; ///< Indicates where the player is in the order of players which have touched the goal pole, 0 being the first.
    int mGoalTouchOrder;
    float mGoalPoleEndY;
    int mTimer_a8;
    mVec3_c mGoalJumpTarget;
    int mGoalJumpFrameCount;

    float m_bc;
    u8 mPad1[0x8];
    float m_c8;
    int m_cc;
    int m_d0;
    int mBossDemoLandTimer;
    int m_d8[5];
    int m_ec;
    int m_f0;
    int mTimer_f4;
    int mSlipEndTimer;
    s8 mAutoSlipTimer;
    dEf::followEffect_c mTurnSmokeEffect; ///< The wind effect when turning around after running.
    int mTurnGroundType;
    u8 mTurnEffectFade;
    mEf::levelEffect_c mHitAttackDropEffect; ///< The wind effect when doing a ground pound.
    u32 m_344;
    mVec3_c m_348;
    float mJumpDaiSpeedF; ///< The forward speed before doing a big jump.
    /// Timer for disabling another big jump after being unable to do a big jump
    /// due to colliding with a ceiling.
    int mJumpDaiFallTimer;

    AnimePlayArg_e mDemoAnime;

    int mIsBeingDeleted;

    /// Effect when being sent upwards by a sand fountain,
    /// also used for the wall slide, water run and death smoke effect.
    mEf::levelEffect_c mSmokeEffect;
    mEf::levelEffect_c mSlipSmokeEffect; ///< Smoke when sliding down a slope or into a cannon.
    mEf::levelEffect_c mBrakeSmokeEffect; ///< Smoke when turning around after running.
    mEf::levelEffect_c mRunEffect; ///< E.g. sand particles / snowflakes when running.
    mEf::levelEffect_c mQuicksandSplashEffect; ///< Sand splash effect when landing on quicksand.
    mEf::levelEffect_c mQuicksandSinkEffect; ///< Sand particles when the player is submerged in quicksand.
    dPyMdlMng_c *mpMdlMng;
    dAudio::SndObjctPly_c mSndObj;
    dAcPyKey_c mKey;
    fBaseID_e mRideActorID;
    fBaseID_e mRelatedActorID; ///< Actor that is eating the player, or the door actor.
    fBaseID_e mHipAttackPlayerID;
    u32 mStatusFlags[7];
    float mHeight;
    u8 mPrevDirection;
    u8 mAmiLayer;
    u8 mPlayerLayer;
    mVec3_c mLastPosDelta;
    mVec3_c mLiftRelatedPos;
    float m_cbc;
    float m_cc0;
    float mAirTopHeight; ///< The highest Y position since being on the ground last.
    float m_cc8;
    const float *mSpeedDataNormal;
    const float *mSpeedDataStar;
    const float *mGravityData;
    int mNoGravityTimer;
    int mStarTimer;
    int mDamageInvulnTimer;
    int mPowerupChangeInvulnTimer;
    int mTimer_ce8;
    s8 mTreadCount;
    s8 mStarCount;
    s8 mPlComboCount;
    u32 mNewFollowMameKuribo;
    u32 mFollowMameKuribo;
    fBaseID_e mIceActorID;
    PLAYER_POWERUP_e mPowerup;
    sBcPointData mHeadBcData;
    sBcPointData mFootBcData;
    sBcPointData mWallBcData;
    mVec3_c mBgPushForce; ///< Belts, quicksand etc.
    float m_d3c;

    u32 mNowBgCross1, mNowBgCross2;
    u32 mOldBgCross1, mOldBgCross2;
    u32 mBgFootHistory[10];

    u32 mStandOnUnitType;
    u32 mPrevStandOnUnitType;
    float m_d80[2];
    GroundType_e mGroundType;

    float m_d8c;
    int mNoHitObjTimer;

    short m_d94;
    short m_d96;
    short m_d98;
    short m_d9a;
    short m_d9c;

    int mAirWalkTimer; ///< Timer to wait before changing to the falling state after walking off a ledge.

    float mWaterHeight;
    float mPrevWaterHeight;
    int mWaterDepth;
    float mSinkSandHeight;
    bool mIsBgDamage;
    s8 mBgDamageType;
    u8 mWaterType; ///< Value is a dBc_c::WATER_TYPE_e.
    mVec3_c mAirWaterHitPos;
    short mAirWaterHitAngle;
    float m_dc8;
    float mRideNutHeight;

    dCc_c mCc1, mAttCc1, mAttCc2, mAttCc3;
    float m_1060;
    float m_1064;
    float m_1068;
    float m_106c;
    bool m_1070;
    bool m_1071;
    int mTimer_1074;
    u8 mDispLimitRelatedL;
    u8 mDispLimitRelatedR;
    float mDispLimitRelatedL2;
    float mDispLimitRelatedR2;

    sFStateMgr_c<daPlBase_c, sStateMethodUsr_FI_c> mDemoStateMgr; ///< The state manager for demo (cutscene) states.
    void *mDemoStateChangeParam; ///< To be used as a kind of argument to the new demo state.
    int mDemoSubstate; ///< Demo states can use this as a kind of sub-state variable (cast to some enum)
    /// Demo states can use this generic timer for various purposes.
    /// It is automatically decrememented in executeState() every frame.
    int mDemoSubstateTimer;
    bool mIsDemoMode; ///< Whether the player is currently in a demo (cutscene) state.

    sFStateMgr_c<daPlBase_c, sStateMethodUsr_FI_c> mStateMgr; ///< The state manager for regular player states.
    void *mStateChangeArg; ///< To be used as an argument to the new state.
    int mSubstate; ///< States can use this as a sub-state variable (cast to some enum).
    /// States can use this generic timer for various purposes.
    /// It is automatically decrememented in executeState() every frame.
    int mSubstateTimer;
    /// States can use this field for various purposes - as a timer, boolean flag, etc.
    int mSubstateValue;

    mVec3_c mPressAttachPos;
    int m_1128;
    float m_112c;
    float m_1130;
    float m_1134;
    float m_1138;
    float m_113c;
    PLAYER_TYPE_e mPlayerType;

    static const float sc_DirSpeed[];
    static const float sc_JumpSpeed;
    static const float sc_JumpSpeedNuma1;
    static const float sc_JumpSpeedNuma2;
    static const float sc_WaterWalkSpeed;
    static const float sc_WaterSwimSpeed;
    static const float sc_WaterJumpSpeed;
    static const float sc_WaterMaxFallSpeed;
    static const float sc_MaxFallSpeed;
    static const float sc_MaxFallSpeed_Foot;
    static const float sc_MaxFallDownSpeed;
    static const float scTurnPowerUpRate;
    static const float scDokanInSpeedX;
    static const float scDokanInWidthX;
    static const float scDokanInMoveSpeed;
    static const float scDokanWaitAnmFixFrame;

    // [Inofficial constants]

    /// Number of walking frames before being able to enter a pipe.
    /// @see mDokanCounterL, mDokanCounterR
    static const int sc_DokanEnterThreshold = 10;
    static const int sc_DemoPoleWaitTurn = 5; ///< Number of frames to wait before turning towards the screen in the goal pole animation.
    static const int sc_DemoPoleWaitEnd = 7; ///< Number of frames to wait before doing the course clear pose in the goal pole animation.
};
