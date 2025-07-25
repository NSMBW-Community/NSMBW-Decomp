#include <game/bases/d_profile.hpp>

fProfile::fProfilePtr_c profileList[] = {
    { .mBaseProfile  = &g_profile_BOOT },
    { .mBaseProfile  = &g_profile_AUTO_SELECT },
    { .mBaseProfile  = &g_profile_SELECT },
    { .mBaseProfile  = &g_profile_WORLD_MAP },
    { .mBaseProfile  = &g_profile_WORLD_9_DEMO },
    { .mBaseProfile  = &g_profile_STAGE },
    { .mBaseProfile  = &g_profile_RESTART_CRSIN },
    { .mBaseProfile  = &g_profile_CRSIN },
    { .mBaseProfile  = &g_profile_MOVIE },
    { .mBaseProfile  = &g_profile_GAMEOVER },
    { .mBaseProfile  = &g_profile_GAME_SETUP },
    { .mBaseProfile  = &g_profile_MULTI_PLAY_COURSE_SELECT },
    { .mActorProfile = &g_profile_CURSOR },
    { .mActorProfile = &g_profile_PLAYER },
    { .mActorProfile = &g_profile_YOSHI },
    { .mActorProfile = &g_profile_WM_TEST },
    { .mActorProfile = &g_profile_WM_TEST2 },
    { .mBaseProfile  = &g_profile_BG_GM },
    { .mBaseProfile  = &g_profile_CAMERA },
    { .mActorProfile = &g_profile_YOSHI_FIRE },
    { .mActorProfile = &g_profile_SLOW_QUICK_TAG },
    { .mActorProfile = &g_profile_TORIDE_KOKOOPA_DEMO },
    { .mActorProfile = &g_profile_BOSS_KAMECK_DEMO },
    { .mActorProfile = &g_profile_BOSS_KOOPA_DEMO },
    { .mActorProfile = &g_profile_CASTLE_KOKOOPA_DEMO_1ST },
    { .mActorProfile = &g_profile_CASTLE_KOKOOPA_DEMO_2ND },
    { .mActorProfile = &g_profile_BOSS_KOOPA_JR_DEMO },
    { .mActorProfile = &g_profile_KOOPA_SHIP_FOR_CASTLE_DEMO },
    { .mActorProfile = &g_profile_KOOPA_JR_FOR_CASTLE_DEMO },
    { .mActorProfile = &g_profile_PEACH_FOR_CASTLE_DEMO },
    { .mActorProfile = &g_profile_KAMECK_FOR_CASTLE_DEMO },
    { .mActorProfile = &g_profile_LEMMY_FOOTHOLD },
    { .mActorProfile = &g_profile_LEMMY_FOOTHOLD_MAIN },
    { .mActorProfile = &g_profile_CASTLE_BOSS_DOOR },
    { .mActorProfile = &g_profile_CASTLE_BOSS_KEY },
    { .mActorProfile = &g_profile_AC_BATTLE_GAME },
    { .mActorProfile = &g_profile_AC_BATTLE_KINOPIO },
    { .mActorProfile = &g_profile_AC_BATTLE_ITEM },
    { .mActorProfile = &g_profile_AC_ENDING_MAIN },
    { .mActorProfile = &g_profile_AC_BLOCK_WIND_TAG },
    { .mActorProfile = &g_profile_AC_GROUPID },
    { .mActorProfile = &g_profile_KAWANAGARE },
    { .mActorProfile = &g_profile_RIVER_PAIPO },
    { .mActorProfile = &g_profile_RIVER_ITEM },
    { .mActorProfile = &g_profile_RIVER_COIN },
    { .mActorProfile = &g_profile_RIVER_STARCOIN },
    { .mActorProfile = &g_profile_RIVER_LIFT },
    { .mActorProfile = &g_profile_RIVER_BARREL },
    { .mActorProfile = &g_profile_RIVER_PUKU },
    { .mActorProfile = &g_profile_RIVER_PAKKUN },
    { .mActorProfile = &g_profile_RIVER_MGR },
    { .mActorProfile = &g_profile_EN_KURIBO },
    { .mActorProfile = &g_profile_EN_PATA_KURIBO },
    { .mActorProfile = &g_profile_EN_MAME_KURIBO },
    { .mActorProfile = &g_profile_EN_NOKONOKO },
    { .mActorProfile = &g_profile_EN_PATAPATA },
    { .mActorProfile = &g_profile_EN_MET },
    { .mActorProfile = &g_profile_EN_TOGEZO },
    { .mActorProfile = &g_profile_EN_SAKASA_TOGEZO },
    { .mActorProfile = &g_profile_EN_BLOCK },
    { .mActorProfile = &g_profile_EN_ITEM },
    { .mActorProfile = &g_profile_EN_STAR_COIN },
    { .mActorProfile = &g_profile_EN_STAR_COIN_LINE },
    { .mActorProfile = &g_profile_EN_STAR_COIN_VOLT },
    { .mActorProfile = &g_profile_AC_FLAGON },
    { .mActorProfile = &g_profile_AC_4SWICHAND },
    { .mActorProfile = &g_profile_AC_4SWICHOR },
    { .mActorProfile = &g_profile_AC_RANDSWICH },
    { .mActorProfile = &g_profile_AC_CHNGESWICH },
    { .mActorProfile = &g_profile_AC_IFSWICH },
    { .mActorProfile = &g_profile_AC_RNSWICH },
    { .mActorProfile = &g_profile_EN_BKBLOCK },
    { .mActorProfile = &g_profile_EN_HNSWICH },
    { .mActorProfile = &g_profile_EN_PSWICH },
    { .mActorProfile = &g_profile_EN_QSWICH },
    { .mActorProfile = &g_profile_EN_BOSS_KOOPA_SWITCH },
    { .mActorProfile = &g_profile_EN_BOSS_KOOPA_BIG_SWITCH },
    { .mActorProfile = &g_profile_EN_BLSWICH },
    { .mActorProfile = &g_profile_EN_BLPSWICH },
    { .mActorProfile = &g_profile_EN_BLQSWICH },
    { .mActorProfile = &g_profile_EN_BUBBLE },
    { .mActorProfile = &g_profile_EN_DOSUN },
    { .mActorProfile = &g_profile_EN_BIGDOSUN },
    { .mActorProfile = &g_profile_EN_JUGEM },
    { .mActorProfile = &g_profile_EN_JUGEM_COIN },
    { .mActorProfile = &g_profile_EN_EATJUGEM },
    { .mActorProfile = &g_profile_EN_JUGEM_BODY },
    { .mActorProfile = &g_profile_EN_TOGEMET },
    { .mActorProfile = &g_profile_EN_FIREBAR },
    { .mActorProfile = &g_profile_EN_TOGETEKKYU },
    { .mActorProfile = &g_profile_EN_BIG_TOGETEKKYU },
    { .mActorProfile = &g_profile_EN_UP_DOKAN_PAKKUN },
    { .mActorProfile = &g_profile_EN_DOWN_DOKAN_PAKKUN },
    { .mActorProfile = &g_profile_EN_RIGHT_DOKAN_PAKKUN },
    { .mActorProfile = &g_profile_EN_LEFT_DOKAN_PAKKUN },
    { .mActorProfile = &g_profile_EN_UP_DOKAN_FPAKKUN },
    { .mActorProfile = &g_profile_EN_DOWN_DOKAN_FPAKKUN },
    { .mActorProfile = &g_profile_EN_RIGHT_DOKAN_FPAKKUN },
    { .mActorProfile = &g_profile_EN_LEFT_DOKAN_FPAKKUN },
    { .mActorProfile = &g_profile_EN_JIMEN_PAKKUN },
    { .mActorProfile = &g_profile_EN_JIMEN_BIG_PAKKUN },
    { .mActorProfile = &g_profile_EN_JIMEN_FPAKKUN },
    { .mActorProfile = &g_profile_EN_JIMEN_BIG_FPAKKUN },
    { .mActorProfile = &g_profile_EN_WALK_PAKKUN },
    { .mActorProfile = &g_profile_ICEBALL },
    { .mActorProfile = &g_profile_PL_FIREBALL },
    { .mActorProfile = &g_profile_PAKKUN_FIREBALL },
    { .mActorProfile = &g_profile_BROS_FIREBALL },
    { .mActorProfile = &g_profile_BOOMERANG },
    { .mActorProfile = &g_profile_EN_HELPOS },
    { .mActorProfile = &g_profile_EN_FIREBROS },
    { .mActorProfile = &g_profile_EN_BOOMERANGBROS },
    { .mActorProfile = &g_profile_EN_HAMMERBROS },
    { .mActorProfile = &g_profile_EN_ICEBROS },
    { .mActorProfile = &g_profile_EN_LIFT_HAMMERBROS },
    { .mActorProfile = &g_profile_HAMMER },
    { .mActorProfile = &g_profile_EN_HIMANBROS },
    { .mActorProfile = &g_profile_MEGA_HAMMER },
    { .mActorProfile = &g_profile_BROS_ICEBALL },
    { .mActorProfile = &g_profile_EN_BOYON },
    { .mActorProfile = &g_profile_EN_REMOCON_TORIMOCHI },
    { .mActorProfile = &g_profile_TAG_WIND },
    { .mActorProfile = &g_profile_TAG_THUNDER },
    { .mActorProfile = &g_profile_TAG_WATER },
    { .mActorProfile = &g_profile_EN_AUTOSCR },
    { .mActorProfile = &g_profile_OBJ_GRASS },
    { .mActorProfile = &g_profile_EN_ENV },
    { .mActorProfile = &g_profile_EN_KILLER },
    { .mActorProfile = &g_profile_EN_SEARCH_KILLER },
    { .mActorProfile = &g_profile_EN_MAGNUM_KILLER },
    { .mActorProfile = &g_profile_EN_SEARCH_MAGNUM_KILLER },
    { .mActorProfile = &g_profile_EN_BASABASA },
    { .mActorProfile = &g_profile_WAKI_PARABOM },
    { .mActorProfile = &g_profile_EN_BOMHEI },
    { .mActorProfile = &g_profile_EN_PARA_BOMHEI },
    { .mActorProfile = &g_profile_EN_MECHA_KOOPA },
    { .mActorProfile = &g_profile_EN_MOUSE },
    { .mActorProfile = &g_profile_EN_GHOST_JUGEM },
    { .mActorProfile = &g_profile_DOKAN_WAKIDASHI },
    { .mActorProfile = &g_profile_EN_CRASHER },
    { .mActorProfile = &g_profile_AC_WAKILIFT },
    { .mActorProfile = &g_profile_EN_BIRIKYU },
    { .mActorProfile = &g_profile_EN_LINE_BIRIKYU },
    { .mActorProfile = &g_profile_EN_BIRIKYU_MAKER },
    { .mActorProfile = &g_profile_EN_CHOROBON },
    { .mActorProfile = &g_profile_EN_SANBO },
    { .mActorProfile = &g_profile_EN_SANBO_PARTS },
    { .mActorProfile = &g_profile_EN_SANBO_EL },
    { .mActorProfile = &g_profile_EN_GURUGURU },
    { .mActorProfile = &g_profile_EN_GESSO },
    { .mActorProfile = &g_profile_EN_BARAMAKI_GESSO },
    { .mActorProfile = &g_profile_EN_GOALPOLE },
    { .mActorProfile = &g_profile_EN_GESSO_CHILD },
    { .mActorProfile = &g_profile_BARNAR_MGR },
    { .mActorProfile = &g_profile_EN_SYNCRO_BARNAR },
    { .mActorProfile = &g_profile_EN_BARNAR },
    { .mActorProfile = &g_profile_EN_LARGE_BARNAR },
    { .mActorProfile = &g_profile_EN_PUKUPUKU_PARENT },
    { .mActorProfile = &g_profile_AC_TEAM_PUKUPUKU },
    { .mActorProfile = &g_profile_EN_PUKUPUKU },
    { .mActorProfile = &g_profile_EN_TOGEPUKU },
    { .mActorProfile = &g_profile_EN_MIDDLE_PUKU },
    { .mActorProfile = &g_profile_EN_PUKUCOIN },
    { .mActorProfile = &g_profile_EN_KARON },
    { .mActorProfile = &g_profile_EN_BIGKARON },
    { .mActorProfile = &g_profile_EN_KARON_HEAD },
    { .mActorProfile = &g_profile_EN_REVERSE },
    { .mActorProfile = &g_profile_EN_WAKI_JUGEM },
    { .mActorProfile = &g_profile_EN_WIRE_TURN },
    { .mActorProfile = &g_profile_EN_BIG_WIRE_TURN },
    { .mActorProfile = &g_profile_EN_NET_NOKONOKO_LR },
    { .mActorProfile = &g_profile_EN_NET_NOKONOKO_UD },
    { .mActorProfile = &g_profile_BIGHANA_MGR },
    { .mActorProfile = &g_profile_HANA_MOUNTAIN },
    { .mActorProfile = &g_profile_EN_HANACHAN },
    { .mActorProfile = &g_profile_EN_BIG_HANACHAN },
    { .mActorProfile = &g_profile_EN_TERESA },
    { .mActorProfile = &g_profile_EN_BIG_TERESA },
    { .mActorProfile = &g_profile_EN_CROW },
    { .mActorProfile = &g_profile_EN_BIGPILE_UNDER },
    { .mActorProfile = &g_profile_EN_BIGPILE_UPPER },
    { .mActorProfile = &g_profile_EN_BIGPILE_RIGHT },
    { .mActorProfile = &g_profile_EN_BIGPILE_LEFT },
    { .mActorProfile = &g_profile_EN_SUPER_BIGPILE_RIGHT },
    { .mActorProfile = &g_profile_EN_SUPER_BIGPILE_LEFT },
    { .mActorProfile = &g_profile_EN_GOKUBUTO_BIGPILE_UNDER },
    { .mActorProfile = &g_profile_EN_GOKUBUTO_BIGPILE_UPPER },
    { .mActorProfile = &g_profile_EN_MORTON_BIGPILE },
    { .mActorProfile = &g_profile_EN_YAJIRUSHI },
    { .mActorProfile = &g_profile_EN_WANWAN },
    { .mActorProfile = &g_profile_EN_WANWAN_PILE },
    { .mActorProfile = &g_profile_EN_JUMPPUKU },
    { .mActorProfile = &g_profile_EN_IGAPUKU },
    { .mActorProfile = &g_profile_EN_WAKI_IGAPUKU },
    { .mActorProfile = &g_profile_EN_FIRESNAKE },
    { .mActorProfile = &g_profile_EN_BOSS_KAMECK },
    { .mActorProfile = &g_profile_EN_SLIP_PENGUIN },
    { .mActorProfile = &g_profile_WAKI_SLIP_PENGUIN2 },
    { .mActorProfile = &g_profile_EN_SLIP_PENGUIN2 },
    { .mActorProfile = &g_profile_SLIP_PENGUIN2_GLASSES },
    { .mActorProfile = &g_profile_EN_IGA_KURIBO },
    { .mActorProfile = &g_profile_EN_WATERSTREAM },
    { .mActorProfile = &g_profile_AWA },
    { .mActorProfile = &g_profile_DUMMY_DOOR_PARENT },
    { .mActorProfile = &g_profile_DUMMY_DOOR_CHILD },
    { .mActorProfile = &g_profile_DUMMY_DOOR },
    { .mActorProfile = &g_profile_NOBORIBOU_OBAKE },
    { .mActorProfile = &g_profile_UNTEI_ROPE },
    { .mActorProfile = &g_profile_REMO_DOOR },
    { .mActorProfile = &g_profile_REMO_SLIDE_DOOR },
    { .mActorProfile = &g_profile_BRANCH },
    { .mActorProfile = &g_profile_BG_CENTER },
    { .mActorProfile = &g_profile_KAMECK_MAGIC },
    { .mActorProfile = &g_profile_SINKDOKAN_UP },
    { .mActorProfile = &g_profile_SINKDOKAN_DOWN },
    { .mActorProfile = &g_profile_SINKDOKAN_LEFT },
    { .mActorProfile = &g_profile_SINKDOKAN_RIGHT },
    { .mActorProfile = &g_profile_FLYDOKAN_UP },
    { .mActorProfile = &g_profile_FLYDOKAN_DOWN },
    { .mActorProfile = &g_profile_FLYDOKAN_LEFT },
    { .mActorProfile = &g_profile_FLYDOKAN_RIGHT },
    { .mActorProfile = &g_profile_EN_HATENA_BALLOON },
    { .mActorProfile = &g_profile_EN_ONEWAY_GATE },
    { .mActorProfile = &g_profile_EN_PATABLOCK },
    { .mActorProfile = &g_profile_EN_KERONPA },
    { .mActorProfile = &g_profile_KERONPA_FIRE },
    { .mActorProfile = &g_profile_AC_NEXTGOTO_BLOCK },
    { .mActorProfile = &g_profile_EN_BAKUBAKU },
    { .mActorProfile = &g_profile_EN_DOOR },
    { .mActorProfile = &g_profile_EN_SWITCHDOOR },
    { .mActorProfile = &g_profile_EN_OBAKEDOOR },
    { .mActorProfile = &g_profile_EN_TORIDEDOOR },
    { .mActorProfile = &g_profile_EN_CASTLEDOOR },
    { .mActorProfile = &g_profile_EN_KOOPADOOR },
    { .mActorProfile = &g_profile_AC_REVERSEBLOCK },
    { .mActorProfile = &g_profile_CHUKAN_POINT },
    { .mActorProfile = &g_profile_EN_MAX_UE_LEFT },
    { .mActorProfile = &g_profile_EN_MAX_UE_RIGHT },
    { .mActorProfile = &g_profile_EN_MAX_UE2_LEFT },
    { .mActorProfile = &g_profile_EN_MAX_UE2_RIGHT },
    { .mActorProfile = &g_profile_EN_ZOOM },
    { .mActorProfile = &g_profile_EN_BOSS_LARRY },
    { .mActorProfile = &g_profile_EN_BOSS_WENDY },
    { .mActorProfile = &g_profile_EN_BOSS_IGGY },
    { .mActorProfile = &g_profile_EN_BOSS_LEMMY },
    { .mActorProfile = &g_profile_EN_BOSS_MORTON },
    { .mActorProfile = &g_profile_EN_BOSS_ROY },
    { .mActorProfile = &g_profile_EN_BOSS_LUDWIG },
    { .mActorProfile = &g_profile_EN_BOSS_CASTLE_LARRY },
    { .mActorProfile = &g_profile_EN_BOSS_CASTLE_ROY },
    { .mActorProfile = &g_profile_EN_BOSS_CASTLE_WENDY },
    { .mActorProfile = &g_profile_EN_BOSS_CASTLE_IGGY },
    { .mActorProfile = &g_profile_CASTLE_LEMMY_BALL },
    { .mActorProfile = &g_profile_EN_BOSS_CASTLE_LEMMY },
    { .mActorProfile = &g_profile_EN_BOSS_CASTLE_MORTON },
    { .mActorProfile = &g_profile_OBJ_LUDWIG },
    { .mActorProfile = &g_profile_EN_BOSS_CASTLE_LUDWIG },
    { .mActorProfile = &g_profile_EN_BOSS_KOOPA },
    { .mActorProfile = &g_profile_EN_BOSS_KOOPA_DEMO_KAMECK },
    { .mActorProfile = &g_profile_EN_BOSS_KOOPA_DEMO_PEACH },
    { .mActorProfile = &g_profile_AC_WAKI_KOOPA_FIRE },
    { .mActorProfile = &g_profile_KOOPA_FIRE },
    { .mActorProfile = &g_profile_LARRY_FIRE },
    { .mActorProfile = &g_profile_KOKOOPA_RING },
    { .mActorProfile = &g_profile_KOKOOPA_BALL },
    { .mActorProfile = &g_profile_KOKOOPA_THREEWAY_BLITZ },
    { .mActorProfile = &g_profile_LEMMY_BALL },
    { .mActorProfile = &g_profile_CASTLE_LEMMY_THROW_BALL },
    { .mActorProfile = &g_profile_OBJ_IGGY_SLED },
    { .mActorProfile = &g_profile_OBJ_IGGY_WANWAN },
    { .mActorProfile = &g_profile_OBJ_MORTON },
    { .mActorProfile = &g_profile_OBJ_ROY },
    { .mActorProfile = &g_profile_OBJ_LARRY },
    { .mActorProfile = &g_profile_OBJ_WENDY },
    { .mActorProfile = &g_profile_AC_BOSS_KOOPA_SCAFFOLD },
    { .mActorProfile = &g_profile_AC_BOSS_KOOPA_RUBBLE },
    { .mActorProfile = &g_profile_AC_BOSS_KOOPA_BG_EFFECT },
    { .mActorProfile = &g_profile_CASTLE_LUDWIG_BLITZ },
    { .mActorProfile = &g_profile_CASTLE_LUDWIG_BLITZ_MGR },
    { .mActorProfile = &g_profile_WAKI_KOKOOPA_FIRE },
    { .mActorProfile = &g_profile_FALL_FIRE },
    { .mActorProfile = &g_profile_WAKI_FIRE },
    { .mActorProfile = &g_profile_FIRE_BLITZ },
    { .mActorProfile = &g_profile_KOKOOPA_SEARCH_FIRE },
    { .mActorProfile = &g_profile_EN_UNIZOO },
    { .mActorProfile = &g_profile_EN_UNIRA },
    { .mActorProfile = &g_profile_EN_KANIBO },
    { .mActorProfile = &g_profile_EN_KANITAMA },
    { .mActorProfile = &g_profile_EN_KOPONE },
    { .mActorProfile = &g_profile_EN_AKOYA },
    { .mActorProfile = &g_profile_EN_MIDDLE_KURIBO },
    { .mActorProfile = &g_profile_EN_LARGE_KURIBO },
    { .mActorProfile = &g_profile_EN_BEANS_KURIBO },
    { .mActorProfile = &g_profile_AC_STRONGBOX },
    { .mActorProfile = &g_profile_EN_REMOCON_CANNON },
    { .mActorProfile = &g_profile_WAKI_TOGETEKKYU },
    { .mActorProfile = &g_profile_JR_CLOWN_A },
    { .mActorProfile = &g_profile_JR_CLOWN_B },
    { .mActorProfile = &g_profile_JR_CLOWN_C },
    { .mActorProfile = &g_profile_JR_CLOWN_FOR_PLAYER },
    { .mActorProfile = &g_profile_FLOOR_JR_A },
    { .mActorProfile = &g_profile_FLOOR_JR_B },
    { .mActorProfile = &g_profile_FLOOR_JR_C },
    { .mActorProfile = &g_profile_FLOOR_JR_C_MGR },
    { .mActorProfile = &g_profile_BOMB_JR_C },
    { .mActorProfile = &g_profile_EN_BOSS_KOOPA_JR_A },
    { .mActorProfile = &g_profile_EN_BOSS_KOOPA_JR_B },
    { .mActorProfile = &g_profile_EN_BOSS_KOOPA_JR_C },
    { .mActorProfile = &g_profile_JR_FIRE },
    { .mActorProfile = &g_profile_JR_FLOOR_FIRE },
    { .mActorProfile = &g_profile_JR_FLOOR_FIRE_MGR },
    { .mActorProfile = &g_profile_JR_CLOWN_FOR_JR_C_DEMO },
    { .mActorProfile = &g_profile_KAMECK_FOR_AIRSHIP_DEMO },
    { .mActorProfile = &g_profile_PEACH_FOR_AIRSHIP_DEMO },
    { .mActorProfile = &g_profile_KOOPA_SHIP_FOR_AIRSHIP_DEMO },
    { .mActorProfile = &g_profile_EN_EATCOIN },
    { .mActorProfile = &g_profile_AC_WAKI_KILLER },
    { .mActorProfile = &g_profile_WAKI_KERONPA },
    { .mActorProfile = &g_profile_AC_WAKI_PUKUPUKU },
    { .mActorProfile = &g_profile_BELT_NEEDLE },
    { .mActorProfile = &g_profile_INTERMITTENT },
    { .mActorProfile = &g_profile_MIST_INTERMITTENT },
    { .mActorProfile = &g_profile_YOGAN_INTERMITTENT },
    { .mActorProfile = &g_profile_EN_IBARAMUSHI },
    { .mActorProfile = &g_profile_WAKI_ANKOH },
    { .mActorProfile = &g_profile_EN_CHOCHIN_ANKOH },
    { .mActorProfile = &g_profile_EN_MISTMAN },
    { .mActorProfile = &g_profile_EN_TESTMAN },
    { .mActorProfile = &g_profile_EN_CLOUD },
    { .mActorProfile = &g_profile_AC_ITEM_KEY },
    { .mActorProfile = &g_profile_AC_YOSHI_EGG },
    { .mActorProfile = &g_profile_EN_WATERPAIPO },
    { .mActorProfile = &g_profile_EN_WATERPAKKUN },
    { .mActorProfile = &g_profile_FOO_BALL },
    { .mActorProfile = &g_profile_MT_HANACHAN },
    { .mActorProfile = &g_profile_EN_LR_STOP_UP },
    { .mActorProfile = &g_profile_EN_LR_STOP_DW },
    { .mActorProfile = &g_profile_EN_ROT_PAKKUN },
    { .mActorProfile = &g_profile_EN_POLTER },
    { .mActorProfile = &g_profile_EN_ICICLE },
    { .mActorProfile = &g_profile_WALLINSECT_MGR },
    { .mActorProfile = &g_profile_EN_WALLINSECT },
    { .mActorProfile = &g_profile_EN_LANDBARREL },
    { .mActorProfile = &g_profile_EN_IWAO },
    { .mActorProfile = &g_profile_IWAO_ROCK },
    { .mActorProfile = &g_profile_KAZAN_MGR },
    { .mActorProfile = &g_profile_KAZAN_ROCK },
    { .mActorProfile = &g_profile_EN_CANNON_BULLET },
    { .mActorProfile = &g_profile_EN_CHOROPU },
    { .mActorProfile = &g_profile_EN_MANHOLE_CHOROPU },
    { .mActorProfile = &g_profile_CHOROPOO_SHADOW },
    { .mActorProfile = &g_profile_MINI_GAME_BALLOON },
    { .mActorProfile = &g_profile_MINI_GAME_BALLOON_STICK },
    { .mActorProfile = &g_profile_EN_JELLY_FISH },
    { .mActorProfile = &g_profile_MINI_GAME_WIRE_MESH },
    { .mActorProfile = &g_profile_MINI_GAME_GUN_BATTERY_MGR },
    { .mBaseProfile  = &g_profile_MINI_GAME_GUN_BATTERY_MGR_OBJ },
    { .mActorProfile = &g_profile_MINI_GAME_WIRE_MESH_MGR },
    { .mBaseProfile  = &g_profile_MINI_GAME_WIRE_MESH_MGR_OBJ },
    { .mActorProfile = &g_profile_PEACH_CASTLE_SEQUENCE_MGR },
    { .mBaseProfile  = &g_profile_PEACH_CASTLE_SEQUENCE_MGR_OBJ },
    { .mActorProfile = &g_profile_PEACH_CASTLE_BLOCK },
    { .mActorProfile = &g_profile_MINI_GAME_KINOPIO },
    { .mActorProfile = &g_profile_AC_KANBAN_ARROW },
    { .mActorProfile = &g_profile_AC_WATER_BUBBLE },
    { .mActorProfile = &g_profile_EN_BUBBLE_CONTROL },
    { .mActorProfile = &g_profile_AC_WAKI_WOODBOX },
    { .mActorProfile = &g_profile_AC_AUTOSCROOL_SWICH },
    { .mActorProfile = &g_profile_AC_ROTATION_GHOST_PARENT },
    { .mActorProfile = &g_profile_EN_ROTATION_GHOST },
    { .mActorProfile = &g_profile_SHIP_WINDOW },
    { .mActorProfile = &g_profile_EN_MADOPU },
    { .mActorProfile = &g_profile_SPANA },
    { .mActorProfile = &g_profile_EN_GABON },
    { .mActorProfile = &g_profile_GABON_ROCK },
    { .mActorProfile = &g_profile_MANHOLE },
    { .mActorProfile = &g_profile_BGM_INTERLOCKING_DUMMY_BLOCK },
    { .mActorProfile = &g_profile_BGM_INTERLOCKING_DUMMY_BLOCK_MGR },
    { .mActorProfile = &g_profile_EN_KING_KILLER },
    { .mActorProfile = &g_profile_EN_WATER_BUBBLE_BULLET },
    { .mActorProfile = &g_profile_EN_FRUIT },
    { .mActorProfile = &g_profile_OBJ_SPARKS },
    { .mActorProfile = &g_profile_OBJ_SNOW },
    { .mActorProfile = &g_profile_TAG_SCROLL },
    { .mActorProfile = &g_profile_AC_BLOCK_GROUP },
    { .mActorProfile = &g_profile_AC_BLOCK_ONEUP },
    { .mActorProfile = &g_profile_AC_BLOCK_COIN },
    { .mActorProfile = &g_profile_AC_WAKI_SEARCH_MAGKILLER },
    { .mActorProfile = &g_profile_EN_TOBIPUKU },
    { .mActorProfile = &g_profile_AC_WAKI_TOBIPUKU },
    { .mActorProfile = &g_profile_NEEDLE_FOR_KOOPA_JR_B },
    { .mActorProfile = &g_profile_TOP_BG_FOR_CASTLE_LUDWIG },
    { .mActorProfile = &g_profile_MIDDLE_BG_FOR_CASTLE_LUDWIG },
    { .mActorProfile = &g_profile_BOTTOM_BG_FOR_CASTLE_LUDWIG },
    { .mActorProfile = &g_profile_MD_ACTOR },
    { .mBaseProfile  = &g_profile_MD_CAMERA },
    { .mActorProfile = &g_profile_EN_CLOUDLT },
    { .mActorProfile = &g_profile_UNIT_RAIL },
    { .mActorProfile = &g_profile_UNIT_CHIKUWA },
    { .mActorProfile = &g_profile_OBJ_SEAWEED },
    { .mActorProfile = &g_profile_TAG_ENVSOUND },
    { .mActorProfile = &g_profile_EN_COIN },
    { .mActorProfile = &g_profile_EN_COIN_JUGEM },
    { .mActorProfile = &g_profile_EN_COIN_JUMP },
    { .mActorProfile = &g_profile_EN_COIN_FLOOR },
    { .mActorProfile = &g_profile_EN_COIN_VOLT },
    { .mActorProfile = &g_profile_EN_COIN_WIND },
    { .mActorProfile = &g_profile_EN_BLUE_COIN },
    { .mActorProfile = &g_profile_EN_COIN_WATER },
    { .mActorProfile = &g_profile_EN_REDCOIN },
    { .mActorProfile = &g_profile_EN_GREENCOIN },
    { .mActorProfile = &g_profile_ICE_ACTOR },
    { .mActorProfile = &g_profile_AC_LIGHT_BLOCK },
    { .mActorProfile = &g_profile_AC_PROP_BLOCK },
    { .mActorProfile = &g_profile_ENEMY_ICE },
    { .mActorProfile = &g_profile_PLAYER_ICE },
    { .mActorProfile = &g_profile_SLIDE_BLOCK },
    { .mActorProfile = &g_profile_POW_BLOCK },
    { .mActorProfile = &g_profile_BLOCK_TARU },
    { .mActorProfile = &g_profile_OBJ_CENTER },
    { .mActorProfile = &g_profile_OBJ_CENTER2 },
    { .mActorProfile = &g_profile_EN_LIFT_ROTATION_FULL },
    { .mActorProfile = &g_profile_LIFT_ROTATION_FULL_CHIKA },
    { .mActorProfile = &g_profile_LIFT_ROTATION_FULL_KINOKO },
    { .mActorProfile = &g_profile_LIFT_ROTATION_FULL_COLOR },
    { .mActorProfile = &g_profile_CAGE_BLOCK },
    { .mActorProfile = &g_profile_LIFT_ZEN_HAMMER },
    { .mActorProfile = &g_profile_LIFT_ZEN_SUISYA },
    { .mActorProfile = &g_profile_LIFT_ZEN_KAITEN_KANAAMI },
    { .mActorProfile = &g_profile_MOUSE_HOLE },
    { .mActorProfile = &g_profile_KATAMUKIYUKA },
    { .mActorProfile = &g_profile_LIFT_TORIDE_ROLL },
    { .mActorProfile = &g_profile_AC_LIFT_RIDE_HMOVE },
    { .mActorProfile = &g_profile_AC_LIFT_OBJBG_VMOVE },
    { .mActorProfile = &g_profile_AC_LIFT_OBJBG_HMOVE },
    { .mActorProfile = &g_profile_AC_LIFT_OBJBG_HMOVE_BIG },
    { .mActorProfile = &g_profile_EN_OBJ_POL },
    { .mActorProfile = &g_profile_EN_OBJ_POL_TORIDE },
    { .mActorProfile = &g_profile_EN_OBJ_POL_NEEDLE_L },
    { .mActorProfile = &g_profile_EN_OBJ_POL_NEEDLE_R },
    { .mActorProfile = &g_profile_EN_OBJ_POL_NEEDLE_LR },
    { .mActorProfile = &g_profile_EN_OBJ_POL_NEEDLE_U },
    { .mActorProfile = &g_profile_EN_OBJ_POL_NEEDLE_D },
    { .mActorProfile = &g_profile_EN_OBJ_POL_NEEDLE_UD },
    { .mActorProfile = &g_profile_LIFT_ZEN_TOGE },
    { .mActorProfile = &g_profile_LIFT_ZEN_SHIRO_YOGAN },
    { .mActorProfile = &g_profile_LIFT_ZEN_KAIHEI },
    { .mActorProfile = &g_profile_LIFT_DOKAN },
    { .mActorProfile = &g_profile_EN_PAIR_OBJ_PARENT },
    { .mActorProfile = &g_profile_EN_PAIR_OBJ_CHILD },
    { .mActorProfile = &g_profile_LIFT_ZEN_SHIRO },
    { .mActorProfile = &g_profile_EN_PAIR_OBJ_CHILD_TORIDE },
    { .mActorProfile = &g_profile_WAKI_ICE_ASHIBA },
    { .mActorProfile = &g_profile_WAKI_ICE_ASHIBA_WATER },
    { .mActorProfile = &g_profile_ICE_ASHIBA },
    { .mActorProfile = &g_profile_ICE_ASHIBA_WATER },
    { .mActorProfile = &g_profile_ICE_ASHIBA_RAIL },
    { .mActorProfile = &g_profile_AC_LIFT_RIDE_VMOVE },
    { .mActorProfile = &g_profile_AC_LIFT_SEESAW },
    { .mActorProfile = &g_profile_AC_LIFT_FALL },
    { .mActorProfile = &g_profile_AC_LIFT_REMOCON_SEESAW },
    { .mActorProfile = &g_profile_EN_LIFT_WHEEL },
    { .mActorProfile = &g_profile_AC_LIFT_BALANCE },
    { .mActorProfile = &g_profile_AC_LIFT_REMOCON_BALANCE_CLINCH },
    { .mActorProfile = &g_profile_AC_LIFT_REMOCON_BALANCE },
    { .mActorProfile = &g_profile_AC_LIFT_REMOCON_XLINE },
    { .mActorProfile = &g_profile_EN_LIFT_REMOCON_TRPLN },
    { .mActorProfile = &g_profile_EN_TRPLN_WALL },
    { .mActorProfile = &g_profile_EN_CHIKUWA_BLOCK },
    { .mActorProfile = &g_profile_EN_CLIFF_CHIKUWA_BLOCK },
    { .mActorProfile = &g_profile_EN_KILLER_HOUDAI },
    { .mActorProfile = &g_profile_EN_MAGNUM_KILLER_HOUDAI },
    { .mActorProfile = &g_profile_AC_WAKILIFT_STEP },
    { .mActorProfile = &g_profile_AC_LINE_LIFT },
    { .mActorProfile = &g_profile_LINE_SPIN_LIFT },
    { .mActorProfile = &g_profile_PALM_TREE },
    { .mActorProfile = &g_profile_NICE_BOAT },
    { .mActorProfile = &g_profile_LINE_KINOKO_BLOCK },
    { .mActorProfile = &g_profile_LADDER },
    { .mActorProfile = &g_profile_TARZAN_TSUTA },
    { .mActorProfile = &g_profile_TARZAN_IVY },
    { .mActorProfile = &g_profile_EN_LIFT_ROTATION_HALF },
    { .mActorProfile = &g_profile_LIFT_HURIKO_CENTER },
    { .mActorProfile = &g_profile_LIFT_HURIKO },
    { .mActorProfile = &g_profile_AC_LIFT_RIDEMOVE },
    { .mActorProfile = &g_profile_EN_LIFT_BURANKO },
    { .mActorProfile = &g_profile_AC_LIFT_WATERWHEEL },
    { .mActorProfile = &g_profile_EN_RAIL_POLYGON },
    { .mActorProfile = &g_profile_EN_RAIL_POLY_ICE },
    { .mActorProfile = &g_profile_EN_RAIL_POLY_PARENT },
    { .mActorProfile = &g_profile_EN_RAIL_POLY_CHILD },
    { .mActorProfile = &g_profile_FREEFALL },
    { .mActorProfile = &g_profile_EN_GURA_ROCK },
    { .mActorProfile = &g_profile_EN_GURA_YUKA },
    { .mActorProfile = &g_profile_EN_KAITEN_HOUDAI },
    { .mActorProfile = &g_profile_EN_REDRING },
    { .mActorProfile = &g_profile_EN_BARREL },
    { .mActorProfile = &g_profile_EN_JUMPDAI },
    { .mActorProfile = &g_profile_EN_BIG_HNSWICH },
    { .mActorProfile = &g_profile_EN_BIGBLOCK },
    { .mActorProfile = &g_profile_EN_SNAKEBLOCK },
    { .mActorProfile = &g_profile_EN_MARUTA },
    { .mActorProfile = &g_profile_OBJ_KAMECK },
    { .mActorProfile = &g_profile_AC_LIFT_SPINROTATION },
    { .mActorProfile = &g_profile_AC_LIFT_SPINROTATION_SYNC },
    { .mActorProfile = &g_profile_EN_RULETBLOCK },
    { .mActorProfile = &g_profile_AC_LIFT_SCALES },
    { .mActorProfile = &g_profile_AC_LIFT_SCALES_STEP },
    { .mActorProfile = &g_profile_EN_TARZANROPE },
    { .mActorProfile = &g_profile_AC_LIFT_BOSS_REMOCON_SEESAW },
    { .mActorProfile = &g_profile_AC_CHENGE_BLOCK },
    { .mActorProfile = &g_profile_EN_FIXATION_ICICLE },
    { .mActorProfile = &g_profile_EN_OBJ_HATENA_BLOCK },
    { .mActorProfile = &g_profile_EN_OBJ_HATENAB_BLOCK },
    { .mActorProfile = &g_profile_EN_OBJ_RENGA_BLOCK },
    { .mActorProfile = &g_profile_EN_OBJ_CLEAR_BLOCK },
    { .mActorProfile = &g_profile_AC_OBJ_CLEAR_BLOCK },
    { .mActorProfile = &g_profile_EN_HATENA_BLOCK_LINE },
    { .mActorProfile = &g_profile_EN_RENGA_BLOCK_LINE },
    { .mActorProfile = &g_profile_EN_BLOCK_HATENA_WATER },
    { .mActorProfile = &g_profile_EN_BLOCK_SOROBAN },
    { .mActorProfile = &g_profile_EN_BLOCK_STAFFROLL },
    { .mActorProfile = &g_profile_ELASTIC_KINOKO_PARENT },
    { .mActorProfile = &g_profile_ELASTIC_KINOKO },
    { .mActorProfile = &g_profile_EN_WHITE_BLOCK },
    { .mActorProfile = &g_profile_MANTA_MGR },
    { .mActorProfile = &g_profile_MANTA_MGR2 },
    { .mActorProfile = &g_profile_MANTA },
    { .mActorProfile = &g_profile_AC_LIFT_SLIDE_KINOKO },
    { .mActorProfile = &g_profile_EN_ROTATION_BLOCK },
    { .mActorProfile = &g_profile_EN_COIN_ANGLE },
    { .mActorProfile = &g_profile_ROT_DOKAN },
    { .mActorProfile = &g_profile_EN_BLOCK_HATENA_ANGLE },
    { .mActorProfile = &g_profile_EN_BLOCK_RENGA_ANGLE },
    { .mActorProfile = &g_profile_AC_WIRE_CIRCLE },
    { .mActorProfile = &g_profile_AC_WIRE_REMOCON },
    { .mActorProfile = &g_profile_AC_LIFT_ICE_SPRING },
    { .mActorProfile = &g_profile_ICELUMP },
    { .mActorProfile = &g_profile_ICECUBE },
    { .mActorProfile = &g_profile_EN_WOODBOX },
    { .mActorProfile = &g_profile_EN_PATAMET },
    { .mActorProfile = &g_profile_EN_BIG_PATAMET },
    { .mActorProfile = &g_profile_NUT },
    { .mActorProfile = &g_profile_EN_BIG_MET },
    { .mActorProfile = &g_profile_EN_GAKE_NOKO },
    { .mActorProfile = &g_profile_AC_DOKAN_CANNON },
    { .mActorProfile = &g_profile_AC_SHIP_CANNON },
    { .mActorProfile = &g_profile_AC_ROT_CANNON },
    { .mActorProfile = &g_profile_AC_ROT_DOKAN_CANNON },
    { .mActorProfile = &g_profile_ROT_BARNAR },
    { .mActorProfile = &g_profile_CIRCLE_RIGHT },
    { .mActorProfile = &g_profile_BLOCK_LIGHT },
    { .mActorProfile = &g_profile_HAND_RIGHT },
    { .mActorProfile = &g_profile_EN_BIG_ICICLE },
    { .mActorProfile = &g_profile_AC_WATER_BUBBLE_PARENT },
    { .mActorProfile = &g_profile_AC_WATER_BUBBLE_CANNON },
    { .mActorProfile = &g_profile_EN_KILLER_HOUDAI_SLIDE },
    { .mActorProfile = &g_profile_EN_BLACK_PAKKUN },
    { .mActorProfile = &g_profile_TORCHILIGHT },
    { .mActorProfile = &g_profile_EN_BLOCK_CLOUD },
    { .mActorProfile = &g_profile_AC_WIRE_MOVE },
    { .mActorProfile = &g_profile_OBJ_FRUITTREE },
    { .mActorProfile = &g_profile_EN_BOUNCE_BALL },
    { .mActorProfile = &g_profile_EN_BLOCK_HATENA_PLAYER },
    { .mActorProfile = &g_profile_EN_BLOCK_RENGA_PLAYER },
    { .mActorProfile = &g_profile_AC_FREEZER },
    { .mActorProfile = &g_profile_AC_WIRE_FOOTHOLD },
    { .mActorProfile = &g_profile_EN_ITEM_IVY },
    { .mActorProfile = &g_profile_TARZAN_ROPE },
    { .mActorProfile = &g_profile_CANNON_PIPE },
    { .mActorProfile = &g_profile_KINOKO_LIFT_PARENT },
    { .mActorProfile = &g_profile_KINOKO_LIFT },
    { .mActorProfile = &g_profile_OBJ_SPIN_PARENT },
    { .mActorProfile = &g_profile_OBJ_SPIN_CHILD },
    { .mActorProfile = &g_profile_OBJ_SPIN_ASHIBA },
    { .mActorProfile = &g_profile_SPIN_WIRE },
    { .mActorProfile = &g_profile_WARP_CANNON },
    { .mActorProfile = &g_profile_OBJ_KINOKO },
    { .mActorProfile = &g_profile_ZOOM_PIPE },
    { .mActorProfile = &g_profile_ZOOM_PIPE_DOWN },
    { .mActorProfile = &g_profile_OBJ_PIPE_UP },
    { .mActorProfile = &g_profile_OBJ_PIPE_UP_NG },
    { .mActorProfile = &g_profile_OBJ_PIPE_DOWN },
    { .mActorProfile = &g_profile_OBJ_PIPE_RIGHT },
    { .mActorProfile = &g_profile_OBJ_PIPE_LEFT },
    { .mActorProfile = &g_profile_LIFT_DOWN_ON },
    { .mActorProfile = &g_profile_OBJ_MOVE_ON_GHOST },
    { .mActorProfile = &g_profile_LIFT_DOWN_ON_NORMAL },
    { .mActorProfile = &g_profile_LIFT_RIDE_OFF },
    { .mActorProfile = &g_profile_LINE_TRAIN },
    { .mActorProfile = &g_profile_REMO_LINE_LIFT },
    { .mActorProfile = &g_profile_WATER_LIFT },
    { .mActorProfile = &g_profile_AC_NICE_COIN },
    { .mActorProfile = &g_profile_AC_NICE_COIN_REGULAR },
    { .mActorProfile = &g_profile_EN_BLOCK_HELP },
    { .mActorProfile = &g_profile_FAR_BG },
    { .mActorProfile = &g_profile_AC_BG_WATER },
    { .mActorProfile = &g_profile_AC_BG_LAVA },
    { .mActorProfile = &g_profile_AC_BG_POISON },
    { .mActorProfile = &g_profile_AC_BG_SAND },
    { .mActorProfile = &g_profile_AC_BG_CLOUD },
    { .mActorProfile = &g_profile_AC_BG_MOYA },
    { .mActorProfile = &g_profile_AC_WATERALL_RAIL },
    { .mActorProfile = &g_profile_EN_BLUR },
    { .mActorProfile = &g_profile_MASK },
    { .mActorProfile = &g_profile_DTEST },
    { .mActorProfile = &g_profile_AC_WATER_MOVE },
    { .mActorProfile = &g_profile_AC_WATER_MOVE_REGULAR },
    { .mActorProfile = &g_profile_AC_WATER_XINFINITE_MOVE },
    { .mActorProfile = &g_profile_AC_FLOOR_GYRATION },
    { .mActorProfile = &g_profile_AC_FLOOR_HOLE_DOKAN },
    { .mActorProfile = &g_profile_AC_FLOOR_DOKAN_EIGHT },
    { .mActorProfile = &g_profile_AC_GEAR_GYRATION },
    { .mActorProfile = &g_profile_AC_SHIP_GEAR },
    { .mActorProfile = &g_profile_AC_BIGSHELL },
    { .mActorProfile = &g_profile_EN_BOSS_KOOPA_DEMO_CAGE },
    { .mActorProfile = &g_profile_AC_STAND },
    { .mActorProfile = &g_profile_AC_HOUSE_LIGHT },
    { .mActorProfile = &g_profile_AC_LIGHT_WATER },
    { .mActorProfile = &g_profile_AC_COPYRIGHT },
    { .mBaseProfile  = &g_profile_WII_STRAP },
    { .mActorProfile = &g_profile_WM_CS_SEQ_MNG },
    { .mBaseProfile  = &g_profile_WORLD_CAMERA },
    { .mActorProfile = &g_profile_WM_MAP },
    { .mActorProfile = &g_profile_WM_IBARA },
    { .mActorProfile = &g_profile_WM_ANTLION_MNG },
    { .mActorProfile = &g_profile_WM_CASTLE },
    { .mActorProfile = &g_profile_WM_TOWER },
    { .mActorProfile = &g_profile_WM_PEACH_CASTLE },
    { .mActorProfile = &g_profile_WM_KILLER },
    { .mActorProfile = &g_profile_WM_KILLERBULLET },
    { .mActorProfile = &g_profile_WM_SINKSHIP },
    { .mActorProfile = &g_profile_WM_SWITCH },
    { .mActorProfile = &g_profile_WM_START },
    { .mActorProfile = &g_profile_WM_KINOKO_BASE },
    { .mActorProfile = &g_profile_WM_KINOKO_RED },
    { .mActorProfile = &g_profile_WM_KINOKO_1UP },
    { .mActorProfile = &g_profile_WM_KINOKO_STAR },
    { .mActorProfile = &g_profile_WM_COURSE },
    { .mActorProfile = &g_profile_WM_STOP },
    { .mActorProfile = &g_profile_WM_TORIDE },
    { .mActorProfile = &g_profile_WM_GHOST },
    { .mActorProfile = &g_profile_WM_DOKAN },
    { .mActorProfile = &g_profile_WM_CANNON },
    { .mActorProfile = &g_profile_WM_KOOPASHIP },
    { .mActorProfile = &g_profile_WM_BOSS_BASE },
    { .mActorProfile = &g_profile_WM_BOSS_LARRY },
    { .mActorProfile = &g_profile_WM_BOSS_ROY },
    { .mActorProfile = &g_profile_WM_BOSS_WENDY },
    { .mActorProfile = &g_profile_WM_BOSS_IGGY },
    { .mActorProfile = &g_profile_WM_BOSS_LEMMY },
    { .mActorProfile = &g_profile_WM_BOSS_MORTON },
    { .mActorProfile = &g_profile_WM_BOSS_LUDWIG },
    { .mActorProfile = &g_profile_WM_BOSS_KAMECK },
    { .mActorProfile = &g_profile_WM_ANTLION },
    { .mActorProfile = &g_profile_WM_KURIBO },
    { .mActorProfile = &g_profile_WM_PUKU },
    { .mActorProfile = &g_profile_WM_PAKKUN },
    { .mActorProfile = &g_profile_WM_BROS },
    { .mActorProfile = &g_profile_WM_JUGEM },
    { .mActorProfile = &g_profile_WM_PLAYER },
    { .mActorProfile = &g_profile_WM_SUBPLAYER },
    { .mActorProfile = &g_profile_WM_NOTE },
    { .mActorProfile = &g_profile_WM_TREASURESHIP },
    { .mActorProfile = &g_profile_WM_DIRECTOR },
    { .mActorProfile = &g_profile_WM_GRID },
    { .mActorProfile = &g_profile_WM_ISLAND },
    { .mActorProfile = &g_profile_WM_ITEM },
    { .mActorProfile = &g_profile_WM_CS_W3_PALM },
    { .mActorProfile = &g_profile_WM_SURRENDER },
    { .mActorProfile = &g_profile_WM_KOOPA_CASTLE },
    { .mActorProfile = &g_profile_WM_ANCHOR },
    { .mActorProfile = &g_profile_WM_PAKKUNHEAD },
    { .mActorProfile = &g_profile_WM_CLOUD },
    { .mActorProfile = &g_profile_WM_SMALLCLOUD },
    { .mActorProfile = &g_profile_WM_KOOPAJR },
    { .mActorProfile = &g_profile_WM_PEACH },
    { .mActorProfile = &g_profile_WM_BOARD },
    { .mActorProfile = &g_profile_WM_BUBBLE },
    { .mActorProfile = &g_profile_WM_KINOBALLOON },
    { .mActorProfile = &g_profile_WM_SANDPILLAR },
    { .mActorProfile = &g_profile_WM_YOGANPILLAR },
    { .mActorProfile = &g_profile_WM_KINOPIO },
    { .mActorProfile = &g_profile_WM_DANCE_PAKKUN },
    { .mActorProfile = &g_profile_WM_DOKANROUTE },
    { .mActorProfile = &g_profile_WM_HANACHAN },
    { .mActorProfile = &g_profile_WM_TOGEZO },
    { .mActorProfile = &g_profile_WM_MANTA },
    { .mActorProfile = &g_profile_WM_TERESA },
    { .mBaseProfile  = &g_profile_WORLD_SELECT },
    { .mBaseProfile  = &g_profile_WORLD_SELECT_GUIDE },
    { .mBaseProfile  = &g_profile_COLLECTION_COIN_BASE },
    { .mBaseProfile  = &g_profile_COLLECTION_COIN },
    { .mBaseProfile  = &g_profile_COURSE_SELECT_MENU },
    { .mBaseProfile  = &g_profile_NUMBER_OF_PEOPLE_CHANGE },
    { .mBaseProfile  = &g_profile_STOCK_ITEM },
    { .mBaseProfile  = &g_profile_STOCK_ITEM_SHADOW },
    { .mBaseProfile  = &g_profile_EASY_PAIRING },
    { .mActorProfile = &g_profile_WM_2D_PLAYER },
    { .mBaseProfile  = &g_profile_EVENT_OPENING_TITLE },
    { .mBaseProfile  = &g_profile_SELECT_PLAYER },
    { .mBaseProfile  = &g_profile_MULTI_COURSE_SELECT },
    { .mBaseProfile  = &g_profile_TIME_UP },
    { .mBaseProfile  = &g_profile_COURSE_TIME_UP },
    { .mBaseProfile  = &g_profile_YES_NO_WINDOW },
    { .mActorProfile = &g_profile_COURSE_SELECT_MANAGER },
    { .mActorProfile = &g_profile_FUKIDASHI_MANAGER },
    { .mActorProfile = &g_profile_SMALL_SCORE_MANAGER },
    { .mActorProfile = &g_profile_GAMEDISPLAY },
    { .mBaseProfile  = &g_profile_OTASUKE_INFO },
    { .mActorProfile = &g_profile_PAUSEWINDOW },
    { .mBaseProfile  = &g_profile_RESULT },
    { .mBaseProfile  = &g_profile_POINT_RESULT_MULTI },
    { .mBaseProfile  = &g_profile_POINT_RESULT_DATE_FILE },
    { .mBaseProfile  = &g_profile_POINT_RESULT_DATE_FILE_FREE },
    { .mActorProfile = &g_profile_GOAL_MANAGER },
    { .mBaseProfile  = &g_profile_COURSE_CLEAR },
    { .mActorProfile = &g_profile_OTEHON_CLEAR },
    { .mBaseProfile  = &g_profile_INFO_WINDOW },
    { .mActorProfile = &g_profile_SELECT_CURSOR },
    { .mBaseProfile  = &g_profile_WARNING_MANAGER },
    { .mBaseProfile  = &g_profile_SEQUENCE_BG },
    { .mBaseProfile  = &g_profile_GAME_OVER },
    { .mBaseProfile  = &g_profile_MODE_SELECT },
    { .mBaseProfile  = &g_profile_DATE_FILE },
    { .mBaseProfile  = &g_profile_FILE_SELECT },
    { .mBaseProfile  = &g_profile_CONTINUE },
    { .mBaseProfile  = &g_profile_MINI_GAME_CANNON },
    { .mBaseProfile  = &g_profile_MINI_GAME_WIRE },
    { .mBaseProfile  = &g_profile_MODEL_PLAY_MANAGER },
    { .mBaseProfile  = &g_profile_MODEL_PLAY_BASE },
    { .mBaseProfile  = &g_profile_MODEL_PLAY_DATE },
    { .mBaseProfile  = &g_profile_MODEL_PLAY_ARROW },
    { .mBaseProfile  = &g_profile_MODEL_PLAY_GUIDE },
    { .mBaseProfile  = &g_profile_MESSAGE_WINDOW },
    { .mBaseProfile  = &g_profile_CHARACTER_CHANGE_SELECT_BASE },
    { .mBaseProfile  = &g_profile_CHARACTER_CHANGE_SELECT_CONTENTS },
    { .mBaseProfile  = &g_profile_CHARACTER_CHANGE_SELECT_ARROW },
    { .mBaseProfile  = &g_profile_CHARACTER_CHANGE_INDICATOR },
    { .mBaseProfile  = &g_profile_MULTI_COURSE_SELECT_CONTENTS },
    { .mBaseProfile  = &g_profile_COLLECTION_COIN_DATE },
    { .mBaseProfile  = &g_profile_CONTROLLER_INFORMATION },
    { .mBaseProfile  = &g_profile_DRAW_GAME },
    { .mBaseProfile  = &g_profile_STAFF_CREDIT_SCORE },
    { .mBaseProfile  = &g_profile_THE_END },
    { .mBaseProfile  = &g_profile_DEMO_MESSAGE },
    { .mBaseProfile  = &g_profile_LETTER_WINDOW },
    { .mActorProfile = &g_profile_DUMMY_ACTOR },
    { .mBaseProfile  = &g_profile_LASTACTOR_STAGE },
    { .mBaseProfile  = &g_profile_LASTACTOR }
};

void initProfile() {
    fProfile::sProfileList = &profileList;
}

void finalizeProlog() {
    initProfile();
}

void finalizeEpilog() {
    fProfile::sProfileList = nullptr;
}
