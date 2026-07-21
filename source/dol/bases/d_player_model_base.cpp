#include <game/bases/d_player_model_base.hpp>
#include <game/bases/d_player_model_manager.hpp>
#include <game/bases/d_actor.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_a_en_noko.hpp>

const float dPyMdlBase_c::scWaterCrouchAnmSpeed = 0.1f;
const float dPyMdlBase_c::scFireShootFrame = 4.0f;

const char *dPyMdlBase_c::scJumpAnmVarDt[] = {
    "jump", "jump_b", "jump_c"
};

const char *dPyMdlBase_c::sc2JumpAnmVarDt[] = {
    "2jmp_c_1", "2jump1", "2jmp_b_1"
};

const char *dPyMdlBase_c::sc2JumpedAnmVarDt[] = {
    "2jmp_c_2", "2jump2", "2jmp_b_2"
};

const dPyMdlBase_c::AnmData_s dPyMdlBase_c::scPyAnmData[PLAYER_ANIM_COUNT] = {
    {
        "wait", "Rwait", "YB_Rwait", "P_wait",
        m3d::FORWARD_LOOP, 1.2f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_2000 | FLAG_0000_0080
    },
    {
        "walk", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 0.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_0000_2000 | FLAG_0000_0080 | FLAG_0000_0040
    },
    {
        "run", "Rrun", "YB_Rrun", "P_run",
        m3d::FORWARD_LOOP, 0.0f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_2000 | FLAG_0000_0080 | FLAG_0000_0040
    },
    {
        "b_dash", "Rb_dash", "YB_Rb_dash", "P_b_dash",
        m3d::FORWARD_LOOP, 0.0f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_2000 | FLAG_0000_0080 | FLAG_0000_0040
    },
    {
        "b_dash2", "Rb_dash2", "YB_Rb_dash2", "P_b_dash2",
        m3d::FORWARD_LOOP, 0.0f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_2000 | FLAG_0000_0080 | FLAG_0000_0040
    },
    {
        "jump", "Rjump", "YB_Rjump", nullptr,
        m3d::FORWARD_ONCE, 0.5f, 1.0f,
        TEX_ANM_WAIT, FLAG_0000_0200 | FLAG_0000_0080
    },
    {
        "jump2", "Rjump2", "YB_Rjump2", nullptr,
        m3d::FORWARD_ONCE, 0.03f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_0080
    },
    {
        "jumped", "Rjumped", "YB_Rjumped", nullptr,
        m3d::FORWARD_ONCE, 0.8f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0080
    },
    {
        "2jmp_c_1", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 0.5f, 1.0f,
        TEX_ANM_JUMP, FLAG_0000_0200 | FLAG_0000_0080
    },
    {
        "2jmp_c_2", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 0.5f, 10.0f,
        TEX_ANM_JUMPED, FLAG_0000_0200 | FLAG_0000_0080
    },
    {
        "2jumped", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.5f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0080
    },
    {
        "roll_jump", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 0.5f, 1.0f,
        TEX_ANM_JUMP, FLAG_NONE
    },
    {
        "2jump2", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 0.5f, 10.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "mame_jump2", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 5.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "turn", "Rturn", "YB_Rturn", "P_turn",
        m3d::FORWARD_ONCE, 0.1f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "turned", "Rturned", "YB_Rturned", "P_turned",
        m3d::FORWARD_ONCE, 3.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "hipsr", "Rhipsr", "YB_Rhipsr", "P_hipsr",
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0001
    },
    {
        "hipat", "Rhipat", "YB_Rhipat", "P_hipat",
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0001
    },
    {
        "hiped", "Rhiped", "YB_Rhiped", "P_hiped",
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0001
    },
    {
        "hip_to_stoop", "Rhip_to_stoop", "YB_Rhip_to_stoop", "P_hip_to_stoop",
        m3d::FORWARD_ONCE, 1.5f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0001
    },
    {
        "stoop", "Rstoop", "YB_Rstoop", "P_stoop",
        m3d::FORWARD_LOOP, 1.0f, 3.0f,
        TEX_ANM_WAIT, FLAG_0000_0400 | FLAG_0000_0001
    },
    {
        "stoop_start", "Rstoop_start", "YB_Rstoop_start", "P_stoop_start",
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0001
    },
    {
        "slip", "Rslip", "YB_Rslip", nullptr,
        m3d::FORWARD_LOOP, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0001
    },
    {
        "sliped", "Rsliped", "YB_Rsliped", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 3.0f,
        TEX_ANM_WAIT, FLAG_0000_0001
    },
    {
        "slip_to_stoop", "Rslip_to_stoop", "YB_Rslip_to_stoop", "P_slip_to_stoop",
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0001
    },
    {
        "carry_wait", nullptr, nullptr, "P_carry_wait",
        m3d::FORWARD_LOOP, 0.5f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0002
    },
    {
        "carry_walk", nullptr, nullptr, "P_carry_walk",
        m3d::FORWARD_LOOP, 0.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_0000_0002
    },
    {
        "carry_throw", nullptr, nullptr, "P_carry_throw",
        m3d::FORWARD_ONCE, 1.0f, -1.0f,
        TEX_ANM_THROW, FLAG_0000_0002
    },
    {
        "carry_wait", nullptr, nullptr, "P_carry_wait",
        m3d::FORWARD_LOOP, 0.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_0000_0002
    },
    {
        "wsld", nullptr, nullptr, "P_wsld",
        m3d::FORWARD_ONCE, 1.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "fire_at", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.2f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0080
    },
    {
        "swim_fire_at", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.2f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "swim_fire_at2", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.2f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "star_roll", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 0.7f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "P_swim", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 10.0f,
        TEX_ANM_WAIT, FLAG_0080_0000
    },
    {
        "swim", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0010
    },
    {
        "swim_wait", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_0010
    },
    {
        "swim_throw", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0002
    },
    {
        "swim_walk", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 0.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "swim_standing", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 10.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "paddle_1", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0010
    },
    {
        "paddle_2", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0010
    },
    {
        "paddle_carry", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 0.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_0000_0002
    },
    {
        "tree_start", "Rtree_start", "YB_Rtree_start", "P_tree_start",
        m3d::FORWARD_ONCE, 0.9f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "tree_wait", "Rtree_wait", "YB_Rtree_wait", "P_tree_wait",
        m3d::FORWARD_LOOP, 1.0f, 10.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "tree_climb", nullptr, nullptr, "P_tree_climb",
        m3d::FORWARD_LOOP, 1.0f, 10.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "tree_pose", "Rtree_pose", "YB_Rtree_pose", "P_tree_pose",
        m3d::FORWARD_LOOP, 0.5f, 10.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "monkey_start", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 0.7f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "monkey_wait_r", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 0.5f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "monkey_wait_l", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 0.5f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "monkey_r_to_l", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "monkey_l_to_r", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "net_wait", nullptr, nullptr, "P_net_wait",
        m3d::FORWARD_LOOP, 1.0f, 5.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "net_walk1", nullptr, nullptr, "P_net_walk1",
        m3d::FORWARD_LOOP, 0.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "net_walk2", nullptr, nullptr, "P_net_walk2",
        m3d::FORWARD_LOOP, 0.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "net_attack", nullptr, nullptr, "P_net_attack",
        m3d::FORWARD_ONCE, 1.2f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "pea_plant", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.5f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "pea_plant_st", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 2.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "pea_plant_wait", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.2f, 10.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "wall_wait", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 0.8f, 10.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "wall_walk_l", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 0.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "wall_walk_r", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 0.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "hang_start", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0008 | FLAG_0000_0004
    },
    {
        "hang_up", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 2.0f, 5.0f,
        TEX_ANM_WAIT, FLAG_0000_0004
    },
    {
        "hang_wait", nullptr, nullptr, "P_hang_wait",
        m3d::FORWARD_LOOP, 0.5f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_0008 | FLAG_0000_0004
    },
    {
        "hang_walk_l", nullptr, nullptr, "P_hang_walk_l",
        m3d::FORWARD_LOOP, 0.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_0000_0008 | FLAG_0000_0004
    },
    {
        "hang_walk_r", nullptr, nullptr, "P_hang_walk_r",
        m3d::FORWARD_LOOP, 0.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_0000_0008 | FLAG_0000_0004
    },
    {
        "w_jump1", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "w_jump2", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "2jumped", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "jump_hang", nullptr, nullptr, "P_jump_hang",
        m3d::FORWARD_ONCE, 1.0f, 3.0f,
        TEX_ANM_WAIT, FLAG_0000_0008 | FLAG_0000_0004
    },
    {
        "spin_st", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "spin_end", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "spin_low_st", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_0000_0001
    },
    {
        "spin_low_ed", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_0000_0001
    },
    {
        "spin_jump2", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 0.5f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "spin_jump_end", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "damF", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "damB", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "dowF", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, -1.0f,
        TEX_ANM_DAM, FLAG_NONE
    },
    {
        "dowB", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, -1.0f,
        TEX_ANM_DAM, FLAG_NONE
    },
    {
        "firejmp", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 0.5f, 0.0f,
        TEX_ANM_DAM, FLAG_NONE
    },
    {
        "e_shock", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 0.5f, 0.0f,
        TEX_ANM_DAM, FLAG_NONE
    },
    {
        "dead", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.5f, 0.0f,
        TEX_ANM_DAM, FLAG_NONE
    },
    {
        "dead_pose", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 0.0f,
        TEX_ANM_DAM, FLAG_NONE
    },
    {
        "goal_start", "Rtree_start", "YB_Rtree_start", nullptr,
        m3d::FORWARD_ONCE, 0.5f, 5.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "goal_wait", "Rtree_wait", "YB_Rtree_wait", nullptr,
        m3d::FORWARD_LOOP, 1.0f, 5.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "goal_pose", "Rtree_pose", "YB_Rtree_pose", nullptr,
        m3d::FORWARD_LOOP, 0.5f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "goal_jump", "Rgoal_jump", "YB_Rgoal_jump", nullptr,
        m3d::FORWARD_ONCE, 0.5f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "goal_jump_ed", "Rgoal_jump_ed", "YB_Rgoal_jump_ed", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "goal_puton_cap", "Rgoal_puton_cap", "YB_Rgoal_puton_cap", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 3.0f,
        TEX_ANM_GOAL_PUTON_CAP, FLAG_NONE
    },
    {
        "PL_goal_puton_cap", "Rgoal_puton_cap", "YB_Rgoal_puton_cap", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_PL_GOAL_PUTON_CAP, FLAG_0100_0000
    },
    {
        "P_goal_puton_cap", "Rgoal_puton_cap", "YB_Rgoal_puton_cap", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_P_GOAL_PUTON_CAP, FLAG_0080_0000
    },
    {
        "wait", "PL_Rgoal_puton_cap", "YB_PL_Rgoal_puton_cap", "PLKB_goal_puton_cap",
        m3d::FORWARD_ONCE, 1.0f, 3.0f,
        TEX_ANM_P_RGOAL_PUTON_CAP, FLAG_0100_0000
    },
    {
        "wait", "P_Rgoal_puton_cap", "YB_P_Rgoal_puton_cap", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 3.0f,
        TEX_ANM_P_RGOAL_PUTON_CAP, FLAG_0080_0000
    },
    {
        "wait", "Rgoal_puton_cap", "YB_KB_Rgoal_puton_cap", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 3.0f,
        TEX_ANM_P_RGOAL_PUTON_CAP, FLAG_NONE
    },
    {
        "wait", "Reat", "YB_Reat", nullptr,
        m3d::FORWARD_ONCE, 3.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0020
    },
    {
        "wait", "Reat_out", "YB_Reat_out", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0020
    },
    {
        "wait", "Reat_success", "YB_Reat_success", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0020
    },
    {
        "wait", "Reat_fail", "YB_Reat_fail", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0020
    },
    {
        "wait", "Reat_successB", "YB_Reat_successB", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0020
    },
    {
        "wait", "Reat_successB", "YB_Reat_successB", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0020
    },
    {
        "wait", "RSeat", "YB_RSeat", nullptr,
        m3d::FORWARD_ONCE, 3.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0020
    },
    {
        "wait", "RSeat_out", "YB_RSeat_out", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0020
    },
    {
        "wait", "RSeat_success", "YB_RSeat_success", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0020
    },
    {
        "wait", "RSeat_fail", "YB_RSeat_fail", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0020
    },
    {
        "wait", "RSeat_successB", "YB_RSeat_successB", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0020
    },
    {
        "wait", "RSeat_successB", "YB_RSeat_successB", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0020
    },
    {
        "carryP_start", nullptr, nullptr, "P_carryP_start",
        m3d::FORWARD_ONCE, 0.3f, -1.0f,
        TEX_ANM_WAIT, FLAG_0000_0002
    },
    {
        "carryP_wait", nullptr, nullptr, "P_carryP_wait",
        m3d::FORWARD_LOOP, 0.5f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0002
    },
    {
        "carryP_walk", nullptr, nullptr, "P_carryP_walk",
        m3d::FORWARD_LOOP, 0.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_0000_0002
    },
    {
        "carryP_throw", nullptr, nullptr, "P_carryP_throw",
        m3d::FORWARD_ONCE, 1.5f, -1.0f,
        TEX_ANM_THROW, FLAG_0000_0002
    },
    {
        "carryP_wait", nullptr, nullptr, "P_carryP_wait",
        m3d::FORWARD_LOOP, 0.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_0000_0002
    },
    {
        "Rwait", "ride_on", "YB_ride_on", nullptr,
        m3d::FORWARD_ONCE, 1.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "wait", "RFjump", "YB_RFjump", nullptr,
        m3d::FORWARD_LOOP, 3.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "wait", nullptr, "YB_set", nullptr,
        m3d::FORWARD_ONCE, 1.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "wait", nullptr, "YB_set_to_wait", nullptr,
        m3d::FORWARD_ONCE, 1.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "star_roll", nullptr, nullptr, "P_star_roll",
        m3d::FORWARD_LOOP, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "Sjump", "RSjump", "YB_RSjump", "P_Sjump",
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0001
    },
    {
        "Sjump2", "RSjump2", "YB_RSjump2", "P_Sjump2",
        m3d::FORWARD_ONCE, 1.0f, 3.0f,
        TEX_ANM_WAIT, FLAG_0000_0001
    },
    {
        "Sjumped", "RSjumped", "YB_RSjumped", "P_Sjumped",
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0001
    },
    {
        "get_down", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "ice_turn", "Rturn", "YB_Rturn", nullptr,
        m3d::FORWARD_LOOP, 1.75f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "ice_turned", "Rturned", "YB_Rturned", nullptr,
        m3d::FORWARD_ONCE, 4.0f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "ice_slipF", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.3f, 3.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "ice_slipB", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.1f, 3.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "rope_swing", nullptr, nullptr, "P_rope_swing",
        m3d::FORWARD_ONCE, 0.0f, 10.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "shoot", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "shoot_slip", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "shoot_slip_end", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "low_walk_start", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 3.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "low_walk", nullptr, nullptr, "P_low_walk",
        m3d::FORWARD_LOOP, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "swim_pipe", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "door_walk", nullptr, nullptr, "P_door_walk",
        m3d::FORWARD_LOOP, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "PL_spin_jump", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "waitL", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.2f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_4000 | FLAG_0000_0080
    },
    {
        "waitR", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.2f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_8000 | FLAG_0000_0080
    },
    {
        "fire_at2", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.2f, -1.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "blow_up", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 10.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "P_slip", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 30.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "P_slip_jump", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 0.7f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "P_slip_jump2", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 0.1f, 40.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "run", "Run", "YB_RFjump", nullptr,  ///< [typo: "Run" should be "Rrun"]
        m3d::FORWARD_LOOP, 2.75f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "stamp", "Rstamp", "YB_Rstamp", "P_stamp",
        m3d::FORWARD_LOOP, 1.0f, 3.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "waitR", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.2f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_4000 | FLAG_0000_0080
    },
    {
        "waitL", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.2f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_8000 | FLAG_0000_0080
    },
    {
        "waitR3", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 5.0f,
        TEX_ANM_WAIT, FLAG_0000_0080
    },
    {
        "waitL3", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 5.0f,
        TEX_ANM_WAIT, FLAG_0000_0080
    },
    {
        "boss_key_get", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 5.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "balloon_wait", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0100
    },
    {
        "slope_waitL", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 0.0f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_4000 | FLAG_0000_1000 | FLAG_0000_0080
    },
    {
        "slope_waitR", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 0.0f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_8000 | FLAG_0000_1000 | FLAG_0000_0080
    },
    {
        "slope_waitL2", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.2f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_4000 | FLAG_0000_0080
    },
    {
        "slope_waitR2", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.2f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_8000 | FLAG_0000_0080
    },
    {
        "carryP_waitL", nullptr, nullptr, "P_carryP_waitL",
        m3d::FORWARD_LOOP, 1.2f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_4000 | FLAG_0000_0002
    },
    {
        "carryP_waitR", nullptr, nullptr, "P_carryP_waitR",
        m3d::FORWARD_LOOP, 1.2f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_8000 | FLAG_0000_0002
    },
    {
        "carry_waitL", nullptr, nullptr, "P_carry_waitL",
        m3d::FORWARD_LOOP, 1.2f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_4000 | FLAG_0000_0002
    },
    {
        "carry_waitR", nullptr, nullptr, "P_carry_waitR",
        m3d::FORWARD_LOOP, 1.2f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_8000 | FLAG_0000_0002
    },
    {
        "spin_jump3", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 30.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "ride_wait", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 10.0f,
        TEX_ANM_WAIT, FLAG_0000_0080
    },
    {
        "P_paddle_1", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0080_0000 | FLAG_0000_0010
    },
    {
        "P_paddle_2", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0080_0000 | FLAG_0000_0010
    },
    {
        "waitL", nullptr, "YB_poseL", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "waitR", nullptr, "YB_poseR", nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "waitR", nullptr, "YB_gorl_wait", nullptr,
        m3d::FORWARD_LOOP, 1.0f, 10.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "dm_notice", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0080
    },
    {
        "dm_noti_wait", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0080
    },
    {
        "dm_surprise", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0080
    },
    {
        "dm_surp_wait", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_0000_0080
    },
    {
        "wait_select", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "course_in", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_COURSE_IN, FLAG_NONE
    },
    {
        "wait", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "wait", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 0.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "dm_escort", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_DM_ESCORT, FLAG_NONE
    },
    {
        "dm_glad", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 5.0f,
        TEX_ANM_DM_GLAD, FLAG_NONE
    },
    {
        "ending_wait", nullptr, nullptr, nullptr,
        m3d::FORWARD_LOOP, 1.0f, 10.0f,
        TEX_ANM_WAIT, FLAG_NONE
    },
    {
        "coin_comp", nullptr, nullptr, nullptr,
        m3d::FORWARD_ONCE, 1.0f, 0.0f,
        TEX_ANM_COIN_COMP, FLAG_NONE
    }
};

dPyMdlBase_c::dPyMdlBase_c(u8 val) :
    mpOwner(nullptr), mpSpinLiftParentMdl(nullptr),
    mHeadPosMaybe(0.0f, 0.0f, 0.0f),
    mHatPosMaybe(0.0f, 0.0f, 0.0f),
    mHeadOffset(0.0f, 0.0f, 0.0f),
    mScale(1.0f, 1.0f, 1.0f),
    mAnotherPlayerID(0), m_151(val),
    mPlayerMode(POWERUP_NONE), mPowerupTex(-1),
    mCurrAnmID(PLAYER_ANIM_NONE), mPrevAnmID(PLAYER_ANIM_NONE), m_15c(PLAYER_ANIM_NONE),
    mJumpAnmVariant(0), mPrevJumpAnmVariant(0),
    mCurrTexAnmType(TEX_ANM_NONE), mNextPatSwitchTimer(0),
    m_178(0), m_17c(0), m_180(0),
    mStoopOffset(0.0f), mStoopOffsetTarget(0.0f), mStoopTimer(0),
    mAngX(0), mAngY(0), mAngZ(0),
    m_204(0), m_208(0)
{
    mFlags = FLAG_NONE;
    m_164 = FLAG_NONE;
}

dPyMdlBase_c::~dPyMdlBase_c() {}

int dPyMdlBase_c::create(u8 a, u8 playerMode, int c) {
    m_180 = c;
    mAnotherPlayerID = a;
    mAllocator.createFrmHeap(0xC000, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], NULL, 0x20);
    createModel();
    mAllocator.adjustFrmHeap();
    setPlayerMode(playerMode);
    initialize();

    return 0;
}

void dPyMdlBase_c::createModel() {}

void dPyMdlBase_c::setPlayerMode(int mode) {
    mPlayerMode = mode;
}

void dPyMdlBase_c::initialize() {}

void dPyMdlBase_c::play() {}

void dPyMdlBase_c::_calc() {}

void dPyMdlBase_c::calc(mMtx_c &baseMtx) {
    mBaseMtx = baseMtx;
    _calc();
}

void dPyMdlBase_c::calc(mVec3_c pos, mAng3_c rot, mVec3_c scale) {
    setBaseMtx(pos, rot, scale);
    _calc();
}

void dPyMdlBase_c::getJointMtx(mMtx_c *mtx, int i) {
    getBodyMdl()->getNodeWorldMtx(i, mtx);
}

void dPyMdlBase_c::getJointPos(mVec3_c *pos, int i) {
    mMtx_c tmp;
    getJointMtx(&tmp, i);

    float x = tmp.m[0][3];
    float y = tmp.m[1][3];
    float z = tmp.m[2][3];
    *pos = mVec3_c(x, y, z);
}

void dPyMdlBase_c::setBaseMtx(mVec3_c &pos, mAng3_c &rot, mVec3_c &scale) {
    if (m_180 != 0) {
        mScale = scale;
    }

    float shiftZ = 0.0f;
    float f2 = scale.y * 8.0f;
    switch (mCurrAnmID) {
        case PLAYER_ANIM_TREE_START:
        case PLAYER_ANIM_TREE_WAIT:
        case PLAYER_ANIM_TREE_CLIMB:
        case PLAYER_ANIM_TREE_POSE:
        case PLAYER_ANIM_ROPE_SWING:
            f2 = 0.0f;
            break;
        case PLAYER_ANIM_SHOOT:
            f2 = scale.y * 5.0f;
            break;
        case PLAYER_ANIM_NET_WAIT:
        case PLAYER_ANIM_NET_WALK1:
        case PLAYER_ANIM_NET_WALK2:
        case PLAYER_ANIM_NET_ATTACK:
            if (mPlayerMode == POWERUP_NONE) {
                shiftZ = 0.0f;
            }
            shiftZ = -1.0f;
            break;
    }

    float f3 = scale.y * 8.0f;

    mMtx_c mtx;
    mtx.YrotS(rot.y);
    mtx.concat(mMtx_c::createTrans(0.0f, f2, shiftZ));

    mtx.XrotM(-rot.x);
    mtx.concat(mMtx_c::createTrans(0.0f, -f2, 0.0f));
    mtx.concat(mMtx_c::createTrans(0.0f, f3, 0.0f));

    mtx.ZrotM(rot.z);
    mtx.concat(mMtx_c::createTrans(0.0f, -f3, 0.0f));
    mtx.concat(mMtx_c::createScale(scale));

    mMtx = mtx;

    mVec3_c shiftedPos = pos;
    switch (mCurrAnmID) {
        case PLAYER_ANIM_DOOR_WALK:
            shiftedPos.y += 1.0f;
            break;
        case PLAYER_ANIM_LOW_WALK_START:
        case PLAYER_ANIM_LOW_WALK:
            shiftedPos.y += 4.0f;
            break;
        default:
            shiftedPos.y -= 0.2f;
            break;
    }
    shiftedPos.y += mStoopOffset;

    if (m_17c & 0x400) {
        shiftedPos.y += dPyMdlMng_c::m_hio.get_04();
    }

    if (m_180 == 0) {
        dActor_c::changePosAngle(&shiftedPos, nullptr, 1);
    }

    mMtx_c finalMtx;
    finalMtx.trans(shiftedPos);
    finalMtx.concat(mtx);

    mBaseMtx = finalMtx;
}

void dPyMdlBase_c::draw() {}

void dPyMdlBase_c::setSoftLight(m3d::bmdl_c &mdl) {
    switch (m_180) {
        case 0:
        case 1:
            dGameCom::SetSoftLight_Player(mdl, 0);
            return;
        case 2:
            dGameCom::SetSoftLight_Player(mdl, 1);
            return;
    }
}

void dPyMdlBase_c::setColorType(u8) {}

bool dPyMdlBase_c::isBodyAnmOn() {
    if (mPrevAnmID == PLAYER_ANIM_NONE || mPrevAnmID == mCurrAnmID) {
        return false;
    }
    return true;
}

void dPyMdlBase_c::setFrame(float f) {
    getFootAnm().setFrame(f);
    if (!isBodyAnmOn()) {
        setBodyFrame(f);
    }
}

void dPyMdlBase_c::setBodyFrame(float f) {
    getBodyAnm().setFrame(f);
}

void dPyMdlBase_c::setRate(float f) {
    getFootAnm().setRate(f);
    if (!isBodyAnmOn()) {
        setBodyRate(f);
    }
}

void dPyMdlBase_c::setBodyRate(float f) {
    getBodyAnm().setRate(f);
}


void dPyMdlBase_c::setJumpAnmRand(dPyMdlBase_c::RndType_e rndType) {
    switch (rndType) {
        case RND_EQUAL:
            mJumpAnmVariant = dGameCom::rndInt(3);
            break;
        case RND_WEIGHTED:
            float x = dGameCom::rndInt(10);
            if (x < 6.0f) {
                mJumpAnmVariant = 0;
            } else if (x < 9.0f) {
                mJumpAnmVariant = 1;
            } else {
                mJumpAnmVariant = 2;
            }
            break;
    }

    if (mJumpAnmVariant > 2) {
        mJumpAnmVariant = 0;
    }
}

void dPyMdlBase_c::setAnm(int animID, float rate, float c, float frame) {
    if (mPlayerMode == POWERUP_MINI_MUSHROOM && animID == PLAYER_ANIM_JUMP2) {
        setAnm(PLAYER_ANIM_MAME_JUMP2, dPyMdlMng_c::m_hio.mPyAnm.mAnm[PLAYER_ANIM_MAME_JUMP2].mRate, c, frame);
        return;
    }

    int prevAnmID = mCurrAnmID;
    m_15c = PLAYER_ANIM_NONE;
    mCurrAnmID = animID;

    const dPyMdlBase_c::AnmData_s &anmData = scPyAnmData[animID];

    if (prevAnmID == animID) {
        getFootAnm().mPlayMode = anmData.mPlayMode;
        setRate(rate);
        if (frame != 0.0f) {
            setFrame(frame);
        }
        return;
    }

    m_164 = anmData.mFlags;
    mFlags = anmData.mFlags;

    nw4r::g3d::ResAnmChr anm = getAnmResFile()->GetResAnmChr(anmData.mName);
    setPersonalAnm(mCurrAnmID, &anm, 0);

    float footFrame = 0.0f;
    if (frame) {
        footFrame = frame;
    } else {
        if (rate < 0.0f) {
            footFrame = anm.GetNumFrame() - 1.0f;
        }

        if ((mFlags & FLAG_0000_0040) && (scPyAnmData[prevAnmID].mFlags & FLAG_0000_0040)) {
            footFrame = getFootAnm().getFrame();
        }
    }

    if (footFrame >= anm.GetNumFrame()) {
        footFrame = anm.GetNumFrame() - 1.0f;
    }

    if (m_17c & (4 | 2)) {
        if (m_17c & 4) {
            setLinkAnm(animID, rate, c, footFrame);
        }

        if (!(m_164 & FLAG_0000_0002)) {
            _setFootAnm(anm, anmData.mPlayMode, rate, footFrame, c);
            setCarryBodyAnm(c);
            return;
        }
    }

    if (((m_17c & 8) || (m_17c & 0x10)) && mPrevAnmID != PLAYER_ANIM_NONE && !(m_164 & FLAG_0000_0020)) {
        _setFootAnm(anm, anmData.mPlayMode, rate, footFrame, 0.0f);
        m3d::anmChr_c &body_anm = getBodyAnm();
        setBodyAnm(mPrevAnmID, body_anm.getRate(), body_anm.getFrame(), 0.0f);
    } else if (m_164 & FLAG_0000_1000) {
        _setFootAnm(anm, anmData.mPlayMode, rate, footFrame, c);
        setSlopeBodyAnm(c);
    } else {
        _setFootAnm(anm, anmData.mPlayMode, rate, footFrame, c);
        mPrevAnmID = PLAYER_ANIM_NONE;
        _setBodyAnm(anm, anmData.mPlayMode, rate, footFrame, c);
        setAnmBind();
        setTexAnmType(anmData.mTexAnmType);
    }
}

const nw4r::g3d::ResFile *dPyMdlBase_c::getAnmResFile() const {
    return nullptr;
}

void dPyMdlBase_c::copyAnm() {}

void dPyMdlBase_c::setBodyAnm(int animID, float rate, float frame, float c) {
    mPrevAnmID = animID;

    const dPyMdlBase_c::AnmData_s &anmData = scPyAnmData[animID];
    m_164 = anmData.mFlags;

    nw4r::g3d::ResAnmChr anm = getAnmResFile()->GetResAnmChr(anmData.mName);
    setPersonalAnm(mPrevAnmID, &anm, 1);
    _setBodyAnm(anm, anmData.mPlayMode, rate, frame, c);
    setAnmBind();
}

void dPyMdlBase_c::releaseBodyAnm(float a) {
    mPrevAnmID = PLAYER_ANIM_NONE;

    const dPyMdlBase_c::AnmData_s &anmData = scPyAnmData[mCurrAnmID];
    m_164 = anmData.mFlags;

    nw4r::g3d::ResAnmChr anm = getAnmResFile()->GetResAnmChr(anmData.mName);
    setPersonalAnm(mCurrAnmID, &anm, 1);

    if ((m_17c & (4 | 2)) && !(m_164 & FLAG_0000_0002)) {
        setCarryBodyAnm(a);
    } else if (m_164 & FLAG_0000_1000) {
        setSlopeBodyAnm(a);
    } else {
        float frame = getFootAnm().getFrame();
        float numFrames = anm.GetNumFrame();
        if (frame >= numFrames) {
            frame = numFrames - 1.0f;
        }
        _setBodyAnm(anm, (m3d::playMode_e) getFootAnm().mPlayMode, getFootAnm().getRate(), frame, a);
    }
}

void dPyMdlBase_c::setCarryBodyAnm(float a) {
    static const u32 lbl_802f2f38[2][2] = {
        {PLAYER_ANIM_CARRY_WAIT_R, PLAYER_ANIM_CARRY_P_WAIT_R},
        {PLAYER_ANIM_CARRY_WAIT_L, PLAYER_ANIM_CARRY_P_WAIT_L}
    };

    static const int lbl_8042cd90[2] = {PLAYER_ANIM_RCARRY_WAIT, PLAYER_ANIM_CARRY_P_WAIT_DUPLICATE};

    int dir = 0;
    if (m_17c & 4) {
        dir = 1;
    }

    if (mFlags & (FLAG_0000_8000 | FLAG_0000_4000)) {
        int carryType = 0;
        if (mFlags & FLAG_0000_4000) {
            carryType = 1;
        }

        float frame = getFootAnm().getFrame();
        setBodyAnm(lbl_802f2f38[carryType][dir], 1.2f, frame, a);
    } else {
        setBodyAnm(lbl_8042cd90[dir], 1.2f, 0.0f, a);
    }
}

void dPyMdlBase_c::setSlopeBodyAnm(float a) {
    int anmID;
    switch (mCurrAnmID) {
        case PLAYER_ANIM_SLOPE_WAIT_R:
            anmID = PLAYER_ANIM_SLOPE_WAIT_R2;
            break;
        default:
            anmID = PLAYER_ANIM_SLOPE_WAIT_L2;
            break;
    }

    float rate = dPyMdlMng_c::getHIO(anmID).mRate;
    setBodyAnm(anmID, rate, 0.0f, a);
}

void dPyMdlBase_c::_setFootAnm(nw4r::g3d::ResAnmChr &, m3d::playMode_e, float rate, float frame, float) {}

void dPyMdlBase_c::_setBodyAnm(nw4r::g3d::ResAnmChr &, m3d::playMode_e, float rate, float frame, float) {}

void dPyMdlBase_c::setAnmBind() {}

bool dPyMdlBase_c::setPersonalRideAnm(int, nw4r::g3d::ResAnmChr *) { return false; }

void dPyMdlBase_c::setRideAnm(int anmID, float rate, float b, float frame) {
    int prev_15c = m_15c;
    mCurrAnmID = PLAYER_ANIM_NONE;
    m_15c = anmID;
    const AnmData_s &anmData = scPyAnmData[anmID];

    if (prev_15c == anmID) {
        getFootAnm().mPlayMode = anmData.mPlayMode;
        setRate(rate);
        if (frame) {
            setFrame(frame);
        }
    } else {
        m_164 = FLAG_NONE;
        mFlags = FLAG_NONE;
        nw4r::g3d::ResAnmChr anm = getAnmResFile()->GetResAnmChr(scPyAnmData[PLAYER_ANIM_WAIT].mRideName);
        if (anmData.mRideName != nullptr) {
            anm = getAnmResFile()->GetResAnmChr(anmData.mRideName);
            setPersonalRideAnm(anmID, &anm);
        }

        float actualFrame = 0.0f;
        if (frame) {
            if (frame < anm.GetNumFrame()) {
                actualFrame = frame;
            }
        } else {
            if (rate < 0.0f) {
                actualFrame = anm.GetNumFrame() - 1.0f;
            }
        }

        _setFootAnm(anm, anmData.mPlayMode, rate, actualFrame, b);
        _setBodyAnm(anm, anmData.mPlayMode, rate, actualFrame, b);
        setTexAnmType(anmData.mTexAnmType);
    }
}

bool dPyMdlBase_c::setPersonalAnm(int, nw4r::g3d::ResAnmChr *, int) { return false; }

bool dPyMdlBase_c::getJumpAnmName(int jumpType, char *anmNameBuf, int p4) {
    bool ret = false;

    if (p4 == 1) {
        if (m_164 & FLAG_0000_0200) {
            ret = true;
        }
    } else if (mFlags & FLAG_0000_0200) {
        ret = true;
    }

    if (ret) {
        switch (jumpType) {
            case 5:
                strncpy(anmNameBuf, scJumpAnmVarDt[mJumpAnmVariant], 0x20);
                break;
            case 8:
                strncpy(anmNameBuf, sc2JumpAnmVarDt[mJumpAnmVariant], 0x20);
                break;
            case 9:
                strncpy(anmNameBuf, sc2JumpedAnmVarDt[mJumpAnmVariant], 0x20);
                break;
        }
        return true;
    } else {
        mPrevJumpAnmVariant = mJumpAnmVariant;
        mJumpAnmVariant = 0;
        return false;
    }
}

void dPyMdlBase_c::setTexAnmType(dPyMdlBase_c::TexAnmType_e anmType) {}

void dPyMdlBase_c::copyLinkAnm(float a) {
    setLinkAnm(mCurrAnmID, getFootAnm().getRate(), a, getFootAnm().getFrame());
}

void dPyMdlBase_c::setLinkAnm(int idx, float rate, float b, float frame) {
    if (mpSpinLiftParentMdl == nullptr) {
        return;
    }

    if (mpSpinLiftParentMdl->m_17c & 0x80) {
        return;
    }

    if ((mFlags & FLAG_0000_0002) || (mFlags & FLAG_0000_0010) || (mFlags & (FLAG_0000_8000 | FLAG_0000_4000))) {
        mpSpinLiftParentMdl->releaseBodyAnm(b);
    } else {
        mpSpinLiftParentMdl->mJumpAnmVariant = mJumpAnmVariant;
        mpSpinLiftParentMdl->setBodyAnm(idx, rate, frame, b);
    }
}

void dPyMdlBase_c::calcStoopOffset() {
    if (mStoopTimer != 0) {
        sLib::chase(&mStoopOffset, mStoopOffsetTarget, (mStoopOffset - mStoopOffsetTarget) / (float) mStoopTimer);
        mStoopTimer--;
    } else {
        mStoopOffset = mStoopOffsetTarget;
    }
}

bool dPyMdlBase_c::isFootStepTiming() {
    switch(mCurrAnmID) {
        case PLAYER_ANIM_WALK:
        case PLAYER_ANIM_RUN:
        case PLAYER_ANIM_B_DASH:
        case PLAYER_ANIM_B_DASH2:
        case PLAYER_ANIM_CARRY_WALK:
        case PLAYER_ANIM_CARRY_P_WALK:
            if (getFootAnm().checkFrame(4.0f) || getFootAnm().checkFrame(34.0f)) {
                return true;
            }
            break;
        case PLAYER_ANIM_DOOR_WALK:
            if (getFootAnm().checkFrame(3.0f) || getFootAnm().checkFrame(18.0f)) {
                return true;
            }
            break;
        case PLAYER_ANIM_LOW_WALK:
            if (getFootAnm().checkFrame(0.0f) || getFootAnm().checkFrame(30.0f)) {
                return true;
            }
            break;
        case PLAYER_ANIM_RF_JUMP:
            if (getFootAnm().checkFrame(0.0f) || getFootAnm().checkFrame(15.0f) || getFootAnm().checkFrame(30.0f) || getFootAnm().checkFrame(45.0f)) {
                return true;
            }
            break;
    }

    return false;
}
