#include <game/bases/d_player_model_base.hpp>
#include <game/bases/d_player_model_manager.hpp>
#include <game/bases/d_actor.hpp>
#include <game/bases/d_game_com.hpp>

const float scWaterCrouchAnmSpeed = 0.1f;
const float scFireShootFrame = 4.0f;

const char * dPyMdlBase_c::scJumpAnmVarDt[] = {
    "jump", "jump_b", "jump_c"
};

const char * dPyMdlBase_c::sc2JumpAnmVarDt[] = {
    "2jmp_c_1", "2jump1", "2jmp_b_1"
};

const char * dPyMdlBase_c::sc2JumpedAnmVarDt[] = {
    "2jmp_c_2", "2jump2", "2jmp_b_2"
};

const dPyMdlBase_c::AnmData_s dPyMdlBase_c::scPyAnmData[] = {
    { "wait", "Rwait", "YB_Rwait", "P_wait", m3d::FORWARD_LOOP, 1.2f, 0.0f, TEX_ANM_1, 0x2080 },
    { "walk", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 0.0f, -1.0f, TEX_ANM_1, 0x20c0 },
    { "run", "Rrun", "YB_Rrun", "P_run", m3d::FORWARD_LOOP, 0.0f, 10.0f, TEX_ANM_1, 0x20c0 },
    { "b_dash", "Rb_dash", "YB_Rb_dash", "P_b_dash", m3d::FORWARD_LOOP, 0.0f, 10.0f, TEX_ANM_1, 0x20c0 },
    { "b_dash2", "Rb_dash2", "YB_Rb_dash2", "P_b_dash2", m3d::FORWARD_LOOP, 0.0f, 10.0f, TEX_ANM_1, 0x20c0 },
    { "jump", "Rjump", "YB_Rjump", nullptr, m3d::FORWARD_ONCE, 0.5f, 1.0f, TEX_ANM_1, 0x280 },
    { "jump2", "Rjump2", "YB_Rjump2", nullptr, m3d::FORWARD_ONCE, 0.03f, 10.0f, TEX_ANM_1, 0x80 },
    { "jumped", "Rjumped", "YB_Rjumped", nullptr, m3d::FORWARD_ONCE, 0.8f, 0.0f, TEX_ANM_1, 0x80 },
    { "2jmp_c_1", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 0.5f, 1.0f, TEX_ANM_7, 0x280 },
    { "2jmp_c_2", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 0.5f, 10.0f, TEX_ANM_8, 0x280 },
    { "2jumped", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.5f, 0.0f, TEX_ANM_1, 0x80 },
    { "roll_jump", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 0.5f, 1.0f, TEX_ANM_7, 0x0 },
    { "2jump2", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 0.5f, 10.0f, TEX_ANM_1, 0x0 },
    { "mame_jump2", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 5.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "turn", "Rturn", "YB_Rturn", "P_turn", m3d::FORWARD_ONCE, 0.1f, -1.0f, TEX_ANM_1, 0x0 },
    { "turned", "Rturned", "YB_Rturned", "P_turned", m3d::FORWARD_ONCE, 3.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "hipsr", "Rhipsr", "YB_Rhipsr", "P_hipsr", m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x1 },
    { "hipat", "Rhipat", "YB_Rhipat", "P_hipat", m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x1 },
    { "hiped", "Rhiped", "YB_Rhiped", "P_hiped", m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x1 },
    { "hip_to_stoop", "Rhip_to_stoop", "YB_Rhip_to_stoop", "P_hip_to_stoop", m3d::FORWARD_ONCE, 1.5f, 0.0f, TEX_ANM_1, 0x1 },
    { "stoop", "Rstoop", "YB_Rstoop", "P_stoop", m3d::FORWARD_LOOP, 1.0f, 3.0f, TEX_ANM_1, 0x401 },
    { "stoop_start", "Rstoop_start", "YB_Rstoop_start", "P_stoop_start", m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x1 },
    { "slip", "Rslip", "YB_Rslip", nullptr, m3d::FORWARD_LOOP, 1.0f, 0.0f, TEX_ANM_1, 0x1 },
    { "sliped", "Rsliped", "YB_Rsliped", nullptr, m3d::FORWARD_ONCE, 1.0f, 3.0f, TEX_ANM_1, 0x1 },
    { "slip_to_stoop", "Rslip_to_stoop", "YB_Rslip_to_stoop", "P_slip_to_stoop", m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x1 },
    { "carry_wait", nullptr, nullptr, "P_carry_wait", m3d::FORWARD_LOOP, 0.5f, 0.0f, TEX_ANM_1, 0x2 },
    { "carry_walk", nullptr, nullptr, "P_carry_walk", m3d::FORWARD_LOOP, 0.0f, -1.0f, TEX_ANM_1, 0x2 },
    { "carry_throw", nullptr, nullptr, "P_carry_throw", m3d::FORWARD_ONCE, 1.0f, -1.0f, TEX_ANM_9, 0x2 },
    { "carry_wait", nullptr, nullptr, "P_carry_wait", m3d::FORWARD_LOOP, 0.0f, -1.0f, TEX_ANM_1, 0x2 },
    { "wsld", nullptr, nullptr, "P_wsld", m3d::FORWARD_ONCE, 1.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "fire_at", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.2f, 0.0f, TEX_ANM_1, 0x80 },
    { "swim_fire_at", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.2f, 0.0f, TEX_ANM_1, 0x0 },
    { "swim_fire_at2", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.2f, 0.0f, TEX_ANM_1, 0x0 },
    { "star_roll", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 0.7f, 0.0f, TEX_ANM_1, 0x0 },
    { "P_swim", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 10.0f, TEX_ANM_1, 0x800000 },
    { "swim", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 0.0f, TEX_ANM_1, 0x10 },
    { "swim_wait", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 10.0f, TEX_ANM_1, 0x10 },
    { "swim_throw", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x2 },
    { "swim_walk", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 0.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "swim_standing", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 10.0f, TEX_ANM_1, 0x0 },
    { "paddle_1", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x10 },
    { "paddle_2", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x10 },
    { "paddle_carry", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 0.0f, -1.0f, TEX_ANM_1, 0x2 },
    { "tree_start", "Rtree_start", "YB_Rtree_start", "P_tree_start", m3d::FORWARD_ONCE, 0.9f, 0.0f, TEX_ANM_1, 0x0 },
    { "tree_wait", "Rtree_wait", "YB_Rtree_wait", "P_tree_wait", m3d::FORWARD_LOOP, 1.0f, 10.0f, TEX_ANM_1, 0x0 },
    { "tree_climb", nullptr, nullptr, "P_tree_climb", m3d::FORWARD_LOOP, 1.0f, 10.0f, TEX_ANM_1, 0x0 },
    { "tree_pose", "Rtree_pose", "YB_Rtree_pose", "P_tree_pose", m3d::FORWARD_LOOP, 0.5f, 10.0f, TEX_ANM_1, 0x0 },
    { "monkey_start", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 0.7f, 0.0f, TEX_ANM_1, 0x0 },
    { "monkey_wait_r", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 0.5f, 0.0f, TEX_ANM_1, 0x0 },
    { "monkey_wait_l", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 0.5f, 0.0f, TEX_ANM_1, 0x0 },
    { "monkey_r_to_l", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "monkey_l_to_r", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "net_wait", nullptr, nullptr, "P_net_wait", m3d::FORWARD_LOOP, 1.0f, 5.0f, TEX_ANM_1, 0x0 },
    { "net_walk1", nullptr, nullptr, "P_net_walk1", m3d::FORWARD_LOOP, 0.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "net_walk2", nullptr, nullptr, "P_net_walk2", m3d::FORWARD_LOOP, 0.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "net_attack", nullptr, nullptr, "P_net_attack", m3d::FORWARD_ONCE, 1.2f, 0.0f, TEX_ANM_1, 0x0 },
    { "pea_plant", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.5f, -1.0f, TEX_ANM_1, 0x0 },
    { "pea_plant_st", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 2.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "pea_plant_wait", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.2f, 10.0f, TEX_ANM_1, 0x0 },
    { "wall_wait", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 0.8f, 10.0f, TEX_ANM_1, 0x0 },
    { "wall_walk_l", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 0.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "wall_walk_r", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 0.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "hang_start", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0xc },
    { "hang_up", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 2.0f, 5.0f, TEX_ANM_1, 0x4 },
    { "hang_wait", nullptr, nullptr, "P_hang_wait", m3d::FORWARD_LOOP, 0.5f, 10.0f, TEX_ANM_1, 0xc },
    { "hang_walk_l", nullptr, nullptr, "P_hang_walk_l", m3d::FORWARD_LOOP, 0.0f, -1.0f, TEX_ANM_1, 0xc },
    { "hang_walk_r", nullptr, nullptr, "P_hang_walk_r", m3d::FORWARD_LOOP, 0.0f, -1.0f, TEX_ANM_1, 0xc },
    { "w_jump1", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "w_jump2", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "2jumped", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "jump_hang", nullptr, nullptr, "P_jump_hang", m3d::FORWARD_ONCE, 1.0f, 3.0f, TEX_ANM_1, 0xc },
    { "spin_st", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "spin_end", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "spin_low_st", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, -1.0f, TEX_ANM_1, 0x1 },
    { "spin_low_ed", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, -1.0f, TEX_ANM_1, 0x1 },
    { "spin_jump2", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 0.5f, 0.0f, TEX_ANM_1, 0x0 },
    { "spin_jump_end", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "damF", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "damB", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "dowF", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, -1.0f, TEX_ANM_6, 0x0 },
    { "dowB", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, -1.0f, TEX_ANM_6, 0x0 },
    { "firejmp", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 0.5f, 0.0f, TEX_ANM_6, 0x0 },
    { "e_shock", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 0.5f, 0.0f, TEX_ANM_6, 0x0 },
    { "dead", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.5f, 0.0f, TEX_ANM_6, 0x0 },
    { "dead_pose", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 0.0f, TEX_ANM_6, 0x0 },
    { "goal_start", "Rtree_start", "YB_Rtree_start", nullptr, m3d::FORWARD_ONCE, 0.5f, 5.0f, TEX_ANM_1, 0x0 },
    { "goal_wait", "Rtree_wait", "YB_Rtree_wait", nullptr, m3d::FORWARD_LOOP, 1.0f, 5.0f, TEX_ANM_1, 0x0 },
    { "goal_pose", "Rtree_pose", "YB_Rtree_pose", nullptr, m3d::FORWARD_LOOP, 0.5f, 0.0f, TEX_ANM_1, 0x0 },
    { "goal_jump", "Rgoal_jump", "YB_Rgoal_jump", nullptr, m3d::FORWARD_ONCE, 0.5f, 0.0f, TEX_ANM_1, 0x0 },
    { "goal_jump_ed", "Rgoal_jump_ed", "YB_Rgoal_jump_ed", nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "goal_puton_cap", "Rgoal_puton_cap", "YB_Rgoal_puton_cap", nullptr, m3d::FORWARD_ONCE, 1.0f, 3.0f, TEX_ANM_2, 0x0 },
    { "PL_goal_puton_cap", "Rgoal_puton_cap", "YB_Rgoal_puton_cap", nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_3, 0x1000000 },
    { "P_goal_puton_cap", "Rgoal_puton_cap", "YB_Rgoal_puton_cap", nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_4, 0x800000 },
    { "wait", "PL_Rgoal_puton_cap", "YB_PL_Rgoal_puton_cap", "PLKB_goal_puton_cap", m3d::FORWARD_ONCE, 1.0f, 3.0f, TEX_ANM_5, 0x1000000 },
    { "wait", "P_Rgoal_puton_cap", "YB_P_Rgoal_puton_cap", nullptr, m3d::FORWARD_ONCE, 1.0f, 3.0f, TEX_ANM_5, 0x800000 },
    { "wait", "Rgoal_puton_cap", "YB_KB_Rgoal_puton_cap", nullptr, m3d::FORWARD_ONCE, 1.0f, 3.0f, TEX_ANM_5, 0x0 },
    { "wait", "Reat", "YB_Reat", nullptr, m3d::FORWARD_ONCE, 3.0f, 0.0f, TEX_ANM_1, 0x20 },
    { "wait", "Reat_out", "YB_Reat_out", nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x20 },
    { "wait", "Reat_success", "YB_Reat_success", nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x20 },
    { "wait", "Reat_fail", "YB_Reat_fail", nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x20 },
    { "wait", "Reat_successB", "YB_Reat_successB", nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x20 },
    { "wait", "Reat_successB", "YB_Reat_successB", nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x20 },
    { "wait", "RSeat", "YB_RSeat", nullptr, m3d::FORWARD_ONCE, 3.0f, 0.0f, TEX_ANM_1, 0x20 },
    { "wait", "RSeat_out", "YB_RSeat_out", nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x20 },
    { "wait", "RSeat_success", "YB_RSeat_success", nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x20 },
    { "wait", "RSeat_fail", "YB_RSeat_fail", nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x20 },
    { "wait", "RSeat_successB", "YB_RSeat_successB", nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x20 },
    { "wait", "RSeat_successB", "YB_RSeat_successB", nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x20 },
    { "carryP_start", nullptr, nullptr, "P_carryP_start", m3d::FORWARD_ONCE, 0.3f, -1.0f, TEX_ANM_1, 0x2 },
    { "carryP_wait", nullptr, nullptr, "P_carryP_wait", m3d::FORWARD_LOOP, 0.5f, 0.0f, TEX_ANM_1, 0x2 },
    { "carryP_walk", nullptr, nullptr, "P_carryP_walk", m3d::FORWARD_LOOP, 0.0f, -1.0f, TEX_ANM_1, 0x2 },
    { "carryP_throw", nullptr, nullptr, "P_carryP_throw", m3d::FORWARD_ONCE, 1.5f, -1.0f, TEX_ANM_9, 0x2 },
    { "carryP_wait", nullptr, nullptr, "P_carryP_wait", m3d::FORWARD_LOOP, 0.0f, -1.0f, TEX_ANM_1, 0x2 },
    { "Rwait", "ride_on", "YB_ride_on", nullptr, m3d::FORWARD_ONCE, 1.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "wait", "RFjump", "YB_RFjump", nullptr, m3d::FORWARD_LOOP, 3.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "wait", nullptr, "YB_set", nullptr, m3d::FORWARD_ONCE, 1.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "wait", nullptr, "YB_set_to_wait", nullptr, m3d::FORWARD_ONCE, 1.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "star_roll", nullptr, nullptr, "P_star_roll", m3d::FORWARD_LOOP, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "Sjump", "RSjump", "YB_RSjump", "P_Sjump", m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x1 },
    { "Sjump2", "RSjump2", "YB_RSjump2", "P_Sjump2", m3d::FORWARD_ONCE, 1.0f, 3.0f, TEX_ANM_1, 0x1 },
    { "Sjumped", "RSjumped", "YB_RSjumped", "P_Sjumped", m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x1 },
    { "get_down", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "ice_turn", "Rturn", "YB_Rturn", nullptr, m3d::FORWARD_LOOP, 1.75f, -1.0f, TEX_ANM_1, 0x0 },
    { "ice_turned", "Rturned", "YB_Rturned", nullptr, m3d::FORWARD_ONCE, 4.0f, -1.0f, TEX_ANM_1, 0x0 },
    { "ice_slipF", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.3f, 3.0f, TEX_ANM_1, 0x0 },
    { "ice_slipB", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.1f, 3.0f, TEX_ANM_1, 0x0 },
    { "rope_swing", nullptr, nullptr, "P_rope_swing", m3d::FORWARD_ONCE, 0.0f, 10.0f, TEX_ANM_1, 0x0 },
    { "shoot", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "shoot_slip", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "shoot_slip_end", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "low_walk_start", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 3.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "low_walk", nullptr, nullptr, "P_low_walk", m3d::FORWARD_LOOP, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "swim_pipe", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "door_walk", nullptr, nullptr, "P_door_walk", m3d::FORWARD_LOOP, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "PL_spin_jump", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "waitL", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.2f, 10.0f, TEX_ANM_1, 0x4080 },
    { "waitR", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.2f, 10.0f, TEX_ANM_1, 0x8080 },
    { "fire_at2", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.2f, -1.0f, TEX_ANM_1, 0x0 },
    { "blow_up", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 10.0f, TEX_ANM_1, 0x0 },
    { "P_slip", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 30.0f, TEX_ANM_1, 0x0 },
    { "P_slip_jump", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 0.7f, 0.0f, TEX_ANM_1, 0x0 },
    { "P_slip_jump2", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 0.1f, 40.0f, TEX_ANM_1, 0x0 },
    { "run", "Run", "YB_RFjump", nullptr, m3d::FORWARD_LOOP, 2.75f, 0.0f, TEX_ANM_1, 0x0 },
    { "stamp", "Rstamp", "YB_Rstamp", "P_stamp", m3d::FORWARD_LOOP, 1.0f, 3.0f, TEX_ANM_1, 0x0 },
    { "waitR", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.2f, 10.0f, TEX_ANM_1, 0x4080 },
    { "waitL", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.2f, 10.0f, TEX_ANM_1, 0x8080 },
    { "waitR3", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 5.0f, TEX_ANM_1, 0x80 },
    { "waitL3", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 5.0f, TEX_ANM_1, 0x80 },
    { "boss_key_get", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 5.0f, TEX_ANM_1, 0x0 },
    { "balloon_wait", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 0.0f, TEX_ANM_1, 0x100 },
    { "slope_waitL", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 0.0f, 10.0f, TEX_ANM_1, 0x5080 },
    { "slope_waitR", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 0.0f, 10.0f, TEX_ANM_1, 0x9080 },
    { "slope_waitL2", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.2f, 10.0f, TEX_ANM_1, 0x4080 },
    { "slope_waitR2", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.2f, 10.0f, TEX_ANM_1, 0x8080 },
    { "carryP_waitL", nullptr, nullptr, "P_carryP_waitL", m3d::FORWARD_LOOP, 1.2f, 10.0f, TEX_ANM_1, 0x4002 },
    { "carryP_waitR", nullptr, nullptr, "P_carryP_waitR", m3d::FORWARD_LOOP, 1.2f, 10.0f, TEX_ANM_1, 0x8002 },
    { "carry_waitL", nullptr, nullptr, "P_carry_waitL", m3d::FORWARD_LOOP, 1.2f, 10.0f, TEX_ANM_1, 0x4002 },
    { "carry_waitR", nullptr, nullptr, "P_carry_waitR", m3d::FORWARD_LOOP, 1.2f, 10.0f, TEX_ANM_1, 0x8002 },
    { "spin_jump3", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 30.0f, TEX_ANM_1, 0x0 },
    { "ride_wait", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 10.0f, TEX_ANM_1, 0x80 },
    { "P_paddle_1", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x800010 },
    { "P_paddle_2", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x800010 },
    { "waitL", nullptr, "YB_poseL", nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "waitR", nullptr, "YB_poseR", nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "waitR", nullptr, "YB_gorl_wait", nullptr, m3d::FORWARD_LOOP, 1.0f, 10.0f, TEX_ANM_1, 0x0 },
    { "dm_notice", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x80 },
    { "dm_noti_wait", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 0.0f, TEX_ANM_1, 0x80 },
    { "dm_surprise", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x80 },
    { "dm_surp_wait", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 0.0f, TEX_ANM_1, 0x80 },
    { "wait_select", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "course_in", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_A, 0x0 },
    { "wait", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "wait", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 0.0f, TEX_ANM_1, 0x0 },
    { "dm_escort", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_C, 0x0 },
    { "dm_glad", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 5.0f, TEX_ANM_D, 0x0 },
    { "ending_wait", nullptr, nullptr, nullptr, m3d::FORWARD_LOOP, 1.0f, 10.0f, TEX_ANM_1, 0x0 },
    { "coin_comp", nullptr, nullptr, nullptr, m3d::FORWARD_ONCE, 1.0f, 0.0f, TEX_ANM_B, 0x0 }
};

const u32 dPyMdlBase_c::lbl_802f2f38[2][2] = {
    {0x9D, 0x9B},
    {0x9C, 0x9A}
};

dPyMdlBase_c::dPyMdlBase_c(u8 val) : mHeadPosMaybe(0.0f, 0.0f, 0.0f), mHatPosMaybe(0.0f, 0.0f, 0.0f), mHeadOffset(0.0f, 0.0f, 0.0f), mScale(1.0f, 1.0f, 1.0f), m_another_player_ID(0), m_151(val), mPlayerMode(0), m_powerup_tex(0xFF), mCurrAnmID(-1), mPrevAnmID(-1), m_158(-1), m_164(0), m_168(0), mCurrHeadPatID(0), mNextPatSwitchTimer(0), m_174(0), m_178(0), m_17c(0), m_180(0.0f), m_184(0.0f), m_188_countdown(0), m_1f8(0), m_1fa(0), m_1fc(0), m_200(0), m_204(0) {
    mFlags = 0;
    m_160 = 0;
}

int dPyMdlBase_c::create(u8 a, u8 b, int c) {
    m_17c = c;
    m_another_player_ID = a;
    mAllocator.createFrmHeap(0xC000, mHeap::g_gameHeaps[0], NULL, 0x20);
    createModel();
    mAllocator.adjustFrmHeap();
    setPlayerMode(b);
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

void dPyMdlBase_c::calc(mMtx_c & a) {
    mBaseMtx = a;
    _calc();
}

void dPyMdlBase_c::calc(mVec3_c pos, mAng3_c rot, mVec3_c scale) {
    setBaseMtx(pos, rot, scale);
    _calc();
}

void dPyMdlBase_c::getJointMtx(mMtx_c* mtx, int i) {
    getBodyMdl()->getNodeWorldMtx(i, mtx);
}

void dPyMdlBase_c::getJointPos(mVec3_c* pos, int i) {
    mMtx_c tmp;
    getJointMtx(&tmp, i);

    mVec3_c tmp2;
    tmp2 = tmp.getTranslation();
    pos->set(tmp2.x, tmp2.y, tmp2.z);
}

void dPyMdlBase_c::setBaseMtx(mVec3_c& pos,mAng3_c& rot,mVec3_c& scale) {
    if (m_17c != 0) {
        mScale = scale;
    }

    float f1 = 0.0f;;
    float f2 = scale.y * 8.0f;
    if (mCurrAnmID == 0x2B || mCurrAnmID == 0x2C || mCurrAnmID == 0x2D) {
        f2 = 0.0f;
    } else if (mCurrAnmID == 0x34 || mCurrAnmID == 0x35 || mCurrAnmID == 0x36 || mCurrAnmID == 0x37) {
        f1 = -1.0f;
    } else if (mCurrAnmID == 0x7E) {
        f2 = 0.0f;
    } else if (mCurrAnmID == 0x7F) {
        f2 = scale.y * 5.0f;
    }

    float f3 = scale.y * 8.0f;
    mMtx_c tmp_70;
    mMtx_c tmp_a0;
    mMtx_c tmp_d0;
    mMtx_c tmp_100;
    mMtx_c tmp_130;
    mMtx_c tmp_160;
    mMtx_c tmp_190;

    tmp_70.YrotS(rot.y);
    PSMTXTrans(tmp_d0, 0.0f, f2, f1);
    PSMTXConcat(tmp_70, tmp_d0, tmp_70);

    tmp_70.XrotM(-rot.x);
    PSMTXTrans(tmp_100, 0.0f, -f2, 0.0f);
    PSMTXConcat(tmp_70, tmp_100, tmp_70);
    PSMTXTrans(tmp_130, 0.0f, f3, 0.0f);
    PSMTXConcat(tmp_70, tmp_130, tmp_70);

    tmp_70.ZrotM(-rot.z);
    PSMTXTrans(tmp_160, 0.0f, -f3, 0.0f);
    PSMTXConcat(tmp_70, tmp_160, tmp_70);
    PSMTXScale(tmp_190, scale.x, scale.y, scale.z);
    PSMTXConcat(tmp_70, tmp_190, tmp_70);

    mMtx = tmp_70;

    mVec3_c tmp_19c = pos;

    if (mCurrAnmID == 0x82 || mCurrAnmID == 0x83) {
        tmp_19c.y += 4.0f;
    } else if (mCurrAnmID == 0x85) {
        tmp_19c.y += 1.0f;
    } else {
        tmp_19c.y -= 0.2f;
    }
    tmp_19c.y += m_180;

    if (m_178 & 0x400) {
        tmp_19c.y += dPyMdlMng_c::m_hio.m_04;
    }

    if (m_17c == 0) {
        dActor_c::changePosAngle(&tmp_19c, nullptr, 1);
    }

    PSMTXTrans(tmp_a0, tmp_19c.x, tmp_19c.y, tmp_19c.z);
    PSMTXConcat(tmp_a0, tmp_70, tmp_a0);

    mBaseMtx = tmp_a0;
}

void dPyMdlBase_c::draw() {}

void dPyMdlBase_c::setSoftLight(m3d::bmdl_c & mdl) {
    if (m_17c > 1) {
        if ((int)m_17c != 2) {
            return;
        }
    } else {
        dGameCom::SetSoftLight_Player(mdl, 0);
        return;
    }

    dGameCom::SetSoftLight_Player(mdl, 1);
}

void dPyMdlBase_c::setColorType(u8) {}

bool dPyMdlBase_c::isBodyAnmOn() {
    if ((mPrevAnmID == -1) || (mPrevAnmID == mCurrAnmID)) {
        return false;
    }
    return true;
}

void dPyMdlBase_c::setFrame(float f) {
    mAnms[0].setFrame(f);
    if (!isBodyAnmOn()) {
        setBodyFrame(f);
    }
}

void dPyMdlBase_c::setBodyFrame(float f) {
    mAnms[1].setFrame(f);
}

void dPyMdlBase_c::setRate(float f) {
    mAnms[0].setRate(f);
    if (!isBodyAnmOn()) {
        setBodyRate(f);
    }
}

void dPyMdlBase_c::setBodyRate(float f) {
    mAnms[1].setRate(f);
}


void dPyMdlBase_c::setJumpAnmRand(dPyMdlBase_c::RndType_e rnd_type) {
    switch (rnd_type)
    {
    case RND_EQUAL:
        m_164 = dGameCom::rndInt(3);
        break;
    case RND_WEIGHTED:
        // ??? why is this converted to a float?
        float x = (float)(u32)dGameCom::rndInt(10);
        if (x < 6.0f) {
            m_164 = 0;
        } else if (x < 9.0f) {
            m_164 = 1;
        } else {
            m_164 = 2;
        }
        break;
    }

    if (m_164 > 2) {
        m_164 = 0;
    }
}

void dPyMdlBase_c::setAnm(int anim_id, float rate, float c, float frame) {

    if ((mPlayerMode == 3 /* mini */) && (anim_id == 6)) {
        setAnm(0xD /* mame_jump2 */, dPyMdlMng_c::m_hio.mPyAnm.mAnm[0xD /* mame_jump2 */].mRate, c, frame);
        return;
    }

    int prev_anm_id = mCurrAnmID;
    m_158 = -1;
    mCurrAnmID = anim_id;

    const dPyMdlBase_c::AnmData_s & anm_data = scPyAnmData[anim_id];

    if (prev_anm_id == anim_id) {
        mAnms[0].mPlayMode = anm_data.play_mode;
        setRate(rate);
        if (frame != 0.0f) {
            setFrame(frame);
        }
        return;
    }

    m_160 = scPyAnmData[anim_id].m_20;
    mFlags = scPyAnmData[anim_id].m_20;

    nw4r::g3d::ResAnmChr anm = getAnmResFile()->GetResAnmChr(scPyAnmData[anim_id].m_00);
    setPersonalAnm(mCurrAnmID, &anm, 0);

    float f31;
    if (frame != 0.0f) {
        f31 = 0.0f;
    } else {
        if (rate < 0.0f) {
            f31 = anm.GetNumFrame() - 1.0f;
        }

        if ((mFlags & 0x40) && (scPyAnmData[prev_anm_id].m_20 & 0x40)) {
            f31 = mAnms[0].getFrame();
        }
    }

    if (f31 >= anm.GetNumFrame()) {
        f31 = anm.GetNumFrame() - 1.0f;
    }

    if (m_178 & (4 | 2)) {
        if (m_178 & 4) {
            setLinkAnm(anim_id, rate, c, f31);
        }

        if (!(m_160 & 2)) {
            _setFootAnm(anm, anm_data.play_mode, rate, f31, c);
            setCarryBodyAnm(c);
            return;
        }
    }

    if (!(((!(m_178 & 8)) && (!(m_178 & 0x10))) || (mPrevAnmID == -1) || (m_160 & 0x20))) {
        _setFootAnm(anm, anm_data.play_mode, rate, f31, 0.0f);
        m3d::anmChr_c & body_anm = mAnms[1];
        setBodyAnm(mPrevAnmID, body_anm.getRate(), body_anm.getFrame(), 0.0f);
    } else {
        if (m_160 & 0x1000) {
            _setFootAnm(anm, anm_data.play_mode, rate, f31, c);
            setSlopeBodyAnm(c);
        } else {
            _setFootAnm(anm, anm_data.play_mode, rate, f31, c);
            mPrevAnmID = -1;
            _setBodyAnm(anm, anm_data.play_mode, rate, f31, c);
            setAnmBind();
            setTexAnmType(anm_data.m_1c);
        }
    }

}

const nw4r::g3d::ResFile * dPyMdlBase_c::getAnmResFile() {
    return nullptr;
}

void dPyMdlBase_c::copyAnm() {}

void dPyMdlBase_c::setBodyAnm(int anim_id, float a, float b, float c) {
    const dPyMdlBase_c::AnmData_s & anm_data = scPyAnmData[anim_id];
    u32 flag = anm_data.m_20;
    mPrevAnmID = anim_id;
    m_160 = flag;

    nw4r::g3d::ResAnmChr anm = getAnmResFile()->GetResAnmChr(anm_data.m_00);
    setPersonalAnm(mPrevAnmID, &anm, 1);
    _setBodyAnm(anm, anm_data.play_mode, a, b, c);
    setAnmBind();
}

void dPyMdlBase_c::releaseBodyAnm(float a) {
    mPrevAnmID = -1;
    const dPyMdlBase_c::AnmData_s & anm_data = scPyAnmData[mCurrAnmID];
    m_160 = anm_data.m_20;
    nw4r::g3d::ResAnmChr anm = getAnmResFile()->GetResAnmChr(anm_data.m_00);
    setPersonalAnm(mCurrAnmID, &anm, 1);

    if (!((!(m_178 & (4 | 2))) || (m_160 & 2))) {
        setCarryBodyAnm(a);
    } else if (m_160 & 0x1000) {
        setSlopeBodyAnm(a);
    } else {
        float frame = mAnms[0].getFrame();
        float num_frames = anm.GetNumFrame();
        if (frame >= num_frames) {
            frame = num_frames - 1.0f;
        }
        _setBodyAnm(anm, (m3d::playMode_e) mAnms[0].mPlayMode, mAnms[0].getRate(), frame, a);
    }
}


int lbl_8042cd90[2] = {0x1C, 0x70};

void dPyMdlBase_c::setCarryBodyAnm(float a) {

    int idx = 0;
    int idxb;
    if (m_178 & 4) {
        idx = 1;
    }

    if (mFlags & 0xC000) {
        idxb = 0;
        if (mFlags & 0x4000) {
            idxb = 1;
        }

        float f = mAnms[0].getFrame();
        setBodyAnm(lbl_802f2f38[idxb][idx], 1.2f, f, a);
    } else {
        setBodyAnm(lbl_8042cd90[idx], 1.2f, 0.0f, a);
    }
}

void dPyMdlBase_c::setSlopeBodyAnm(float a) {
    int idx = mCurrAnmID == 0x97 ? 0x99 : 0x98;
    float rate = dPyMdlMng_c::m_hio.mPyAnm.mAnm[idx & 0xFF].mRate;
    setBodyAnm(idx, rate, 0.0f, a);
}

void dPyMdlBase_c::_setFootAnm(nw4r::g3d::ResAnmChr&, m3d::playMode_e, float, float, float) {}

void dPyMdlBase_c::_setBodyAnm(nw4r::g3d::ResAnmChr&, m3d::playMode_e, float, float, float) {}

void dPyMdlBase_c::setAnmBind() {}

int dPyMdlBase_c::setPersonalRideAnm(int, nw4r::g3d::ResAnmChr *) { return 0; }

void dPyMdlBase_c::setRideAnm(int idx, float a, float b, float c) {
    int prev_158 = m_158;
    mCurrAnmID = -1;
    m_158 = idx;
    const AnmData_s & anm_data = scPyAnmData[idx];

    if (prev_158 == idx) {
        mAnms[0].mPlayMode = anm_data.play_mode;
        setRate(a);
        if (c != 0.0f) {
            setFrame(c);
        }
    } else {
        m_160 = 0;
        mFlags = 0;
        nw4r::g3d::ResAnmChr anm = getAnmResFile()->GetResAnmChr("Rwait");
        if (anm_data.m_04 != nullptr) {
            anm = getAnmResFile()->GetResAnmChr(anm_data.m_04);
            setPersonalRideAnm(idx, &anm);
        }

        float f6 = 0.0f;
        if (c != 0.0f) {
            if (c < anm.GetNumFrame()) {
                f6 = c;
            }
        } else {
            if (a < 0.0f) {
                f6 = anm.GetNumFrame() - 1.0f;
            }
        }

        _setFootAnm(anm, anm_data.play_mode, a, f6, b);
        _setBodyAnm(anm, anm_data.play_mode, a, f6, b);
        setTexAnmType(anm_data.m_1c);
    }
}

int dPyMdlBase_c::setPersonalAnm(int, nw4r::g3d::ResAnmChr *, int) { return 0; }

bool dPyMdlBase_c::getJumpAnmName(int p2, char * p3, int p4) {
    bool ret = false;

    if (p4 == 1) {
        if (m_160 & 0x200) {
            ret = true;
        }
    } else if (mFlags & 0x200) {
        ret = true;
    }

    if (ret) {
        switch (p2) {
            case 5:
                strncpy(p3, scJumpAnmVarDt[m_164], 0x20);
                break;
            case 8:
                strncpy(p3, sc2JumpAnmVarDt[m_164], 0x20);
                break;
            case 9:
                strncpy(p3, sc2JumpedAnmVarDt[m_164], 0x20);
                break;
        }
        return true;
    } else {
        m_168 = m_164;
        m_164 = 0;
        return false;
    }
}

void dPyMdlBase_c::setTexAnmType(dPyMdlBase_c::TexAnmType_e) {}

void dPyMdlBase_c::copyLinkAnm(float a) {
    setLinkAnm(mCurrAnmID, mAnms[0].getRate(), a, mAnms[0].getFrame());
}

void dPyMdlBase_c::setLinkAnm(int idx, float rate, float b, float frame) {
    dPyMdlBase_c * linkMdl = mpSpinLiftParentMdl;
    if (linkMdl == nullptr) {
        return;
    }

    if (linkMdl->m_178 & 0x80) {
        return;
    }

    int flags = mFlags;
    if ((flags & 2) || (flags & 0x10) || (flags & 0xC000)) {
        linkMdl->releaseBodyAnm(b);
    } else {
        linkMdl->m_164 = m_164;
        mpSpinLiftParentMdl->setBodyAnm(idx, rate, frame, b);
    }
}

void dPyMdlBase_c::calcStoopOffset() {
    if (m_188_countdown != 0) {
        sLib::chase(&m_180, m_184, (m_180 - m_184) / (float)m_188_countdown);
        m_188_countdown--;
    } else {
        m_180 = m_184;
    }
}

bool dPyMdlBase_c::isFootStepTiming() {
    if ((mCurrAnmID == 1) || (mCurrAnmID == 2) || (mCurrAnmID == 3) || (mCurrAnmID == 4) || (mCurrAnmID == 0x1A) || (mCurrAnmID == 0x6E)) {
        if (mAnms[0].checkFrame(4.0f) || mAnms[0].checkFrame(34.0f)) {
            return true;
        }
    } else if (mCurrAnmID == 0x85) {
        if (mAnms[0].checkFrame(3.0f) || mAnms[0].checkFrame(18.0f)) {
            return true;
        }
    } else if (mCurrAnmID == 0x83) {
        if (mAnms[0].checkFrame(0.0f) || mAnms[0].checkFrame(30.0f)) {
            return true;
        }
    } else if (mCurrAnmID == 0x8e) {
        if (mAnms[0].checkFrame(0.0f) || mAnms[0].checkFrame(15.0f) || mAnms[0].checkFrame(30.0f) || mAnms[0].checkFrame(45.0f)) {
            return true;
        }
    }

    return false;
}

float * dPyMdlBase_c::getLegLengthP(u8) { return nullptr; }

void dPyMdlBase_c::setPropelScale(float) {}

s16 dPyMdlBase_c::getPropelRollAngle() const { return 0; }

void dPyMdlBase_c::setPropelRollAngle(s16) {}

s16 dPyMdlBase_c::getPropelRollSpeed() const { return 0; }

void dPyMdlBase_c::setPropelRollSpeed(s16) {}

bool dPyMdlBase_c::getHeadPropelJointMtx(mMtx_c *) { return false; }

void dPyMdlBase_c::offStarAnm() {
    m_178 &= ~0x100;
}

void dPyMdlBase_c::onStarAnm() {
    m_178 |= 0x100;
}

void dPyMdlBase_c::setDark(int) {}

void dPyMdlBase_c::calc2() {}

int dPyMdlBase_c::getFaceJointIdx() const { return 0; }
